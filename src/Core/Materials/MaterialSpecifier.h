/*

OOMaterialSpecifier.h

Key declarations and convenience methods for material specifiers.


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

#pragma once

#include <memory>
#include <string>

#include "Color.h"  // the colour readers answer std::shared_ptr<OOColor>
#include "Foundation.h"
#include "oofnd/PList.hpp"

/*	Material configuration properties. A material configuration is an
   oo::PList dictionary: property-list data plus PList::Object nodes for the
   live objects callers put in it (colours, textures); a null PList is a nil
   configuration, and every function then answers as a message to nil did.
   Specifiers are oo::PList dictionaries (null = nil).
*/
namespace oo {

std::shared_ptr<oo::Color> MaterialDiffuseColor(const oo::PList& configuration);
std::shared_ptr<oo::Color> MaterialAmbientColor(const oo::PList& configuration);
std::shared_ptr<oo::Color> MaterialSpecularColor(
    const oo::PList& configuration);
std::shared_ptr<oo::Color> MaterialSpecularModulateColor(
    const oo::PList& configuration);
std::shared_ptr<oo::Color> MaterialEmissionColor(
    const oo::PList& configuration);
std::shared_ptr<oo::Color> MaterialEmissionModulateColor(
    const oo::PList& configuration);
std::shared_ptr<oo::Color> MaterialIlluminationModulateColor(
    const oo::PList& configuration);

oo::PList MaterialDiffuseMapSpecifier(
    const oo::PList& configuration,
    const std::optional<std::string>& defaultName);
oo::PList MaterialCombinedSpecularMapSpecifier(const oo::PList& configuration);
oo::PList MaterialSpecularColorMapSpecifier(const oo::PList& configuration);
oo::PList MaterialSpecularExponentMapSpecifier(const oo::PList& configuration);
oo::PList MaterialNormalMapSpecifier(const oo::PList& configuration);
oo::PList MaterialParallaxMapSpecifier(const oo::PList& configuration);
oo::PList MaterialNormalAndParallaxMapSpecifier(const oo::PList& configuration);
oo::PList MaterialEmissionMapSpecifier(const oo::PList& configuration);
oo::PList MaterialIlluminationMapSpecifier(const oo::PList& configuration);
oo::PList MaterialEmissionAndIlluminationMapSpecifier(
    const oo::PList& configuration);

float MaterialParallaxScale(const oo::PList& configuration);
float MaterialParallaxBias(const oo::PList& configuration);
bool MaterialGammaCorrect(const oo::PList& configuration);
float MaterialGloss(const oo::PList& configuration);
int MaterialSpecularExponent(const oo::PList& configuration);

// Configuration keys.
inline constexpr const char* kMaterialDiffuseColorName = "diffuse_color";
inline constexpr const char* kMaterialDiffuseColorLegacyName = "diffuse";
inline constexpr const char* kMaterialAmbientColorName = "ambient_color";
inline constexpr const char* kMaterialAmbientColorLegacyName = "ambient";
inline constexpr const char* kMaterialSpecularColorName = "specular_color";
inline constexpr const char* kMaterialSpecularColorLegacyName = "specular";
inline constexpr const char* kMaterialSpecularModulateColorName =
    "specular_modulate_color";
inline constexpr const char* kMaterialEmissionColorName = "emission_color";
inline constexpr const char* kMaterialEmissionColorLegacyName = "emission";
inline constexpr const char* kMaterialEmissionModulateColorName =
    "emission_modulate_color";
inline constexpr const char* kMaterialIlluminationModulateColorName =
    "illumination_modulate_color";
inline constexpr const char* kMaterialDiffuseMapName = "diffuse_map";
inline constexpr const char* kMaterialSpecularColorMapName =
    "specular_color_map";
inline constexpr const char* kMaterialSpecularExponentMapName =
    "specular_exponent_map";
// Combined specular_color_map and specular_exponent_map (unfortunate name
// required for backwards-compatibility).
inline constexpr const char* kMaterialCombinedSpecularMapName = "specular_map";
inline constexpr const char* kMaterialNormalMapName = "normal_map";
inline constexpr const char* kMaterialParallaxMapName = "parallax_map";
inline constexpr const char* kMaterialNormalAndParallaxMapName =
    "normal_and_parallax_map";
inline constexpr const char* kMaterialEmissionMapName = "emission_map";
inline constexpr const char* kMaterialIlluminationMapName = "illumination_map";
inline constexpr const char* kMaterialEmissionAndIlluminationMapName =
    "emission_and_illumination_map";
inline constexpr const char* kMaterialParallaxScaleName = "parallax_scale";
inline constexpr const char* kMaterialParallaxBiasName = "parallax_bias";
inline constexpr const char* kMaterialGammaCorrectName = "gamma_correct";
inline constexpr const char* kMaterialGlossName = "gloss";
inline constexpr const char* kMaterialSpecularExponentName =
    "specular_exponent";
inline constexpr const char* kMaterialSpecularExponentLegacyName = "shininess";
inline constexpr const char* kMaterialLightMapsName = "light_map";

}  // namespace oo

#define kOOMaterialDefaultParallaxScale (0.01f)
