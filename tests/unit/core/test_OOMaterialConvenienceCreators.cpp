/*	test_OOMaterialConvenienceCreators.mm
        Unit tests for OOMaterialConvenienceCreators
   (src/Core/Materials/OOMaterialConvenienceCreators.h).

        The two class methods that pick and make a material for a mesh from its
   configuration: a shader material (named in the configuration, or synthesized
   from the configuration's maps when shaders are on and the configuration asks
   for more than fixed function), a multitexture material for emission and
   illumination maps, a single-texture material, or a basic one. This pins,
   through the C++ static members its caller (OOMesh) uses, which material each
   configuration gets under each universe setting, and the synthesized
   configuration itself as it is written to the cache, against the real material
   classes and GLSL programs compiled in a hidden GL context. The universe, the
   resource manager (shader files), the cache manager, the textures and the
   emission-map generator are stand-ins. Run: tests/run.sh unit
*/

#include <cstdlib>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>

#include <gtest/gtest.h>

#include "MyOpenGLView.h"
#include "BasicMaterial.h"
#include "CombinedEmissionMapGenerator.h"
#include "MaterialConvenienceCreators.h"
#include "MultiTextureMaterial.h"
#include "OpenGLExtensionManager.h"
#include "RegExpMatcher.h"
#include "ShaderMaterial.h"
#include "SingleTextureMaterial.h"
#include "Texture.h"
#include "oo_gl_test_context.hpp"
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
const char* const kLogFileNotFound = "files.notFound";

}  // namespace oo

uint32_t gDebugFlags = 0;

// Link stubs, never reached here: OOStringParsing.mm's parsers of a vector or
// quaternion written as a string (OOPListGameTypes.mm links them).
std::optional<Vector> ScanVectorFromString(const std::optional<std::string>&) {
  std::abort();
}
std::optional<HPVector> ScanHPVectorFromString(const std::optional<std::string>&) {
  std::abort();
}
std::optional<Quaternion> ScanQuaternionFromString(const std::optional<std::string>&) {
  std::abort();
}

// The binding whitelist, which the game implements elsewhere: everything.
namespace oo {

bool UniformBindingPermitted(const std::string& /*propertyName*/,
                               Entity* /*bindingTarget*/) {
  return true;
}

}  // namespace oo


// The resource manager: the extension manager's two class methods, and shader
// files from a table.
static std::map<std::string, std::string> gShaderFiles;

#include "ResourceManager.h"

// ResourceManager: C++ definitions of the statics the code under test calls,
// with the stand-in's answers.
std::vector<std::string> ResourceManager::paths() { return {}; }
oo::PList ResourceManager::dictionaryFromFilesNamed(
    const std::string& /*fileName*/,
    const std::optional<std::string>& /*folderName*/, bool /*mergeFiles*/) {
  return oo::PList(oo::PList::Dict{});
}
std::optional<std::string> ResourceManager::stringFromFilesNamed(
    const std::string& fileName,
    const std::optional<std::string>& /*folderName*/) {
  auto found = gShaderFiles.find(fileName);
  if (found == gShaderFiles.end()) return std::nullopt;
  return found->second;
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


// The cache manager: the synthesized configurations, by key, as they are
// written and read.
static std::map<std::string, oo::PList> gCache;
static std::vector<std::string> gCacheWrites;

#include "CacheManager.h"

// The cache manager: a C++ stand-in.
oo::CacheManager* oo::CacheManager::sharedCache() {
  static oo::CacheManager* shared = new oo::CacheManager;
  return shared;
}
namespace oo {

CacheManager::~CacheManager() {}

}  // namespace oo

oo::PList oo::CacheManager::pListForKey(const std::string& key,
                                      const std::string& cache) {
  auto found = gCache.find(cache + "|" + key);
  return found != gCache.end() ? found->second : oo::PList();
}
void oo::CacheManager::setPList(const oo::PList& value, const std::string& key,
                              const std::string& cache) {
  gCacheWrites.push_back(cache + "|" + key);
  gCache[cache + "|" + key] = value;
}

// The textures: any configuration but "missing" loads; a name with no
// configuration loads unless "missing". The stand-in is C++: the root's static
// members the creators call, TestTexture (what they answer), and the root's own
// members.
class TestTexture final : public oo::Texture {
 public:
  oo::PList _configuration;
};

void oo::Texture::applyNone() {}

std::shared_ptr<oo::Texture> oo::Texture::textureWithConfiguration(
    const oo::PList& configuration) {
  return textureWithConfiguration(configuration, oo::TextureFlags::kNone);
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithConfiguration(
    const oo::PList& configuration, oo::TextureFlags) {
  if (configuration.isNull() || configuration == oo::PList("missing"))
    return nullptr;
  std::shared_ptr<TestTexture> texture = oo::makeShared<TestTexture>();
  texture->_configuration = configuration;
  return texture;
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithName(
    const std::optional<std::string>& name, const std::optional<std::string>&) {
  if (!name.has_value() || *name == "missing") return nullptr;
  return textureWithConfiguration(oo::PList(*name));
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithGenerator(
    ::oo::TextureGenerator* generator) {
  return generator != nullptr ? textureWithConfiguration(oo::PList("generated"))
                              : nullptr;
}
std::shared_ptr<oo::Texture> oo::Texture::nullTexture() {
  return textureWithConfiguration(oo::PList("null"));
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

/*	The emission-map generator. The factories answer null for nothing to
   bake; its own and its bases' other members are no-ops (no texture loader is
   linked). The material hands the C++ generator to the OOTexture stand-in,
   which only asks whether it is nil.
*/
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
    const oo::PList& emissionMapSpec, oo::Color*, oo::Texture*, oo::Color*,
    const oo::PList& illuminationMapSpec, oo::Color*, const oo::PList&) {
  if (emissionMapSpec.isNull() && illuminationMapSpec.isNull()) return {};
  return oo::makeShared<oo::CombinedEmissionMapGenerator>();
}
std::shared_ptr<oo::CombinedEmissionMapGenerator>
oo::CombinedEmissionMapGenerator::generatorWithEmissionAndIlluminationMapSpec(
    const oo::PList&, oo::Texture*, oo::Color*, oo::Color*, oo::Color*,
    const oo::PList&) {
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


// The universe: the three settings the creators and the materials ask about.
class Universe;
Universe* gSharedUniverse = nullptr;

// The game view is C++ (MyOpenGLView). There is none here: its manager
// accessor, which the shader program's matrix uniforms would use, is never
// reached.
oo::OpenGLMatrixManager* MyOpenGLView::getOpenGLMatrixManager() {
  return matrixManager.get();
}

#include "UniverseStandIn.h"

// The universe as the code under test sees it: a stand-in for the C++ class,
// with the header's names and signatures. It answers virtual members, so it has
// the game's vtable (UniverseStandIn.h) and TestUniverse overrides them.
class Universe : public oo::Object {
 public:
  OO_UNIVERSE_STAND_IN_VIRTUAL_MEMBERS

  bool _useShaders;
  bool _reducedDetail;
  oo::GraphicsDetail _detailLevel;

  bool useShaders();
  oo::GraphicsDetail getDetailLevel();
};

class TestUniverse : public Universe {
 public:
  bool reducedDetail() override { return _reducedDetail; }
};

bool Universe::useShaders() { return _useShaders; }
oo::GraphicsDetail Universe::getDetailLevel() { return _detailLevel; }

namespace {

TestUniverse* gUniverse = nullptr;

const char* kVertex = "void main() { gl_Position = gl_Vertex; }\n";
const char* kFragment =
    "uniform sampler2D uDiffuseMap;\nvoid main() { gl_FragColor = "
    "texture2D(uDiffuseMap, vec2(0.0)); }\n";

bool SetUp(bool useShaders, oo::GraphicsDetail detail = oo::GraphicsDetail::kMinimum) {
  if (!OOTestGLContext()) return false;
  oo::OpenGLExtensionManager::sharedManager();
  if (gUniverse == nullptr) gUniverse = new TestUniverse();
  gUniverse->_useShaders = useShaders;
  gUniverse->_detailLevel = detail;
  gUniverse->_reducedDetail = false;
  gSharedUniverse = gUniverse;
  gShaderFiles = {
      {"oolite-tangent-space-vertex.vertex", kVertex},
      {"oolite-default-shader.fragment", kFragment},
      {"named.vertex", kVertex},
      {"named.fragment", kFragment},
  };
  gCache.clear();
  gCacheWrites.clear();
  return true;
}

std::shared_ptr<oo::Material> Make(
    const char* name, oo::PList::Dict config, bool smooth = false,
    const std::optional<std::string>& cacheKey = std::nullopt) {
  return oo::Material::materialWithName(
      name != nullptr ? std::optional<std::string>(name) : std::nullopt,
      cacheKey, oo::PList(std::move(config)), oo::PList(), nullptr, smooth);
}

// The material's C++ class, exactly.
template <typename T>
bool IsExactly(const std::shared_ptr<oo::Material>& material) {
  if (material == nullptr) return false;
  const oo::Material& object = *material.get();
  return typeid(object) == typeid(T);
}

}  // namespace

TEST(OOMaterialConvenienceCreators, withoutShaders) {
  if (!::SetUp(false)) {
    EXPECT_TRUE(false);
    return;
  }
  {
    // A diffuse map (named, or the material's own name): one texture.
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(Make("hull.png", {})));
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(
        Make("hull.png", {{"diffuse_map", oo::PList("d.png")}})));
    // With no name the single-texture initialiser fails (it needs one), so
    // basic.
    EXPECT_TRUE(IsExactly<oo::BasicMaterial>(
        Make(nullptr, {{"diffuse_map", oo::PList("d.png")}})));
    // No diffuse map: basic; one that fails to load: basic too.
    EXPECT_TRUE(IsExactly<oo::BasicMaterial>(Make(nullptr, {})));
    EXPECT_TRUE(IsExactly<oo::BasicMaterial>(Make("missing", {})));
    // Emission or illumination maps: multitexture, when the combiners are there
    // (they are).
    EXPECT_TRUE(IsExactly<oo::MultiTextureMaterial>(
        Make("hull.png", {{"emission_map", oo::PList("e.png")}})));
    EXPECT_TRUE(IsExactly<oo::MultiTextureMaterial>(
        Make("hull.png", {{"illumination_map", oo::PList("i.png")}})));
    EXPECT_TRUE(IsExactly<oo::MultiTextureMaterial>(Make(
        "hull.png", {{"emission_and_illumination_map", oo::PList("ei.png")}})));
    // A shader configuration without shaders: its fixed-function equivalent.
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(
        Make("hull.png", {{"vertex_shader", oo::PList("named")}})));
    EXPECT_TRUE(Make("Named", {})->name() == std::optional<std::string>("Named"));
  }
}

TEST(OOMaterialConvenienceCreators, namedShaders) {
  if (!::SetUp(true)) {
    EXPECT_TRUE(false);
    return;
  }
  {
    std::shared_ptr<oo::Material> m =
        Make("hull.png", {{"vertex_shader", oo::PList("named")},
                          {"fragment_shader", oo::PList("named")}});
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(m));
    EXPECT_TRUE(gCacheWrites.empty());

    // A shader that fails, at low detail and not smoothed: the fixed-function
    // material.
    m = Make("hull.png", {{"vertex_shader", oo::PList("absent")}});
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(m));
  }
}

TEST(OOMaterialConvenienceCreators, synthesizedShaders) {
  if (!::SetUp(true, oo::GraphicsDetail::kShaders)) {
    EXPECT_TRUE(false);
    return;
  }
  {
    // Full shader detail: the configuration is synthesized into a shader
    // material, and the synthesized configuration is cached under the cache
    // key, the name and the configuration.
    std::shared_ptr<oo::Material> m =
        Make("hull.png", {{"emission_map", oo::PList("e.png")}}, false,
             std::string("ship"));
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(m));
    EXPECT_TRUE(gCacheWrites.size() == 1 &&
             gCacheWrites[0].starts_with(
                 "synthesized shader materials|ship/hull.png/"));

    const oo::PList& config = gCache.begin()->second;
    EXPECT_TRUE(config.get<std::string>("_oo_is_synthesized_config") == "true");
    EXPECT_TRUE(config.get<std::string>("vertex_shader") ==
             "oolite-tangent-space-vertex.vertex");
    EXPECT_TRUE(config.get<std::string>("fragment_shader") ==
             "oolite-default-shader.fragment");
    EXPECT_TRUE(config.get<std::string>("diffuse_map") == "hull.png");
    const oo::PList* textures = config.find("textures");
    EXPECT_TRUE(textures != nullptr &&
             *textures == oo::PList(oo::PList::Array{oo::PList("hull.png"),
                                                     oo::PList("e.png")}));
    const oo::PList* uniforms = config.find("uniforms");
    EXPECT_TRUE(uniforms != nullptr);
    EXPECT_TRUE(uniforms->find("uDiffuseMap") != nullptr &&
             *uniforms->find("uDiffuseMap") ==
                 oo::PList(oo::PList::Dict{
                     {"type", oo::PList("texture")},
                     {"value", oo::PList::unsignedInteger(0)}}));
    EXPECT_TRUE(uniforms->find("uEmissionMap") != nullptr &&
             *uniforms->find("uEmissionMap") ==
                 oo::PList(oo::PList::Dict{
                     {"type", oo::PList("texture")},
                     {"value", oo::PList::unsignedInteger(1)}}));
    EXPECT_TRUE(uniforms->get<std::string>("uHullHeatLevel") == "hullHeatLevel" &&
             uniforms->get<std::string>("uTime") == "timeElapsedSinceSpawn");
    EXPECT_TRUE(uniforms->get<std::string>("uFogColor") == "fogUniform");
    const oo::PList* macros = config.find("_oo_synthesized_material_macros");
    EXPECT_TRUE(macros != nullptr &&
             macros->get<std::string>("OOSTD_DIFFUSE_MAP") == "1" &&
             macros->get<std::string>("OOSTD_EMISSION_MAP") == "1");
    EXPECT_TRUE(macros->get<std::string>("OOSTD_SPECULAR") ==
             "1");  // the default exponent (10) and colour

    // Asked again: read from the cache, not written.
    gCacheWrites.clear();
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(
        Make("hull.png", {{"emission_map", oo::PList("e.png")}}, false,
             std::string("ship"))));
    EXPECT_TRUE(gCacheWrites.empty());

    // No cache key: synthesized, not cached.
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(Make("hull.png", {})) &&
             gCacheWrites.empty());
  }
}

TEST(OOMaterialConvenienceCreators, synthesisTriggersAndLoops) {
  if (!::SetUp(true, oo::GraphicsDetail::kMinimum)) {
    EXPECT_TRUE(false);
    return;
  }
  {
    // Low detail: fixed function, unless smoothed or a map needs the shader.
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(Make("hull.png", {})));
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(Make("hull.png", {}, true)));
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(
        Make("hull.png", {{"normal_map", oo::PList("n.png")}})));
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(
        Make("hull.png", {{"specular_map", oo::PList("s.png")}})));

    /*	The synthesized shaders fail to build: no loop, and the fixed-function
       material of the synthesized configuration, which the inner call makes.
       That configuration keeps only the diffuse map outside its shader
       textures, so an emission map is dropped and the material is
       single-texture, not multitexture.
    */
    gShaderFiles.erase("oolite-default-shader.fragment");
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(Make("hull.png", {}, true)));
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(
        Make("hull.png", {{"emission_map", oo::PList("e.png")}}, true)));
    // Without the smoothing that forces the shader, at low detail, the emission
    // map still asks for the shader, and the result is the same.
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(
        Make("hull.png", {{"emission_map", oo::PList("e.png")}})));
  }
}

TEST(OOMaterialConvenienceCreators, fromDictionaries) {
  if (!::SetUp(true)) {
    EXPECT_TRUE(false);
    return;
  }
  {
    const oo::PList materials = oo::PList(oo::PList::Dict{
        {"hull.png",
         oo::PList(oo::PList::Dict{{"diffuse_map", oo::PList("m.png")}})}});
    const oo::PList shaders = oo::PList(oo::PList::Dict{
        {"hull.png",
         oo::PList(oo::PList::Dict{{"vertex_shader", oo::PList("named")},
                                   {"fragment_shader", oo::PList("named")}})}});

    auto make = [&](const char* name, const oo::PList& m, const oo::PList& s) {
      return oo::Material::materialWithName(std::string(name), std::nullopt, m, s,
                                          oo::PList(), nullptr, false);
    };

    // With shaders, the shaders dictionary's entry; without, the materials
    // dictionary's.
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(make("hull.png", materials, shaders)));
    gUniverse->_useShaders = false;
    std::shared_ptr<oo::Material> m = make("hull.png", materials, shaders);
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(m));
    // Neither: an empty configuration if the texture of that name loads, else
    // nothing.
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(
        make("other.png", materials, shaders)));
    EXPECT_TRUE(make("missing", materials, shaders) == nullptr);
    EXPECT_TRUE(make("missing", oo::PList(), oo::PList()) == nullptr);
  }
}

// --- The C++ static members
// ------------------------------------------------------------------------

TEST(OOMaterialConvenienceCreators, cxxStaticMembers) {
  if (!::SetUp(false)) {
    EXPECT_TRUE(false);
    return;
  }
  {
    EXPECT_TRUE(IsExactly<oo::SingleTextureMaterial>(Make("hull.png", {})));
    EXPECT_TRUE(IsExactly<oo::BasicMaterial>(Make(nullptr, {})));
    EXPECT_TRUE(IsExactly<oo::MultiTextureMaterial>(
        Make("hull.png", {{"emission_map", oo::PList("e.png")}})));
    EXPECT_TRUE(Make("Named", {})->name() == std::optional<std::string>("Named"));
  }

  if (!::SetUp(true, oo::GraphicsDetail::kShaders)) {
    EXPECT_TRUE(false);
    return;
  }
  {
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(
        Make("hull.png", {{"emission_map", oo::PList("e.png")}}, false,
             std::string("ship"))));
    EXPECT_TRUE(gCacheWrites.size() == 1);

    const oo::PList materials = oo::PList(oo::PList::Dict{
        {"hull.png",
         oo::PList(oo::PList::Dict{{"diffuse_map", oo::PList("m.png")}})}});
    const oo::PList shaders = oo::PList(oo::PList::Dict{
        {"hull.png",
         oo::PList(oo::PList::Dict{{"vertex_shader", oo::PList("named")},
                                   {"fragment_shader", oo::PList("named")}})}});
    EXPECT_TRUE(IsExactly<oo::ShaderMaterial>(oo::Material::materialWithName(
        std::string("hull.png"), std::nullopt, materials, shaders, oo::PList(),
        nullptr, false)));
    EXPECT_TRUE(oo::Material::materialWithName(std::string("missing"), std::nullopt,
                                          oo::PList(), oo::PList(), oo::PList(),
                                          nullptr, false) == nullptr);
  }
}
