/*	test_OOMaterial.mm
        Unit tests for OOMaterial (src/Core/Materials/OOMaterial.h). The test
   materials are C++ subclasses.

        OOMaterial is the root of the material hierarchy (OOBasicMaterial and
   the three materials under it) and owns the one piece of global render state,
   the current material. So this pins what the root computes, through the API its callers
   use: the defaults a subclass inherits, -apply / +applyNone / +current (the
   previous material told what comes next, the current material retained, a
   failed -doApply leaving none), -willDealloc's imbalance path, the
   description, and a call to the root from a subclass. Then the
   hierarchy's crossing both ways, as test_OOOXPVerifierStage.mm does, and the
   current material keeping a material alive while C++ holds it.
   Run: bash tools/check-core-tests.sh
*/

#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "Logging.h"
#include "Material.h"

/*	The game's OOLogging.mm reaches the resource manager, so it is not
   linked. The two functions of it that the root calls are defined here instead,
   and count.
*/
static int gSubclassResponsibilities = 0;
static int gParameterErrors = 0;

namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
  gSubclassResponsibilities++;
}

void LogGenericParameterErrorForFunction(const char* inFunction) {
  (void)inFunction;
  gParameterErrors++;
}

}  // namespace oo


// Every unapplyWithNext() sent to a test material, in order: (material, next).
static std::vector<std::pair<oo::Material*, oo::Material*>> gUnapplied;
static int gDeallocs = 0;

// A C++ subclass of the root, named so that its
// description is unchanged.
class TestObjCMaterial : public oo::Material {
 public:
  ~TestObjCMaterial() override {
    willDealloc();
    gDeallocs++;
  }

  std::optional<std::string> name() override { return _testName; }

  bool doApply() override {
    _doApplies++;
    return _applies;
  }

  void unapplyWithNext(oo::Material* next) override {
    gUnapplied.emplace_back(this, next);
    oo::Material::unapplyWithNext(next);
  }

  std::string _testName;
  bool _applies = false;
  int _doApplies = 0;
};

// Asks its superclass, as OOBasicMaterial's allTextures() and doApply() do.
class TestSuperCallingMaterial : public TestObjCMaterial {
 public:
  bool doApply() override {
    return TestObjCMaterial::doApply() || isFinishedLoading();
  }
  bool wantsNormalsAsTextureCoordinates() override {
    return !TestObjCMaterial::wantsNormalsAsTextureCoordinates();
  }
};

// A converted material: a C++ subclass that overrides the virtual members.
// Global, as a game class is, so that its description names it as the game's
// would.
static int gCxxDestroyed = 0;

class TestCxxMaterial : public oo::Material {
 public:
  explicit TestCxxMaterial(std::string testName)
      : _testName(std::move(testName)) {}
  ~TestCxxMaterial() override { gCxxDestroyed++; }

  std::optional<std::string> name() override { return _testName; }
  bool wantsNormalsAsTextureCoordinates() override { return true; }
  bool doApply() override {
    doApplies++;
    return true;
  }
  void unapplyWithNext(oo::Material* next) override {
    unappliedNext.push_back(next);
  }

  int doApplies = 0;
  std::vector<oo::Material*> unappliedNext;

 private:
  std::string _testName;
};

namespace {

std::shared_ptr<TestObjCMaterial> MakeMaterial(const char* name, bool applies = true) {
  std::shared_ptr<TestObjCMaterial> material = oo::makeShared<TestObjCMaterial>();
  material->_testName = name;
  material->_applies = applies;
  return material;
}

bool Unapplied(
    std::initializer_list<std::pair<oo::Material*, oo::Material*>> expected) {
  const bool result =
      std::vector<std::pair<oo::Material*, oo::Material*>>(expected) == gUnapplied;
  gUnapplied.clear();
  return result;
}

}  // namespace

TEST(OOMaterial, rootDefaults) {
  {
    const std::shared_ptr<oo::Material> material = oo::makeShared<oo::Material>();
    const int parameterErrors = gParameterErrors,
              responsibilities = gSubclassResponsibilities;
    EXPECT_TRUE(
        !material->name().has_value());  // logs a parameter error, answers nil
    EXPECT_TRUE(gParameterErrors == parameterErrors + 1);
    EXPECT_TRUE(!material->doApply());  // subclass responsibility
    EXPECT_TRUE(gSubclassResponsibilities == responsibilities + 1);
    material->ensureFinishedLoading();
    EXPECT_TRUE(material->isFinishedLoading());
    EXPECT_TRUE(!material->wantsNormalsAsTextureCoordinates());
    material->setBindingTarget(nullptr);
    material->unapplyWithNext(nullptr);
#if OO_MULTITEXTURE
    EXPECT_TRUE(material->countOfTextureUnitsWithBaseCoordinates() == 1);
#endif
#ifndef NDEBUG
    EXPECT_TRUE(material->allTextures().empty());
#endif
    oo::Material::setUp();

    // The root cannot apply: doApply() answers false, so there is still no
    // current material.
    material->apply();
    EXPECT_TRUE(oo::Material::current() == nullptr);
  }
}

TEST(OOMaterial, applyMakesCurrent) {
  {
    const std::shared_ptr<TestObjCMaterial> a = MakeMaterial("A"),
                                    b = MakeMaterial("B");
    gUnapplied.clear();
    EXPECT_TRUE(oo::Material::current() == nullptr);

    a->apply();
    EXPECT_TRUE(oo::Material::current().get() == a.get() && a->_doApplies == 1);
    EXPECT_TRUE(Unapplied({}));

    b->apply();  // the previous material is told what comes next
    EXPECT_TRUE(oo::Material::current().get() == b.get());
    EXPECT_TRUE(Unapplied({{a.get(), b.get()}}));

    b->apply();  // again: told itself
    EXPECT_TRUE(oo::Material::current().get() == b.get() && b->_doApplies == 2);
    EXPECT_TRUE(Unapplied({{b.get(), b.get()}}));

    oo::Material::applyNone();
    EXPECT_TRUE(oo::Material::current() == nullptr);
    EXPECT_TRUE(Unapplied({{b.get(), nullptr}}));

    oo::Material::applyNone();  // nothing current: nothing told
    EXPECT_TRUE(Unapplied({}));
  }
}

TEST(OOMaterial, failedApplyLeavesNone) {
  {
    const std::shared_ptr<TestObjCMaterial> a = MakeMaterial("A"),
                                    failing = MakeMaterial("Failing", false);
    a->apply();
    gUnapplied.clear();
    failing->apply();
    EXPECT_TRUE(oo::Material::current() == nullptr && failing->_doApplies == 1);
    EXPECT_TRUE(Unapplied({{a.get(), failing.get()}}));
    oo::Material::applyNone();
    EXPECT_TRUE(Unapplied({}));
  }
}

TEST(OOMaterial, currentIsRetained) {
  const int deallocs = gDeallocs;
  TestObjCMaterial* material = nullptr;
  {
    const std::shared_ptr<TestObjCMaterial> made = MakeMaterial("Kept");
    material = made.get();
    material->apply();
  }
  // The maker's reference is gone; being current keeps it.
  EXPECT_TRUE(gDeallocs == deallocs);
  {
    EXPECT_TRUE(oo::Material::current().get() == material);
    EXPECT_TRUE(oo::Material::current()->description().ends_with(">{\"Kept\"}"));
    gUnapplied.clear();
    oo::Material::applyNone();
    EXPECT_TRUE(Unapplied({{material, nullptr}}));
  }
  EXPECT_TRUE(gDeallocs == deallocs + 1);
}

TEST(OOMaterial, willDeallocWhileCurrent) {
  {
    const std::shared_ptr<TestObjCMaterial> material = MakeMaterial("Imbalanced");
    material->apply();
    const long retained = material.use_count();
    gUnapplied.clear();

    // What -dealloc did first. Current: unapplied with no next, and no longer
    // current, but the reference that being current held is not released (the
    // object was being deallocated).
    material->willDealloc();
    EXPECT_TRUE(Unapplied({{material.get(), nullptr}}));
    EXPECT_TRUE(oo::Material::current() == nullptr);
    EXPECT_TRUE(material.use_count() == retained);
    // The reference being current held stays in the holder willDealloc() moved
    // it to (never deleted; there is no manual release to give it back).

    material->willDealloc();  // not current: nothing
    EXPECT_TRUE(Unapplied({}));
  }
}

TEST(OOMaterial, superCallsReachTheRoot) {
  {
    const std::shared_ptr<TestSuperCallingMaterial> material =
        oo::makeShared<TestSuperCallingMaterial>();
    material->_applies = false;
    const int responsibilities = gSubclassResponsibilities;
    EXPECT_TRUE(material->doApply());  // false from TestObjCMaterial, then the
                                    // root's isFinishedLoading()
    EXPECT_TRUE(gSubclassResponsibilities == responsibilities);
    EXPECT_TRUE(material->wantsNormalsAsTextureCoordinates());
  }
}

TEST(OOMaterial, description) {
  {
    const std::string text = MakeMaterial("Hull")->description();
    EXPECT_TRUE(text.starts_with("<TestObjCMaterial 0x"));
    EXPECT_TRUE(text.ends_with(">{\"Hull\"}"));
    EXPECT_TRUE(
        oo::makeShared<oo::Material>()->description().ends_with(">{\"(null)\"}"));
  }
}

// A C++ material's overrides and the root's defaults, as its callers reach them
// .
TEST(OOMaterial, cxxMaterial) {
  {
    const std::shared_ptr<TestCxxMaterial> material =
        oo::makeShared<TestCxxMaterial>("Cxx");
    EXPECT_TRUE(material->name() == std::optional<std::string>("Cxx"));
    EXPECT_TRUE(material->wantsNormalsAsTextureCoordinates());
    EXPECT_TRUE(material->isFinishedLoading());

    material->apply();
    EXPECT_TRUE(material->doApplies == 1);
    EXPECT_TRUE(oo::Material::current().get() == material.get());
    oo::Material::applyNone();
    EXPECT_TRUE(material->unappliedNext == std::vector<oo::Material*>{nullptr});
    EXPECT_TRUE(oo::Material::current() == nullptr);

    // Described with the C++ class's name, as [self class] named it.
    const std::string text = material->description();
    EXPECT_TRUE(text.starts_with("<TestCxxMaterial 0x"));
    EXPECT_TRUE(text.ends_with(">{\"Cxx\"}"));
    EXPECT_TRUE(material->descriptionComponents() ==
             std::optional<std::string>("\"Cxx\""));
  }
}

// The current material is retained: a C++ material made current lives until it
// is no longer (the C++ half of currentIsRetainedAcrossTheBridge).
TEST(OOMaterial, currentRetainsACxxMaterial) {
  const int destroyed = gCxxDestroyed;
  {
    oo::makeShared<TestCxxMaterial>("Cxx")->apply();
  }
  EXPECT_TRUE(gCxxDestroyed == destroyed);
  {
    EXPECT_TRUE(oo::Material::current()->name() ==
             std::optional<std::string>("Cxx"));
    oo::Material::applyNone();
  }
  EXPECT_TRUE(gCxxDestroyed == destroyed + 1);
}
