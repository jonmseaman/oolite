/*

OOMaterialSpecifier.m


Copyright (C) 2010-2013 Jens Ayton

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

#include "MaterialSpecifier.h"

#include <memory>
#include <string>
#include <string_view>

#include "OOColor.h"
#include "Texture.h"
#include "Universe.h"
#include "oofnd/Defaults.hpp"

namespace {

// The value for key; null if there is none.
oo::PList ValueFor(const oo::PList& configuration, std::string_view key) {
  const oo::PList* value = configuration.find(key);
  return value != nullptr ? *value : oo::PList();
}

std::shared_ptr<OOColor> ColorFor(const oo::PList& configuration,
                                  std::string_view key) {
  return OOColor::colorWithDescription(ValueFor(configuration, key));
}

// OOTextureSpecFromObject() applied to the value for key, as
// -oo_textureSpecifierForKey:defaultName: did.
oo::PList TextureSpecifierFor(const oo::PList& configuration,
                              std::string_view key,
                              const std::optional<std::string>& defaultName) {
  return oo::TextureSpecFromObject(ValueFor(configuration, key), defaultName);
}

// PList dict with extract_channel set (nil stays nil).
oo::PList AddingExtractChannelA(oo::PList specifier) {
  if (oo::PList::Dict* dict = specifier.getIf<oo::PList::Dict>())
    (*dict)["extract_channel"] = oo::PList("a");
  return specifier;
}

// Internal. Used to avoid mutual recusion between the specular exponent map
// specifier and the specular exponent.
int RawSpecularExponentValue(const oo::PList& configuration) {
  const oo::PList* value =
      configuration.find(oo::kMaterialSpecularExponentName);
  if (value == nullptr)
    value = configuration.find(oo::kMaterialSpecularExponentLegacyName);
  return oo::PListGet<int>::from(value, -1);
}

}  // namespace

namespace oo {

std::shared_ptr<OOColor> MaterialDiffuseColor(const oo::PList& configuration) {
  std::shared_ptr<OOColor> result =
      ColorFor(configuration, kMaterialDiffuseColorName);
  if (result == nullptr)
    result = ColorFor(configuration, kMaterialDiffuseColorLegacyName);

  if (result != nullptr && result->isWhite())
    result = nullptr;  // -isWhite of nil was NO
  return result;
}

std::shared_ptr<OOColor> MaterialAmbientColor(const oo::PList& configuration) {
  std::shared_ptr<OOColor> result =
      ColorFor(configuration, kMaterialAmbientColorName);
  if (result == nullptr)
    result = ColorFor(configuration, kMaterialAmbientColorLegacyName);
  return result;
}

std::shared_ptr<OOColor> MaterialSpecularColor(const oo::PList& configuration) {
  std::shared_ptr<OOColor> result =
      ColorFor(configuration, kMaterialSpecularColorName);
  if (result == nullptr)
    result = ColorFor(configuration, kMaterialSpecularColorLegacyName);
  if (result == nullptr) {
    result = OOColor::colorWithWhite(0.2f, 1.0f);
  }
  return result;
}

std::shared_ptr<OOColor> MaterialSpecularModulateColor(
    const oo::PList& configuration) {
  std::shared_ptr<OOColor> result =
      ColorFor(configuration, kMaterialSpecularModulateColorName);
  if (result == nullptr) result = OOColor::whiteColor();

  return result;
}

std::shared_ptr<OOColor> MaterialEmissionColor(const oo::PList& configuration) {
  std::shared_ptr<OOColor> result =
      ColorFor(configuration, kMaterialEmissionColorName);
  if (result == nullptr)
    result = ColorFor(configuration, kMaterialEmissionColorLegacyName);

  if (result != nullptr && result->isBlack())
    result = nullptr;  // -isBlack of nil was NO
  return result;
}

std::shared_ptr<OOColor> MaterialEmissionModulateColor(
    const oo::PList& configuration) {
  std::shared_ptr<OOColor> result =
      ColorFor(configuration, kMaterialEmissionModulateColorName);

  if (result != nullptr && result->isWhite())
    result = nullptr;  // -isWhite of nil was NO
  return result;
}

std::shared_ptr<OOColor> MaterialIlluminationModulateColor(
    const oo::PList& configuration) {
  std::shared_ptr<OOColor> result =
      ColorFor(configuration, kMaterialIlluminationModulateColorName);

  if (result != nullptr && result->isWhite())
    result = nullptr;  // -isWhite of nil was NO
  return result;
}

oo::PList MaterialDiffuseMapSpecifier(
    const oo::PList& configuration,
    const std::optional<std::string>& defaultName) {
  return TextureSpecifierFor(configuration, kMaterialDiffuseMapName,
                             defaultName);
}

oo::PList MaterialCombinedSpecularMapSpecifier(const oo::PList& configuration) {
  if (RawSpecularExponentValue(configuration) == 0) return oo::PList();
  return TextureSpecifierFor(configuration, kMaterialCombinedSpecularMapName,
                             std::nullopt);
}

oo::PList MaterialSpecularColorMapSpecifier(const oo::PList& configuration) {
  if (RawSpecularExponentValue(configuration) == 0) return oo::PList();
  oo::PList result = TextureSpecifierFor(
      configuration, kMaterialSpecularColorMapName, std::nullopt);
  if (result.isNull())
    result = MaterialCombinedSpecularMapSpecifier(configuration);
  return result;
}

oo::PList MaterialSpecularExponentMapSpecifier(const oo::PList& configuration) {
  if (RawSpecularExponentValue(configuration) == 0) return oo::PList();
  oo::PList result = TextureSpecifierFor(
      configuration, kMaterialSpecularExponentMapName, std::nullopt);
  if (result.isNull())
    result = AddingExtractChannelA(
        MaterialCombinedSpecularMapSpecifier(configuration));
  return result;
}

oo::PList MaterialNormalMapSpecifier(const oo::PList& configuration) {
  if (!MaterialNormalAndParallaxMapSpecifier(configuration).isNull())
    return oo::PList();
  return TextureSpecifierFor(configuration, kMaterialNormalMapName,
                             std::nullopt);
}

oo::PList MaterialParallaxMapSpecifier(const oo::PList& configuration) {
  oo::PList spec = TextureSpecifierFor(configuration, kMaterialParallaxMapName,
                                       std::nullopt);
  if (spec.isNull()) {
    // Default is alpha channel of normal_and_parallax_map.
    spec = AddingExtractChannelA(
        MaterialNormalAndParallaxMapSpecifier(configuration));
  }

  return spec;
}

oo::PList MaterialNormalAndParallaxMapSpecifier(
    const oo::PList& configuration) {
  return TextureSpecifierFor(configuration, kMaterialNormalAndParallaxMapName,
                             std::nullopt);
}

oo::PList MaterialEmissionMapSpecifier(const oo::PList& configuration) {
  return TextureSpecifierFor(configuration, kMaterialEmissionMapName,
                             std::nullopt);
}

oo::PList MaterialIlluminationMapSpecifier(const oo::PList& configuration) {
  return TextureSpecifierFor(configuration, kMaterialIlluminationMapName,
                             std::nullopt);
}

oo::PList MaterialEmissionAndIlluminationMapSpecifier(
    const oo::PList& configuration) {
  if (!MaterialEmissionMapSpecifier(configuration).isNull() ||
      !MaterialIlluminationMapSpecifier(configuration).isNull())
    return oo::PList();
  return TextureSpecifierFor(
      configuration, kMaterialEmissionAndIlluminationMapName, std::nullopt);
}

float MaterialParallaxScale(const oo::PList& configuration) {
  return configuration.get<float>(kMaterialParallaxScaleName,
                                  kOOMaterialDefaultParallaxScale);
}

float MaterialParallaxBias(const oo::PList& configuration) {
  return configuration.get<float>(kMaterialParallaxBiasName);
}

bool MaterialGammaCorrect(const oo::PList& configuration) {
  return configuration.get<bool>(
      kMaterialGammaCorrectName,
      !oo::Defaults::standard().boolForKey("no-gamma-correct"));
}

float MaterialGloss(const oo::PList& configuration) {
  return OOClamp_0_1_f(configuration.get<float>(kMaterialGlossName, 0.375f));
}

int MaterialSpecularExponent(const oo::PList& configuration) {
  int result = RawSpecularExponentValue(configuration);
  if (result < 0) {
    if ((UNIVERSE != nullptr ? UNIVERSE->useShaders() : false) &&
        !MaterialSpecularExponentMapSpecifier(configuration).isNull()) {
      result = 128;
    } else {
      result = 10;
    }
  }

  return result;
}

}  // namespace oo
