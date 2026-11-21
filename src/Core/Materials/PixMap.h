/*

OOPixMap.h

Types for low-level pixel map manipulation.


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

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Maths.h"

namespace oo {

using PixMapDimension = uint32_t;

}  // namespace oo

// Note: dimensions are assumed to be less than 1048576 (2^20) pixels.

#define OORoundUpToPowerOf2_PixMap oo::RoundUpToPowerOf2_32

// A pixel map's format: its number of components per pixel, which sizes rows
// and buffers: a number used in arithmetic, so not an enumeration.
namespace oo {

using PixMapFormat = int;
inline constexpr PixMapFormat kPixMapInvalidFormat = 0;
inline constexpr PixMapFormat kPixMapGrayscale = 1;
inline constexpr PixMapFormat kPixMapGrayscaleAlpha = 2;
inline constexpr PixMapFormat kPixMapRGBA = 4;

/*	OOPixelBuffer
        The bytes a pixmap's pixels point into. A pixmap is a value: its copies
        share its buffer, which goes with the last of them. A pixmap made over
        memory it does not own (OOMakePixMap() with a pointer) has no buffer.
*/
using PixelBuffer = std::shared_ptr<std::vector<std::uint8_t>>;

// A new zero-filled buffer of size bytes.
PixelBuffer MakePixelBuffer(size_t size);

struct PixMap {
  void* pixels;
  PixMapDimension width, height;
  PixMapFormat format;
  size_t rowBytes;
  size_t bufferSize;
  PixelBuffer buffer;  // owns pixels, or null (see OOPixelBuffer)
};

extern const PixMap kNullPixMap;

inline bool IsNullPixMap(PixMap pixMap) { return pixMap.pixels == nullptr; }

bool IsValidPixMap(PixMap pixMap);

inline size_t MinimumPixMapBufferSize(PixMap pixMap) {
  return pixMap.rowBytes * pixMap.height;
}

/*	OOMakePixMap()
        Stuff an OOPixMap struct. Returns kOONullPixMap if the result would be
        invalid. If rowBytes or bufferSize are zero, minimum valid values will
   be used.
*/
PixMap MakePixMap(void* pixels, PixMapDimension width, PixMapDimension height,
                  PixMapFormat format, size_t rowBytes, size_t bufferSize);

/*	As above, over the start of buffer and owning it. If bufferSize is zero,
        the buffer's size is used.
*/
PixMap MakePixMap(PixelBuffer buffer, PixMapDimension width,
                  PixMapDimension height, PixMapFormat format, size_t rowBytes,
                  size_t bufferSize);

/*	OOPixMapOwnedBuffer()
        The buffer that owns pixMap's pixels and starts at them; a copy of the
        pixels in a new buffer where the pixmap has none, or points into the
        middle of one. Null for a null pixmap.
*/
PixelBuffer PixMapOwnedBuffer(const PixMap& pixMap);

/*	OOAllocatePixMap()
        Create an OOPixMap, allocating storage. If rowBytes or bufferSize are
   zero, minimum valid values will be used.
*/
PixMap AllocatePixMap(PixMapDimension width, PixMapDimension height,
                      PixMapFormat format, size_t rowBytes, size_t bufferSize);

/*	OOFreePixMap()
        Let go of a pixmap's buffer (freed with the last pixmap that shares
        it), and clear out the struct.
*/
void FreePixMap(PixMap* ioPixMap);

/*	OODuplicatePixMap()
        Create a pixmap with the same pixel contents as a source pixmap, and
        optional padding. If desiredSize is less than the required space for the
        pixmap, it will be ignored. The contents of padding bytes are
   unspecified.
*/
PixMap DuplicatePixMap(PixMap srcPixMap, size_t desiredSize);

/*	OOResizePixMap()
        Set the size of a pixmap's buffer. Fails if specified size is smaller
   than required to fit the current pixels. A pixmap without a buffer of its
   own gets one, holding a copy of its pixels.
*/
bool ResizePixMap(PixMap* ioPixMap, size_t desiredSize);

/*	OOCompactPixMap()
        Remove any trailing space in a pixmap's buffer, if possible.
*/
inline void CompactPixMap(PixMap* ioPixMap) {
  ResizePixMap(ioPixMap, MinimumPixMapBufferSize(*ioPixMap));
}

/*	OOExpandPixMap()
        Expand pixmap to at least desiredSize bytes. Returns false on failure.
*/
bool ExpandPixMap(PixMap* ioPixMap, size_t desiredSize);

}  // namespace oo

#ifndef NDEBUG
void OODumpPixMap(oo::PixMap pixMap, const std::string& name);
#else
#define OODumpPixMap(p, n) \
  do {                     \
  } while (0)
#endif

namespace oo {

bool IsValidPixMapFormat(PixMapFormat format);

}  // namespace oo

#ifndef NDEBUG
namespace oo {

std::uint16_t PixMapBytesPerPixelForFormat(PixMapFormat format) PURE_FUNC;

}  // namespace oo

#else
namespace oo {

inline std::uint16_t PixMapBytesPerPixelForFormat(PixMapFormat format) {
  // Currently, format values are component counts. This is subject to change.
  return format;
}

}  // namespace oo

#endif

namespace oo {

inline std::uint16_t PixMapBytesPerPixel(PixMap pixMap) {
  return PixMapBytesPerPixelForFormat(pixMap.format);
}

std::string PixMapFormatName(PixMapFormat format) PURE_FUNC;

bool PixMapFormatHasAlpha(PixMapFormat format) PURE_FUNC;

}  // namespace oo
