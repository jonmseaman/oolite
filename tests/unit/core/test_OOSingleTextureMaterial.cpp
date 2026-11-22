/*	test_OOSingleTextureMaterial.mm
        Unit tests for OOSingleTextureMaterial
   (src/Core/Materials/OOSingleTextureMaterial.h).

        A basic material with one texture. This pins what it does: which
   texture specifier each initialiser asks for and when it fails (no name, no
   texture), the configuration reaching the basic material, the texture kept
   for the material's life, what -doApply / -unapplyWithNext: do to the texture
   and the GL material, the loading and cube-map questions it passes to the
   texture, and the description. The cases ask the C++ class (alloc/init is
   materialWithName(), nil is null, retain/release a held Ref);
   crossesAsTheBasicMaterialFacade pins what the root sees: the nearest
   base, OOBasicMaterial's. Run: bash tools/check-core-tests.sh
*/

#include <cstdlib>
#include <vector>

#include <gtest/gtest.h>

#include "SingleTextureMaterial.h"
#include "Texture.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/String.hpp"

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
void LogIndent(void) {}
void LogOutdent(void) {}

}  // namespace oo


/*	OOTexture (OOTexture.mm links the texture loaders and the game) is a
   stand-in that records what the material asks of it.
   +textureWithConfiguration: answers nil for a null specifier, or for the
   name "missing", as a texture that fails to load does.
*/
static int gTextureApplyNones = 0;
static int gTexturesDeallocated = 0;
static std::vector<oo::PList> gTextureConfigurations;

/*	The stand-in is C++: the root's static
	members the material calls, the root's own members (a texture needs them), and TestTexture, the
	texture the factory answers.
*/
class TestTexture final : public oo::Texture {
 public:
  ~TestTexture() override { gTexturesDeallocated++; }
  void apply() override { _applies++; }
  void ensureFinishedLoading() override { _ensures++; }
  bool isFinishedLoading() override { return _finished; }
  bool isCubeMap() override { return _cubeMap; }
  std::string description() const override { return "<TestTexture>"; }

  oo::PList _configuration;
  int _applies = 0;
  int _ensures = 0;
  bool _finished = false;
  bool _cubeMap = false;
};

void oo::Texture::applyNone() { gTextureApplyNones++; }

std::shared_ptr<oo::Texture> oo::Texture::textureWithConfiguration(
    const oo::PList& configuration) {
  gTextureConfigurations.push_back(configuration);
  if (configuration.isNull() || configuration == oo::PList("missing"))
    return nullptr;
  std::shared_ptr<TestTexture> texture = oo::makeShared<TestTexture>();
  texture->_configuration = configuration;
  return texture;
}

oo::Texture::Texture() {}
namespace oo {

Texture::~Texture() {}

}  // namespace oo

void oo::Texture::apply() {}
void oo::Texture::ensureFinishedLoading() {}
bool oo::Texture::isFinishedLoading() { return true; }
std::optional<std::string> oo::Texture::cacheKey() { return std::nullopt; }
NSSize oo::Texture::dimensions() { return NSSize{}; }
NSSize oo::Texture::originalDimensions() { return dimensions(); }
bool oo::Texture::isMipMapped() { return false; }
oo::PixMap oo::Texture::copyPixMapRepresentation() { return oo::PixMap{}; }
bool oo::Texture::isRectangleTexture() { return false; }
bool oo::Texture::isCubeMap() { return false; }
NSSize oo::Texture::texCoordsScale() { return NSSize{1.0, 1.0}; }
GLint oo::Texture::glTextureName() { return 0; }
#ifndef NDEBUG
std::optional<std::string> oo::Texture::name() { return std::nullopt; }
#endif
std::optional<std::string> oo::Texture::descriptionComponents() const {
  return std::nullopt;
}
std::optional<std::string> oo::Texture::shortDescriptionComponents() const {
  return std::nullopt;
}
std::string oo::Texture::description() const {
  return oo::str::format("<OOTexture %s>",
                         oo::str::pointerDescription(this).c_str());
}
std::string oo::Texture::shortDescription() const { return description(); }
std::string oo::Texture::className() const { return "OOTexture"; }
void oo::Texture::forceRebind() {}

/*	The texture specifier OOTexture.mm makes of a diffuse_map value,
   recorded: a dictionary naming what it was given (the game's own is pinned by
   the texture's tests).
*/
namespace oo {

oo::PList TextureSpecFromObject(
    const oo::PList& object, const std::optional<std::string>& defaultName) {
  return oo::PList(oo::PList::Dict{
      {"object", object.isNull() ? oo::PList("(null)") : object},
      {"defaultName", oo::PList(defaultName.value_or("(nil)"))},
  });
}

}  // namespace oo


// Link stubs, never reached here: OOOpenGL.mm's GL
// state dump.
oo::OpenGLExtensionManager* oo::OpenGLExtensionManager::sharedManager() {
  std::abort();
}
GLint oo::OpenGLExtensionManager::textureUnitCount() { std::abort(); }
// OOOpenGLStateManager.mm's (linked for the debug build's
// OOGLNoteCurrentFunction) multitexture entry points live in
// OOOpenGLExtensionManager.mm, which this test stubs; never called here.
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = nullptr;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = nullptr;

// UNIVERSE is gSharedUniverse: nil, so not reduced detail and no shaders.
// UNIVERSE stays nil here, so the code under test's guarded calls never reach
// it; the stand-in only links them (they were messages to nil).
class Universe {
 public:
  bool useShaders();
};
bool Universe::useShaders() { return false; }

Universe* gSharedUniverse = nullptr;

namespace {

TestTexture* MakeTexture() {
  // The factory's autoreleased result: the texture, kept until the current
  // autorelease scope drains.
  return static_cast<TestTexture*>(oo::autorelease(
      oo::Texture::textureWithConfiguration(oo::PList("hull.png"))));
}

std::shared_ptr<oo::SingleTextureMaterial> Make(
    const std::optional<std::string>& name, oo::Texture* texture,
    const oo::PList& configuration = oo::PList()) {
  return oo::SingleTextureMaterial::materialWithName(name, texture,
                                                   configuration);
}

// The material's description ("(null)" for none).
std::string Description(oo::Material* material) {
  return material != nullptr ? material->description() : "(null)";
}

bool Same(const GLfloat* a, std::initializer_list<GLfloat> b) {
  const GLfloat* e = b.begin();
  for (int i = 0; i < 4; i++) {
    if (a[i] != e[i]) return false;
  }
  return true;
}

}  // namespace

TEST(OOSingleTextureMaterial, initWithTexture) {
  {
    oo::AutoreleaseScope pool;
    TestTexture* texture = MakeTexture();
    const std::shared_ptr<oo::SingleTextureMaterial> m = Make(
        std::string("Hull"), texture,
        oo::PList(oo::PList::Dict{{"diffuse_color", oo::PList("redColor")}}));
    EXPECT_TRUE(m != nullptr && m->name() == std::optional<std::string>("Hull"));
    GLfloat c[4] = {};
    m->getDiffuseComponents(c);
    EXPECT_TRUE(
        Same(c, {1, 0, 0, 1}));  // the configuration reached the basic material
    EXPECT_TRUE(m->shininess() == 10);
#ifndef NDEBUG
    const std::vector<std::shared_ptr<oo::Texture>> textures = m->allTextures();
    EXPECT_TRUE(textures.size() == 1 && textures[0].get() == texture);
#endif

    // No name, or no texture: no material.
    EXPECT_TRUE(Make(std::nullopt, texture) == nullptr);
    EXPECT_TRUE(Make(std::string("Bare"), nullptr) == nullptr);
  }
}

TEST(OOSingleTextureMaterial, initWithConfiguration) {
  {
    oo::AutoreleaseScope pool;
    // A configuration: the texture is the diffuse_map's specifier, with the
    // name as its default.
    gTextureConfigurations.clear();
    std::shared_ptr<oo::SingleTextureMaterial> m =
        oo::SingleTextureMaterial::materialWithName(
            std::string("Hull"),
            oo::PList(oo::PList::Dict{
                {"diffuse_map", oo::PList("hull_diffuse.png")},
            }));
    EXPECT_TRUE(m != nullptr);
    EXPECT_TRUE(
        gTextureConfigurations.size() == 1 &&
        gTextureConfigurations[0] ==
            oo::PList(oo::PList::Dict{{"object", oo::PList("hull_diffuse.png")},
                                      {"defaultName", oo::PList("Hull")}}));

    // No diffuse_map: the specifier of nothing, still with the name as its
    // default.
    gTextureConfigurations.clear();
    m = oo::SingleTextureMaterial::materialWithName(std::string("Plain"),
                                                  oo::PList(oo::PList::Dict{}));
    EXPECT_TRUE(m != nullptr && gTextureConfigurations.size() == 1);
    EXPECT_TRUE(gTextureConfigurations[0] ==
             oo::PList(oo::PList::Dict{{"object", oo::PList("(null)")},
                                       {"defaultName", oo::PList("Plain")}}));

    // No configuration: the name is the texture's specifier.
    gTextureConfigurations.clear();
    m = oo::SingleTextureMaterial::materialWithName(std::string("named.png"),
                                                  oo::PList());
    EXPECT_TRUE(m != nullptr && gTextureConfigurations.size() == 1 &&
             gTextureConfigurations[0] == oo::PList("named.png"));
    EXPECT_TRUE(m->name() == std::optional<std::string>("named.png"));

    // Neither: a null specifier, no texture, no material. A texture that fails:
    // no material.
    gTextureConfigurations.clear();
    EXPECT_TRUE(oo::SingleTextureMaterial::materialWithName(std::nullopt,
                                                       oo::PList()) == nullptr);
    EXPECT_TRUE(gTextureConfigurations.size() == 1 &&
             gTextureConfigurations[0].isNull());
    EXPECT_TRUE(oo::SingleTextureMaterial::materialWithName(std::string("missing"),
                                                       oo::PList()) == nullptr);
  }
}

TEST(OOSingleTextureMaterial, textureKeptForTheMaterialsLife) {
  const int deallocated = gTexturesDeallocated;
  std::shared_ptr<oo::SingleTextureMaterial> m;
  {
    oo::AutoreleaseScope pool;
    m = Make(std::string("Kept"), MakeTexture());
  }
  EXPECT_TRUE(gTexturesDeallocated == deallocated);
  m = nullptr;
  EXPECT_TRUE(gTexturesDeallocated == deallocated + 1);
}

TEST(OOSingleTextureMaterial, texturesQuestions) {
  {
    oo::AutoreleaseScope pool;
    TestTexture* texture = MakeTexture();
    const std::shared_ptr<oo::SingleTextureMaterial> m =
        Make(std::string("Asks"), texture);
    EXPECT_TRUE(!m->isFinishedLoading() && !m->wantsNormalsAsTextureCoordinates());
    texture->_finished = true;
    texture->_cubeMap = true;
    EXPECT_TRUE(m->isFinishedLoading() && m->wantsNormalsAsTextureCoordinates());
    m->ensureFinishedLoading();
    EXPECT_TRUE(texture->_ensures == 1);
#if OO_MULTITEXTURE
    EXPECT_TRUE(m->countOfTextureUnitsWithBaseCoordinates() == 1);
#endif
    m->setBindingTarget(nullptr);
    EXPECT_TRUE(m->permitSpecular());
  }
}

TEST(OOSingleTextureMaterial, doApply) {
  if (!OOTestGLContext()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    oo::AutoreleaseScope pool;
    TestTexture* texture = MakeTexture();
    const std::shared_ptr<oo::SingleTextureMaterial> m =
        Make(std::string("Applied"), texture);
    m->setDiffuseRed(0.5f, 0.25f, 0.125f, 1);
    const int applyNones = gTextureApplyNones;
    EXPECT_TRUE(m->doApply());
    EXPECT_TRUE(texture->_applies == 1);
    EXPECT_TRUE(gTextureApplyNones == applyNones);  // not exactly a basic material
    GLfloat c[4] = {};
    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, c);
    EXPECT_TRUE(Same(c, {0.5f, 0.25f, 0.125f, 1}));

    m->apply();
    EXPECT_TRUE(oo::Material::current().get() == m.get() && texture->_applies == 2);
    oo::Material::applyNone();
    EXPECT_TRUE(oo::Material::current() == nullptr);
  }
}

TEST(OOSingleTextureMaterial, unapplyWithNext) {
  if (!OOTestGLContext()) {
    EXPECT_TRUE(false);
    return;
  }
  {
    oo::AutoreleaseScope pool;
    const std::shared_ptr<oo::SingleTextureMaterial> m =
        Make(std::string("Current"), MakeTexture());
    m->setDiffuseRed(0.5f, 0.25f, 0.125f, 1);
    m->doApply();
    GLfloat c[4] = {};

    // Another textured material: the texture stays, and so does the GL
    // material.
    int applyNones = gTextureApplyNones;
    m->unapplyWithNext(Make(std::string("Next"), MakeTexture()).get());
    EXPECT_TRUE(gTextureApplyNones == applyNones);
    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, c);
    EXPECT_TRUE(Same(c, {0.5f, 0.25f, 0.125f, 1}));

    // A basic material: no texture; it sets its own GL material.
    m->unapplyWithNext(
        oo::BasicMaterial::materialWithName(std::string("Basic")).get());
    EXPECT_TRUE(gTextureApplyNones == applyNones + 1);
    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, c);
    EXPECT_TRUE(Same(c, {0.5f, 0.25f, 0.125f, 1}));

    // Nothing: no texture, and the default material (which also has none).
    applyNones = gTextureApplyNones;
    m->unapplyWithNext(nullptr);
    EXPECT_TRUE(gTextureApplyNones == applyNones + 2);
    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, c);
    EXPECT_TRUE(Same(c, {1, 1, 1, 1}));
  }
}

TEST(OOSingleTextureMaterial, description) {
  {
    oo::AutoreleaseScope pool;
    const std::string text =
        Description(Make(std::string("Hull"), MakeTexture()).get());
    EXPECT_TRUE(text.starts_with("<OOSingleTextureMaterial 0x"));
    EXPECT_TRUE(text.ends_with(">{<TestTexture>}"));
  }
}

// --- The C++ class and the root
// -----------------------------------------

TEST(OOSingleTextureMaterial, cxxAPI) {
  {
    oo::AutoreleaseScope pool;
    TestTexture* texture = MakeTexture();
    const std::shared_ptr<oo::SingleTextureMaterial> m =
        oo::SingleTextureMaterial::materialWithName(std::string("Cxx"), texture,
                                                  oo::PList());
    EXPECT_TRUE(m != nullptr && m->name() == std::optional<std::string>("Cxx") &&
             m->shininess() == 10);
    EXPECT_TRUE(m->descriptionComponents() ==
             std::optional<std::string>("<TestTexture>"));
    EXPECT_TRUE(!m->isFinishedLoading() && !m->wantsNormalsAsTextureCoordinates());
    texture->_cubeMap = true;
    EXPECT_TRUE(m->wantsNormalsAsTextureCoordinates());
#ifndef NDEBUG
    EXPECT_TRUE(m->allTextures().size() == 1 &&
             m->allTextures()[0].get() == texture);
#endif

    EXPECT_TRUE(oo::SingleTextureMaterial::materialWithName(std::nullopt, texture,
                                                       oo::PList()) == nullptr);
    EXPECT_TRUE(oo::SingleTextureMaterial::materialWithName(std::string("Bare"), nullptr,
                                                       oo::PList()) == nullptr);
    EXPECT_TRUE(oo::SingleTextureMaterial::materialWithName(std::string("named.png"),
                                                       oo::PList()) != nullptr);
    EXPECT_TRUE(oo::SingleTextureMaterial::materialWithName(std::string("missing"),
                                                       oo::PList()) == nullptr);

    if (OOTestGLContext()) {
      const int applies = texture->_applies, applyNones = gTextureApplyNones;
      m->apply();
      EXPECT_TRUE(texture->_applies == applies + 1 &&
               oo::Material::current().get() == m.get());
      oo::Material::applyNone();  // told nothing comes next: no texture, then the
                                // default material
      EXPECT_TRUE(gTextureApplyNones == applyNones + 2);
    }
  }
}

// The name and description.
TEST(OOSingleTextureMaterial, nameAndDescription) {
  {
    oo::AutoreleaseScope pool;
    const std::shared_ptr<oo::SingleTextureMaterial> m =
        oo::SingleTextureMaterial::materialWithName(std::string("Cxx"),
                                                  MakeTexture(), oo::PList());
    EXPECT_TRUE(m->name() == std::optional<std::string>("Cxx"));
    const std::string text = m->description();
    EXPECT_TRUE(text.starts_with("<OOSingleTextureMaterial 0x") &&
             text.ends_with(">{<TestTexture>}"));
  }
}
