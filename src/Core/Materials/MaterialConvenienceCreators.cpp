/*

OOMaterialConvenienceCreators.m


Copyright (C) 2007-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef USE_NEW_SHADER_SYNTHESIZER
#define USE_NEW_SHADER_SYNTHESIZER 0
#endif

#include "MaterialConvenienceCreators.h"

#include "Color.h"
#include "MaterialSpecifier.h"

#if USE_NEW_SHADER_SYNTHESIZER
#include "DefaultShaderSynthesizer.h"
#include "ResourceManager.h"
#endif

#include <cstddef>
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "CacheManager.h"
#include "DebugFlags.h"
#include "MultiTextureMaterial.h"
#include "OpenGLExtensionManager.h"
#include "ShaderMaterial.h"
#include "SingleTextureMaterial.h"
#include "Texture.h"
#include "Universe.h"
#include "oofnd/Exception.hpp"
#include "oofnd/objc/Assert.h"

#if !USE_NEW_SHADER_SYNTHESIZER
namespace {

struct MaterialSynthContext {
  oo::PList inConfig;
  oo::PList::Dict outConfig;
  std::size_t texturesUsed;
  std::size_t maxTextures;

  oo::PList::Dict macros;
  std::vector<oo::PList> textures;
  oo::PList::Dict uniforms;
};

void SetUniform(oo::PList::Dict& uniforms, const std::string& key,
                std::string_view type, const oo::PList& value);
void SetUniformFloat(MaterialSynthContext* context, const std::string& key,
                     float value);

/*	AddTexture(): add a texture to the configuration being synthesized.
 * specifier is added to the textures array.
 * uniformName is mapped to the appropriate texture unit in the uniforms
 * dictionary. If nonShaderKey is not nil, nonShaderKey (e.g. diffuse_map) is
 * set to specifier. If macroName is not nil, macroName is set to 1 in the
 * macros dictionary.
 */
void AddTexture(MaterialSynthContext* context, std::string_view uniformName,
                std::optional<std::string_view> nonShaderKey,
                std::optional<std::string_view> macroName,
                const oo::PList& specifier);

void AddColorIfAppropriate(MaterialSynthContext* context, oo::Color* color,
                           std::string_view key,
                           std::optional<std::string_view> macroName);
void AddMacroColorIfAppropriate(MaterialSynthContext* context, oo::Color* color,
                                std::string_view macroName);

void SynthDiffuse(MaterialSynthContext* context,
                  const std::optional<std::string>& name);
void SynthEmissionAndIllumination(MaterialSynthContext* context);
void SynthNormalMap(MaterialSynthContext* context);
void SynthSpecular(MaterialSynthContext* context);

}  // namespace
#endif

#if !USE_NEW_SHADER_SYNTHESIZER

oo::PList oo::Material::synthesizeMaterialDictionaryWithName(
    const std::optional<std::string>& name, const oo::PList& configuration,
    const oo::PList& macros) {
  oo::PList::Dict macrosCopy;
  if (const oo::PList::Dict* src = macros.getIf<oo::PList::Dict>()) {
    macrosCopy = *src;
  }

  MaterialSynthContext context = {
      .inConfig =
          configuration.isNull() ? oo::PList(oo::PList::Dict{}) : configuration,
      .outConfig = oo::PList::Dict{},
      .texturesUsed = 0,
      .maxTextures = static_cast<std::size_t>(
          oo::OpenGLExtensionManager::sharedManager()->textureImageUnitCount()),

      .macros = std::move(macrosCopy),
      .textures = {},
      .uniforms = {}};

  if ((UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false)) {
    context.maxTextures = 3;
  }

  //	Basic stuff.

  /*	Set up the various material attributes.
          Order is significant here, because it determines the order in which
          features will be dropped if we exceed the hardware's texture image
          unit limit.
  */
  SynthDiffuse(&context, name);
  SynthEmissionAndIllumination(&context);
  SynthNormalMap(&context);
  SynthSpecular(&context);

  if ((UNIVERSE != nullptr
           ? UNIVERSE->getDetailLevel()
           : oo::GraphicsDetail{}) >= oo::GraphicsDetail::kShaders) {
    //	Add uniforms required for hull heat glow.
    context.uniforms["uHullHeatLevel"] = oo::PList("hullHeatLevel");
    context.uniforms["uTime"] = oo::PList("timeElapsedSinceSpawn");
    context.uniforms["uFogColor"] = oo::PList("fogUniform");
  }

  //	Stuff in the general properties.
  context.outConfig["_oo_is_synthesized_config"] = oo::PList("true");
  context.outConfig["vertex_shader"] =
      oo::PList("oolite-tangent-space-vertex.vertex");
  context.outConfig["fragment_shader"] =
      oo::PList("oolite-default-shader.fragment");

  if (!context.textures.empty())
    context.outConfig["textures"] =
        oo::PList(oo::PList::Array(context.textures));
  if (!context.uniforms.empty())
    context.outConfig["uniforms"] = oo::PList(context.uniforms);
  if (!context.macros.empty())
    context.outConfig["_oo_synthesized_material_macros"] =
        oo::PList(context.macros);

  return oo::PList(std::move(context.outConfig));
}

std::shared_ptr<oo::Material> oo::Material::defaultShaderMaterialWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& cacheKey, const oo::PList& configuration,
    const oo::PList& macros, Entity* target) {
  ::oo::CacheManager* cache = nullptr;
  oo::PList synthesizedConfig;
  std::shared_ptr<oo::Material> result;

  // Avoid looping (can happen if shader fails to compile).
  if (configuration.find("_oo_is_synthesized_config") != nullptr) {
    OO_LOG("material.synthesize.loop", "Synthesis loop for material {}.",
           name ? *name : std::string("(null)"));
    return nullptr;
  }

  std::string cacheKeyStr;
  if (cacheKey.has_value()) {
    cache = oo::CacheManager::sharedCache();
    // configuration must be in cache key, as otherwise changes in
    // non-diffuse map can end up miscached
    cacheKeyStr = std::format("{}/{}/{}", *cacheKey, name.value_or(""),
                              oo::DescriptionOf(configuration));
    synthesizedConfig =
        cache->pListForKey(cacheKeyStr, "synthesized shader materials");
  }

  if (synthesizedConfig.isNull()) {
    synthesizedConfig = synthesizeMaterialDictionaryWithName(
        name,
        configuration.isNull() ? oo::PList(oo::PList::Dict{}) : configuration,
        macros);
    if (!synthesizedConfig.isNull() && cacheKey.has_value()) {
      cache->setPList(synthesizedConfig, cacheKeyStr,
                      "synthesized shader materials");
    }
  }

  if (!synthesizedConfig.isNull()) {
    oo::PList macrosPList;
    if (const oo::PList* found =
            synthesizedConfig.find("_oo_synthesized_material_macros")) {
      macrosPList = *found;
    }
    result = materialWithName(name, cacheKey, synthesizedConfig, macrosPList,
                              target, true);
  }

  return result;
}

#else

#ifndef NDEBUG
namespace {

// Read once, on first use (was +initialize, sent before the class's first
// message).
bool DumpShaderSource() {
  static const bool dump =
      oo::Defaults::standard().boolForKey("dump-synthesized-shaders");
  return dump;
}

}  // namespace
#endif

std::shared_ptr<oo::Material> oo::Material::defaultShaderMaterialWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& cacheKey, const oo::PList& configuration,
    const oo::PList& macros, Entity* target) {
  (void)macros;
  std::optional<oo::SynthesizedShader> synthesis = oo::SynthesizeMaterialShader(
      configuration, name,
      cacheKey /* FIXME: entity name for error reporting */);
  if (!synthesis.has_value()) return nullptr;
  std::string& vertexShaderSource = synthesis->vertexShader;
  std::string& fragmentShaderSource = synthesis->fragmentShader;
  const oo::PList& textureSpecList = synthesis->textureSpecs;
  const oo::PList& uniformSpecDict = synthesis->uniformSpecs;
  // A failed synthesis leaves the texture list null (and the shaders empty)
  // where it left all four nil. Mirror dictionaryWithObjectsAndKeys: nil-stop —
  // unsynthesized keeps only the is-synthesized flag.
  bool synthesized = !textureSpecList.isNull();
  oo::PList::Dict synthesizedConfigDict;
  synthesizedConfigDict[oo::kIsSynthesizedMaterialConfigurationKey] =
      oo::PList(true);
  if (synthesized) {
    if (!textureSpecList.isNull())
      synthesizedConfigDict[oo::kTexturesKey] = textureSpecList;
    if (!uniformSpecDict.isNull())
      synthesizedConfigDict[oo::kUniformsKey] = uniformSpecDict;
    synthesizedConfigDict[oo::kVertexShaderSourceKey] =
        oo::PList(vertexShaderSource);
    synthesizedConfigDict[oo::kFragmentShaderSourceKey] =
        oo::PList(fragmentShaderSource);
  }
  oo::PList synthesizedConfig(std::move(synthesizedConfigDict));

#ifndef NDEBUG
  if (DumpShaderSource()) {
    const std::string_view cacheKeyText =
        cacheKey ? std::string_view(*cacheKey) : "(null)";
    const std::string_view nameText = name ? std::string_view(*name) : "(null)";
    std::string dumpPath =
        std::format("Synthesized Materials/{}/{}", cacheKeyText, nameText);

                ResourceManager::writeDiagnosticString(synthesized ?, std::string(), dumpPath + ".vertex");
                ResourceManager::writeDiagnosticString(synthesized ?, std::string(), dumpPath + ".fragment");

                // Hide internal keys in the synthesized config before writing
                // it.
                oo::PList::Dict humanFriendlyConfig =
                    *synthesizedConfig.getIf<oo::PList::Dict>();
                humanFriendlyConfig.erase(oo::kVertexShaderSourceKey);
                humanFriendlyConfig.erase(oo::kFragmentShaderSourceKey);
                humanFriendlyConfig.erase(
                    oo::kIsSynthesizedMaterialConfigurationKey);
                humanFriendlyConfig[oo::kVertexShaderNameKey] =
                    oo::PList(std::format("{}.vertex", nameText));
                humanFriendlyConfig[oo::kFragmentShaderNameKey] =
                    oo::PList(std::format("{}.fragment", nameText));

                ResourceManager::writeDiagnosticPList(
                    oo::PList(humanFriendlyConfig), dumpPath + ".plist");

                ResourceManager::writeDiagnosticPList(
                    configuration, dumpPath + "-original.plist");
  }
#endif

  return materialWithName(name, cacheKey, synthesizedConfig, oo::PList(),
                          target, true);
}

#endif

std::shared_ptr<oo::Material> oo::Material::materialWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& cacheKey, const oo::PList& configuration,
    const oo::PList& macros, Entity* object, bool smooth) {
  // Internally, this flg really means "force use of shaders".
  std::shared_ptr<oo::Material> result;

#if OO_SHADERS

  if ((UNIVERSE != nullptr ? UNIVERSE->useShaders() : false)) {
    if (oo::ShaderMaterial::configurationDictionarySpecifiesShaderMaterial(
            configuration)) {
      result = oo::ShaderMaterial::shaderMaterialWithName(name, configuration,
                                                          macros, object);
    }

    // Use default shader if smoothing is on, or shader detail is full,
    // DEBUG_NO_SHADER_FALLBACK is set, or material uses an effect map.
    if (result == nullptr &&
        (smooth || oo::DebugFlagIsOn(oo::DebugFlags::kNoShaderFallback) ||
         (UNIVERSE != nullptr
              ? UNIVERSE->getDetailLevel()
              : oo::GraphicsDetail{}) >= oo::GraphicsDetail::kShaders ||
         !oo::MaterialCombinedSpecularMapSpecifier(configuration).isNull() ||
         !oo::MaterialNormalMapSpecifier(configuration).isNull() ||
         !oo::MaterialParallaxMapSpecifier(configuration).isNull() ||
         !oo::MaterialNormalAndParallaxMapSpecifier(configuration).isNull() ||
         !oo::MaterialEmissionMapSpecifier(configuration).isNull() ||
         !oo::MaterialIlluminationMapSpecifier(configuration).isNull() ||
         !oo::MaterialEmissionAndIlluminationMapSpecifier(configuration)
              .isNull())) {
      result = defaultShaderMaterialWithName(name, cacheKey, configuration,
                                             macros, object);
    }
  }
#endif

#if OO_MULTITEXTURE
  if (result == nullptr /*&& !(UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false)*/) {
    if (!oo::MaterialEmissionMapSpecifier(configuration).isNull() ||
        !oo::MaterialIlluminationMapSpecifier(configuration).isNull() ||
        !oo::MaterialEmissionAndIlluminationMapSpecifier(configuration)
             .isNull()) {
      result = oo::MultiTextureMaterial::materialWithName(name, configuration);
    }
  }
#endif

  if (result == nullptr) {
    if (oo::MaterialDiffuseMapSpecifier(configuration, name).isNull()) {
      result = oo::BasicMaterial::materialWithName(name, configuration);
    } else {
      result = oo::SingleTextureMaterial::materialWithName(name, configuration);
    }
    if (result == nullptr) {
      result = oo::BasicMaterial::materialWithName(name, configuration);
    }
  }
  return result;
}

// The materials and shaders come from Ship.setMaterials() and setShaders() as
// whatever the script passed. Reading a key from one that is neither a
// dictionary nor null (an array, a string) raises NSInvalidArgumentException,
// as -oo_dictionaryForKey: sent to it did; the script gets a native exception.
static void RaiseUnlessDictionary(const oo::PList& value, const char* what) {
  if (!value.isNull() && !value.isDict()) {
    oo::RaiseException(oo::InvalidArgumentException, "%s is not a dictionary",
                       what);
  }
}

std::shared_ptr<oo::Material> oo::Material::materialWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& cacheKey, const oo::PList& materialDict,
    const oo::PList& shadersDict, const oo::PList& macros, Entity* object,
    bool smooth) {
  oo::PList configuration;

#if OO_SHADERS

  if (UNIVERSE != nullptr ? UNIVERSE->useShaders() : false) {
    RaiseUnlessDictionary(shadersDict, "shaders");
    if (name.has_value()) {
      if (const oo::PList* found = shadersDict.get<oo::PList::Dict>(*name)) {
        configuration = *found;
      }
    }
  }
#endif

  if (configuration.isNull()) {
    RaiseUnlessDictionary(materialDict, "materials");
    if (name.has_value()) {
      if (const oo::PList* found = materialDict.get<oo::PList::Dict>(*name)) {
        configuration = *found;
      }
    }
  }

  if (configuration.isNull()) {
    // Use fallback material for non-existent simple texture.
    // Texture caching means this won't be wasted in the general case.
    std::shared_ptr<oo::Texture> texture = oo::Texture::textureWithName(
        name, std::optional<std::string>("Textures"));
    if (texture == nullptr) return nullptr;

    configuration = oo::PList(oo::PList::Dict{});
  }

  return materialWithName(name, cacheKey, configuration, macros, object,
                          smooth);
}

#if !USE_NEW_SHADER_SYNTHESIZER
namespace {

void SetUniform(oo::PList::Dict& uniforms, const std::string& key,
                std::string_view type, const oo::PList& value) {
  uniforms[key] =
      oo::PList(oo::PList::Dict{{"type", oo::PList(type)}, {"value", value}});
}

void SetUniformFloat(MaterialSynthContext* context, const std::string& key,
                     float value) {
  SetUniform(context->uniforms, key, "float", oo::PList::singleReal(value));
}

void AddTexture(MaterialSynthContext* context, std::string_view uniformName,
                std::optional<std::string_view> nonShaderKey,
                std::optional<std::string_view> macroName,
                const oo::PList& specifier) {
  OOCParameterAssert(context->texturesUsed < context->maxTextures);

  context->texturesUsed++;
  SetUniform(context->uniforms, std::string(uniformName), "texture",
             oo::PList::unsignedInteger(context->textures.size()));
  context->textures.push_back(specifier);
  if (nonShaderKey.has_value()) {
    // Upstream always wrote kOOMaterialDiffuseMapName when nonShaderKey was
    // non-nil.
    context->outConfig[oo::kMaterialDiffuseMapName] = specifier;
  }
  if (macroName.has_value()) {
    context->macros[std::string(*macroName)] = oo::PList("1");
  }
}

void AddColorIfAppropriate(MaterialSynthContext* context, oo::Color* color,
                           std::string_view key,
                           std::optional<std::string_view> macroName) {
  if (color != nullptr) {
    oo::PList::Array components;
    for (float component : color->normalizedArray()) {
      components.push_back(oo::PList::singleReal(component));
    }
    context->outConfig[std::string(key)] = oo::PList(std::move(components));
    if (macroName.has_value())
      context->macros[std::string(*macroName)] = oo::PList("1");
  }
}

void AddMacroColorIfAppropriate(MaterialSynthContext* context, oo::Color* color,
                                std::string_view macroName) {
  if (color != nullptr) {
    std::string macroText =
        std::format("vec4({:g}, {:g}, {:g}, {:g})", color->redComponent(),
                    color->greenComponent(), color->blueComponent(),
                    color->alphaComponent());
    context->macros[std::string(macroName)] = oo::PList(std::move(macroText));
  }
}

void SynthDiffuse(MaterialSynthContext* context,
                  const std::optional<std::string>& name) {
  // Set up diffuse map if appropriate.
  oo::PList diffuseMapSpec =
      oo::MaterialDiffuseMapSpecifier(context->inConfig, name);
  if (!diffuseMapSpec.isNull() &&
      context->texturesUsed < context->maxTextures) {
    AddTexture(context, "uDiffuseMap", oo::kMaterialDiffuseMapName,
               "OOSTD_DIFFUSE_MAP", diffuseMapSpec);

    if (diffuseMapSpec.get<bool>("cube_map")) {
      context->macros["OOSTD_DIFFUSE_MAP_IS_CUBE_MAP"] = oo::PList("1");
    }
  } else {
    // No diffuse map must be specified explicitly.
    context->outConfig[oo::kMaterialDiffuseMapName] = oo::PList("");
  }

  // Set up diffuse colour if any.
  AddColorIfAppropriate(context,
                        oo::MaterialDiffuseColor(context->inConfig).get(),
                        oo::kMaterialDiffuseColorName, std::nullopt);
}

void SynthEmissionAndIllumination(MaterialSynthContext* context) {
  // Read the various emission and illumination textures, and decide what to do
  // with them.
  oo::PList emissionMapSpec =
      oo::MaterialEmissionMapSpecifier(context->inConfig);
  oo::PList illuminationMapSpec =
      oo::MaterialIlluminationMapSpecifier(context->inConfig);
  oo::PList emissionAndIlluminationSpec =
      oo::MaterialEmissionAndIlluminationMapSpecifier(context->inConfig);
  bool isCombinedSpec = false;
  bool haveIlluminationMap = false;

  if (emissionMapSpec.isNull() && !emissionAndIlluminationSpec.isNull()) {
    emissionMapSpec = emissionAndIlluminationSpec;
    if (illuminationMapSpec.isNull())
      isCombinedSpec = true;  // Else use only emission part of
                              // emission_and_illumination_map, combined with
                              // full illumination_map.
  }

  if (!emissionMapSpec.isNull() &&
      context->texturesUsed < context->maxTextures) {
    /*	FIXME: at this point, if there is an illumination map, we should
            consider merging it into the emission map using
            OOCombinedEmissionMapGenerator if the total number of texture
            specifiers is greater than context->maxTextures. This will
            require adding a new type of texture specifier - not a big deal.
            -- Ahruman 2010-05-21
    */
    AddTexture(context, "uEmissionMap", std::nullopt,
               isCombinedSpec ? "OOSTD_EMISSION_AND_ILLUMINATION_MAP"
                              : "OOSTD_EMISSION_MAP",
               emissionMapSpec);
    /*	Note that this sets emission_color, not emission_modulate_color.
            This is because the emission colour value is sent through the
            standard OpenGL emission colour attribute by OOBasicMaterial.
    */
    AddColorIfAppropriate(
        context, oo::MaterialEmissionModulateColor(context->inConfig).get(),
        oo::kMaterialEmissionColorName, "OOSTD_EMISSION");

    haveIlluminationMap = isCombinedSpec;
  } else {
    //	No emission map, use overall emission colour if specified.
    AddColorIfAppropriate(context,
                          oo::MaterialEmissionColor(context->inConfig).get(),
                          oo::kMaterialEmissionColorName, "OOSTD_EMISSION");
  }

  if (!illuminationMapSpec.isNull() &&
      context->texturesUsed < context->maxTextures) {
    AddTexture(context, "uIlluminationMap", std::nullopt,
               "OOSTD_ILLUMINATION_MAP", illuminationMapSpec);
    haveIlluminationMap = true;
  }

  if (haveIlluminationMap) {
    AddMacroColorIfAppropriate(
        context, oo::MaterialIlluminationModulateColor(context->inConfig).get(),
        "OOSTD_ILLUMINATION_COLOR");
  }
}

void SynthNormalMap(MaterialSynthContext* context) {
  if (context->texturesUsed < context->maxTextures) {
    bool hasParallax = true;
    oo::PList normalMapSpec =
        oo::MaterialNormalAndParallaxMapSpecifier(context->inConfig);
    if (normalMapSpec.isNull()) {
      hasParallax = false;
      normalMapSpec = oo::MaterialNormalMapSpecifier(context->inConfig);
    }

    if (!normalMapSpec.isNull()) {
      AddTexture(context, "uNormalMap", std::nullopt, "OOSTD_NORMAL_MAP",
                 normalMapSpec);

      if (hasParallax) {
        context->macros["OOSTD_NORMAL_AND_PARALLAX_MAP"] = oo::PList("1");
        SetUniformFloat(context, "uParallaxScale",
                        oo::MaterialParallaxScale(context->inConfig));
        SetUniformFloat(context, "uParallaxBias",
                        oo::MaterialParallaxBias(context->inConfig));
      }
    }
  }
}

void SynthSpecular(MaterialSynthContext* context) {
  GLint shininess = oo::MaterialSpecularExponent(context->inConfig);
  if (shininess <= 0) return;

  GLfloat gloss = oo::MaterialGloss(context->inConfig);
  if (gloss < 0.0f || gloss > 1.0f) return;

  bool gammaCorrect =
      oo::MaterialGammaCorrect(context->inConfig) ? true : false;

  oo::PList specularMapSpec;
  std::shared_ptr<oo::Color> specularColor;

  if (context->texturesUsed < context->maxTextures) {
    specularMapSpec =
        oo::MaterialCombinedSpecularMapSpecifier(context->inConfig);
  }

  if (!specularMapSpec.isNull())
    specularColor = oo::MaterialSpecularModulateColor(context->inConfig);
  else
    specularColor = oo::MaterialSpecularColor(context->inConfig);
  if (specularColor->isBlack()) return;

  SetUniformFloat(context, "uGloss", gloss);

  context->outConfig[oo::kMaterialSpecularExponentLegacyName] =
      oo::PList::unsignedInteger(static_cast<unsigned int>(shininess));

  if (!specularMapSpec.isNull()) {
    AddTexture(context, "uSpecularMap", oo::kMaterialDiffuseMapName,
               "OOSTD_SPECULAR_MAP", specularMapSpec);
  }

  if (specularColor != nullptr) {
    /*	As with emission colour, specular_modulate_color is transformed to
     specular_color here because the shader reads it from the standard
     material specular colour property set by OOBasicMaterial.
     */
    oo::PList::Array components;
    for (float component : specularColor->normalizedArray()) {
      components.push_back(oo::PList::singleReal(component));
    }
    context->outConfig[oo::kMaterialSpecularColorName] =
        oo::PList(std::move(components));
  }
  context->macros["OOSTD_SPECULAR"] = oo::PList("1");

  // setting a bool as a float uniform, to be used in the shader as a bool again
  // this is how hackish I can get... maybe a better way exists, but this is
  // quick and can be used also for the shader materials in a not too different
  // way
  // - Nikos 20181001
  SetUniformFloat(context, "uGammaCorrect", static_cast<float>(gammaCorrect));
}

}  // namespace
#endif
