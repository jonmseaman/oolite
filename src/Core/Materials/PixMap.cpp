/*

OOPixMap.c


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

#include "PixMap.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <format>
#include <memory>
#include <string>
#include <utility>

namespace oo {

const PixMap kNullPixMap = {.pixels = nullptr,
                            .width = 0,
                            .height = 0,
                            .format = kPixMapInvalidFormat,
                            .rowBytes = 0,
                            .bufferSize = 0,
                            .buffer = nullptr};

PixelBuffer MakePixelBuffer(size_t size) {
  return std::make_shared<std::vector<std::uint8_t>>(size);
}

bool IsValidPixMap(PixMap pixMap) {
  return pixMap.pixels != nullptr && pixMap.width > 0 && pixMap.height > 0 &&
         IsValidPixMapFormat(pixMap.format) &&
         pixMap.rowBytes >=
             pixMap.width * PixMapBytesPerPixelForFormat(pixMap.format) &&
         pixMap.bufferSize >= pixMap.rowBytes * pixMap.height;
}

PixMap MakePixMap(void* pixels, PixMapDimension width, PixMapDimension height,
                  PixMapFormat format, size_t rowBytes, size_t bufferSize) {
  if (rowBytes == 0) rowBytes = width * PixMapBytesPerPixelForFormat(format);
  if (bufferSize == 0) bufferSize = rowBytes * height;

  PixMap result = {.pixels = pixels,
                   .width = width,
                   .height = height,
                   .format = format,
                   .rowBytes = rowBytes,
                   .bufferSize = bufferSize};

  if (IsValidPixMap(result))
    return result;
  else
    return kNullPixMap;
}

PixMap MakePixMap(PixelBuffer buffer, PixMapDimension width,
                  PixMapDimension height, PixMapFormat format, size_t rowBytes,
                  size_t bufferSize) {
  if (buffer == nullptr) return kNullPixMap;
  if (bufferSize == 0 || bufferSize > buffer->size())
    bufferSize = buffer->size();

  PixMap result =
      MakePixMap(buffer->data(), width, height, format, rowBytes, bufferSize);
  if (IsValidPixMap(result)) result.buffer = std::move(buffer);
  return result;
}

PixelBuffer PixMapOwnedBuffer(const PixMap& pixMap) {
  if (pixMap.pixels == nullptr) return nullptr;
  if (pixMap.buffer != nullptr && pixMap.buffer->data() == pixMap.pixels &&
      pixMap.buffer->size() >= pixMap.bufferSize) {
    return pixMap.buffer;
  }

  PixelBuffer copy = MakePixelBuffer(pixMap.bufferSize);
  memcpy(copy->data(), pixMap.pixels, pixMap.bufferSize);
  return copy;
}

PixMap AllocatePixMap(PixMapDimension width, PixMapDimension height,
                      PixMapFormat format, size_t rowBytes, size_t bufferSize) {
  // Create pixmap struct with dummy pixel pointer to test validity.
  PixMap pixMap = MakePixMap(reinterpret_cast<void*>(-1), width, height, format,
                             rowBytes, bufferSize);
  if (!IsValidPixMap(pixMap)) [[unlikely]]
    return kNullPixMap;

  pixMap.buffer = MakePixelBuffer(pixMap.bufferSize);
  pixMap.pixels = pixMap.buffer->data();

  return pixMap;
}

void FreePixMap(PixMap* ioPixMap) {
  if (ioPixMap == nullptr) [[unlikely]]
    return;

  *ioPixMap = kNullPixMap;
}

PixMap DuplicatePixMap(PixMap srcPixMap, size_t desiredSize) {
  if (!IsValidPixMap(srcPixMap)) [[unlikely]]
    return kNullPixMap;

  size_t minSize = MinimumPixMapBufferSize(srcPixMap);
  if (desiredSize < minSize) desiredSize = minSize;

  PixMap result =
      AllocatePixMap(srcPixMap.width, srcPixMap.width, srcPixMap.format,
                     srcPixMap.rowBytes, desiredSize);
  if (!IsValidPixMap(result)) [[unlikely]]
    return kNullPixMap;

  memcpy(result.pixels, srcPixMap.pixels, minSize);
  return result;
}

bool ResizePixMap(PixMap* ioPixMap, size_t desiredSize) {
  if (ioPixMap == nullptr || !IsValidPixMap(*ioPixMap)) [[unlikely]]
    return false;
  if (desiredSize == ioPixMap->bufferSize) return true;
  if (desiredSize < MinimumPixMapBufferSize(*ioPixMap)) return false;

  if (ioPixMap->buffer != nullptr &&
      ioPixMap->buffer->data() == ioPixMap->pixels) {
    ioPixMap->buffer->resize(desiredSize);
  } else {
    // Not ours to resize: a buffer of its own, holding the pixels.
    PixelBuffer buffer = MakePixelBuffer(desiredSize);
    memcpy(buffer->data(), ioPixMap->pixels,
           std::min(ioPixMap->bufferSize, desiredSize));
    ioPixMap->buffer = std::move(buffer);
  }
  ioPixMap->pixels = ioPixMap->buffer->data();
  ioPixMap->bufferSize = desiredSize;
  return true;
}

bool ExpandPixMap(PixMap* ioPixMap, size_t desiredSize) {
  if (ioPixMap == nullptr || !IsValidPixMap(*ioPixMap)) [[unlikely]]
    return false;
  if (desiredSize <= ioPixMap->bufferSize) return true;

  return ResizePixMap(ioPixMap, desiredSize);
}

}  // namespace oo

#ifndef NDEBUG

#include "MyOpenGLView.h"
#include "Universe.h"

void OODumpPixMap(oo::PixMap pixMap, const std::string& name) {
  if (!oo::IsValidPixMap(pixMap)) return;

  MyOpenGLView* gameView =
      (UNIVERSE != nullptr ? UNIVERSE->getGameView() : nullptr);

  switch (pixMap.format) {
    case oo::kPixMapInvalidFormat:
      break;

    case oo::kPixMapGrayscale:
      if (gameView != nullptr)
        gameView->dumpGrayToFileNamed(
            name, static_cast<uint8_t*>(pixMap.pixels), pixMap.width,
            pixMap.height, pixMap.rowBytes);
      break;

    case oo::kPixMapGrayscaleAlpha:
      if (gameView != nullptr)
        gameView->dumpGrayAlphaToFileNamed(
            name, static_cast<uint8_t*>(pixMap.pixels), pixMap.width,
            pixMap.height, pixMap.rowBytes);
      break;

    case oo::kPixMapRGBA:
      if (gameView != nullptr)
        gameView->dumpRGBAToRGBFileNamed(name + " rgb", name + " alpha",
                                         static_cast<uint8_t*>(pixMap.pixels),
                                         pixMap.width, pixMap.height,
                                         pixMap.rowBytes);
      break;
  }
}

#endif

namespace oo {

bool IsValidPixMapFormat(PixMapFormat format) {
  switch (format) {
    case kPixMapInvalidFormat:
      return false;
    case kPixMapGrayscale:
    case kPixMapGrayscaleAlpha:
    case kPixMapRGBA:
      return true;
  }

  return false;
}

}  // namespace oo

#ifndef NDEBUG
namespace oo {

std::uint16_t PixMapBytesPerPixelForFormat(PixMapFormat format) {
  switch (format) {
    case kPixMapInvalidFormat:
      return 0;
    case kPixMapGrayscale:
      return 1;
    case kPixMapGrayscaleAlpha:
      return 2;
    case kPixMapRGBA:
      return 4;
  }

  return static_cast<std::uint16_t>(-1);
}

}  // namespace oo

#endif

namespace oo {

std::string PixMapFormatName(PixMapFormat format) {
  switch (format) {
    case kPixMapInvalidFormat:
      return "invalid";
    case kPixMapGrayscale:
      return "grayscale";
    case kPixMapGrayscaleAlpha:
      return "grayscale+alpha";
    case kPixMapRGBA:
      return "RGBA";
  }

  return std::format("invalid<{}>", static_cast<int>(format));
}

bool PixMapFormatHasAlpha(PixMapFormat format) {
  switch (format) {
    case kPixMapInvalidFormat:
      return false;
    case kPixMapGrayscale:
      return false;
    case kPixMapGrayscaleAlpha:
      return true;
    case kPixMapRGBA:
      return true;
  }

  return false;
}

}  // namespace oo
