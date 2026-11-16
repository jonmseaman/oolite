/*

OODefaultShaderSynthesizer.m


Copyright © 2011-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "DefaultShaderSynthesizer.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Color.h"
#include "MaterialSpecifier.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "oofnd/Log.hpp"
#include "oofnd/objc/Assert.h"
#include "oofnd/objc/Exception.h"

namespace {

oo::PList CanonicalizeMaterialSpecifier(
    const oo::PList& spec, const std::optional<std::string>& materialKey);

}  // namespace

namespace {

std::string FormatFloat(double value);

}  // namespace

/*
        REQUIRE_STAGE(): pull in the required stage. A stage must have a
        zero-parameter member function and a matching _completed_stage member.

        In debug/testrelease builds, this dispatches through performStage()
   which checks for recursive calls. The stage is named by its text and called
   through a member-function pointer where it was named by a selector.
*/
#ifndef NDEBUG
#define REQUIRE_STAGE(NAME)                                   \
  if (!_completed_##NAME) {                                   \
    performStage(#NAME, &oo::DefaultShaderSynthesizer::NAME); \
    _completed_##NAME = true;                                 \
  }
#else
#define REQUIRE_STAGE(NAME)   \
  if (!_completed_##NAME) {   \
    NAME();                   \
    _completed_##NAME = true; \
  }
#endif

namespace {

std::optional<std::string> GetExtractMode(const oo::PList& textureSpecifier);

}  // namespace

namespace oo {

std::optional<SynthesizedShader> SynthesizeMaterialShader(
    const oo::PList& configuration,
    const std::optional<std::string>& materialKey,
    const std::optional<std::string>& entityName) {
  OOCParameterAssert(!configuration.isNull());

  std::shared_ptr<DefaultShaderSynthesizer> synthesizer =
      oo::makeShared<DefaultShaderSynthesizer>(configuration, materialKey,
                                               entityName);

  SynthesizedShader result;
  if (synthesizer->run()) {
    result.vertexShader = synthesizer->vertexShader();
    result.fragmentShader = synthesizer->fragmentShader();
    result.textureSpecs = synthesizer->textureSpecifications();
    result.uniformSpecs = synthesizer->uniformSpecifications();
  }
  return result;
}

}  // namespace oo

oo::DefaultShaderSynthesizer::DefaultShaderSynthesizer(
    const oo::PList& configuration,
    const std::optional<std::string>& materialKey,
    const std::optional<std::string>& /*name*/) {
  _configuration = CanonicalizeMaterialSpecifier(configuration, materialKey);
  _materialKey = materialKey;
  // _entityName stays nil: upstream copied it from itself ([_entityName copy])
  // instead of name.
}

/*	-dealloc sent -destroyTemporaries, which only empties members that the
   destructor destroys anyway; the destructor is the implicit one.
*/

std::string oo::DefaultShaderSynthesizer::vertexShader() {
  return _vertexShader;
}

std::string oo::DefaultShaderSynthesizer::fragmentShader() {
  return _fragmentShader;
}

oo::PList oo::DefaultShaderSynthesizer::textureSpecifications() {
  return oo::PList(oo::PList::Array(_textures));
}

oo::PList oo::DefaultShaderSynthesizer::uniformSpecifications() {
  return oo::PList(_uniforms);
}

bool oo::DefaultShaderSynthesizer::run() {
  createTemporaries();
  _uniforms.clear();
  _vertexBody += "void main(void)\n{\n";
  _fragmentPreTextures += "void main(void)\n{\n";

  const oo::Finally temporaries([this] {
    destroyTemporaries();
  });  // was @finally: runs after the handler
  try {
    REQUIRE_STAGE(writeFinalColorComposite);

    composeVertexShader();
    composeFragmentShader();
  } catch (...) {
    const auto exception = oo::CurrentOOException();
    if (!exception) throw;  // not an OOException: it goes on up, as before
    // Error should have been reported already.
    return false;
  }

  return true;
}

std::optional<std::string> oo::DefaultShaderSynthesizer::materialKey() {
  return _materialKey;
}

std::optional<std::string> oo::DefaultShaderSynthesizer::entityName() {
  return _entityName;
}

// MARK: - Utilities

namespace {

void AppendIfNotEmpty(std::string& buffer, const std::string& segment,
                      std::string_view name) {
  if (!segment.empty()) {
    if (!buffer.empty()) buffer += "\n\n";
    if (!name.empty()) {
      buffer += "// ";
      buffer += name;
      buffer += "\n";
    }
    buffer += segment;
  }
}

}  // namespace

namespace {

/*	The extract_channel of a texture specifier if it is one to four of r, g,
   b and a, else nil. (A number never qualifies, so only a string value is read;
   all-ASCII, so its UTF-8 length is its length in characters.)
*/
std::optional<std::string> GetExtractMode(const oo::PList& textureSpecifier) {
  std::optional<std::string> result;

  const oo::PList* value =
      textureSpecifier.find(oo::kTextureSpecifierSwizzleKey);
  const std::string* rawMode =
      (value != nullptr) ? value->getIf<std::string>() : nullptr;
  if (rawMode != nullptr) {
    std::size_t length = rawMode->size();
    if (1 <= length && length <= 4 &&
        rawMode->find_first_not_of("rgba") == std::string::npos) {
      result = *rawMode;
    }
  }

  return result;
}

// The string-typed read of the value for key that oo::PListView did: a string,
// or a number as a string; anything else nil.
std::optional<std::string> OptionalStringFor(const oo::PList& spec,
                                             std::string_view key) {
  const oo::PList* value = spec.find(key);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return spec.get<std::string>(key);
}

}  // namespace

void oo::DefaultShaderSynthesizer::appendVariable(const std::string& name,
                                                  const std::string& type,
                                                  const std::string& prefix,
                                                  std::string& buffer) {
  std::size_t typeDeclLength = prefix.size() + type.size() + 1;
  std::size_t padding = (typeDeclLength < 20) ? (23 - typeDeclLength) / 4 : 1;
  buffer += prefix + " " + type + std::string(padding, '\t') + name + ";\n";
}

void oo::DefaultShaderSynthesizer::addAttribute(const std::string& name,
                                                const std::string& type) {
  appendVariable(name, type, "attribute", _attributes);
}

void oo::DefaultShaderSynthesizer::addVarying(const std::string& name,
                                              const std::string& type) {
  appendVariable(name, type, "varying", _varyings);
}

void oo::DefaultShaderSynthesizer::addVertexUniform(const std::string& name,
                                                    const std::string& type) {
  appendVariable(name, type, "uniform", _vertexUniforms);
}

void oo::DefaultShaderSynthesizer::addFragmentUniform(const std::string& name,
                                                      const std::string& type) {
  appendVariable(name, type, "uniform", _fragmentUniforms);
}

std::optional<std::string> oo::DefaultShaderSynthesizer::defineBindingUniform(
    const oo::PList& binding, const std::string& type) {
  std::string name =
      OptionalStringFor(binding, "binding").value_or(std::string());
  OOCParameterAssert(!name.empty());

  oo::PList bindingSpec = binding;
  if (!OptionalStringFor(bindingSpec, "type").has_value()) {
    if (oo::PList::Dict* dict = bindingSpec.getIf<oo::PList::Dict>())
      (*dict)["type"] = oo::PList("binding");
  }

  // Use existing uniform if one is defined.
  for (const auto& [spec, uniformName] : _uniformBindingNames) {
    if (spec == bindingSpec) return uniformName;
  }

  // Capitalize first char of name, and prepend u. (Binding names are ASCII
  // property names.)
  std::string baseName = "u";
  baseName += static_cast<char>(toupper(static_cast<unsigned char>(name[0])));
  baseName += name.substr(1);

  // Ensure name is unique.
  name = baseName;
  unsigned idx = 1;
  while (_uniforms.contains(name)) {
    name = std::format("{}{}", baseName, ++idx);
  }

  addFragmentUniform(name, type);

  _uniforms[name] = bindingSpec;
  _uniformBindingNames.emplace_back(bindingSpec, name);

  return name;
}

void oo::DefaultShaderSynthesizer::composeVertexShader() {
  while (_vertexBody.ends_with("\t\n")) {
    _vertexBody.erase(_vertexBody.size() - 2);
  }
  _vertexBody += "}";

  std::string vertexShader;
  AppendIfNotEmpty(vertexShader, _attributes, "Attributes");
  AppendIfNotEmpty(vertexShader, _vertexUniforms, "Uniforms");
  AppendIfNotEmpty(vertexShader, _varyings, "Varyings");
  AppendIfNotEmpty(vertexShader, _vertexHelpers, "Helper functions");
  AppendIfNotEmpty(vertexShader, _vertexBody, "");

  _vertexShader = vertexShader;
}

void oo::DefaultShaderSynthesizer::composeFragmentShader() {
  while (_fragmentBody.ends_with("\t\n")) {
    _fragmentBody.erase(_fragmentBody.size() - 2);
  }

  std::string fragmentShader;
  AppendIfNotEmpty(fragmentShader, _fragmentUniforms, "Uniforms");
  AppendIfNotEmpty(fragmentShader, _varyings, "Varyings");
  AppendIfNotEmpty(fragmentShader, _fragmentHelpers, "Helper functions");
  AppendIfNotEmpty(fragmentShader, _fragmentPreTextures, "");
  if (!_fragmentTextureLookups.empty()) {
    fragmentShader += "\t\n\t// Texture lookups\n";
    fragmentShader += _fragmentTextureLookups;
  }
  fragmentShader += "\t\n";
  fragmentShader += _fragmentBody;
  fragmentShader += "}";

  _fragmentShader = fragmentShader;
}

/*
        Build a key for a texture specifier, taking all texture configuration
        options into account and ignoring the other stuff that might be there.

        FIXME: efficiency and stuff.
*/
namespace {

std::string KeyFromTextureParameters(const std::string& name,
                                     oo::TextureFlags options, float anisotropy,
                                     float lodBias) {
#ifndef NDEBUG
  options = oo::ApplyTextureOptionDefaults(options);
#endif

  // Extraction modes are ignored in synthesized shaders, since we use swizzling
  // instead.
  options &= ~oo::TextureFlags::kExtractChannelMask;

  return std::format("{}:{:X}:{:g}:{:g}", name, oo::ToUnderlying(options),
                     anisotropy, lodBias);
}

std::string KeyFromTextureSpec(const oo::PList& spec) {
  const std::optional<oo::TextureSpecifier> interpreted =
      oo::InterpretTextureSpecifier(spec, true);
  if (!interpreted.has_value()) {
    // OOInterpretTextureSpecifier() will have logged something.
    oo::RaiseException(oo::GenericException, "Invalid texture specifier");
  }
  const std::string& texName = interpreted->name;
  const oo::TextureFlags texOptions = interpreted->options;
  const float anisotropy = interpreted->anisotropy;
  const float lodBias = interpreted->lodBias;

  return KeyFromTextureParameters(texName, texOptions, anisotropy, lodBias);
}

}  // namespace

std::size_t oo::DefaultShaderSynthesizer::assignIDForTexture(
    const oo::PList& textureSpec) {
  OOCParameterAssert(!textureSpec.isNull());

  // extract_channel doesn't affect uniqueness, and we don't want OOTexture to
  // do actual extraction.
  oo::PList spec = textureSpec;
  if (oo::PList::Dict* dict = spec.getIf<oo::PList::Dict>()) {
    dict->erase(oo::kTextureSpecifierSwizzleKey);
  }

  const std::optional<oo::TextureSpecifier> interpreted =
      oo::InterpretTextureSpecifier(spec, true);
  if (!interpreted.has_value()) {
    // OOInterpretTextureSpecifier() will have logged something.
    oo::RaiseException(oo::GenericException, "Invalid texture specifier");
  }
  const std::string& texName = interpreted->name;
  const oo::TextureFlags texOptions = interpreted->options;
  const float anisotropy = interpreted->anisotropy;
  const float lodBias = interpreted->lodBias;

  if (oo::Any(texOptions & oo::TextureFlags::kAllowCubeMap)) {
    // cube_map = true; fail regardless of whether actual texture qualifies.
    OO_LOG_ERR("material.synthesis.error.cubeMap",
               "The material \"{}\" of \"{}\" specifies a cube map texture, "
               "but doesn't have custom shaders. Cube map textures are not "
               "supported with the default shaders.",
               materialKey().value_or("(null)"),
               entityName().value_or("(null)"));
    oo::RaiseException(oo::GenericException, "Invalid material");
  }

  std::string key =
      KeyFromTextureParameters(texName, texOptions, anisotropy, lodBias);
  std::size_t texID;
  if (_texturesByName.find(key) == _texturesByName.end()) {
    texID = _texturesByName.size();
    std::string texUniform = std::format("uTexture{}", texID);

#ifndef NDEBUG
    bool useInternalFormat = false;
#else
    bool useInternalFormat = true;
#endif

    _textures.push_back(oo::MakeTextureSpecifier(
        texName, texOptions, anisotropy, lodBias, useInternalFormat));
    _texturesByName[key] = spec;
    _textureIDs[key] = texID;
    oo::PList uniform(
        oo::PList::Dict{{"type", oo::PList("texture")},
                        {"value", oo::PList::unsignedInteger(texID)}});
    _uniforms[texUniform] = uniform;

    addFragmentUniform(texUniform, "sampler2D");
  } else {
    // Upstream looks the ID up by texName, not by key, so it finds none and
    // answers 0 (kept).
    auto found = _textureIDs.find(texName);
    texID = (found != _textureIDs.end()) ? found->second : 0;
  }

  return texID;
}

std::size_t oo::DefaultShaderSynthesizer::textureIDForSpec(
    const oo::PList& textureSpec) {
  auto found = _textureIDs.find(KeyFromTextureSpec(textureSpec));
  return (found != _textureIDs.end()) ? found->second : 0;
}

void oo::DefaultShaderSynthesizer::setUpOneTexture(
    const oo::PList& textureSpec) {
  if (textureSpec.isNull()) return;

  REQUIRE_STAGE(writeTextureCoordRead);

  std::size_t texID = assignIDForTexture(textureSpec);
  if (_sampledTextures.insert(texID).second) {
    _fragmentTextureLookups += std::format(
        "\tvec4 tex{}Sample = texture2D(uTexture{}, texCoords);  // {}\n",
        texID, texID,
        OptionalStringFor(textureSpec, oo::kTextureSpecifierNameKey)
            .value_or("(null)"));
  }
}

oo::DefaultShaderSynthesizer::SampleName
oo::DefaultShaderSynthesizer::getSampleName(const oo::PList& textureSpec) {
  OOCParameterAssert(!textureSpec.isNull());

  setUpOneTexture(textureSpec);
  std::size_t texID = textureIDForSpec(textureSpec);

  return {std::format("tex{}Sample", texID),
          GetExtractMode(textureSpec).value_or(std::string())};
}

std::optional<std::string> oo::DefaultShaderSynthesizer::readRGBForTextureSpec(
    const oo::PList& textureSpec, const std::string& mapName) {
  auto [sample, swizzle] = getSampleName(textureSpec);

  if (swizzle.empty()) {
    return sample + ".rgb";
  }

  std::size_t channelCount = swizzle.size();

  if (channelCount == 1) {
    return sample + "." + swizzle + swizzle + swizzle;
  } else if (channelCount == 3) {
    return sample + "." + swizzle;
  }

  OO_LOG_WARN("material.synthesis.warning.extractionMismatch",
              "The {} map for material \"{}\" of \"{}\" specifies {} channels "
              "to extract, but only {} may be used.",
              mapName, materialKey().value_or("(null)"),
              entityName().value_or("(null)"), channelCount, "1 or 3");
  return std::nullopt;
}

std::optional<std::string>
oo::DefaultShaderSynthesizer::readOneChannelForTextureSpec(
    const oo::PList& textureSpec, const std::string& mapName) {
  auto [sample, swizzle] = getSampleName(textureSpec);

  if (swizzle.empty()) {
    return sample + ".r";
  }

  std::size_t channelCount = swizzle.size();

  if (channelCount == 1) {
    return sample + "." + swizzle;
  }

  OO_LOG_WARN("material.synthesis.warning.extractionMismatch",
              "The {} map for material \"{}\" of \"{}\" specifies {} channels "
              "to extract, but only {} may be used.",
              mapName, materialKey().value_or("(null)"),
              entityName().value_or("(null)"), channelCount, "1");
  return std::nullopt;
}

#ifndef NDEBUG
void oo::DefaultShaderSynthesizer::performStage(std::string_view name,
                                                Stage stage) {
  // Ensure that we aren’t recursing.
  if (_stagesInProgress.count(name) != 0) {
    OO_LOG_ERR("material.synthesis.error.recursion",
               "Shader synthesis recursion for stage {}.", name);
    oo::RaiseException(oo::InternalInconsistencyException, "stage recursion");
  }

  _stagesInProgress.insert(name);

  (this->*stage)();

  _stagesInProgress.erase(name);
}
#endif

void oo::DefaultShaderSynthesizer::createTemporaries() {
  _attributes.clear();
  _varyings.clear();
  _vertexUniforms.clear();
  _fragmentUniforms.clear();
  _vertexHelpers.clear();
  _fragmentHelpers.clear();
  _vertexBody.clear();
  _fragmentPreTextures.clear();
  _fragmentTextureLookups.clear();
  _fragmentBody.clear();

  _textures.clear();
  _texturesByName.clear();
  _textureIDs.clear();
  _sampledTextures.clear();

  _uniformBindingNames.clear();

#ifndef NDEBUG
  _stagesInProgress.clear();
#endif
}

void oo::DefaultShaderSynthesizer::destroyTemporaries() {
  _attributes.clear();
  _varyings.clear();
  _vertexUniforms.clear();
  _fragmentUniforms.clear();
  _vertexHelpers.clear();
  _fragmentHelpers.clear();
  _vertexBody.clear();
  _fragmentPreTextures.clear();
  _fragmentTextureLookups.clear();
  _fragmentBody.clear();

  _texturesByName.clear();
  _textureIDs.clear();
  _sampledTextures.clear();

  _uniformBindingNames.clear();

#ifndef NDEBUG
  _stagesInProgress.clear();
#endif
}

// MARK: - Synthesis stages

// MARK: - Synthesis stages

void oo::DefaultShaderSynthesizer::writeTextureCoordRead() {
  addVarying("vTexCoords", "vec2");
  _vertexBody += "\tvTexCoords = gl_MultiTexCoord0.st;\n\t\n";

  bool haveTexCoords = false;
  oo::PList parallaxMap = oo::MaterialParallaxMapSpecifier(_configuration);

  if (!parallaxMap.isNull()) {
    float parallaxScale = oo::MaterialParallaxScale(_configuration);
    if (parallaxScale != 0.0f) {
      /*
              We can’t call -getSampleName:... here because the standard
              texture loading mechanism has to occur after determining
              texture coordinates (duh).
      */
      std::string swizzle = GetExtractMode(parallaxMap).value_or("a");
      std::size_t channelCount = swizzle.size();
      if (channelCount == 1) {
        haveTexCoords = true;

        REQUIRE_STAGE(writeEyeVector);

        _fragmentPreTextures += "\t// Parallax mapping\n";

        std::size_t texID = assignIDForTexture(parallaxMap);
        _fragmentPreTextures += std::format(
            "\tfloat parallax = texture2D(uTexture{}, vTexCoords).{};\n", texID,
            swizzle);

        if (parallaxScale != 1.0f) {
          _fragmentPreTextures +=
              std::format("\tparallax *= {};  // Parallax scale\n",
                          FormatFloat(parallaxScale));
        }

        float parallaxBias = oo::MaterialParallaxBias(_configuration);
        if (parallaxBias != 0.0) {
          _fragmentPreTextures +=
              std::format("\tparallax += {};  // Parallax bias\n",
                          FormatFloat(parallaxBias));
        }

        _fragmentPreTextures +=
            "\tvec2 texCoords = vTexCoords - parallax * eyeVector.xy * "
            "vec2(1.0, -1.0);\n";
      } else {
        OO_LOG_WARN("material.synthesis.warning.extractionMismatch",
                    "The {} map for material \"{}\" of \"{}\" specifies {} "
                    "channels to extract, but only {} may be used.",
                    "parallax", materialKey().value_or("(null)"),
                    entityName().value_or("(null)"), channelCount, "1");
      }
    }
  }

  if (!haveTexCoords) {
    _fragmentPreTextures += "\tvec2 texCoords = vTexCoords;\n";
  }
}

void oo::DefaultShaderSynthesizer::writeDiffuseColorTermIfNeeded() {
  oo::PList diffuseMap =
      oo::MaterialDiffuseMapSpecifier(_configuration, materialKey());
  std::shared_ptr<oo::Color> diffuseColor(
      oo::MaterialDiffuseColor(_configuration));
  if (diffuseColor == nullptr) diffuseColor = oo::Color::whiteColor();

  if (diffuseColor->isBlack()) return;
  _usesDiffuseTerm = true;

  bool haveDiffuseColor = false;
  if (!diffuseMap.isNull()) {
    std::optional<std::string> readInstr =
        readRGBForTextureSpec(diffuseMap, "diffuse");
    if (!readInstr.has_value()) [[unlikely]] {
      _fragmentBody += "\t// INVALID EXTRACTION KEY\n\t\n";
    } else {
      _fragmentBody += std::format("\tvec3 diffuseColor = {};\n", *readInstr);
      haveDiffuseColor = true;
    }
  }

  if (!haveDiffuseColor || !diffuseColor->isWhite()) {
    float rgba[4];
    diffuseColor->getRed(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
    if (haveDiffuseColor) {
      _fragmentBody += std::format("\tdiffuseColor *= vec3({}, {}, {});\n",
                                   FormatFloat(rgba[0]), FormatFloat(rgba[1]),
                                   FormatFloat(rgba[2]));
    } else {
      _fragmentBody += std::format(
          "\tconst vec3 diffuseColor = vec3({}, {}, {});\n",
          FormatFloat(rgba[0]), FormatFloat(rgba[1]), FormatFloat(rgba[2]));
      haveDiffuseColor = true;
    }
  }

  (void)haveDiffuseColor;
  _fragmentBody += "\t\n";
}

void oo::DefaultShaderSynthesizer::writeDiffuseColorTerm() {
  REQUIRE_STAGE(writeDiffuseColorTermIfNeeded);

  if (!_usesDiffuseTerm) {
    _fragmentBody +=
        "\tconst vec3 diffuseColor = vec3(0.0);  // Diffuse colour is "
        "black.\n\t\n";
  }
}

void oo::DefaultShaderSynthesizer::writeDiffuseLighting() {
  REQUIRE_STAGE(writeDiffuseColorTermIfNeeded);
  if (!_usesDiffuseTerm) return;

  REQUIRE_STAGE(writeTotalColor);
  REQUIRE_STAGE(writeVertexPosition);
  REQUIRE_STAGE(writeNormalIfNeeded);
  REQUIRE_STAGE(writeLightVector);

  // FIXME: currently uncoloured diffuse and ambient lighting.
  const std::string_view normalDotLight =
      _constZNormal ? "lightVector.z" : "dot(normal, lightVector)";

  _fragmentBody += std::format(
      "\t// Diffuse (Lambertian) and ambient lighting\n\tvec3 diffuseLight = "
      "(gl_LightSource[1].diffuse * max(0.0, {}) + "
      "gl_LightModel.ambient).rgb;\n\t\n",
      normalDotLight);

  _haveDiffuseLight = true;
}

void oo::DefaultShaderSynthesizer::writeLightVector() {
  REQUIRE_STAGE(writeVertexPosition);
  REQUIRE_STAGE(writeNormalIfNeeded);

  addVarying("vLightVector", "vec3");

  _vertexBody +=
      "\tvec3 lightVector = gl_LightSource[1].position.xyz;\n"
      "\tvLightVector = lightVector * TBN;\n\t\n";
  _fragmentBody += "\tvec3 lightVector = normalize(vLightVector);\n\t\n";
}

void oo::DefaultShaderSynthesizer::writeEyeVector() {
  REQUIRE_STAGE(writeVertexPosition);
  REQUIRE_STAGE(writeVertexTangentBasis);

  addVarying("vEyeVector", "vec3");

  _vertexBody += "\tvEyeVector = position.xyz * TBN;\n\t\n";
  _fragmentPreTextures += "\tvec3 eyeVector = normalize(vEyeVector);\n\t\n";
}

void oo::DefaultShaderSynthesizer::writeVertexTangentBasis() {
  addAttribute("tangent", "vec3");

  _vertexBody +=
      "\t// Build tangent space basis\n"
      "\tvec3 n = gl_NormalMatrix * gl_Normal;\n"
      "\tvec3 t = gl_NormalMatrix * tangent;\n"
      "\tvec3 b = cross(n, t);\n"
      "\tmat3 TBN = mat3(t, b, n);\n\t\n";
}

void oo::DefaultShaderSynthesizer::writeNormalIfNeeded() {
  REQUIRE_STAGE(writeVertexPosition);
  REQUIRE_STAGE(writeVertexTangentBasis);

  oo::PList normalMap = oo::MaterialNormalMapSpecifier(_configuration);
  if (normalMap.isNull()) {
    // FIXME: this stuff should be handled in OOMaterialSpecifier.m when
    // synthesizer takes over the world. -- Ahruman 2012-02-08
    normalMap = oo::MaterialNormalAndParallaxMapSpecifier(_configuration);
  }
  if (!normalMap.isNull()) {
    auto [sample, swizzle] = getSampleName(normalMap);
    if (swizzle.empty()) swizzle = "rgb";
    if (swizzle.size() == 3) {
      _fragmentBody += std::format(
          "\tvec3 normal = normalize({}.{} - 0.5);\n\t\n", sample, swizzle);
      _usesNormalMap = true;
      return;
    } else {
      OO_LOG_WARN("material.synthesis.warning.extractionMismatch",
                  "The {} map for material \"{}\" of \"{}\" specifies {} "
                  "channels to extract, but only {} may be used.",
                  "normal", materialKey().value_or("(null)"),
                  entityName().value_or("(null)"), swizzle.size(), "3");
    }
  }
  _constZNormal = true;
}

void oo::DefaultShaderSynthesizer::writeNormal() {
  REQUIRE_STAGE(writeNormalIfNeeded);

  if (_constZNormal) {
    _fragmentBody += "\tconst vec3 normal = vec3(0.0, 0.0, 1.0);\n\t\n";
  }
}

void oo::DefaultShaderSynthesizer::writeSpecularLighting() {
  float specularExponent =
      static_cast<float>(oo::MaterialSpecularExponent(_configuration));
  if (specularExponent <= 0) return;

  oo::PList specularColorMap =
      oo::MaterialSpecularColorMapSpecifier(_configuration);
  oo::PList specularExponentMap =
      oo::MaterialSpecularExponentMapSpecifier(_configuration);
  float scaleFactor = 1.0f;

  if (!specularColorMap.isNull()) {
    scaleFactor = static_cast<float>(specularColorMap.get<double>(
        oo::kTextureSpecifierScaleFactorKey, 1.0f));
  }

  std::shared_ptr<oo::Color> specularColor;  // never null: both functions below
                                             // answer a default colour
  if (specularColorMap.isNull()) {
    specularColor = oo::MaterialSpecularColor(_configuration);
  } else {
    specularColor = oo::MaterialSpecularModulateColor(_configuration);
  }

  if (specularColor->isBlack()) return;

  bool modulateWithDiffuse =
      specularColorMap.get<bool>(oo::kTextureSpecifierSelfColorKey);

  REQUIRE_STAGE(writeTotalColor);
  REQUIRE_STAGE(writeNormalIfNeeded);
  REQUIRE_STAGE(writeEyeVector);
  REQUIRE_STAGE(writeLightVector);
  if (modulateWithDiffuse) {
    REQUIRE_STAGE(writeDiffuseColorTerm);
  }

  _fragmentBody += "\t// Specular (Blinn-Phong) lighting\n";

  bool haveSpecularColor = false;
  if (!specularColorMap.isNull()) {
    std::optional<std::string> readInstr =
        readRGBForTextureSpec(specularColorMap, "specular colour");
    if (!readInstr.has_value()) [[unlikely]] {
      _fragmentBody += "\t// INVALID EXTRACTION KEY\n\t\n";
      return;
    }

    _fragmentBody += std::format("\tvec3 specularColor = {};\n", *readInstr);
    haveSpecularColor = true;
  }

  if (!haveSpecularColor || !specularColor->isWhite()) {
    float rgba[4];
    specularColor->getRed(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);

    const std::string_view comment = (scaleFactor == 1.0f)
                                         ? "Constant colour"
                                         : "Constant colour and scale factor";

    // Handle scale factor, colour, and colour alpha scaling as one multiply.
    scaleFactor *= rgba[3];
    rgba[0] *= scaleFactor;
    rgba[1] *= scaleFactor;
    rgba[2] *= scaleFactor;

    // Avoid reapplying scaleFactor below.
    scaleFactor = 1.0;

    if (haveSpecularColor) {
      _fragmentBody += std::format(
          "\tspecularColor *= vec3({}, {}, {});  // {}\n", FormatFloat(rgba[0]),
          FormatFloat(rgba[1]), FormatFloat(rgba[2]), comment);
    } else {
      _fragmentBody +=
          std::format("\tvec3 specularColor = vec3({}, {}, {});  // {}\n",
                      FormatFloat(rgba[0]), FormatFloat(rgba[1]),
                      FormatFloat(rgba[2]), comment);
      haveSpecularColor = true;
    }
  }

  // Handle scale_factor if no constant colour.
  if (haveSpecularColor && scaleFactor != 1.0f) {
    _fragmentBody += std::format("\tspecularColor *= {};  // Scale factor\n",
                                 FormatFloat(scaleFactor));
  }

  // Handle self_color.
  if (modulateWithDiffuse) {
    _fragmentBody += "\tspecularColor *= diffuseColor;  // Self-colouring\n";
  }

  // Specular exponent.
  bool haveSpecularExponent = false;
  if (!specularExponentMap.isNull()) {
    std::optional<std::string> readInstr =
        readOneChannelForTextureSpec(specularExponentMap, "specular exponent");
    if (!readInstr.has_value()) [[unlikely]] {
      _fragmentBody += "\t// INVALID EXTRACTION KEY\n\t\n";
      return;
    }

    _fragmentBody += std::format("\tfloat specularExponent = {} * {:.1f};\n",
                                 *readInstr, specularExponent);
    haveSpecularExponent = true;
  }
  if (!haveSpecularExponent) {
    _fragmentBody += std::format("\tconst float specularExponent = {:.1f};\n",
                                 specularExponent);
  }

  if (_usesNormalMap) {
    _fragmentBody += "\tvec3 reflection = reflect(lightVector, normal);\n";
  } else {
    /*	reflect(I, N) is defined as I - 2 * dot(N, I) * N
            If N is (0,0,1), this becomes (I.x,I.y,-I.z).
    */
    _fragmentBody +=
        "\tvec3 reflection = vec3(lightVector.x, lightVector.y, "
        "-lightVector.z);  // Equivalent to reflect(lightVector, normal) since "
        "normal is known to be (0, 0, 1) in tangent space.\n";
  }

  _fragmentBody +=
      "\tfloat specIntensity = dot(reflection, eyeVector);\n"
      "\tspecIntensity = pow(max(0.0, specIntensity), specularExponent);\n"
      "\ttotalColor += specIntensity * specularColor * "
      "gl_LightSource[1].specular.rgb;\n\t\n";
}

void oo::DefaultShaderSynthesizer::writeLightMaps() {
  const oo::PList* lightMaps =
      _configuration.get<oo::PList::Array>(oo::kMaterialLightMapsName);
  std::size_t idx, count = (lightMaps != nullptr) ? lightMaps->count() : 0;
  if (count == 0) return;

  REQUIRE_STAGE(writeTotalColor);

  // Check if we need the diffuse colour term.
  for (idx = 0; idx < count; idx++) {
    const oo::PList* lightMapSpec = lightMaps->at<oo::PList::Dict>(idx);
    if (lightMapSpec != nullptr &&
        lightMapSpec->get<bool>(oo::kTextureSpecifierIlluminationModeKey)) {
      REQUIRE_STAGE(writeDiffuseColorTerm);
      REQUIRE_STAGE(writeDiffuseLighting);
      break;
    }
  }

  _fragmentBody += "\tvec3 lightMapColor;\n";

  const oo::PList notADictionary;  // a light map entry that is not a dictionary
                                   // reads as nil
  for (idx = 0; idx < count; idx++) {
    const oo::PList* entry = lightMaps->at<oo::PList::Dict>(idx);
    const oo::PList& lightMapSpec =
        (entry != nullptr) ? *entry : notADictionary;
    oo::PList textureSpec =
        oo::TextureSpecFromObject(lightMapSpec, std::nullopt);
    const oo::PList* color = lightMapSpec.get<oo::PList::Array>(
        oo::kTextureSpecifierModulateColorKey);
    float rgba[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    bool isIllumination =
        lightMapSpec.get<bool>(oo::kTextureSpecifierIlluminationModeKey);

    if (color == nullptr && textureSpec.isNull()) [[unlikely]] {
      _fragmentBody +=
          "\t// Light map with neither colour nor texture has no effect.\n\t\n";
      continue;
    }

    if (color != nullptr) {
      std::size_t component, componentCount = color->count();
      if (componentCount > 4) componentCount = 4;
      for (component = 0; component < componentCount; component++) {
        rgba[component] = static_cast<float>(color->at<double>(component));
      }
      rgba[0] *= rgba[3];
      rgba[1] *= rgba[3];
      rgba[2] *= rgba[3];
    }

    if ((rgba[0] == 0.0f && rgba[1] == 0.0f && rgba[2] == 0.0f) ||
        (!_usesDiffuseTerm && isIllumination)) [[unlikely]] {
      _fragmentBody += "\t// Light map tinted black has no effect.\n\t\n";
      continue;
    }

    if (!textureSpec.isNull()) {
      std::optional<std::string> readInstr =
          readRGBForTextureSpec(textureSpec, "light");
      if (!readInstr.has_value()) [[unlikely]] {
        _fragmentBody += "\t// INVALID EXTRACTION KEY\n\n";
        continue;
      }

      _fragmentBody += std::format("\tlightMapColor = {};\n", *readInstr);

      if (rgba[0] != 1.0f || rgba[1] != 1.0f || rgba[2] != 1.0f) {
        _fragmentBody += std::format("\tlightMapColor *= vec3({}, {}, {});\n",
                                     FormatFloat(rgba[0]), FormatFloat(rgba[1]),
                                     FormatFloat(rgba[2]));
      }
    } else {
      _fragmentBody += std::format("\tlightMapColor = vec3({}, {}, {});\n",
                                   FormatFloat(rgba[0]), FormatFloat(rgba[1]),
                                   FormatFloat(rgba[2]));
    }

    const oo::PList* binding =
        textureSpec.get<oo::PList::Dict>(oo::kTextureSpecifierBindingKey);
    if (binding != nullptr) {
      std::string bindingName = binding->get<std::string>("binding");
      oo::PList bindingTypes = ResourceManager::shaderBindingTypesDictionary();
      const oo::PList* typeDict = bindingTypes.get<oo::PList::Dict>(
          "player");  // FIXME: select appropriate binding subset.
      std::optional<std::string> bindingType =
          (typeDict != nullptr) ? OptionalStringFor(*typeDict, bindingName)
                                : std::nullopt;
      std::optional<std::string_view> glslType;
      std::string_view swizzle;

      if (bindingType == "float") {
        glslType = "float";
      } else if (bindingType == "vector") {
        glslType = "vec3";
      } else if (bindingType == "color") {
        glslType = "vec4";
        swizzle = ".rgb";
      }

      if (glslType.has_value()) {
        std::optional<std::string> uniformName =
            defineBindingUniform(*binding, bindingType.value_or(std::string()));
        _fragmentBody +=
            std::format("\tlightMapColor *= {}{};\n",
                        uniformName.value_or(std::string()), swizzle);
      } else {
        if (!bindingType.has_value()) {
          OO_LOG_ERR("material.binding.error.unknown",
                     "Cannot bind light map to unknown attribute \"{}\".",
                     bindingName);
        } else {
          OO_LOG_ERR("material.binding.error.badType",
                     "Cannot bind light map to attribute \"{}\" of type {}.",
                     bindingName, *bindingType);
        }
        _fragmentBody +=
            "\tlightMapColor = vec3(0.0);  // Bad binding, see log.\n";
      }
    }

    if (!isIllumination) {
      _fragmentBody += "\ttotalColor += lightMapColor;\n\t\n";
    } else {
      _fragmentBody += "\tdiffuseLight += lightMapColor;\n\t\n";
    }
  }
}

void oo::DefaultShaderSynthesizer::writeVertexPosition() {
  _vertexBody +=
      "\tvec4 position = gl_ModelViewMatrix * gl_Vertex;\n"
      "\tgl_Position = gl_ProjectionMatrix * position;\n\t\n";
}

void oo::DefaultShaderSynthesizer::writeTotalColor() {
  _fragmentPreTextures += "\tvec3 totalColor = vec3(0.0);\n\t\n";
}

void oo::DefaultShaderSynthesizer::writeFinalColorComposite() {
  REQUIRE_STAGE(writeTotalColor);  // Needed even if none of the following
                                   // stages does anything.
  REQUIRE_STAGE(writeDiffuseLighting);
  REQUIRE_STAGE(writeSpecularLighting);
  REQUIRE_STAGE(writeLightMaps);

  if (_haveDiffuseLight) {
    _fragmentBody += "\ttotalColor += diffuseColor * diffuseLight;\n";
  }

  _fragmentBody += "\tgl_FragColor = vec4(totalColor, 1.0);\n\t\n";
}

namespace {

// A texture specifier naming just a file: a dictionary with the one key
// kOOTextureSpecifierNameKey.
oo::PList NameSpecifier(const std::string& name) {
  return oo::PList(
      oo::PList::Dict{{oo::kTextureSpecifierNameKey, oo::PList(name)}});
}

// A string becomes a name specifier, a dictionary is kept, anything else (or no
// value) is nil.
oo::PList StringOrDictionarySpecifier(const oo::PList* texSpec) {
  if (texSpec == nullptr) return oo::PList();
  if (const std::string* name = texSpec->getIf<std::string>())
    return NameSpecifier(*name);
  if (texSpec->isDict()) return *texSpec;
  return oo::PList();
}

// PList dict with one added entry on a dictionary specifier.
oo::PList AddingValue(oo::PList specifier, std::string_view key,
                      oo::PList value) {
  if (oo::PList::Dict* dict = specifier.getIf<oo::PList::Dict>())
    (*dict)[std::string(key)] = std::move(value);
  return specifier;
}

// OOColor::colorWithDescription() of the value for key (null if there is none).
std::shared_ptr<oo::Color> ColorIn(const oo::PList& spec,
                                   std::string_view key) {
  const oo::PList* value = spec.find(key);
  return oo::Color::colorWithDescription((value != nullptr) ? *value
                                                            : oo::PList());
}

// OOColor::normalizedArray(): four +numberWithFloat: components. (The callers
// never pass null.)
oo::PList NormalizedArray(oo::Color* color) {
  oo::PList::Array result;
  for (float component : color->normalizedArray())
    result.push_back(oo::PList::singleReal(component));
  return oo::PList(std::move(result));
}

/*
        Convert any legacy properties and simplified forms in a material
   specifier to the standard form.

        FIXME: this should be done up front in OOShipRegistry. When doing that,
   it also need to be done when materials are set on the fly through JS,
*/
oo::PList CanonicalizeMaterialSpecifier(
    const oo::PList& spec, const std::optional<std::string>& materialKey) {
  oo::PList::Dict result;
  std::shared_ptr<oo::Color> col;
  oo::PList texSpec;

  // Colours.
  col = ColorIn(spec, oo::kMaterialDiffuseColorName);
  if (col == nullptr) col = ColorIn(spec, oo::kMaterialDiffuseColorLegacyName);
  if (col != nullptr)
    result[oo::kMaterialDiffuseColorName] = NormalizedArray(col.get());

  col = ColorIn(spec, oo::kMaterialAmbientColorName);
  if (col == nullptr) col = ColorIn(spec, oo::kMaterialAmbientColorLegacyName);
  if (col != nullptr)
    result[oo::kMaterialAmbientColorName] = NormalizedArray(col.get());

  col = ColorIn(spec, oo::kMaterialSpecularColorName);
  if (col == nullptr) col = ColorIn(spec, oo::kMaterialSpecularColorLegacyName);
  if (col != nullptr)
    result[oo::kMaterialSpecularColorName] = NormalizedArray(col.get());

  col = ColorIn(spec, oo::kMaterialSpecularModulateColorName);
  if (col != nullptr)
    result[oo::kMaterialSpecularModulateColorName] = NormalizedArray(col.get());

  col = ColorIn(spec, oo::kMaterialEmissionColorName);
  if (col == nullptr) col = ColorIn(spec, oo::kMaterialEmissionColorLegacyName);
  if (col != nullptr)
    result[oo::kMaterialEmissionColorName] = NormalizedArray(col.get());

  // Diffuse map.
  const oo::PList* diffuseSpec = spec.find(oo::kMaterialDiffuseMapName);
  if (diffuseSpec != nullptr && diffuseSpec->isString()) {
    const std::string& name = *diffuseSpec->getIf<std::string>();
    texSpec = name.empty() ? *diffuseSpec : NameSpecifier(name);
  } else if (diffuseSpec != nullptr && diffuseSpec->isDict()) {
    /*	Special case for diffuse map: no name is changed to
            name = materialKey, while name = "" is changed to no name.
    */
    texSpec = *diffuseSpec;
    const oo::PList* name = diffuseSpec->find(oo::kTextureSpecifierNameKey);
    if (name == nullptr) {
      if (materialKey.has_value())
        texSpec = AddingValue(texSpec, oo::kTextureSpecifierNameKey,
                              oo::PList(*materialKey));
    } else if (name->isString() && name->getIf<std::string>()->empty()) {
      texSpec.getIf<oo::PList::Dict>()->erase(oo::kTextureSpecifierNameKey);
    }
  } else {
    // Special case for unspecified diffuse map. (The one caller always has a
    // material key.)
    texSpec = NameSpecifier(materialKey.value_or(std::string()));
  }
  result[oo::kMaterialDiffuseMapName] = texSpec;

  // Specular maps.
  {
    bool haveNewSpecular = false;
    texSpec = StringOrDictionarySpecifier(
        spec.find(oo::kMaterialSpecularColorMapName));
    if (!texSpec.isNull()) {
      haveNewSpecular = true;
      result[oo::kMaterialSpecularColorMapName] = texSpec;
    }

    texSpec = StringOrDictionarySpecifier(
        spec.find(oo::kMaterialSpecularExponentMapName));
    if (!texSpec.isNull()) {
      haveNewSpecular = true;
      result[oo::kMaterialSpecularExponentMapName] = texSpec;
    }

    if (!haveNewSpecular) {
      // Fall back to legacy combined specular map if defined.
      texSpec = StringOrDictionarySpecifier(
          spec.find(oo::kMaterialCombinedSpecularMapName));
      if (!texSpec.isNull()) {
        result[oo::kMaterialSpecularColorMapName] = texSpec;
        texSpec = AddingValue(texSpec, oo::kTextureSpecifierSwizzleKey,
                              oo::PList("a"));
        result[oo::kMaterialSpecularExponentMapName] = texSpec;
      }
    }
  }

  // Normal and parallax maps.
  {
    bool haveParallax = false;
    bool haveNewNormal = false;
    texSpec =
        StringOrDictionarySpecifier(spec.find(oo::kMaterialNormalMapName));
    if (!texSpec.isNull()) {
      haveNewNormal = true;
      result[oo::kMaterialNormalMapName] = texSpec;
    }

    texSpec =
        StringOrDictionarySpecifier(spec.find(oo::kMaterialParallaxMapName));
    if (!texSpec.isNull()) {
      haveNewNormal = true;
      haveParallax = true;
      result[oo::kMaterialParallaxMapName] = texSpec;
    }

    if (!haveNewNormal) {
      // Fall back to legacy combined normal and parallax map if defined.
      texSpec = StringOrDictionarySpecifier(
          spec.find(oo::kMaterialNormalAndParallaxMapName));
      if (!texSpec.isNull()) {
        haveParallax = true;
        result[oo::kMaterialNormalMapName] = texSpec;
        texSpec = AddingValue(texSpec, oo::kTextureSpecifierSwizzleKey,
                              oo::PList("a"));
        result[oo::kMaterialParallaxMapName] = texSpec;
      }
    }

    // Additional parallax parameters (-oo_setFloat:forKey: stores a double).
    if (haveParallax) {
      float parallaxScale = spec.get<float>(oo::kMaterialParallaxScaleName,
                                            kOOMaterialDefaultParallaxScale);
      result[oo::kMaterialParallaxScaleName] =
          oo::PList(static_cast<double>(parallaxScale));

      float parallaxBias = spec.get<float>(oo::kMaterialParallaxBiasName);
      result[oo::kMaterialParallaxBiasName] =
          oo::PList(static_cast<double>(parallaxBias));
    }
  }

  // Light maps.
  {
    oo::PList::Array lightMaps;
    oo::PList::Array lightMapSpecs;
    if (const oo::PList* value = spec.find(oo::kMaterialLightMapsName)) {
      if (const oo::PList::Array* array = value->getIf<oo::PList::Array>())
        lightMapSpecs = *array;
      else
        lightMapSpecs.push_back(*value);
    }

    for (const oo::PList& entry : lightMapSpecs) {
      oo::PList lmSpec;
      if (const std::string* name = entry.getIf<std::string>()) {
        lmSpec = NameSpecifier(*name);
      } else if (entry.isDict()) {
        lmSpec = entry;
      } else {
        continue;
      }
      oo::PList::Dict& lmDict = *lmSpec.getIf<oo::PList::Dict>();

      auto modulateColor = lmDict.find(oo::kTextureSpecifierModulateColorKey);
      if (modulateColor != lmDict.end() && !modulateColor->second.isArray()) {
        // Don't convert arrays here, because we specifically don't want the
        // behaviour of treating numbers greater than 1 as 0..255 components.
        col = oo::Color::colorWithDescription(modulateColor->second);
        // A description that is no colour leaves no colour (upstream set nil,
        // which Foundation refuses).
        if (col != nullptr)
          modulateColor->second = NormalizedArray(col.get());
        else
          lmDict.erase(modulateColor);
      }

      auto binding = lmDict.find(oo::kTextureSpecifierBindingKey);
      if (binding != lmDict.end()) {
        if (const std::string* bindingName =
                binding->second.getIf<std::string>()) {
          oo::PList expandedBinding(
              oo::PList::Dict{{"type", oo::PList("binding")},
                              {"binding", oo::PList(*bindingName)}});
          binding->second = std::move(expandedBinding);
        } else if (!binding->second.isDict() ||
                   binding->second.get<std::string>("binding").empty()) {
          lmDict.erase(binding);
        }
      }

      lightMaps.push_back(std::move(lmSpec));
    }

    if (lightMaps.empty()) {
      // If light_map isn't use, handle legacy emission_map, illumination_map
      // and emission_and_illumination_map.
      const oo::PList* emissionValue = spec.find(oo::kMaterialEmissionMapName);
      const oo::PList* illuminationValue =
          spec.find(oo::kMaterialIlluminationMapName);
      oo::PList emissionSpec =
          (emissionValue != nullptr) ? *emissionValue : oo::PList();
      oo::PList illuminationSpec =
          (illuminationValue != nullptr) ? *illuminationValue : oo::PList();

      if (emissionSpec.isNull() && illuminationSpec.isNull()) {
        // Redundantish string check required because we want to modify this as
        // a dictionary to make illuminationSpec.
        emissionSpec = StringOrDictionarySpecifier(
            spec.find(oo::kMaterialEmissionAndIlluminationMapName));

        if (!emissionSpec.isNull()) {
          illuminationSpec = AddingValue(
              emissionSpec, oo::kTextureSpecifierSwizzleKey, oo::PList("a"));
        }
      }

      if (!emissionSpec.isNull()) {
        if (const std::string* name = emissionSpec.getIf<std::string>()) {
          emissionSpec = NameSpecifier(*name);
        }
        if (emissionSpec.isDict()) {
          col = ColorIn(spec, oo::kMaterialEmissionModulateColorName);
          if (col != nullptr)
            emissionSpec =
                AddingValue(emissionSpec, oo::kTextureSpecifierModulateColorKey,
                            NormalizedArray(col.get()));

          lightMaps.push_back(emissionSpec);
        }
      }

      if (!illuminationSpec.isNull()) {
        if (const std::string* name = illuminationSpec.getIf<std::string>()) {
          illuminationSpec = NameSpecifier(*name);
        }
        if (illuminationSpec.isDict()) {
          col = ColorIn(spec, oo::kMaterialIlluminationModulateColorName);
          if (col != nullptr)
            illuminationSpec = AddingValue(
                illuminationSpec, oo::kTextureSpecifierModulateColorKey,
                NormalizedArray(col.get()));

          illuminationSpec = AddingValue(
              illuminationSpec, oo::kTextureSpecifierIlluminationModeKey,
              oo::PList(true));

          lightMaps.push_back(illuminationSpec);
        }
      }
    }

    result[oo::kMaterialLightMapsName] = oo::PList(std::move(lightMaps));
  }

  oo::PList canonical(std::move(result));
  OO_LOG("material.canonicalForm",
         "Canonicalized material {}:\nORIGINAL:\n{}\n\n@CANONICAL:\n{}",
         materialKey.value_or("(null)"), oo::DescriptionOf(spec),
         oo::DescriptionOf(canonical));

  return canonical;
}

std::string FormatFloat(double value) {
  std::int64_t intValue = static_cast<std::int64_t>(value);
  if (value == static_cast<double>(intValue)) {
    return std::format("{}.0", intValue);
  } else {
    return std::format("{:g}", value);
  }
}

}  // namespace
