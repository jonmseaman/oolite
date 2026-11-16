/*

OOPixMapTextureLoader.cpp


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

#include "PixMapTextureLoader.h"

#include <format>
#include <memory>

#include "TextureScaling.h"
#include "oofnd/String.hpp"

std::shared_ptr<oo::PixMapTextureLoader>
oo::PixMapTextureLoader::loaderWithPixMap(oo::PixMap pixMap,
                                          oo::TextureFlags options,
                                          bool freeWhenDone) {
  std::shared_ptr<oo::PixMapTextureLoader> result =
      oo::makeShared<oo::PixMapTextureLoader>();
  if (!result->initWithPixMap(pixMap, options, freeWhenDone)) return {};
  return result;
}

bool oo::PixMapTextureLoader::initWithPixMap(oo::PixMap pixMap,
                                             oo::TextureFlags options,
                                             bool freeWhenDone) {
  if (!initWithPath(
          std::format("OOPixMap@{}", oo::str::pointerDescription(this)),
          options))
    return false;

  if (freeWhenDone)
    _pixMap = pixMap;
  else
    _pixMap = oo::DuplicatePixMap(_pixMap, 0);

  _texOptions = oo::ApplyTextureOptionDefaults(options);

  if (!oo::IsValidPixMap(_pixMap))
    return false;  // was DESTROY(self): the destructor frees _pixMap

  return true;
}

namespace oo {

PixMapTextureLoader::~PixMapTextureLoader() { FreePixMap(&_pixMap); }

}  // namespace oo

void oo::PixMapTextureLoader::loadTexture() {
  // Generate mip maps if needed.
  if ((_texOptions & oo::TextureFlags::kMinFilterMask) ==
      oo::TextureFlags::kMinFilterMipMap) {
    size_t size = oo::MinimumPixMapBufferSize(_pixMap) * 4 / 3;
    bool generateMipMaps = oo::ExpandPixMap(&_pixMap, size);
    if (generateMipMaps) {
      oo::GenerateMipMaps(_pixMap.pixels, _pixMap.width, _pixMap.height,
                          _pixMap.format);
    } else {
      _texOptions = (_texOptions & ~oo::TextureFlags::kMinFilterMask) |
                    oo::TextureFlags::kMinFilterMipMap;
    }
  }

  // Set up output ivars as per OOTextureLoader contract.
  _data = oo::PixMapOwnedBuffer(_pixMap);
  _width = _pixMap.width;
  _height = _pixMap.height;
  _rowBytes = _pixMap.rowBytes;
  _format = _pixMap.format;

  //	The pixels' ownership passes to the texture.
  oo::FreePixMap(&_pixMap);
}

oo::TextureFlags oo::PixMapTextureLoader::textureOptions() {
  return _texOptions;
}
