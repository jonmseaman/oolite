/*

OOTextureLoader.m

Copyright (C) 2007-2014 Jens Ayton

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

// Kept first, in a block of its own: which header comes first decides whether
// MinGW's or the UCRT's printf/strtod family is used.
#include "TextureLoader.h"

#include <stdlib.h>

#include <algorithm>
#include <cstring>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeinfo>
#include <utility>

#include "ConvertCubeMapToLatLong.h"
#include "DebugStandards.h"
#include "Demangle.h"
#include "Headless.h"
#include "PNGTextureLoader.h"
#include "PixMapChannelOperations.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "TextureGenerator.h"
#include "TextureScaling.h"
#include "Universe.h"
#include "oofnd/Defaults.hpp"
#include "oofnd/Exception.hpp"
#include "oofnd/PListGet.hpp"
#include "oofnd/String.hpp"
#include "oofnd/objc/Assert.h"

#define DUMP_CONVERTED_CUBE_MAPS 0

// Thresholds for reduced-detail texture shrinking in different circumstances.
constexpr unsigned kNeverShrinkThreshold = UINT32_MAX;
constexpr unsigned kDefaultShrinkThreshold = 512;
constexpr unsigned kExtraShrinkThreshold = 128;
constexpr unsigned kExtraShrinkMaxSize = 256;
constexpr unsigned kCubeShrinkThreshold = 256;

static unsigned sGLMaxSize;
static uint32_t sUserMaxSize;
static bool sReducedDetail;
static bool sHaveNPOTTextures =
    false;  // TODO: support "true" non-power-of-two textures.
static bool sHaveSetUp = false;

std::shared_ptr<oo::TextureLoader> oo::TextureLoader::loaderWithPath(
    const std::optional<std::string>& inPath, oo::TextureFlags options) {
  std::string extension;
  std::shared_ptr<oo::TextureLoader> result;

  if (!inPath.has_value()) [[unlikely]]
    return nullptr;
  if (!sHaveSetUp) [[unlikely]]
    setUp();

  // Get reduced detail setting (every time, in case it changes; we don't want
  // to call through to Universe on the loading thread in case the
  // implementation becomes non-trivial).
  sReducedDetail = (UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false);

  // Get a suitable loader. FIXME -- this should sniff the data instead of
  // relying on extensions.
  extension = oo::str::lowercase(oo::str::pathExtension(*inPath));
  if (extension == "png") {
    // A C++ loader; the work manager retains it while it is queued.
    const std::shared_ptr<::oo::PNGTextureLoader> loader =
        oo::makeShared<::oo::PNGTextureLoader>();
    if (loader->initWithPath(inPath, options)) result = loader;
  } else {
    OO_LOG("texture.load.unknownType",
           "Can't use {} as a texture - extension \"{}\" does not identify a "
           "known type.",
           *inPath, extension);
  }

  if (result != nullptr) {
    if (!oo::AsyncWorkManager::sharedAsyncWorkManager()->addTask(
            result.get(), oo::AsyncWorkPriority::kMedium))
      result = nullptr;
  }

  return result;
}

std::shared_ptr<oo::TextureLoader>
oo::TextureLoader::loaderWithTextureSpecifier(
    const oo::PList& specifier, oo::TextureFlags extraOptions,
    const std::optional<std::string>& folder) {
  std::optional<std::string> path;

  const std::optional<oo::TextureSpecifier> spec =
      oo::InterpretTextureSpecifier(specifier, false);
  if (!spec.has_value()) return nullptr;
  const std::string& name = spec->name;
  oo::TextureFlags options = spec->options | extraOptions;
  path = ResourceManager::pathForFileNamed(name, folder);
  if (!path.has_value()) {
    if (!oo::Any(options & oo::TextureFlags::kNoFNFMessage)) {
      OO_LOG_WARN(oo::kLogFileNotFound, "Could not find texture file \"{}\".",
                  name);
      oo::StandardsError("Texture file not found");
    }
    return nullptr;
  }

  return loaderWithPath(path, options);
}

oo::TextureLoader::TextureLoader() {}

/*	The part of -initWithPath:options: after [super init]; false where
   it released the receiver and answered nil.
*/
bool oo::TextureLoader::initWithPath(const std::optional<std::string>& inPath,
                                     oo::TextureFlags options) {
  if (!inPath.has_value()) [[unlikely]] {
    return false;
  }
  _path = *inPath;

  _options = options;

  _maxSize = std::min(sUserMaxSize, sGLMaxSize);

  _generateMipMaps = (options & oo::TextureFlags::kMinFilterMask) ==
                     oo::TextureFlags::kMinFilterMipMap;
  _avoidShrinking = oo::Any(options & oo::TextureFlags::kNoShrink);
  _noScalingWhatsoever = oo::Any(options & oo::TextureFlags::kNeverScale);
  if (_avoidShrinking || _noScalingWhatsoever) {
    _shrinkThreshold = kNeverShrinkThreshold;
  } else if (oo::Any(options & oo::TextureFlags::kExtraShrink)) {
    _shrinkThreshold = kExtraShrinkThreshold;
    _maxSize = std::min(_maxSize, static_cast<uint32_t>(kExtraShrinkMaxSize));
  } else {
    _shrinkThreshold = kDefaultShrinkThreshold;
  }
#if OO_TEXTURE_CUBE_MAP
  _allowCubeMap = oo::Any(options & oo::TextureFlags::kAllowCubeMap);
#endif

  if (oo::Any(options & oo::TextureFlags::kExtractChannelMask)) {
    _extractChannel = true;
    switch (options & oo::TextureFlags::kExtractChannelMask) {
      case oo::TextureFlags::kExtractChannelR:
        _extractChannelIndex = 0;
        break;

      case oo::TextureFlags::kExtractChannelG:
        _extractChannelIndex = 1;
        break;

      case oo::TextureFlags::kExtractChannelB:
        _extractChannelIndex = 2;
        break;

      case oo::TextureFlags::kExtractChannelA:
        _extractChannelIndex = 3;
        break;

      default:
        OO_LOG_ERR(
            "texture.load.unknownExtractChannelMask",
            "Unknown texture extract channel mask (0x{:04X}). This is an "
            "internal error, please report it.",
            static_cast<unsigned>(options &
                                  oo::TextureFlags::kExtractChannelMask));
        _extractChannel = false;
    }
  }

  return true;
}

namespace oo {

TextureLoader::~TextureLoader() = default;

}  // namespace oo

std::optional<std::string> oo::TextureLoader::descriptionComponents() const {
  std::string_view state;

  if (_ready) {
    if (_data != nullptr)
      state = "ready";
    else
      state = "failed";
  } else {
    state = "loading";
#if INSTRUMENT_TEXTURE_LOADING
    if (debugHasLoaded) state = "loaded";
#endif
  }

  return std::format("{{{} -- {}}}", _path, state);
}

std::optional<std::string> oo::TextureLoader::shortDescriptionComponents()
    const {
  return oo::str::lastPathComponent(_path);
}

namespace {

std::string DemangledName(const std::type_info& type) {
  std::string result = oo::DescriptionClassName(type.name());
  return result;
}

}  // namespace

std::string oo::TextureLoader::className() const {
  return DemangledName(typeid(*this));
}

std::string oo::TextureLoader::description() const {
  std::string result =
      std::format("<{} {}>", className(), oo::str::pointerDescription(this));
  if (const std::optional<std::string> components = descriptionComponents())
    result += "{" + *components + "}";
  return result;
}

std::string oo::TextureLoader::shortDescription() const {
  const std::string_view facadeClass =
      (dynamic_cast<const oo::TextureGenerator*>(this) != nullptr)
          ? "OOTextureGenerator"
          : "OOTextureLoader";
  std::string result =
      std::format("<{} {}>", facadeClass, oo::str::pointerDescription(this));
  if (const std::optional<std::string> components =
          shortDescriptionComponents())
    result += "{" + *components + "}";
  return result;
}

std::optional<std::string> oo::TextureLoader::path() { return _path; }

bool oo::TextureLoader::isReady() { return _ready; }

std::optional<oo::TextureLoader::Result> oo::TextureLoader::getResult() {
  if (!_ready) {
    // The work manager knows the task as the C++ loader.
    oo::AsyncWorkManager::sharedAsyncWorkManager()->waitForTaskToComplete(this);
  }
  if (_data == nullptr) return std::nullopt;

  Result result{
      .pixMap = oo::MakePixMap(std::move(_data), _width, _height,
                               static_cast<oo::PixMapFormat>(
                                   oo::TextureComponentsForFormat(_format)),
                               0, 0),
      .format = _format,
      .originalWidth = _originalWidth,
      .originalHeight = _originalHeight,
  };
  _data = nullptr;
  if (!oo::IsValidPixMap(result.pixMap)) return std::nullopt;
  return result;
}

std::optional<std::string> oo::TextureLoader::cacheKey() {
  return std::format("{}:0x{:04X}", oo::str::lastPathComponent(*path()),
                     oo::ToUnderlying(_options));
}

void oo::TextureLoader::loadTexture() { OOLogGenericSubclassResponsibility(); }

void oo::TextureLoader::setUp() {
  // Load two maximum sizes - graphics hardware limit and user-specified limit.
  GLint maxSize = 0;  // headless: no GL to ask
  if (oo::GraphicsAvailable())
    OOGL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize));
  sGLMaxSize = static_cast<unsigned int>(std::max(maxSize, 64));
  OO_LOG("texture.load.rescale.maxSize", "GL maximum texture size: {}",
         static_cast<unsigned>(sGLMaxSize));

  // Why 0x80000000? Because it's the biggest number OORoundUpToPowerOf2() can
  // handle.
  {
    const oo::PList maxTex =
        oo::Defaults::standard().object("max-texture-size");
    sUserMaxSize = oo::PListGet<unsigned int>::from(
        maxTex.isNull() ? nullptr : &maxTex, 0x80000000);
  }
  if (sUserMaxSize < 0x80000000)
    OO_LOG("texture.load.rescale.maxSize", "User maximum texture size: {}",
           static_cast<unsigned>(sUserMaxSize));
  sUserMaxSize = oo::RoundUpToPowerOf2_32(sUserMaxSize);
  sUserMaxSize = std::max(sUserMaxSize, 64U);

  sHaveSetUp = true;
}

/*** Methods performed on the loader thread. ***/

void oo::TextureLoader::performAsyncTask() {
  try {
    OO_LOG("texture.load.asyncLoad", "Loading texture {}",
           oo::str::lastPathComponent(_path));

    loadTexture();

    // Catch an error I've seen but not diagnosed yet.
    if (_data != nullptr && oo::TextureComponentsForFormat(_format) == 0) {
      OO_LOG("texture.load.failed.internalError",
             "Texture loader internal error for {}: data is non-null but data "
             "format is invalid ({}).",
             _path, static_cast<unsigned>(_format));
      _data = nullptr;
    }

    if (_data != nullptr) applySettings();

    OO_LOG("texture.load.asyncLoad.done", "{}", "Loading complete.");
  } catch (...) {
    const auto exception = oo::CurrentOOException();
    if (!exception) throw;  // not an OOException: it goes on up, as before
    OO_LOG("texture.load.asyncLoad.exception",
           "***** Exception loading texture {}: {} ({}).", _path,
           exception->name(), exception->reason());

    // Be sure to signal load failure.
    _data = nullptr;
  }
}

void oo::TextureLoader::generateMipMapsForCubeMap() {
  // Generate mip maps for each cube face.
  OOCParameterAssert(_data != nullptr);

  uint8_t components = oo::TextureComponentsForFormat(_format);
  size_t srcSideSize =
      _width * _width * components;  // Space for one side without mip-maps.
  size_t newSideSize =
      srcSideSize * 4 / 3;  // Space for one side with mip-maps.
  newSideSize = (newSideSize + 15) &
                static_cast<size_t>(~15);  // Round up to multiple of 16 bytes.
  size_t newSize = newSideSize * 6;        // Space for all six sides.

  oo::PixelBuffer newData = oo::MakePixelBuffer(newSize);

  unsigned i;
  for (i = 0; i < 6; i++) {
    void* srcBytes = _data->data() + srcSideSize * i;
    void* dstBytes = newData->data() + newSideSize * i;

    memcpy(dstBytes, srcBytes, srcSideSize);
    oo::GenerateMipMaps(dstBytes, _width, _width, _format);
  }

  _data = std::move(newData);
}

void oo::TextureLoader::applySettings() {
  oo::PixMapDimension desiredWidth, desiredHeight;
  bool rescale;
  size_t newSize;
  uint8_t components;
  oo::PixMap pixMap;

  components = oo::TextureComponentsForFormat(_format);

  // Apply defaults.
  if (_originalWidth == 0) _originalWidth = _width;
  if (_originalHeight == 0) _originalHeight = _height;
  if (_rowBytes == 0) _rowBytes = _width * components;

  pixMap = oo::MakePixMap(_data, _width, _height,
                          static_cast<oo::PixMapFormat>(components), _rowBytes,
                          _rowBytes * _height);

  if (_extractChannel) {
    if (oo::ExtractPixMapChannel(&pixMap, _extractChannelIndex, false)) {
      _format = oo::kTextureDataGrayscale;
      components = 1;
    } else {
      OO_LOG_WARN("texture.load.extractChannel.invalid",
                  "Cannot extract channel from texture \"{}\"",
                  oo::str::lastPathComponent(_path));
    }
  }

  const DesiredSize desired = getDesiredSize();
  desiredWidth = desired.width;
  desiredHeight = desired.height;

  if (_isCubeMap && !oo::CubeMapsAvailable()) {
    oo::PixMapToRGBA(&pixMap);
    desiredHeight = std::min(desiredWidth * 2, 512U);
    if (sReducedDetail && desiredHeight > kCubeShrinkThreshold)
      desiredHeight /= 2;
    desiredWidth = desiredHeight * 2;

    oo::PixMap converted =
        oo::ConvertCubeMapToLatLong(pixMap, desiredHeight, _generateMipMaps);
    oo::FreePixMap(&pixMap);
    pixMap = converted;
    _isCubeMap = false;

#if DUMP_CONVERTED_CUBE_MAPS
    // -stringByDeletingPathExtension of the last path component (probed): cut
    // at its last '.', unless that starts the name.
    std::string dumpName = oo::str::lastPathComponent(_path);
    const std::string::size_type dot = dumpName.rfind('.');
    if (dot != std::string::npos && dot != 0) dumpName.erase(dot);
    OODumpPixMap(pixMap, std::format("converted cube map {}", dumpName));
#endif
  }

  // Rescale if needed.
  rescale = (_width != desiredWidth || _height != desiredHeight);
  if (rescale) {
    bool leaveSpaceForMipMaps = _generateMipMaps;
#if OO_TEXTURE_CUBE_MAP
    if (_isCubeMap) leaveSpaceForMipMaps = false;
#endif

    OO_LOG("texture.load.rescale",
           "Rescaling texture \"{}\" from {} x {} to {} x {}.",
           oo::str::lastPathComponent(_path),
           static_cast<unsigned>(pixMap.width),
           static_cast<unsigned>(pixMap.height),
           static_cast<unsigned>(desiredWidth),
           static_cast<unsigned>(desiredHeight));

    pixMap = oo::ScalePixMap(pixMap, desiredWidth, desiredHeight,
                             leaveSpaceForMipMaps);
    if (!oo::IsValidPixMap(pixMap)) [[unlikely]]
      return;

    _data = oo::PixMapOwnedBuffer(pixMap);
    pixMap.buffer = _data;
    pixMap.pixels = _data->data();
    _width = pixMap.width;
    _height = pixMap.height;
    _rowBytes = pixMap.rowBytes;
  }

#if OO_TEXTURE_CUBE_MAP
  if (_isCubeMap) {
    if (_generateMipMaps) {
      generateMipMapsForCubeMap();
    }
    return;
  }
#endif

  // Generate mip maps if needed.
  if (_generateMipMaps) {
    // Make space if needed.
    newSize = desiredWidth * components * desiredHeight;
    newSize = (newSize * 4) / 3;
    // +1 to fix overrun valgrind spotted - CIM
    _generateMipMaps = oo::ExpandPixMap(&pixMap, newSize + 1);

    _data = oo::PixMapOwnedBuffer(pixMap);
    _width = pixMap.width;
    _height = pixMap.height;
    _rowBytes = pixMap.rowBytes;
  }
  if (_generateMipMaps) {
    oo::GenerateMipMaps(_data->data(), _width, _height, _format);
  }

  // All done.
}

oo::TextureLoader::DesiredSize oo::TextureLoader::getDesiredSize() {
  oo::PixMapDimension desiredWidth, desiredHeight;

  // Work out appropriate final size for textures.
  if (!_noScalingWhatsoever) {
    // Cube maps are six times as high as they are wide, and we need to preserve
    // that.
    if (_allowCubeMap && _height == _width * 6) {
      _isCubeMap = true;

      desiredWidth = OORoundUpToPowerOf2_PixMap((2 * _width) / 3);
      desiredWidth = std::min(desiredWidth, sGLMaxSize / 8);
      if (sReducedDetail) {
        if (256 < desiredWidth) desiredWidth /= 2;
      }
      desiredWidth = std::min(desiredWidth, sUserMaxSize / 4);

      desiredHeight = desiredWidth * 6;
    } else {
      if (!sHaveNPOTTextures) {
        // Round to nearest power of two. NOTE: this is duplicated in
        // OOTextureVerifierStage.m.
        desiredWidth = OORoundUpToPowerOf2_PixMap((2 * _width) / 3);
        desiredHeight = OORoundUpToPowerOf2_PixMap((2 * _height) / 3);
      } else {
        desiredWidth = _width;
        desiredHeight = _height;
      }

      desiredWidth = std::min(desiredWidth, sGLMaxSize);
      desiredHeight = std::min(desiredHeight, sGLMaxSize);

      if (!_avoidShrinking) {
        if (sReducedDetail) {
          if (_shrinkThreshold < desiredWidth) desiredWidth /= 2;
          if (_shrinkThreshold < desiredHeight) desiredHeight /= 2;
        }

        desiredWidth = std::min(desiredWidth, _maxSize);
        desiredHeight = std::min(desiredHeight, _maxSize);
      }
    }
  } else {
    desiredWidth = _width;
    desiredHeight = _height;
  }

  return {desiredWidth, desiredHeight};
}

void oo::TextureLoader::completeAsyncTask() { _ready = true; }
