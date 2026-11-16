/*

OOConvertCubeMapToLatLong.m

Convert a cube map texture to a lat/long texture.


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

#define OO_PLAIN_CPP_UNIT \
  1  // a plain C++ unit: the shared headers' game-unit parts stay out of it
#include "OOConvertCubeMapToLatLong.h"

#include <vector>

#include "OOTextureScaling.h"
#include "oofnd/objc/Assert.h"

#define kPiF (3.14159265358979323846264338327950288f)

oo::PixMap OOConvertCubeMapToLatLong(oo::PixMap sourcePixMap,
                                     oo::PixMapDimension height,
                                     bool leaveSpaceForMipMaps) {
  if (!oo::IsValidPixMap(sourcePixMap) ||
      sourcePixMap.format != oo::kPixMapRGBA ||
      sourcePixMap.height != sourcePixMap.width * 6) {
    return oo::kNullPixMap;
  }

  OOCParameterAssert(height > 0);

  height *= 2;
  oo::PixMapDimension width = height * 2;
  oo::PixMap outPixMap =
      oo::AllocatePixMap(width, height, static_cast<oo::PixMapFormat>(4), 0, 0);
  if (!oo::IsValidPixMap(outPixMap)) return oo::kNullPixMap;

  oo::PixMapDimension x, y;
  uint32_t* pixel = static_cast<uint32_t*>(outPixMap.pixels);
  float fheight = static_cast<float>(height);
  float rheight = 1.0f / fheight;

  float halfSize = (static_cast<float>(sourcePixMap.width - 1)) * 0.5f;
  uint8_t* srcBytes = static_cast<uint8_t*>(sourcePixMap.pixels);

  // Build tables of sin/cos of longitude.
  std::vector<float> sinTable(width);
  std::vector<float> cosTable(width);
  for (x = 0; x < width; x++) {
    float lon = (static_cast<float>(x) * rheight) * kPiF;
    sinTable[x] = sin(lon);
    cosTable[x] = cos(lon);
  }

  for (y = 0; y < height; y++) {
    // Calcuate sin/cos of latitude.
    /*
            Clang static analyzer (Xcode 3.2.5 version, through to Xcode 4.4
            and freestanding checker-268 at least) says:
            "Assigned value is garbage or undefined."
            Memsetting sinTable to all-zeros moves this to the cosTable line.
            Since every value in each of those tables is in fact defined, this
            is an error in the analyzer.
            -- Ahruman 2011-01-25/2012-09-14
    */
    float cy = -sinTable[width * 3 / 4 - y];
    float lac = -cosTable[width * 3 / 4 - y];
    float ay = fabs(cy);

    for (x = 0; x < width; x++) {
      float cx = sinTable[x] * lac;
      float cz = cosTable[x] * lac;

      float ax = fabs(cx);
      float az = fabs(cz);

      // Y offset of start of this face in image.
      oo::PixMapDimension yOffset;

      // Coordinates within selected face.
      float faceX, faceY, r;

      // Select source face.
      if (ax >= ay && ax >= az) {
        faceX = cz;
        faceY = -cy;
        r = ax;
        if (0.0f < cx) {
          yOffset = 0;
        } else {
          faceX = -faceX;
          yOffset = 1;
        }
      } else if (ay >= ax && ay >= az) {
        faceX = cx;
        faceY = cz;
        r = ay;
        if (0.0f < cy) {
          faceY = -faceY;
          yOffset = 2;
        } else {
          yOffset = 3;
        }
      } else {
        faceX = cx;
        faceY = -cy;
        r = az;
        if (0.0f < cz) {
          faceX = -faceX;
          yOffset = 5;
        } else {
          yOffset = 4;
        }
      }

      // Scale coordinates.
      r = 1.0f / r;
      oo::PixMapDimension ix =
          static_cast<oo::PixMapDimension>((faceX * r + 1.0f) * halfSize);
      oo::PixMapDimension iy =
          static_cast<oo::PixMapDimension>((faceY * r + 1.0f) * halfSize);

#ifndef NDEBUG
      assert(ix < sourcePixMap.width && iy < sourcePixMap.width);
#endif

      // Look up pixel.
      iy += sourcePixMap.width * yOffset;

      uint32_t* row =
          reinterpret_cast<uint32_t*>(srcBytes + iy * sourcePixMap.rowBytes);
      *pixel++ = row[ix];
    }
  }

  // Scale to half size for supersamplingness.
  return OOScalePixMap(outPixMap, width / 2, height / 2, leaveSpaceForMipMaps);
}
