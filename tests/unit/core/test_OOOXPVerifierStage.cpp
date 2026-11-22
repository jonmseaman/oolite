/*	test_OOOXPVerifierStage.mm
        Unit tests for OOOXPVerifierStage
   (src/Core/OXPVerifier/OOOXPVerifierStage.h).

        The base class has a dozen subclasses, and OOOXPVerifier drives every
   stage. So this pins what the base computes: the defaults a subclass inherits,
   the dependency graph (direct and recursive dependency, no duplicates,
   dependents told once in registration order), running and skipping, an
   exception from -run caught, the description, and the super call from a
   subclass. The test stages are C++ subclasses (the super call a qualified base
   call), and the description is description(). Run: bash
   tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "Logging.h"
#include "OXPVerifierStage.h"
#include "oofnd/Exception.hpp"

/*	The game's OOLogging.mm reaches the resource manager, so it is not
   linked. The one function of it that the base calls is defined here instead,
   and counts: "subclass responsibility" is checked, not only logged.
*/
static int gSubclassResponsibilities = 0;

namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
  gSubclassResponsibilities++;
}

}  // namespace oo


/*	A verifier for verifierIsNotRetained: the stage keeps the C++ verifier.
   Its constructor is private; the class names this struct
   its friend for tests. Nothing the verifier defines out of line is called, so
   OOOXPVerifier.mm is not linked.
*/
struct OOOXPVerifierTestAccess {
  static std::shared_ptr<oo::OXPVerifier> Make() {
    return std::shared_ptr<oo::OXPVerifier>(new oo::OXPVerifier(), oo::Dispose{});
  }
};

/*	A stage that overrides the subclass responsibilities.
 */
class TestObjCStage : public oo::OXPVerifierStage {
 public:
  std::optional<std::string> name() override { return _testName; }
  std::optional<std::vector<std::string>> dependencies() override {
    return _testDependencies;
  }

  void run() override {
    _runs++;
    if (_raise) oo::RaiseException("TestException", "%s", "raised by the test");
  }

  std::string _testName;
  std::optional<std::vector<std::string>> _testDependencies;
  int _runs = 0;
  bool _raise = false;
};

// Extends its superclass's dependents through its base's dependents(), as
// OOTextureHandlingStage does.
class TestSuperCallingStage : public TestObjCStage {
 public:
  std::optional<std::vector<std::string>> dependents() override {
    std::vector<std::string> result =
        TestObjCStage::dependents().value_or(std::vector<std::string>());
    result.push_back("Listing unused files");
    return result;
  }
};

// A converted stage: a C++ subclass that overrides the virtual members. Global,
// as a game class is, so that its description names it as the game's would.
class TestCxxStage : public oo::OXPVerifierStage {
 public:
  explicit TestCxxStage(std::string testName)
      : _testName(std::move(testName)) {}

  std::optional<std::string> name() override { return _testName; }
  std::optional<std::vector<std::string>> dependents() override {
    return std::vector<std::string>{"Dependent"};
  }
  bool shouldRun() override { return _shouldRun; }
  void run() override { runs++; }

  int runs = 0;
  bool _shouldRun = false;

 private:
  std::string _testName;
};

namespace {

std::shared_ptr<TestObjCStage> MakeStage(const char* name) {
  std::shared_ptr<TestObjCStage> stage = oo::makeShared<TestObjCStage>();
  stage->_testName = name;
  return stage;
}

bool Holds(const std::vector<std::shared_ptr<oo::OXPVerifierStage>>& stages,
           std::initializer_list<oo::OXPVerifierStage*> expected) {
  if (stages.size() != expected.size()) return false;
  std::size_t i = 0;
  for (oo::OXPVerifierStage* stage : expected) {
    if (stages[i++].get() != stage) return false;
  }
  return true;
}

}  // namespace

TEST(OOOXPVerifierStage, baseDefaults) {
  {
    const std::shared_ptr<oo::OXPVerifierStage> stage = oo::makeShared<oo::OXPVerifierStage>();
    const int before = gSubclassResponsibilities;
    EXPECT_TRUE(!stage->name()
                  .has_value());  // subclass responsibility: logs, answers nil
    EXPECT_TRUE(gSubclassResponsibilities == before + 1);
    stage->run();
    EXPECT_TRUE(gSubclassResponsibilities == before + 2);
    EXPECT_TRUE(!stage->dependencies().has_value());
    EXPECT_TRUE(!stage->dependents().has_value());
    EXPECT_TRUE(stage->shouldRun());
    EXPECT_TRUE(!stage->completed());
    EXPECT_TRUE(!stage->canRun());
    EXPECT_TRUE(stage->verifier() == nullptr);
    EXPECT_TRUE(stage->resolvedDependencies().empty() &&
             stage->resolvedDependents().empty());
    stage->dependencyRegistrationComplete();
    EXPECT_TRUE(stage->canRun());  // nothing to wait for
  }
}

TEST(OOOXPVerifierStage, verifierIsNotRetained) {
  {
    const std::shared_ptr<oo::OXPVerifier> object = OOOXPVerifierTestAccess::Make();
    oo::OXPVerifier* verifier = object.get();
    const std::shared_ptr<TestObjCStage> stage = MakeStage("Stage");
    const long before = object->weak_from_this().use_count();
    stage->setVerifier(verifier);
    EXPECT_TRUE(object->weak_from_this().use_count() == before);
    {
      EXPECT_TRUE(stage->verifier() == verifier);
    }
    EXPECT_TRUE(object->weak_from_this().use_count() == before);
  }
}

TEST(OOOXPVerifierStage, dependencyGraph) {
  {
    const std::shared_ptr<TestObjCStage> a = MakeStage("A"), b = MakeStage("B"),
                                 c = MakeStage("C"), d = MakeStage("D");
    b->registerDependency(a.get());
    c->registerDependency(b.get());
    c->registerDependency(b.get());  // no duplicates
    d->registerDependency(a.get());

    EXPECT_TRUE(b->isDependentOf(a.get()) && c->isDependentOf(b.get()));
    EXPECT_TRUE(c->isDependentOf(a.get()));  // recursive
    EXPECT_TRUE(!a->isDependentOf(c.get()) && !b->isDependentOf(c.get()) &&
             !d->isDependentOf(b.get()));
    EXPECT_TRUE(!c->isDependentOf(nullptr));
    EXPECT_TRUE(Holds(c->resolvedDependencies(), {b.get()}));
    EXPECT_TRUE(Holds(a->resolvedDependents(),
                   {b.get(), d.get()}));  // registration order
    EXPECT_TRUE(Holds(b->resolvedDependents(), {c.get()}));

    for (TestObjCStage* stage : {a.get(), b.get(), c.get(), d.get()})
      stage->dependencyRegistrationComplete();
    EXPECT_TRUE(a->canRun() && !b->canRun() && !c->canRun() && !d->canRun());

    a->performRun();
    EXPECT_TRUE(a->_runs == 1 && a->completed() && !a->canRun());
    EXPECT_TRUE(b->canRun() && d->canRun() &&
             !c->canRun());  // every dependent told

    b->noteSkipped();
    EXPECT_TRUE(b->_runs == 0 && b->completed() && !b->canRun());
    EXPECT_TRUE(c->canRun());
  }
}

TEST(OOOXPVerifierStage, exceptionInRunIsCaught) {
  {
    const std::shared_ptr<TestObjCStage> raising = MakeStage("Raising"),
                                 after = MakeStage("After");
    raising->_raise = true;
    after->registerDependency(raising.get());
    raising->dependencyRegistrationComplete();
    after->dependencyRegistrationComplete();

    raising->performRun();  // logs verifyOXP.exception
    EXPECT_TRUE(raising->_runs == 1 && raising->completed());
    EXPECT_TRUE(after->canRun());
  }
}

TEST(OOOXPVerifierStage, superCallsReachTheBase) {
  {
    const std::shared_ptr<TestSuperCallingStage> stage =
        oo::makeShared<TestSuperCallingStage>();
    const std::optional<std::vector<std::string>> dependents =
        stage->dependents();
    EXPECT_TRUE(dependents == std::vector<std::string>{"Listing unused files"});
  }
}

TEST(OOOXPVerifierStage, description) {
  {
    const std::string text = MakeStage("Checking things")->description();
    EXPECT_TRUE(text.starts_with("<TestObjCStage 0x"));
    EXPECT_TRUE(text.ends_with(">{\"Checking things\"}"));
    EXPECT_TRUE(oo::makeShared<oo::OXPVerifierStage>()->description().ends_with(
        ">{\"(null)\"}"));
  }
}

TEST(OOOXPVerifierStage, cxxStageBehindTheFacade) {
  {
    const std::shared_ptr<TestCxxStage> stage = oo::makeShared<TestCxxStage>("Cxx");

    // The verifier's calls reach the C++ overrides, and the base's defaults.
    EXPECT_TRUE(stage->name() == std::optional<std::string>("Cxx"));
    EXPECT_TRUE(!stage->dependencies().has_value());
    EXPECT_TRUE(stage->dependents() == std::vector<std::string>{"Dependent"});
    EXPECT_TRUE(!stage->shouldRun());
    stage->dependencyRegistrationComplete();
    stage->performRun();
    EXPECT_TRUE(stage->runs == 1 && stage->completed() && stage->completed());

    // Described with the C++ class's name, as [self class] named it.
    const std::string text = stage->description();
    EXPECT_TRUE(text.starts_with("<TestCxxStage 0x"));
    EXPECT_TRUE(text.ends_with(">{\"Cxx\"}"));
    EXPECT_TRUE(stage->descriptionComponents() ==
             std::optional<std::string>("\"Cxx\""));
  }
}

TEST(OOOXPVerifierStage, objCStageBehindACxxPointer) {
  {
    const std::shared_ptr<TestObjCStage> objCStage = MakeStage("ObjC");
    oo::OXPVerifierStage* part = objCStage.get();
    EXPECT_TRUE(part != nullptr);

    // Virtual calls through the base reach the subclass's overrides.
    EXPECT_TRUE(part->name() == std::optional<std::string>("ObjC"));
    objCStage->_testDependencies = std::vector<std::string>{"Cxx"};
    EXPECT_TRUE(part->dependencies() == objCStage->_testDependencies);
    EXPECT_TRUE(part->shouldRun());  // not overridden: the base's answer
    EXPECT_TRUE(!part->dependents().has_value());

    // The subclass's base call still reaches the base, not back to the
    // subclass.
    const std::shared_ptr<TestSuperCallingStage> superCalling =
        oo::makeShared<TestSuperCallingStage>();
    EXPECT_TRUE(
        static_cast<oo::OXPVerifierStage*>(superCalling.get())->dependents() ==
        std::vector<std::string>{"Listing unused files"});
  }
}

TEST(OOOXPVerifierStage, mixedDependencyGraph) {
  {
    const std::shared_ptr<TestCxxStage> cxxStage = oo::makeShared<TestCxxStage>("Cxx");
    cxxStage->_shouldRun = true;
    const std::shared_ptr<TestObjCStage> objCStage = MakeStage("ObjC");

    // The verifier wires the stages together.
    objCStage->registerDependency(cxxStage.get());
    EXPECT_TRUE(objCStage->isDependentOf(cxxStage.get()));
    EXPECT_TRUE(objCStage->isDependentOf(cxxStage.get()));
    EXPECT_TRUE(Holds(cxxStage->resolvedDependents(), {objCStage.get()}));
    EXPECT_TRUE(cxxStage->resolvedDependents().size() == 1 &&
             cxxStage->resolvedDependents()[0].get() == objCStage.get());

    cxxStage->dependencyRegistrationComplete();
    objCStage->dependencyRegistrationComplete();
    EXPECT_TRUE(cxxStage->canRun() && !objCStage->canRun());
    cxxStage->performRun();
    EXPECT_TRUE(cxxStage->runs == 1 && objCStage->canRun());
    objCStage->performRun();  // virtual run() reaches the override
    EXPECT_TRUE(objCStage->_runs == 1 && objCStage->completed());
  }
}
