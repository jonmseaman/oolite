/*

OOCombinedEmissionMapGenerator.cpp


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

#include "CombinedEmissionMapGenerator.h"

#include <algorithm>
#include <format>
#include <memory>
#include <optional>
#include <string>

#include "OOColor.h"
#include "OOTextureScaling.h"
#include "PixMapChannelOperations.h"
#include "TextureInternal.h"
#include "TextureLoader.h"
#include "oofnd/Log.hpp"
#include "oofnd/objc/Assert.h"

#define DUMP_COMBINER 0

namespace {

oo::PList SpecWithExtraShrink(const oo::PList& spec) {
  if (spec.isNull()) return oo::PList();
  oo::PList copy = spec;
  if (oo::PList::Dict* dict = copy.getIf<oo::PList::Dict>()) {
    (*dict)[oo::kTextureSpecifierExtraShrinkKey] = oo::PList(true);
  }
  return copy;
}

std::shared_ptr<OOColor> ModulateColor(OOColor* a, OOColor* b) {
  if (a == nullptr) return oo::share(b);
  if (b == nullptr) return oo::share(a);

  OORGBAComponents ac, bc;
  ac = a->rgbaComponents();
  bc = b->rgbaComponents();

  ac.r *= bc.r;
  ac.g *= bc.g;
  ac.b *= bc.b;
  ac.a *= bc.a;

  return OOColor::colorWithRGBAComponents(ac);
}

void ScaleToMatch(oo::PixMap* pmA, oo::PixMap* pmB) {
  OOCParameterAssert(pmA != nullptr && pmB != nullptr &&
                     oo::IsValidPixMap(*pmA) && oo::IsValidPixMap(*pmB));

  oo::PixMapDimension minWidth = std::min(pmA->width, pmB->width);
  oo::PixMapDimension minHeight = std::min(pmA->height, pmB->height);

  if (pmA->width != minWidth || pmA->height != minHeight) {
    *pmA = OOScalePixMap(*pmA, minWidth, minHeight, false);
  }
  if (pmB->width != minWidth || pmB->height != minHeight) {
    *pmB = OOScalePixMap(*pmB, minWidth, minHeight, false);
  }
}

}  // namespace

std::shared_ptr<oo::CombinedEmissionMapGenerator>
oo::CombinedEmissionMapGenerator::generatorWithEmissionMapSpec(
    const oo::PList& emissionMapSpec, OOColor* emissionColor,
    oo::Texture* diffuseMap, OOColor* diffuseColor,
    const oo::PList& illuminationMapSpec, OOColor* illuminationColor,
    const oo::PList& spec) {
  std::shared_ptr<oo::CombinedEmissionMapGenerator> result =
      oo::makeShared<oo::CombinedEmissionMapGenerator>();
  if (!result->initWithEmissionMapSpec(
          emissionMapSpec, emissionColor, diffuseMap, diffuseColor,
          illuminationMapSpec, illuminationColor, false, spec))
    return {};
  return result;
}

std::shared_ptr<oo::CombinedEmissionMapGenerator>
oo::CombinedEmissionMapGenerator::generatorWithEmissionAndIlluminationMapSpec(
    const oo::PList& emissionAndIlluminationMapSpec, oo::Texture* diffuseMap,
    OOColor* diffuseColor, OOColor* emissionColor, OOColor* illuminationColor,
    const oo::PList& spec) {
  std::shared_ptr<oo::CombinedEmissionMapGenerator> result =
      oo::makeShared<oo::CombinedEmissionMapGenerator>();
  if (!result->initWithEmissionMapSpec(
          emissionAndIlluminationMapSpec, emissionColor, diffuseMap,
          diffuseColor, oo::PList(), illuminationColor, true, spec))
    return {};
  return result;
}

// Was the private designated initialiser; false where it answered nil.
bool oo::CombinedEmissionMapGenerator::initWithEmissionMapSpec(
    const oo::PList& emissionMapSpec, OOColor* emissionColor,
    oo::Texture* diffuseMap, OOColor* diffuseColor,
    const oo::PList& illuminationMapSpec, OOColor* illuminationColor,
    bool isCombinedMap, const oo::PList& spec) {
  if (emissionMapSpec.isNull() && illuminationMapSpec.isNull()) {
    return false;
  }

  OOCParameterAssert(illuminationMapSpec.isNull() || !isCombinedMap);

  // A specifier that is not one leaves the defaults (they were uninitialised).
  const oo::TextureSpecifier interpreted =
      oo::InterpretTextureSpecifier(spec, true)
          .value_or(oo::TextureSpecifier{"", oo::TextureFlags::kDefaultOptions,
                                         kOOTextureDefaultAnisotropy,
                                         kOOTextureDefaultLODBias});
  oo::TextureFlags options =
      oo::ApplyTextureOptionDefaults(interpreted.options);
  const GLfloat anisotropy = interpreted.anisotropy;
  const GLfloat lodBias = interpreted.lodBias;

  if (initWithPath(std::string("<generated emission map>"), options)) {
    /*	Illumination contribution is:
            illuminationMap * illuminationColor * diffuseMap * diffuseColor
            Since illuminationColor and diffuseColor aren't used otherwise,
            we may as well combine them up front.
    */
    const std::shared_ptr<OOColor> modulatedIlluminationColor =
        ModulateColor(diffuseColor, illuminationColor);
    illuminationColor = modulatedIlluminationColor.get();

    if (emissionColor != nullptr && emissionColor->isWhite())
      emissionColor = nullptr;
    if (illuminationColor != nullptr && illuminationColor->isWhite())
      illuminationColor = nullptr;
    if (!isCombinedMap && illuminationMapSpec.isNull())
      diffuseMap = nullptr;  // Diffuse map is only used with illumination

    // Insert extraShrink flag here instead of using extraOptions later because
    // we need it in cache key too.
    _emissionSpec = SpecWithExtraShrink(emissionMapSpec);
    _illuminationSpec = SpecWithExtraShrink(illuminationMapSpec);

    _diffuseMap = oo::share(diffuseMap);

    _emissionColor = oo::share(emissionColor);
    _illuminationColor = oo::share(illuminationColor);
    _isCombinedMap = isCombinedMap;

    _textureOptions = options;
    _anisotropy = anisotropy;
    _lodBias = lodBias;

    _cacheKey = constructCacheKey();

    if (oo::Texture::existingTextureForKey(_cacheKey) == nullptr) {
      /*	Extract pixmap from diffuse map. This must be done in the main
              thread even if scheduling is fixed, because it might involve
              reading back pixels from OpenGL.
      */
      if (diffuseMap != nullptr) {
        _diffusePx = diffuseMap->copyPixMapRepresentation();
#ifndef NDEBUG
        _diffuseDesc = diffuseMap->shortDescription();
#endif
      }

      /*	Extract emission and illumination pixmaps from loaders. Ideally,
              this would be done asynchronously, but that requires dependency
              management in OOAsyncWorkManager.
      */
      // A loader that fails leaves a null pixmap, as its getResult did.
      auto PixMapOf = [](oo::TextureLoader& loader) {
        const std::optional<oo::TextureLoader::Result> loaded =
            loader.getResult();
        return loaded.has_value() ? loaded->pixMap : oo::kNullPixMap;
      };
      if (!_emissionSpec.isNull()) {
        const std::shared_ptr<oo::TextureLoader> emissionMapLoader =
            oo::TextureLoader::loaderWithTextureSpecifier(
                _emissionSpec, oo::TextureFlags::kNone,
                std::string("Textures"));
        if (emissionMapLoader) _emissionPx = PixMapOf(*emissionMapLoader);
#ifndef NDEBUG
        _emissionDesc =
            (emissionMapLoader ? emissionMapLoader->shortDescription()
                               : std::string());
#endif
      }
      if (!_illuminationSpec.isNull()) {
        const std::shared_ptr<oo::TextureLoader> illuminationMapLoader =
            oo::TextureLoader::loaderWithTextureSpecifier(
                _illuminationSpec, oo::TextureFlags::kNone,
                std::string("Textures"));
        if (illuminationMapLoader)
          _illuminationPx = PixMapOf(*illuminationMapLoader);
#ifndef NDEBUG
        _illuminationDesc =
            (illuminationMapLoader ? illuminationMapLoader->shortDescription()
                                   : std::string());
#endif
      }
    }
    return true;
  }

  return false;
}

std::string oo::CombinedEmissionMapGenerator::constructCacheKey() {
  std::string cacheKey;

  if (_isCombinedMap)
    cacheKey += "combined emission and illumination map;";
  else if (_emissionSpec.isNull())
    cacheKey += "illumination map;";
  else if (_illuminationSpec.isNull())
    cacheKey += "emission map;";
  else
    cacheKey += "merged emission and illumination map;";

  std::string emissionDesc;
  if (!_emissionSpec.isNull()) {
    emissionDesc = oo::TextureCacheKeyForSpecifier(_emissionSpec);
    cacheKey += std::format("emission:{{{}}}", emissionDesc);
    if (_emissionColor) {
      const std::optional<std::string> rgba = _emissionColor->rgbaDescription();
      cacheKey += std::format("*{}", rgba ? rgba->c_str() : "");
    }
    cacheKey += ";";
  }

  std::string illuminationDesc;
  if (_isCombinedMap) {
    illuminationDesc = emissionDesc + ":a";
  } else if (!_illuminationSpec.isNull()) {
    illuminationDesc = oo::TextureCacheKeyForSpecifier(_illuminationSpec);
  }

  if (!illuminationDesc.empty()) {
    cacheKey += std::format(
        "illumination:{{{}}}*{{{}}}", illuminationDesc,
        (_diffuseMap ? _diffuseMap->cacheKey() : std::nullopt).value_or(""));
    if (_illuminationColor) {
      const std::optional<std::string> rgba =
          _illuminationColor->rgbaDescription();
      cacheKey += std::format("*{}", rgba ? rgba->c_str() : "");
    }
    cacheKey += ";";
  }

  return cacheKey;
}

// Was -dealloc: the specs, the diffuse map and the colours are released with
// their members.
namespace oo {

CombinedEmissionMapGenerator::~CombinedEmissionMapGenerator() {
  FreePixMap(&_emissionPx);
  FreePixMap(&_illuminationPx);
  FreePixMap(&_diffusePx);
}

}  // namespace oo

#ifndef NDEBUG
std::optional<std::string>
oo::CombinedEmissionMapGenerator::descriptionComponents() const {
  std::string result;
  bool haveIllumination = false;

  if (!_emissionDesc.empty()) {
    result += std::format("emission map: {}", _emissionDesc);
    if (_isCombinedMap) {
      result += ".rgb";
    }
    if (_emissionColor) {
      const std::optional<std::string> rgba = _emissionColor->rgbaDescription();
      result += std::format(" * {}", rgba ? rgba->c_str() : "");
    }

    if (_isCombinedMap) {
      result += std::format(", illumination map: {}.a", _emissionDesc);
      haveIllumination = true;
    }
  }

  if (!_illuminationDesc.empty()) {
    if (!_emissionDesc.empty()) result += ", ";
    result += std::format("illumination map: {}", _illuminationDesc);
    haveIllumination = true;
  }

  if (haveIllumination) {
    if (!_diffuseDesc.empty()) {
      result += std::format(" * {}", _diffuseDesc);
    }
    if (_illuminationColor) {
      const std::optional<std::string> rgba =
          _illuminationColor->rgbaDescription();
      result += std::format(" * {}", rgba ? rgba->c_str() : "");
    }
  }

  return result;
}
#endif

oo::TextureFlags oo::CombinedEmissionMapGenerator::textureOptions() {
  return _textureOptions;
}

GLfloat oo::CombinedEmissionMapGenerator::anisotropy() { return _anisotropy; }

GLfloat oo::CombinedEmissionMapGenerator::lodBias() { return _lodBias; }

std::optional<std::string> oo::CombinedEmissionMapGenerator::cacheKey() {
  return _cacheKey;
}

void oo::CombinedEmissionMapGenerator::loadTexture() {
  oo::PixMap illuminationPx = oo::kNullPixMap;
  bool haveEmission = false, haveIllumination = false, haveDiffuse = false;

#if DUMP_COMBINER
  static unsigned sTexID = 0;
  unsigned texID = ++sTexID, dumpCount = 0;

#define DUMP(pm, label) \
  oo::DumpPixMap(       \
      pm, std::format("lightmap {}.{} - {}", texID, ++dumpCount, label));
#else
#define DUMP(pm, label) \
  do {                  \
  } while (0)
#endif

  haveEmission = !oo::IsNullPixMap(_emissionPx);
  if (haveEmission) DUMP(_emissionPx, "source emission map");

  // Extract illumination component if emission_and_illumination_map.
  if (haveEmission && _isCombinedMap &&
      oo::PixMapFormatHasAlpha(_emissionPx.format)) {
    oo::PixMapToRGBA(&_emissionPx);
    illuminationPx = oo::DuplicatePixMap(_emissionPx, 0);
    oo::ExtractPixMapChannel(&illuminationPx, 3, true);
    haveIllumination = true;
    DUMP(illuminationPx, "extracted illumination map");
  }

  // Tint emission map if necessary.
  if (haveEmission && _emissionColor) {
    oo::PixMapModulateUniform(&_emissionPx, _emissionColor->redComponent(),
                              _emissionColor->greenComponent(),
                              _emissionColor->blueComponent(), 1.0);
    DUMP(_emissionPx, "modulated emission map");
  }

  if (!oo::IsNullPixMap(_illuminationPx)) {
    OOCAssert(!_isCombinedMap,
              "OOCombinedEmissionMapGenerator configured with both "
              "illumination map and combined emission/illumination map.");

    illuminationPx = _illuminationPx;
    _illuminationPx.pixels = nullptr;
    haveIllumination = true;
    DUMP(illuminationPx, "source illumination map");
  }

  // Tint illumination map if necessary.
  if (haveIllumination && _illuminationColor) {
    oo::PixMapModulateUniform(&illuminationPx,
                              _illuminationColor->redComponent(),
                              _illuminationColor->greenComponent(),
                              _illuminationColor->blueComponent(), 1.0);
    DUMP(illuminationPx, "modulated illumination map");
  }

  // Load diffuse map and combine with illumination map.
  haveDiffuse = !oo::IsNullPixMap(_diffusePx);
  if (haveDiffuse) DUMP(_diffusePx, "source diffuse map");

  if (haveIllumination && haveDiffuse) {
    // Modulate illumination with diffuse map.
    ScaleToMatch(&_diffusePx, &illuminationPx);
    oo::PixMapToRGBA(&_diffusePx);
    oo::PixMapModulatePixMap(&illuminationPx, _diffusePx);
    DUMP(illuminationPx, "combined diffuse and illumination map");
  }
  oo::FreePixMap(&_diffusePx);

  if (haveIllumination) {
    if (haveEmission) {
      oo::PixMapToRGBA(&illuminationPx);
      oo::PixMapAddPixMap(&_emissionPx, illuminationPx);
      oo::FreePixMap(&illuminationPx);
      DUMP(_emissionPx, "combined emission and illumination map");
    } else if (haveIllumination) {
      // No explicit emission map -> modulated illumination map is our only
      // emission map.
      _emissionPx = illuminationPx;
      haveEmission = true;
      illuminationPx.pixels = nullptr;
    }
    haveIllumination =
        false;  // Either way, illumination is now baked into emission.
  }

  (void)haveEmission;
  (void)haveIllumination;

  // Done: emissionPx now contains combined emission map.
  oo::CompactPixMap(&_emissionPx);
  if (oo::IsValidPixMap(_emissionPx)) {
    _data = oo::PixMapOwnedBuffer(_emissionPx);
    _width = _emissionPx.width;
    _height = _emissionPx.height;
    _rowBytes = _emissionPx.rowBytes;
    _format = _emissionPx.format;

    oo::FreePixMap(&_emissionPx);  // the pixels' ownership passes to _data
  }
  if (_data == nullptr) {
    OO_LOG_ERR("texture.combinedEmissionMap.error", "Unknown error loading {}",
               description());
  }
}
