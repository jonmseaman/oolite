/*	test_OOMultiTextureMaterial.mm
        Unit tests for OOMultiTextureMaterial
   (src/Core/Materials/OOMultiTextureMaterial.h), a subclass of OOBasicMaterial.

        A basic material with a diffuse map and an emission map on two texture
   units, combined by the fixed-function texture combiners. This pins, through
   the API its caller (the convenience creators) uses, what it computes: which
   maps each configuration asks for (a plain emission map, or one baked by
   OOCombinedEmissionMapGenerator from what else is configured), the emission
   colour kept from the basic material when a map replaces it, the units used,
   -apply (the maps, the combiner state, texture unit 0 active after),
   -unapplyWithNext: (the units the next material does not use are cleared), the
   loading question passed to the maps, and the description. The GL context is a
   hidden window's; the extension manager runs with its collaborators stubbed as
   in test_OOOpenGLExtensionManager. The cases ask the C++ class
   (materialWithName(), a held Ref); crossesAsTheBasicMaterialFacade pins what
   the root sees: the nearest base, OOBasicMaterial's. Run: bash
   tools/check-core-tests.sh
*/

#include <cstdlib>
#include <string>
#include <vector>

#include "CombinedEmissionMapGenerator.h"
#include "MultiTextureMaterial.h"
#include "OpenGLExtensionManager.h"
#include "RegExpMatcher.h"
#include "Texture.h"
#include "oo_gl_test_context.hpp"
#include "oo_test.hpp"
#include "oofnd/String.hpp"

// --- Stubs
// --------------------------------------------------------------------------------------

// OOLogging.mm's (it would bring the resource manager into the link).
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


// The extension manager's collaborators, as test_OOOpenGLExtensionManager stubs
// them.
#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
std::vector<std::string> ResourceManager::paths() { return {}; }
oo::PList ResourceManager::dictionaryFromFilesNamed(
    const std::string& /*fileName*/,
    const std::optional<std::string>& /*folderName*/, bool /*mergeFiles*/) {
  return oo::PList(oo::PList::Dict{});
}

// OORegExpMatcher: the stub answers no match.
std::shared_ptr<oo::RegExpMatcher> oo::RegExpMatcher::regExpMatcher() {
  return oo::makeShared<oo::RegExpMatcher>();
}
bool oo::RegExpMatcher::string(const std::string&, const std::string&) {
  return false;
}
namespace oo {

RegExpMatcher::~RegExpMatcher() {}

ShaderSetting ShaderSettingFromString(const std::string& string) {
  return string == "SHADERS_FULL" ? ShaderSetting::kFull : ShaderSetting::kNotSupported;
}

}  // namespace oo


// A specifier as text: its string, or "-".
static std::string Str(const oo::PList& plist) {
  const std::string* string = plist.getIf<std::string>();
  return string != nullptr ? *string : "-";
}

/*	OOTexture: a stand-in recording what the material asks. A texture's
   configuration is the specifier it was made from, or "generated".
*/
// OOTextureFlags::kExtraShrink, spelled out as before.
static const uint32_t kExtraShrink = 0x000020UL;

static int gTextureApplyNones = 0;
static int gTexturesDeallocated = 0;
static std::vector<std::pair<oo::PList, uint32_t>>
    gTextureConfigurations;  // (specifier, extra options)

// The stand-in is C++: the root's static
// members the material calls, TestTexture (what they answer), and the root's
// own members; the short description is every texture's here (they are all
// TestTextures).
class TestTexture final : public oo::Texture {
 public:
  ~TestTexture() override { gTexturesDeallocated++; }
  void apply() override { _applies++; }
  void ensureFinishedLoading() override { _ensures++; }

  oo::PList _configuration;
  int _applies = 0;
  int _ensures = 0;
};

static TestTexture* AsTest(const std::shared_ptr<oo::Texture>& texture) {
  return static_cast<TestTexture*>(texture.get());
}

void oo::Texture::applyNone() { gTextureApplyNones++; }

std::shared_ptr<oo::Texture> oo::Texture::textureWithConfiguration(
    const oo::PList& configuration) {
  return textureWithConfiguration(configuration, oo::TextureFlags::kNone);
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithConfiguration(
    const oo::PList& configuration, oo::TextureFlags extraOptions) {
  gTextureConfigurations.emplace_back(configuration, oo::ToUnderlying(extraOptions));
  if (configuration.isNull()) return nullptr;
  std::shared_ptr<TestTexture> texture = oo::makeShared<TestTexture>();
  texture->_configuration = configuration;
  return texture;
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithGenerator(
    ::oo::TextureGenerator* generator) {
  if (generator == nullptr) return nullptr;
  std::shared_ptr<TestTexture> texture = oo::makeShared<TestTexture>();
  texture->_configuration = oo::PList("generated");
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
std::string oo::Texture::className() const { return "OOTexture"; }
void oo::Texture::forceRebind() {}
std::string oo::Texture::shortDescription() const {
  return "tex:" + Str(static_cast<const TestTexture*>(this)->_configuration);
}

/*	OOCombinedEmissionMapGenerator: records which factory was called and
   with what. It answers null when it would have nothing to bake (no emission
   map and no illumination map), as the material's callers never see a generated
   map for none. It is C++: the stand-in defines the factories, and its own and
   its bases' other members as no-ops (no texture loader is linked). The
   material hands the C++ generator to the OOTexture stand-in, which only asks
   whether it is nil.
*/
static std::vector<std::string> gGenerators;

oo::TextureLoader::TextureLoader() {}
namespace oo {

TextureLoader::~TextureLoader() {}

}  // namespace oo

std::optional<oo::TextureLoader::Result> oo::TextureLoader::getResult() {
  return std::nullopt;
}
std::optional<std::string> oo::TextureLoader::cacheKey() { return std::nullopt; }
std::optional<std::string> oo::TextureLoader::descriptionComponents() const {
  return std::nullopt;
}
std::optional<std::string> oo::TextureLoader::shortDescriptionComponents() const {
  return std::nullopt;
}
void oo::TextureLoader::loadTexture() {}
// The loader is the work manager's task (its vtable): never reached.
void oo::TextureLoader::performAsyncTask() { std::abort(); }
void oo::TextureLoader::completeAsyncTask() { std::abort(); }
oo::TextureFlags oo::TextureGenerator::textureOptions() { return oo::TextureFlags::kNone; }
GLfloat oo::TextureGenerator::anisotropy() { return 0; }
GLfloat oo::TextureGenerator::lodBias() { return 0; }
std::optional<std::string> oo::TextureGenerator::cacheKey() {
  return std::nullopt;
}
bool oo::TextureGenerator::enqueue() { return false; }

std::shared_ptr<oo::CombinedEmissionMapGenerator>
oo::CombinedEmissionMapGenerator::generatorWithEmissionMapSpec(
    const oo::PList& emissionMapSpec, oo::Color* emissionColor,
    oo::Texture* diffuseMap, oo::Color* diffuseColor,
    const oo::PList& illuminationMapSpec, oo::Color* illuminationColor,
    const oo::PList& spec) {
  gGenerators.push_back(
      "emission:" + Str(emissionMapSpec) +
      " emissionColor:" + (emissionColor != nullptr ? "yes" : "no") +
      " diffuseMap:" + (diffuseMap != nullptr ? "yes" : "no") +
      " diffuseColor:" + (diffuseColor != nullptr ? "yes" : "no") +
      " illumination:" + Str(illuminationMapSpec) + " illuminationColor:" +
      (illuminationColor != nullptr ? "yes" : "no") + " options:" + Str(spec));
  if (emissionMapSpec.isNull() && illuminationMapSpec.isNull()) return {};
  return oo::makeShared<oo::CombinedEmissionMapGenerator>();
}

std::shared_ptr<oo::CombinedEmissionMapGenerator>
oo::CombinedEmissionMapGenerator::generatorWithEmissionAndIlluminationMapSpec(
    const oo::PList& emissionAndIlluminationMapSpec, oo::Texture* diffuseMap,
    oo::Color*, oo::Color*, oo::Color*, const oo::PList& spec) {
  gGenerators.push_back(
      "emissionAndIllumination:" + Str(emissionAndIlluminationMapSpec) +
      " diffuseMap:" + (diffuseMap != nullptr ? "yes" : "no") +
      " options:" + Str(spec));
  return oo::makeShared<oo::CombinedEmissionMapGenerator>();
}

namespace oo {

CombinedEmissionMapGenerator::~CombinedEmissionMapGenerator() {}

}  // namespace oo

#ifndef NDEBUG
std::optional<std::string>
oo::CombinedEmissionMapGenerator::descriptionComponents() const {
  return std::nullopt;
}
#endif
oo::TextureFlags oo::CombinedEmissionMapGenerator::textureOptions() { return oo::TextureFlags::kNone; }
GLfloat oo::CombinedEmissionMapGenerator::anisotropy() { return 0; }
GLfloat oo::CombinedEmissionMapGenerator::lodBias() { return 0; }
std::optional<std::string> oo::CombinedEmissionMapGenerator::cacheKey() {
  return std::nullopt;
}
void oo::CombinedEmissionMapGenerator::loadTexture() {}

// OOTexture.mm's specifier of a map value: the value itself, or the default
// name, or none.
namespace oo {

oo::PList TextureSpecFromObject(
    const oo::PList& object, const std::optional<std::string>& defaultName) {
  if (!object.isNull()) return object;
  return defaultName.has_value() ? oo::PList(*defaultName) : oo::PList();
}

}  // namespace oo


// UNIVERSE is gSharedUniverse: nil, so not reduced detail and no shaders.
// UNIVERSE stays nil here, so the code under test's guarded calls never reach
// it; the stand-in only links them.
class Universe {
 public:
  bool useShaders();
};
bool Universe::useShaders() { return false; }

Universe* gSharedUniverse = nullptr;

namespace {

bool CombinersSupported() {
  return OOTestGLContext() &&
         oo::OpenGLExtensionManager::sharedManager()->textureCombinersSupported();
}

std::shared_ptr<oo::MultiTextureMaterial> Make(const char* name, oo::PList::Dict config) {
  gTextureConfigurations.clear();
  gGenerators.clear();
  return oo::MultiTextureMaterial::materialWithName(std::string(name),
                                                  oo::PList(std::move(config)));
}

GLint TexEnvMode(GLenum unit) {
  GLint mode = 0;
  glActiveTextureARB(unit);
  glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &mode);
  glActiveTextureARB(GL_TEXTURE0_ARB);
  return mode;
}

}  // namespace

OO_TEST(combinersRequired) {
  if (!OOTestGLContext()) {
    OO_CHECK(false);
    return;
  }
  {
    // On a GL without texture combiners the initialiser answers nil; this
    // context has them.
    OO_CHECK(CombinersSupported());
    OO_CHECK(Make("Hull", {}) != nullptr);
  }
}

OO_TEST(mapsFromTheConfiguration) {
  if (!CombinersSupported()) {
    OO_CHECK(false);
    return;
  }
  {
    // Only a name: the diffuse map is named after it, and there is no emission
    // map to bake.
    std::shared_ptr<oo::MultiTextureMaterial> m = Make("hull.png", {});
    OO_CHECK(gTextureConfigurations.size() == 1 &&
             gTextureConfigurations[0].first == oo::PList("hull.png") &&
             gTextureConfigurations[0].second == 0);
    OO_CHECK(gGenerators ==
             std::vector<std::string>{
                 "emission:- emissionColor:no diffuseMap:yes diffuseColor:no "
                 "illumination:- illuminationColor:no options:-"});
    OO_CHECK(m->textureUnitCount() == 1 &&
             m->countOfTextureUnitsWithBaseCoordinates() == 1);

    // A plain emission map: loaded shrunk, not baked.
    m = Make("Hull", {{"diffuse_map", oo::PList("d.png")},
                      {"emission_map", oo::PList("e.png")}});
    OO_CHECK(gTextureConfigurations.size() == 2);
    OO_CHECK(gTextureConfigurations[0].first == oo::PList("d.png") &&
             gTextureConfigurations[0].second == 0);
    OO_CHECK(gTextureConfigurations[1].first == oo::PList("e.png") &&
             gTextureConfigurations[1].second == kExtraShrink);
    OO_CHECK(gGenerators.empty() && m->textureUnitCount() == 2);

    // An emission map with a modulating colour: baked, and the basic material's
    // emission colour is dropped from what it is given.
    m = Make("Hull", {
                         {"diffuse_map", oo::PList("d.png")},
                         {"emission_map", oo::PList("e.png")},
                         {"emission_modulate_color", oo::PList("redColor")},
                         {"emission_color", oo::PList("greenColor")},
                         {"diffuse_color", oo::PList("blueColor")},
                     });
    OO_CHECK(
        gGenerators ==
        std::vector<std::string>{
            "emission:e.png emissionColor:yes diffuseMap:yes diffuseColor:yes "
            "illumination:- illuminationColor:no options:e.png"});
    GLfloat c[4] = {};
    m->getEmissionComponents(c);
    OO_CHECK(c[0] == 0 && c[1] == 0 && c[2] == 0 && c[3] == 1);
    m->getDiffuseComponents(c);
    OO_CHECK(c[0] == 0 && c[2] == 1);
    OO_CHECK(m->textureUnitCount() == 2);

    // An illumination map alone: baked, with it as the options specifier; the
    // emission colour stays.
    m = Make("Hull", {{"illumination_map", oo::PList("i.png")},
                      {"emission_color", oo::PList("greenColor")}});
    OO_CHECK(gGenerators ==
             std::vector<std::string>{
                 "emission:- emissionColor:no diffuseMap:yes diffuseColor:no "
                 "illumination:i.png illuminationColor:no options:i.png"});
    m->getEmissionComponents(c);
    OO_CHECK(c[1] == 1);

    // The combined map: the other generator.
    m = Make("Hull", {{"emission_and_illumination_map", oo::PList("ei.png")}});
    OO_CHECK(
        gGenerators ==
        std::vector<std::string>{
            "emissionAndIllumination:ei.png diffuseMap:yes options:ei.png"});
    OO_CHECK(m->textureUnitCount() == 2);
  }
}

OO_TEST(texturesKeptAndAsked) {
  if (!CombinersSupported()) {
    OO_CHECK(false);
    return;
  }
  const int deallocated = gTexturesDeallocated;
  std::shared_ptr<oo::MultiTextureMaterial> m;
  {
    m = Make("Hull", {{"diffuse_map", oo::PList("d.png")},
                      {"emission_map", oo::PList("e.png")}});
  }
  OO_CHECK(gTexturesDeallocated == deallocated);
  {
    m->ensureFinishedLoading();
#ifndef NDEBUG
    const std::vector<std::shared_ptr<oo::Texture>> textures = m->allTextures();
    OO_CHECK(textures.size() == 2 && AsTest(textures[0])->_ensures == 1 &&
             AsTest(textures[1])->_ensures == 1);
    OO_CHECK(AsTest(textures[0])->_configuration == oo::PList("d.png") &&
             AsTest(textures[1])->_configuration == oo::PList("e.png"));
#endif
    const std::string text = m->description();
    OO_CHECK(text.starts_with("<OOMultiTextureMaterial 0x"));
    OO_CHECK(text.ends_with(
        ">{\"Hull\" - diffuse map: tex:d.png,emission map: tex:e.png}"));
  }
  m = nullptr;
  OO_CHECK(gTexturesDeallocated == deallocated + 2);

  {
    // No diffuse map (no name): only the emission map.
    gTextureConfigurations.clear();
    const std::shared_ptr<oo::MultiTextureMaterial> emissive =
        oo::MultiTextureMaterial::materialWithName(
            std::nullopt,
            oo::PList(oo::PList::Dict{{"emission_map", oo::PList("e.png")}}));
    OO_CHECK(emissive->textureUnitCount() == 1);
#ifndef NDEBUG
    OO_CHECK(emissive->allTextures().size() == 1);
#endif
    OO_CHECK(emissive->description().ends_with(
        ">{\"(null)\" - emission map: tex:e.png}"));
  }
}

OO_TEST(applyAndUnapply) {
  if (!CombinersSupported()) {
    OO_CHECK(false);
    return;
  }
  {
    const std::shared_ptr<oo::MultiTextureMaterial> two =
        Make("Two", {{"diffuse_map", oo::PList("d.png")},
                     {"emission_map", oo::PList("e.png")}});
    const std::shared_ptr<oo::MultiTextureMaterial> one = Make("one.png", {});
#ifndef NDEBUG
    TestTexture *diffuse = AsTest(two->allTextures()[0]),
                *emission = AsTest(two->allTextures()[1]);
#endif

    two->apply();
    OO_CHECK(oo::Material::current().get() == two.get());
#ifndef NDEBUG
    OO_CHECK(diffuse->_applies == 1 && emission->_applies == 1);
#endif
    OO_CHECK(TexEnvMode(GL_TEXTURE0_ARB) == GL_COMBINE_ARB &&
             TexEnvMode(GL_TEXTURE1_ARB) == GL_COMBINE_ARB);
    GLint active = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &active);
    OO_CHECK(active == GL_TEXTURE0_ARB);

    // The next material uses two units: nothing to clear, and it is a basic
    // material.
    int applyNones = gTextureApplyNones;
    two->unapplyWithNext(Make("Other", {{"diffuse_map", oo::PList("x.png")},
                                        {"emission_map", oo::PList("y.png")}})
                             .get());
    OO_CHECK(gTextureApplyNones == applyNones);

    // One unit: the second is cleared, back to modulate.
    two->unapplyWithNext(one.get());
    OO_CHECK(gTextureApplyNones == applyNones + 1 &&
             TexEnvMode(GL_TEXTURE1_ARB) == GL_MODULATE);

    // Nothing: the default material (no texture), then both units cleared.
    applyNones = gTextureApplyNones;
    oo::Material::applyNone();
    OO_CHECK(gTextureApplyNones == applyNones + 3 &&
             TexEnvMode(GL_TEXTURE0_ARB) == GL_MODULATE);
    OO_CHECK(oo::Material::current() == nullptr);
    OO_CHECK(glGetError() == GL_NO_ERROR);
  }
}

// --- The C++ class and the root
// -----------------------------------------

OO_TEST(cxxAPI) {
  if (!CombinersSupported()) {
    OO_CHECK(false);
    return;
  }
  {
    gTextureConfigurations.clear();
    const std::shared_ptr<oo::MultiTextureMaterial> m =
        oo::MultiTextureMaterial::materialWithName(
            std::string("Cxx"),
            oo::PList(oo::PList::Dict{{"diffuse_map", oo::PList("d.png")},
                                      {"emission_map", oo::PList("e.png")}}));
    OO_CHECK(m != nullptr && m->textureUnitCount() == 2 &&
             m->countOfTextureUnitsWithBaseCoordinates() == 2);
    OO_CHECK(m->descriptionComponents() ==
             std::optional<std::string>(
                 "\"Cxx\" - diffuse map: tex:d.png,emission map: tex:e.png"));
#ifndef NDEBUG
    OO_CHECK(m->allTextures().size() == 2);
#endif

    // apply() is virtual: through a root pointer, twice.
    oo::Material* root = m.get();
    root->apply();
#ifndef NDEBUG
    OO_CHECK(AsTest(m->allTextures()[0])->_applies == 1 &&
             AsTest(m->allTextures()[1])->_applies == 1);
#endif
    OO_CHECK(oo::Material::current().get() == m.get());
    root->apply();
#ifndef NDEBUG
    OO_CHECK(AsTest(m->allTextures()[0])->_applies == 2);
#endif
    const int applyNones = gTextureApplyNones;
    oo::Material::applyNone();
    OO_CHECK(gTextureApplyNones == applyNones + 3);
  }
}

// The description names the C++ class.
OO_TEST(descriptionNamesTheClass) {
  if (!CombinersSupported()) {
    OO_CHECK(false);
    return;
  }
  {
    const std::shared_ptr<oo::MultiTextureMaterial> m =
        oo::MultiTextureMaterial::materialWithName(std::string("Cxx"),
                                                 oo::PList());
    OO_CHECK(m->description().starts_with("<OOMultiTextureMaterial 0x"));
  }
}

OO_TEST_MAIN()
