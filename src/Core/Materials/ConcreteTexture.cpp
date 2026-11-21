/*

        OOConcreteTexture.m

        Copyright (C) 2007-2013 Jens Ayton and contributors

        Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
        furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

#include "ConcreteTexture.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "CPUInfoEndian.h"
#include "EnumFlags.h"
#include "FastArithmetic.h"
#include "Logging.h"
#include "MacroOpenGL.h"
#include "OpenGLCheck.h"
#include "OpenGLExtensionPointers.h"
#include "PixMap.h"
#include "Texture.h"
#include "TextureFlags.h"
#include "TextureGenerator.h"
#include "TextureInternal.h"
#include "TextureLoader.h"
#include "oofnd/Log.hpp"
#include "oofnd/Object.hpp"
#include "oofnd/String.hpp"
#include "oofnd/objc/FoundationTypes.h"
#ifdef OO_DEBUG_MONITOR_ENABLED
#include "TextureGenerator.h"
#include "oofnd/Exception.hpp"
#include "oofnd/objc/Assert.h"
#endif

#if OOLITE_BIG_ENDIAN
#define RGBA_IMAGE_TYPE GL_UNSIGNED_INT_8_8_8_8_REV
#elif OOLITE_LITTLE_ENDIAN
#define RGBA_IMAGE_TYPE GL_UNSIGNED_BYTE
#else
#error Neither OOLITE_BIG_ENDIAN nor OOLITE_LITTLE_ENDIAN is defined as nonzero!
#endif

static bool DecodeFormat(oo::TextureDataFormat format, oo::TextureFlags options,
                         GLenum* outFormat, GLenum* outInternalFormat,
                         GLenum* outType);

oo::ConcreteTexture::ConcreteTexture(oo::TextureLoader* loader,
                                     const std::optional<std::string>& key,
                                     oo::TextureFlags options,
                                     GLfloat anisotropy, GLfloat lodBias) {
  _loader = oo::share(loader);
  _options = options;

#if GL_EXT_texture_filter_anisotropic
  _anisotropy = oo::Clamp_0_1_f(anisotropy) * gOOTextureInfo.anisotropyScale;
#endif
#if GL_EXT_texture_lod_bias
  _lodBias = lodBias;
#endif

#ifdef OO_DEBUG_MONITOR_ENABLED
  // "<" + [loader class] + ">" of a generator: the C++ class's name.
  if (dynamic_cast<oo::TextureGenerator*>(loader) != nullptr) {
    _name = "<" + loader->className() + ">";
  }
#endif

  _key = key;
}

std::shared_ptr<oo::ConcreteTexture> oo::ConcreteTexture::initWithLoader(
    oo::TextureLoader* loader, const std::optional<std::string>& key,
    oo::TextureFlags options, GLfloat anisotropy, GLfloat lodBias) {
  if (loader == nullptr) {
    return nullptr;
  }

  std::shared_ptr<oo::ConcreteTexture> self(
      new oo::ConcreteTexture(loader, key, options, anisotropy, lodBias),
      oo::Dispose{});

  self->addToCaches();

  return self;
}

std::shared_ptr<oo::ConcreteTexture> oo::ConcreteTexture::initWithPath(
    const std::string& path, const std::optional<std::string>& key,
    oo::TextureFlags options, float anisotropy, GLfloat lodBias) {
  const std::shared_ptr<oo::TextureLoader> loader =
      oo::TextureLoader::loaderWithPath(path, options);
  if (loader == nullptr) {
    return nullptr;
  }

  std::shared_ptr<oo::ConcreteTexture> self =
      initWithLoader(loader.get(), key, options, anisotropy, lodBias);
  if (self != nullptr) {
#if OOTEXTURE_RELOADABLE
    self->_path = path;
#endif
  }

  return self;
}

namespace oo {

ConcreteTexture::~ConcreteTexture() {
#ifdef OO_DEBUG_MONITOR_ENABLED
  // The object's address is the one printed.
  OO_LOG(_trace ? "texture.allocTrace.dealloc" : "texture.dealloc",
         "Deallocating and uncaching texture {}",
         oo::str::pointerDescription(this));
#endif

#if OOTEXTURE_RELOADABLE
  _path = std::nullopt;
#endif

  if (_loaded) {
    if (_textureName != 0) {
      OO_ENTER_OPENGL();
      OOGL(glDeleteTextures(1, &_textureName));
      _textureName = 0;
    }
    _bytes = nullptr;
  }

#ifndef OOTEXTURE_NO_CACHE
  removeFromCaches();
  _key = std::nullopt;
#endif

  _loader = nullptr;

#ifdef OO_DEBUG_MONITOR_ENABLED
  _name = std::nullopt;
#endif
}

}  // namespace oo

std::optional<std::string> oo::ConcreteTexture::descriptionComponents() const {
  std::string stateDesc;

  if (_loaded) {
    if (_valid) {
      stateDesc = std::format("{} x {}", _width, _height);
    } else {
      stateDesc = "LOAD ERROR";
    }
  } else {
    stateDesc = "loading";
  }

  // "%@, %@": nil printed (null)
  return _key.value_or("(null)") + ", " + stateDesc;
}

std::optional<std::string> oo::ConcreteTexture::shortDescriptionComponents()
    const {
  return _key;
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::optional<std::string> oo::ConcreteTexture::name() {
  if (_name.has_value()) return _name;

  // -lastPathComponent of a nil path is nil, and so is the name.
#if OOTEXTURE_RELOADABLE
  if (!_path.has_value()) return std::nullopt;
  std::string name = oo::str::lastPathComponent(*_path);
#else
  const std::optional<std::string> key = cacheKey();
  if (!key.has_value()) return std::nullopt;
  const std::string::size_type colon = key->find(':');
  const std::string head =
      colon == std::string::npos ? *key : key->substr(0, colon);
  std::string name = oo::str::lastPathComponent(head);
#endif

  std::optional<std::string_view> channelSuffix;
  switch (_options & oo::TextureFlags::kExtractChannelMask) {
    case oo::TextureFlags::kExtractChannelR:
      channelSuffix = ":r";
      break;

    case oo::TextureFlags::kExtractChannelG:
      channelSuffix = ":g";
      break;

    case oo::TextureFlags::kExtractChannelB:
      channelSuffix = ":b";
      break;

    case oo::TextureFlags::kExtractChannelA:
      channelSuffix = ":a";
      break;

    default:  // the other enumerators are outside the mask
      break;
  }

  if (channelSuffix.has_value()) name += *channelSuffix;

  return name;
}
#endif

void oo::ConcreteTexture::apply() {
  OO_ENTER_OPENGL();

  if (!_loaded) [[unlikely]]
    setUpTexture();
  else if (!_uploaded) [[unlikely]]
    uploadTexture();
  else
    OOGL(glBindTexture(glTextureTarget(), _textureName));

#if GL_EXT_texture_lod_bias
  if (gOOTextureInfo.textureLODBiasAvailable)
    OOGL(glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT,
                   _lodBias));
#endif
}

void oo::ConcreteTexture::ensureFinishedLoading() {
  if (!_loaded) setUpTexture();
}

bool oo::ConcreteTexture::isFinishedLoading() {
  // a message to nil answered NO
  return _loaded || (_loader != nullptr && _loader->isReady());
}

std::optional<std::string> oo::ConcreteTexture::cacheKey() { return _key; }

NSSize oo::ConcreteTexture::dimensions() {
  ensureFinishedLoading();

  return NSMakeSize(_width, _height);
}

NSSize oo::ConcreteTexture::originalDimensions() {
  ensureFinishedLoading();

  return NSMakeSize(_originalWidth, _originalHeight);
}

bool oo::ConcreteTexture::isMipMapped() {
  ensureFinishedLoading();

  return _mipLevels != 0;
}

oo::PixMap oo::ConcreteTexture::copyPixMapRepresentation() {
  ensureFinishedLoading();

  oo::PixMap px = oo::kNullPixMap;

  if (_bytes != nullptr) {
    // If possible, just copy our existing buffer.
    px = oo::MakePixMap(_bytes, _width, _height, _format, 0, 0);
    px = oo::DuplicatePixMap(px, 0);
  }
#if OOTEXTURE_RELOADABLE
  else {
    // Otherwise, read it back from OpenGL.
    OO_ENTER_OPENGL();

    GLenum format, internalFormat, type;
    if (!DecodeFormat(_format, _options, &format, &internalFormat, &type)) {
      return oo::kNullPixMap;
    }

    if (!isCubeMap()) {
      px = oo::AllocatePixMap(_width, _height, _format, 0, 0);
      if (!oo::IsValidPixMap(px)) return oo::kNullPixMap;

      glGetTexImage(GL_TEXTURE_2D, 0, format, type, px.pixels);
    }
#if OO_TEXTURE_CUBE_MAP
    else {
      px = oo::AllocatePixMap(_width, _width * 6, _format, 0, 0);
      if (!oo::IsValidPixMap(px)) return oo::kNullPixMap;
      uint8_t* pixels = static_cast<uint8_t*>(px.pixels);

      unsigned i;
      for (i = 0; i < 6; i++) {
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, type,
                      pixels);
        pixels += oo::PixMapBytesPerPixelForFormat(_format) * _width * _width;
      }
    }
#endif
  }
#endif

  return px;
}

bool oo::ConcreteTexture::isRectangleTexture() {
#if GL_EXT_texture_rectangle
  return _isRectTexture;
#else
  return false;
#endif
}

bool oo::ConcreteTexture::isCubeMap() {
#if OO_TEXTURE_CUBE_MAP
  return _isCubeMap;
#else
  return false;
#endif
}

NSSize oo::ConcreteTexture::texCoordsScale() {
#if GL_EXT_texture_rectangle
  if (_loaded) {
    if (!_isRectTexture) {
      return NSMakeSize(1.0f, 1.0f);
    } else {
      return NSMakeSize(_width, _height);
    }
  } else {
    // Not loaded
    if (!oo::Any(_options & oo::TextureFlags::kAllowRectTexture)) {
      return NSMakeSize(1.0f, 1.0f);
    } else {
      // Finishing may clear the rectangle texture flag (if the texture turns
      // out to be POT)
      ensureFinishedLoading();
      return texCoordsScale();
    }
  }
#else
  return NSMakeSize(1.0f, 1.0f);
#endif
}

GLint oo::ConcreteTexture::glTextureName() {
  ensureFinishedLoading();

  return static_cast<GLint>(_textureName);
}

// Private (was the (Private) category).

void oo::ConcreteTexture::setUpTexture() {
  // This will block until loading is completed, if necessary.
  std::optional<oo::TextureLoader::Result> loaded;
  if (_loader != nullptr) {  // a message to nil answered NO
    loaded = _loader->getResult();
    // A failed load left the format invalid.
    _format = loaded.has_value() ? loaded->format : oo::kTextureDataInvalid;
    if (loaded.has_value()) {
      _originalWidth = loaded->originalWidth;
      _originalHeight = loaded->originalHeight;
    }
  }
  if (loaded.has_value()) {
    const oo::PixMap& pm = loaded->pixMap;
    _bytes = oo::PixMapOwnedBuffer(pm);
    _width = pm.width;
    _height = pm.height;

#if OO_TEXTURE_CUBE_MAP
    if (oo::Any(_options & oo::TextureFlags::kAllowCubeMap) &&
        _height == _width * 6 && gOOTextureInfo.cubeMapAvailable) {
      _isCubeMap = true;
    }
#endif

#if defined(OO_DEBUG_MONITOR_ENABLED) && OOTEXTURE_RELOADABLE
    if (_trace) {
      static unsigned dumpID = 0;
      const std::string name = std::format("tex dump {} \"", ++dumpID) +
                               this->name().value_or("(null)") + "\"";
      OO_LOG("texture.trace.dump", "Dumped traced texture {} to '{}.png'",
             description(), name);
      OODumpPixMap(pm, name);
    }
#endif

    uploadTexture();
  } else {
    _textureName = 0;
    _valid = false;
    _uploaded = true;
  }

  _loaded = true;

  _loader = nullptr;
}

void oo::ConcreteTexture::uploadTexture() {
  GLint filter;
  bool mipMap = false;

  OO_ENTER_OPENGL();

  if (!_uploaded) {
    GLenum texTarget = glTextureTarget();

    OOGL(glGenTextures(1, &_textureName));
    OOGL(glBindTexture(texTarget, _textureName));

    // Select wrap mode
    GLint clampMode =
        gOOTextureInfo.clampToEdgeAvailable ? GL_CLAMP_TO_EDGE : GL_CLAMP;
    GLint wrapS =
        oo::Any(_options & oo::TextureFlags::kRepeatS) ? GL_REPEAT : clampMode;
    GLint wrapT =
        oo::Any(_options & oo::TextureFlags::kRepeatT) ? GL_REPEAT : clampMode;

#if OO_TEXTURE_CUBE_MAP
    if (texTarget == GL_TEXTURE_CUBE_MAP) {
      wrapS = wrapT = clampMode;
      OOGL(glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, clampMode));
    }
#endif

    OOGL(glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, wrapS));
    OOGL(glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, wrapT));

    // Select min filter
    const oo::TextureFlags minFilter =
        _options & oo::TextureFlags::kMinFilterMask;
    if (minFilter == oo::TextureFlags::kMinFilterNearest) {
      filter = GL_NEAREST;
    } else if (minFilter == oo::TextureFlags::kMinFilterMipMap) {
      mipMap = true;
      filter = GL_LINEAR_MIPMAP_LINEAR;
    } else {
      filter = GL_LINEAR;
    }
    OOGL(glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, filter));

#if GL_EXT_texture_filter_anisotropic
    if (gOOTextureInfo.anisotropyAvailable && mipMap && 1.0 < _anisotropy) {
      OOGL(glTexParameterf(texTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                           _anisotropy));
    }
#endif

    // Select mag filter
    const oo::TextureFlags magFilter =
        _options & oo::TextureFlags::kMagFilterMask;
    if (magFilter == oo::TextureFlags::kMagFilterNearest)
      filter = GL_NEAREST;
    else
      filter = GL_LINEAR;
    OOGL(glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, filter));

    //	if (gOOTextureInfo.clientStorageAvailable)  EnableClientStorage();

    if (texTarget == GL_TEXTURE_2D) {
      uploadTextureDataWithMipMap(mipMap, _format);
      OO_LOG("texture.upload", "Uploaded texture {} ({}x{} pixels, {})",
             _textureName, _width, _height, _key.value_or("(null)"));
    }
#if OO_TEXTURE_CUBE_MAP
    else if (texTarget == GL_TEXTURE_CUBE_MAP) {
      uploadTextureCubeMapDataWithMipMap(mipMap, _format);
      OO_LOG("texture.upload",
             "Uploaded cube map texture {} ({}x{}x6 pixels, {})", _textureName,
             _width, _width, _key.value_or("(null)"));
    }
#endif
    else {
      oo::RaiseException(oo::InternalInconsistencyException,
                         "Unhandled texture target 0x%X.", texTarget);
    }

    _valid = true;
    _uploaded = true;

#if OOTEXTURE_RELOADABLE
    if (isReloadable()) {
      _bytes = nullptr;
    }
#endif
  }
}

void oo::ConcreteTexture::uploadTextureDataWithMipMap(
    bool mipMap, oo::TextureDataFormat format) {
  GLenum glFormat = 0, internalFormat = 0, type = 0;
  unsigned w = _width, h = _height, level = 0;
  char* bytes =
      (_bytes != nullptr ? reinterpret_cast<char*>(_bytes->data()) : nullptr);
  uint8_t components = oo::TextureComponentsForFormat(format);

  OO_ENTER_OPENGL();

  if (!DecodeFormat(format, _options, &glFormat, &internalFormat, &type))
    return;

  while (0 < w && 0 < h) {
    OOGL(glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level++),
                      static_cast<GLint>(internalFormat),
                      static_cast<GLsizei>(w), static_cast<GLsizei>(h), 0,
                      glFormat, type, bytes));
    if (!mipMap) return;
    bytes += w * components * h;
    w >>= 1;
    h >>= 1;
  }

  // Note: we only reach here if (mipMap).
  _mipLevels = static_cast<uint8_t>(level - 1);
  OOGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, _mipLevels));
}

#if OO_TEXTURE_CUBE_MAP
void oo::ConcreteTexture::uploadTextureCubeMapDataWithMipMap(
    bool mipMap, oo::TextureDataFormat format) {
  OO_ENTER_OPENGL();

  GLenum glFormat = 0, internalFormat = 0, type = 0;
  if (!DecodeFormat(format, _options, &glFormat, &internalFormat, &type))
    return;
  uint8_t components = oo::TextureComponentsForFormat(format);

  // Calculate stride between cube map sides.
  size_t sideSize = _width * _width * components;
  if (mipMap) {
    sideSize = sideSize * 4 / 3;
    sideSize = (sideSize + 15) & static_cast<size_t>(~15);
  }

  unsigned side;
  for (side = 0; side < 6; side++) {
    char* bytes =
        (_bytes != nullptr ? reinterpret_cast<char*>(_bytes->data()) : nullptr);
    bytes += side * sideSize;

    unsigned w = _width, level = 0;

    while (0 < w) {
      OOGL(glTexImage2D(
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, static_cast<GLint>(level++),
          static_cast<GLint>(internalFormat), static_cast<GLsizei>(w),
          static_cast<GLsizei>(w), 0, glFormat, type, bytes));
      if (!mipMap) break;
      bytes += w * w * components;
      w >>= 1;
    }
  }
}
#endif

GLenum oo::ConcreteTexture::glTextureTarget() {
  GLenum texTarget = GL_TEXTURE_2D;
#if OO_TEXTURE_CUBE_MAP
  if (_isCubeMap) {
    texTarget = GL_TEXTURE_CUBE_MAP;
  }
#endif
  return texTarget;
}

void oo::ConcreteTexture::forceRebind() {
  if (_loaded && _uploaded && _valid) {
    OO_ENTER_OPENGL();

    _uploaded = false;
    OOGL(glDeleteTextures(1, &_textureName));
    _textureName = 0;

#if OOTEXTURE_RELOADABLE
    if (isReloadable()) {
      OO_LOG("texture.reload", "Reloading texture {}", description());
      _bytes = nullptr;
      _loaded = false;
      _uploaded = false;
      _valid = false;

      _loader = oo::TextureLoader::loaderWithPath(_path, _options);
    }
#endif
  }
}

#if OOTEXTURE_RELOADABLE

bool oo::ConcreteTexture::isReloadable() { return _path.has_value(); }

#endif

static bool DecodeFormat(oo::TextureDataFormat format, oo::TextureFlags options,
                         GLenum* outFormat, GLenum* outInternalFormat,
                         GLenum* outType) {
  OOCParameterAssert(outFormat != nullptr && outInternalFormat != nullptr &&
                     outType != nullptr);

  switch (format) {
    case oo::kTextureDataRGBA:
      *outFormat = GL_RGBA;
      *outInternalFormat =
          oo::Any(options & oo::TextureFlags::kSRGBA) ? GL_SRGB_ALPHA : GL_RGBA;
      *outType = RGBA_IMAGE_TYPE;
      return true;

    case oo::kTextureDataGrayscale:
      if (oo::Any(options & oo::TextureFlags::kAlphaMask)) {
        *outFormat = GL_ALPHA;
        *outInternalFormat = GL_ALPHA8;
      } else {
        *outFormat = GL_LUMINANCE;
        *outInternalFormat = GL_LUMINANCE8;
      }
      *outType = GL_UNSIGNED_BYTE;
      return true;

    case oo::kTextureDataGrayscaleAlpha:
      *outFormat = GL_LUMINANCE_ALPHA;
      *outInternalFormat = GL_LUMINANCE8_ALPHA8;
      *outType = GL_UNSIGNED_BYTE;
      return true;

    default:
      OO_LOG(oo::kLogParameterError, "Unexpected texture format {}.",
             static_cast<unsigned>(format));
      return false;
  }
}
