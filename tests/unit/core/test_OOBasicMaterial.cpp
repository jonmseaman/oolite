/*	test_OOBasicMaterial.mm
        Unit tests for OOBasicMaterial (src/Core/Materials/OOBasicMaterial.h).

        OOBasicMaterial is the fixed-function material and the superclass of the
   three texture and shader materials. This pins what it computes, through its
   C++ API: the defaults, what a configuration
   dictionary sets (through OOMaterialSpecifier), the colour and component
   accessors, the clamped shininess, permitSpecular(), the GL state doApply()
   sets (read back from a real context), unapplyWithNext() falling back to the
   default material, and the description; and a C++ subclass overriding
   permitSpecular() and calling the base class. Last, how Objective-C sees one
   (as an OOMaterial). Run: tests/run.sh unit
*/

#include <cmath>
#include <cstdlib>

#include <gtest/gtest.h>

#include "BasicMaterial.h"
#include "Texture.h"
#include "oo_gl_test_context.hpp"

/*	The game's OOLogging.mm reaches the resource manager, so it is not
   linked; what the material and the GL error check call of it is defined here.
*/
namespace oo {

void LogGenericSubclassResponsibilityForFunction(const char* inFunction) {
  (void)inFunction;
}
void LogGenericParameterErrorForFunction(const char* inFunction) {
  (void)inFunction;
}
void LogIndent(void) {
}  // OOOpenGL.mm's error check (OOGL) indents what it logs
void LogOutdent(void) {}

}  // namespace oo


/*	OOTexture (OOTexture.mm links the texture loaders and the game) stands
   in for the one class method the material sends, and counts it.
*/
static int gTextureApplyNones = 0;

// The C++ OOTexture's static member.
void oo::Texture::applyNone() { gTextureApplyNones++; }

// Link stubs, never reached here:
// OOMaterialSpecifier.mm's texture specifiers, and OOOpenGL.mm's GL state dump.
namespace oo {

oo::PList TextureSpecFromObject(const oo::PList&,
                                      const std::optional<std::string>&) {
  std::abort();
}

}  // namespace oo

oo::OpenGLExtensionManager* oo::OpenGLExtensionManager::sharedManager() {
  std::abort();
}
GLint oo::OpenGLExtensionManager::textureUnitCount() { std::abort(); }
// OOOpenGLStateManager.mm's (linked for the debug build's
// OOGLNoteCurrentFunction) multitexture entry points live in
// OOOpenGLExtensionManager.mm, which this test stubs; never called here.
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = nullptr;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = nullptr;

/*	UNIVERSE is gSharedUniverse. The material asks it -reducedDetail
   (-permitSpecular), and the specifier asks it -useShaders; a stand-in answers
   both.
*/
class Universe;
Universe* gSharedUniverse = nullptr;

#include "UniverseStandIn.h"

// The universe as the code under test sees it: a stand-in for the C++ class,
// with the header's names and signatures. It answers virtual members, so it has
// the game's vtable (UniverseStandIn.h) and TestUniverse overrides them.
class Universe : public oo::Object {
 public:
  OO_UNIVERSE_STAND_IN_VIRTUAL_MEMBERS

  bool _reducedDetail;

  bool useShaders();
};

class TestUniverse : public Universe {
 public:
  bool reducedDetail() override { return _reducedDetail; }
};

bool Universe::useShaders() { return false; }

// A C++ subclass, as OOSingleTextureMaterial is: it denies specular, and asks
// the base class.
class TestSubMaterial : public oo::BasicMaterial {
 public:
  bool permitSpecular() override { return false; }
  bool doApply() override {
    doApplies++;
    return oo::BasicMaterial::doApply();
  }

  int doApplies = 0;
};

namespace {

struct Components {
  GLfloat v[4];
};

bool Same(const GLfloat* a, std::initializer_list<GLfloat> b) {
  const GLfloat* e = b.begin();
  for (int i = 0; i < 4; i++) {
    if (std::fabs(a[i] - e[i]) > 1e-5f) return false;
  }
  return true;
}

Components Diffuse(oo::BasicMaterial* m) {
  Components c = {};
  m->getDiffuseComponents(c.v);
  return c;
}
Components Ambient(oo::BasicMaterial* m) {
  Components c = {};
  m->getAmbientComponents(c.v);
  return c;
}
Components Specular(oo::BasicMaterial* m) {
  Components c = {};
  m->getSpecularComponents(c.v);
  return c;
}
Components Emission(oo::BasicMaterial* m) {
  Components c = {};
  m->getEmissionComponents(c.v);
  return c;
}

bool SameColor(const std::shared_ptr<oo::Color>& color,
               std::initializer_list<GLfloat> b) {
  if (color == nullptr) return false;
  const GLfloat v[4] = {color->redComponent(), color->greenComponent(),
                        color->blueComponent(), color->alphaComponent()};
  return Same(v, b);
}

Components GLMaterial(GLenum which) {
  Components c = {};
  glGetMaterialfv(GL_FRONT, which, c.v);
  return c;
}

GLfloat GLShininess() {
  GLfloat value = -1;
  glGetMaterialfv(GL_FRONT, GL_SHININESS, &value);
  return value;
}

oo::PList Config(oo::PList::Dict dict) { return oo::PList(std::move(dict)); }

std::shared_ptr<oo::BasicMaterial> Named(const char* name) {
  return oo::BasicMaterial::materialWithName(std::string(name));
}

std::shared_ptr<TestSubMaterial> NamedSub(
    const std::optional<std::string>& name,
    const oo::PList& configuration = oo::PList()) {
  std::shared_ptr<TestSubMaterial> result = oo::makeShared<TestSubMaterial>();
  result->initWithName(name, configuration);
  return result;
}

}  // namespace

TEST(OOBasicMaterial, defaults) {
  {
    const std::shared_ptr<oo::BasicMaterial> m = Named("Hull");
    EXPECT_TRUE(m->name() == std::optional<std::string>("Hull"));
    EXPECT_TRUE(Same(Diffuse(m.get()).v, {1, 1, 1, 1}));
    EXPECT_TRUE(Same(Ambient(m.get()).v, {1, 1, 1, 1}));
    EXPECT_TRUE(Same(Specular(m.get()).v, {0, 0, 0, 1}));
    EXPECT_TRUE(Same(Emission(m.get()).v, {0, 0, 0, 1}));
    EXPECT_TRUE(m->shininess() == 0);
    EXPECT_TRUE(m->permitSpecular());  // no universe: not reduced detail
    EXPECT_TRUE(m->isFinishedLoading() && !m->wantsNormalsAsTextureCoordinates());
#ifndef NDEBUG
    EXPECT_TRUE(m->allTextures().empty());
#endif
    EXPECT_TRUE(
        !oo::BasicMaterial::materialWithName(std::nullopt)->name().has_value());
  }
}

TEST(OOBasicMaterial, emptyConfiguration) {
  {
    // A null configuration is an empty one: the specifier's defaults apply
    // (exponent 10, 0.2 white).
    const std::shared_ptr<oo::BasicMaterial> m =
        oo::BasicMaterial::materialWithName(std::string("Plain"), oo::PList());
    EXPECT_TRUE(m->name() == std::optional<std::string>("Plain"));
    EXPECT_TRUE(Same(Diffuse(m.get()).v, {1, 1, 1, 1}));
    EXPECT_TRUE(Same(Ambient(m.get()).v, {1, 1, 1, 1}));
    EXPECT_TRUE(Same(Specular(m.get()).v, {0.2f, 0.2f, 0.2f, 1}));
    EXPECT_TRUE(Same(Emission(m.get()).v, {0, 0, 0, 1}));
    EXPECT_TRUE(m->shininess() == 10);
  }
}

TEST(OOBasicMaterial, configuration) {
  {
    std::shared_ptr<oo::BasicMaterial> m = oo::BasicMaterial::materialWithName(
        std::string("Painted"),
        Config({
            {"diffuse_color", oo::PList("redColor")},
            {"emission_color", oo::PList("greenColor")},
            {"specular_color", oo::PList("blueColor")},
            {"specular_exponent", oo::PList(std::int64_t(200))},
        }));
    EXPECT_TRUE(Same(Diffuse(m.get()).v, {1, 0, 0, 1}));
    EXPECT_TRUE(Same(Ambient(m.get()).v,
                  {1, 0, 0, 1}));  // no ambient: the diffuse colour
    EXPECT_TRUE(Same(Emission(m.get()).v, {0, 1, 0, 1}));
    EXPECT_TRUE(Same(Specular(m.get()).v, {0, 0, 1, 1}));
    EXPECT_TRUE(m->shininess() == 128);  // clamped

    // Legacy names; white diffuse and black emission count as none; exponent 0:
    // no specular.
    m = oo::BasicMaterial::materialWithName(
        std::string("Legacy"), Config({
                                   {"diffuse", oo::PList("whiteColor")},
                                   {"ambient", oo::PList("yellowColor")},
                                   {"emission", oo::PList("blackColor")},
                                   {"shininess", oo::PList(std::int64_t(0))},
                                   {"specular", oo::PList("blueColor")},
                               }));
    EXPECT_TRUE(Same(Diffuse(m.get()).v, {1, 1, 1, 1}));
    EXPECT_TRUE(Same(Ambient(m.get()).v, {1, 1, 0, 1}));
    EXPECT_TRUE(Same(Emission(m.get()).v, {0, 0, 0, 1}));
    EXPECT_TRUE(Same(Specular(m.get()).v, {0, 0, 0, 1}));
    EXPECT_TRUE(m->shininess() == 0);
  }
}

TEST(OOBasicMaterial, permitSpecular) {
  {
    const oo::PList config =
        Config({{"specular_exponent", oo::PList(std::int64_t(20))}});

    // Reduced detail denies specular: the exponent and colour are not applied.
    TestUniverse* universe = new TestUniverse();
    universe->_reducedDetail = true;
    gSharedUniverse = universe;
    const std::shared_ptr<oo::BasicMaterial> m =
        oo::BasicMaterial::materialWithName(std::string("Reduced"), config);
    EXPECT_TRUE(!m->permitSpecular());
    EXPECT_TRUE(m->shininess() == 0 && Same(Specular(m.get()).v, {0, 0, 0, 1}));
    universe->_reducedDetail = false;
    EXPECT_TRUE(m->permitSpecular());
    gSharedUniverse = nullptr;

    // A subclass's override is asked while the superclass initialises.
    const std::shared_ptr<TestSubMaterial> sub = NamedSub(std::string("Sub"), config);
    EXPECT_TRUE(sub->shininess() == 0 &&
             Same(Specular(sub.get()).v, {0, 0, 0, 1}));
    EXPECT_TRUE(sub->name() == std::optional<std::string>("Sub"));
    EXPECT_TRUE(Same(Diffuse(sub.get()).v, {1, 1, 1, 1}));
  }
}

TEST(OOBasicMaterial, accessors) {
  {
    const std::shared_ptr<oo::BasicMaterial> mRef = Named("Accessors");
    oo::BasicMaterial* m = mRef.get();

    m->setDiffuseRed(0.1f, 0.2f, 0.3f, 0.4f);
    EXPECT_TRUE(SameColor(m->diffuseColor(), {0.1f, 0.2f, 0.3f, 0.4f}));
    m->setDiffuseColor(nullptr);  // null: unchanged
    EXPECT_TRUE(Same(Diffuse(m).v, {0.1f, 0.2f, 0.3f, 0.4f}));
    m->setDiffuseColor(oo::Color::redColor().get());
    EXPECT_TRUE(Same(Diffuse(m).v, {1, 0, 0, 1}));

    m->setAmbientAndDiffuseColor(oo::Color::greenColor().get());
    EXPECT_TRUE(Same(Diffuse(m).v, {0, 1, 0, 1}) &&
             Same(Ambient(m).v, {0, 1, 0, 1}));
    m->setAmbientColor(oo::Color::blueColor().get());
    EXPECT_TRUE(SameColor(m->ambientColor(), {0, 0, 1, 1}));
    m->setAmbientColor(nullptr);
    EXPECT_TRUE(Same(Ambient(m).v, {0, 0, 1, 1}));

    m->setSpecularColor(oo::Color::yellowColor().get());
    EXPECT_TRUE(SameColor(m->specularColor(), {1, 1, 0, 1}));
    m->setSpecularColor(nullptr);
    EXPECT_TRUE(Same(Specular(m).v, {1, 1, 0, 1}));
    m->setEmissionColor(oo::Color::magentaColor().get());
    EXPECT_TRUE(SameColor(m->emmisionColor(), {1, 0, 1, 1}));
    m->setEmissionColor(nullptr);
    EXPECT_TRUE(Same(Emission(m).v, {1, 0, 1, 1}));

    const GLfloat c[4] = {0.5f, 0.25f, 0.125f, 1};
    m->setDiffuseComponents(c);
    m->setSpecularComponents(c);
    m->setEmissionComponents(c);
    m->setAmbientComponents(c);
    EXPECT_TRUE(Same(Diffuse(m).v, {0.5f, 0.25f, 0.125f, 1}) &&
             Same(Specular(m).v, {0.5f, 0.25f, 0.125f, 1}));
    EXPECT_TRUE(Same(Emission(m).v, {0.5f, 0.25f, 0.125f, 1}) &&
             Same(Ambient(m).v, {0.5f, 0.25f, 0.125f, 1}));
    const GLfloat d[4] = {0, 0.5f, 0, 0.5f};
    m->setAmbientAndDiffuseComponents(d);
    EXPECT_TRUE(Same(Diffuse(m).v, {0, 0.5f, 0, 0.5f}) &&
             Same(Ambient(m).v, {0, 0.5f, 0, 0.5f}));

    m->setAmbientAndDiffuseRed(1, 0, 1, 1);
    EXPECT_TRUE(Same(Diffuse(m).v, {1, 0, 1, 1}) &&
             Same(Ambient(m).v, {1, 0, 1, 1}));
    m->setAmbientRed(0.25f, 0.25f, 0.25f, 1);
    m->setSpecularRed(0.75f, 0.75f, 0.75f, 1);
    m->setEmissionRed(0.5f, 0, 0, 0);
    EXPECT_TRUE(Same(Ambient(m).v, {0.25f, 0.25f, 0.25f, 1}) &&
             Same(Specular(m).v, {0.75f, 0.75f, 0.75f, 1}));
    EXPECT_TRUE(Same(Emission(m).v, {0.5f, 0, 0, 0}));

    m->setShininess(5);
    EXPECT_TRUE(m->shininess() == 5);
    m->setShininess(200);
    EXPECT_TRUE(m->shininess() == 128);
  }
}

TEST(OOBasicMaterial, doApplySetsTheGLMaterial) {
  if (!OOTestGLContext()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    const std::shared_ptr<oo::BasicMaterial> m = Named("Applied");
    m->setDiffuseRed(0.1f, 0.2f, 0.3f, 0.4f);
    m->setAmbientRed(0.5f, 0.6f, 0.7f, 0.8f);
    m->setSpecularRed(0.9f, 0.8f, 0.7f, 0.6f);
    m->setEmissionRed(0.4f, 0.3f, 0.2f, 0.1f);
    m->setShininess(64);

    const int applyNones = gTextureApplyNones;
    EXPECT_TRUE(m->doApply());
    EXPECT_TRUE(gTextureApplyNones ==
             applyNones + 1);  // exactly an OOBasicMaterial: no texture
    EXPECT_TRUE(Same(GLMaterial(GL_DIFFUSE).v, {0.1f, 0.2f, 0.3f, 0.4f}));
    EXPECT_TRUE(Same(GLMaterial(GL_AMBIENT).v, {0.5f, 0.6f, 0.7f, 0.8f}));
    EXPECT_TRUE(Same(GLMaterial(GL_SPECULAR).v, {0.9f, 0.8f, 0.7f, 0.6f}));
    EXPECT_TRUE(Same(GLMaterial(GL_EMISSION).v, {0.4f, 0.3f, 0.2f, 0.1f}));
    EXPECT_TRUE(GLShininess() == 64);

    // A subclass's base-class doApply() sets the same state, and leaves the
    // texture to the subclass.
    const std::shared_ptr<TestSubMaterial> sub = NamedSub(std::string("Sub"));
    sub->setShininess(3);
    EXPECT_TRUE(sub->doApply() && sub->doApplies == 1);
    EXPECT_TRUE(gTextureApplyNones == applyNones + 1);
    EXPECT_TRUE(Same(GLMaterial(GL_DIFFUSE).v, {1, 1, 1, 1}) &&
             GLShininess() == 3);
    EXPECT_TRUE(glGetError() == GL_NO_ERROR);
  }
}

TEST(OOBasicMaterial, unapplyFallsBackToTheDefaultMaterial) {
  if (!OOTestGLContext()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    const std::shared_ptr<oo::BasicMaterial> m = Named("Current");
    m->setDiffuseRed(0.1f, 0.2f, 0.3f, 0.4f);
    m->setSpecularRed(0.9f, 0.8f, 0.7f, 0.6f);
    m->setShininess(64);
    m->doApply();

    // Followed by another basic material (or a subclass): that one sets its own
    // state.
    const int applyNones = gTextureApplyNones;
    m->unapplyWithNext(Named("Next").get());
    m->unapplyWithNext(NamedSub(std::nullopt).get());
    EXPECT_TRUE(gTextureApplyNones == applyNones);
    EXPECT_TRUE(Same(GLMaterial(GL_DIFFUSE).v, {0.1f, 0.2f, 0.3f, 0.4f}) &&
             GLShininess() == 64);

    // Followed by nothing, or by a material that is not a basic one: the
    // default material's state.
    m->unapplyWithNext(nullptr);
    EXPECT_TRUE(gTextureApplyNones == applyNones + 1);
    EXPECT_TRUE(Same(GLMaterial(GL_DIFFUSE).v, {1, 1, 1, 1}) &&
             Same(GLMaterial(GL_AMBIENT).v, {1, 1, 1, 1}));
    EXPECT_TRUE(Same(GLMaterial(GL_SPECULAR).v, {0, 0, 0, 1}) &&
             Same(GLMaterial(GL_EMISSION).v, {0, 0, 0, 1}));
    EXPECT_TRUE(GLShininess() == 0);

    m->doApply();
    const std::shared_ptr<oo::Material> plainMaterial = oo::makeShared<oo::Material>();
    m->unapplyWithNext(plainMaterial.get());
    EXPECT_TRUE(gTextureApplyNones == applyNones + 3);
    EXPECT_TRUE(Same(GLMaterial(GL_DIFFUSE).v, {1, 1, 1, 1}) &&
             GLShininess() == 0);
  }
}

TEST(OOBasicMaterial, applyAndCurrent) {
  if (!OOTestGLContext()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    const std::shared_ptr<oo::BasicMaterial> a = Named("A");
    const std::shared_ptr<TestSubMaterial> b = NamedSub(std::string("B"));
    a->apply();
    EXPECT_TRUE(oo::Material::current().get() == a.get());
    b->apply();
    EXPECT_TRUE(oo::Material::current().get() == b.get() && b->doApplies == 1);
    oo::Material::applyNone();
    EXPECT_TRUE(oo::Material::current() == nullptr);
  }
}

TEST(OOBasicMaterial, description) {
  {
    const std::shared_ptr<oo::BasicMaterial> m = Named("Hull");
    const std::string text = m->description();
    EXPECT_TRUE(text.starts_with("<OOBasicMaterial 0x"));
    EXPECT_TRUE(text.ends_with(">{\"Hull\"}"));
    const std::shared_ptr<TestSubMaterial> sub = NamedSub(std::string("Sub"));
    const std::string subText = sub->description();
    EXPECT_TRUE(subText.starts_with("<TestSubMaterial 0x") &&
             subText.ends_with(">{\"Sub\"}"));
  }
}

TEST(OOBasicMaterial, cxxAPI) {
  {
    const std::shared_ptr<oo::BasicMaterial> m =
        oo::BasicMaterial::materialWithName(std::string("Cxx"));
    EXPECT_TRUE(m->name() == std::optional<std::string>("Cxx") &&
             m->shininess() == 0);
    GLfloat c[4] = {};
    m->getDiffuseComponents(c);
    EXPECT_TRUE(Same(c, {1, 1, 1, 1}));
    m->getSpecularComponents(c);
    EXPECT_TRUE(Same(c, {0, 0, 0, 1}));
    m->setShininess(200);
    EXPECT_TRUE(m->shininess() == 128);
    m->setAmbientAndDiffuseColor(oo::Color::redColor().get());
    m->setDiffuseColor(nullptr);
    EXPECT_TRUE(m->diffuseColor()->redComponent() == 1 &&
             m->ambientColor()->greenComponent() == 0);
    EXPECT_TRUE(m->descriptionComponents() ==
             std::optional<std::string>("\"Cxx\""));
    EXPECT_TRUE(m->permitSpecular() && m->isFinishedLoading());

    const std::shared_ptr<oo::BasicMaterial> configured =
        oo::BasicMaterial::materialWithName(
            std::nullopt,
            Config({
                {"diffuse_color", oo::PList("blueColor")},
                {"specular_exponent", oo::PList(std::int64_t(20))},
            }));
    EXPECT_TRUE(!configured->name().has_value());
    configured->getAmbientComponents(c);
    EXPECT_TRUE(Same(c, {0, 0, 1, 1}) && configured->shininess() == 20);
    configured->getSpecularComponents(c);
    EXPECT_TRUE(Same(c, {0.2f, 0.2f, 0.2f, 1}));

    // The C++ subclass's override is asked by the initialiser.
    const std::shared_ptr<TestSubMaterial> denying =
        NamedSub(std::string("Denying"),
                 Config({{"specular_exponent", oo::PList(std::int64_t(20))}}));
    EXPECT_TRUE(denying->shininess() == 0 &&
             denying->name() == std::optional<std::string>("Denying"));
  }
}

// The name of a C++ material.
TEST(OOBasicMaterial, nameOfACxxMaterial) {
  {
    const std::shared_ptr<oo::BasicMaterial> m =
        oo::BasicMaterial::materialWithName(std::string("Cxx"));
    EXPECT_TRUE(m->name() == std::optional<std::string>("Cxx"));
  }
}
