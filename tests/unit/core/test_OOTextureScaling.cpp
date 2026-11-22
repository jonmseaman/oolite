/*	test_OOTextureScaling.mm
        Unit tests for the texture scaler (src/Core/OOTextureScaling.h).

        The three format-dispatch wrappers (SqueezeVertically,
   StretchHorizontally, SqueezeHorizontally) raise
   an OOException in their unreachable default arm. They are
   file-static, so this test reaches them through OOScalePixMap(): each scaling
   direction, for each of the three pixel formats, pinned byte for byte, plus
   uniform images (which every scaler must keep uniform) and the invalid pixmap
   that OOScalePixMap() refuses before any wrapper runs. Run: bash
   meson test -C build/meson_test test_OOTextureScaling
*/

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "MyOpenGLView.h"
#include "Logging.h"
#include "TextureScaling.h"
#include "Universe.h"
#include "oofnd/Exception.hpp"

// Link stubs: the scaler's object and OOPixMap.mm reach these, and the test
// does not link the game.
namespace {
int sParameterErrors = 0;
}

namespace oo {

void LogGenericParameterErrorForFunction(const char* inFunction) {
  (void)inFunction;
  ++sParameterErrors;
}

const char* const kLogParameterError =
    "general.error.parameterError";

}  // namespace oo
  // OOLogging.mm's
Universe* gSharedUniverse =
    nullptr;  // OODumpPixMap's UNIVERSE (debug builds); never called here

// OODumpPixMap's view dumps: members of the view; never called here.
#ifndef NDEBUG
void MyOpenGLView::dumpGrayToFileNamed(const std::string&, uint8_t*, std::size_t,
                                       std::size_t, std::size_t) {
  std::abort();
}
void MyOpenGLView::dumpGrayAlphaToFileNamed(const std::string&, uint8_t*,
                                            std::size_t, std::size_t,
                                            std::size_t) {
  std::abort();
}
void MyOpenGLView::dumpRGBAToRGBFileNamed(const std::optional<std::string>&,
                                          const std::optional<std::string>&,
                                          uint8_t*, std::size_t, std::size_t,
                                          std::size_t) {
  std::abort();
}
#endif

namespace {

// A malloc'd pixmap (OOScalePixMap frees its source) filled with a fixed
// pattern.
oo::PixMap MakePatterned(oo::PixMapDimension width, oo::PixMapDimension height,
                       oo::PixMapFormat format) {
  oo::PixMap pixMap = oo::AllocatePixMap(width, height, format, 0, 0);
  uint8_t* bytes = static_cast<uint8_t*>(pixMap.pixels);
  for (size_t i = 0; i < pixMap.rowBytes * height; i++)
    bytes[i] = static_cast<uint8_t>((i * 37 + 13) & 0xFF);
  return pixMap;
}

oo::PixMap MakeUniform(oo::PixMapDimension width, oo::PixMapDimension height,
                     oo::PixMapFormat format, uint8_t value) {
  oo::PixMap pixMap = oo::AllocatePixMap(width, height, format, 0, 0);
  std::memset(pixMap.pixels, value, pixMap.rowBytes * height);
  return pixMap;
}

std::vector<int> Bytes(oo::PixMap pixMap) {
  std::vector<int> result;
  const uint8_t* bytes = static_cast<const uint8_t*>(pixMap.pixels);
  for (oo::PixMapDimension y = 0; y < pixMap.height; y++) {
    for (size_t x = 0; x < pixMap.width * oo::PixMapBytesPerPixel(pixMap); x++)
      result.push_back(bytes[y * pixMap.rowBytes + x]);
  }
  return result;
}

// Checks the result's shape and bytes; on a mismatch the failure shows the
// bytes it got, so a pin can be read.
void Pinned(const char* what, oo::PixMap pixMap, oo::PixMapDimension width,
            oo::PixMapDimension height, std::vector<int> expected) {
  EXPECT_TRUE(pixMap.pixels != nullptr) << what;
  EXPECT_EQ(pixMap.width, width) << what;
  EXPECT_EQ(pixMap.height, height) << what;
  EXPECT_EQ(pixMap.rowBytes,
            static_cast<size_t>(width) * oo::PixMapBytesPerPixel(pixMap))
      << what;
  if (pixMap.pixels == nullptr) return;
  std::vector<int> actual = Bytes(pixMap);
  EXPECT_TRUE(actual == expected)
      << what << " got " << ::testing::PrintToString(actual);
}

bool AllEqual(oo::PixMap pixMap, uint8_t value) {
  for (int byte : Bytes(pixMap))
    if (byte != value) return false;
  return true;
}

const oo::PixMapFormat kFormats[] = {oo::kPixMapRGBA, oo::kPixMapGrayscale,
                                   oo::kPixMapGrayscaleAlpha};

}  // namespace

// SqueezeVertically(): 2x4 -> 2x2, in place, for each format.
TEST(OOTextureScaling, squeezesVertically) {
  oo::PixMap rgba = oo::ScalePixMap(MakePatterned(2, 4, oo::kPixMapRGBA), 2, 2, false);
  Pinned("rgba", rgba, 2, 2,
         {33, 70, 107, 144, 181, 218, 127, 36, 113, 150, 187, 224, 133, 42, 79,
          116});
  oo::FreePixMap(&rgba);

  oo::PixMap gray =
      oo::ScalePixMap(MakePatterned(2, 4, oo::kPixMapGrayscale), 2, 2, false);
  Pinned("gray", gray, 2, 2, {50, 87, 198, 107});
  oo::FreePixMap(&gray);

  oo::PixMap grayAlpha =
      oo::ScalePixMap(MakePatterned(2, 4, oo::kPixMapGrayscaleAlpha), 2, 2, false);
  Pinned("grayAlpha", grayAlpha, 2, 2, {87, 124, 161, 70, 127, 164, 73, 110});
  oo::FreePixMap(&grayAlpha);
}

// StretchHorizontally(): 2x2 -> 4x2, into a new buffer, for each format.
TEST(OOTextureScaling, stretchesHorizontally) {
  oo::PixMap rgba = oo::ScalePixMap(MakePatterned(2, 2, oo::kPixMapRGBA), 4, 2, false);
  Pinned("rgba", rgba, 4, 2,
         {13,  50,  87,  124, 13, 50, 87,  124, 161, 198, 235,
          16,  161, 198, 235, 16, 53, 90,  127, 164, 53,  90,
          127, 164, 201, 238, 19, 56, 201, 238, 19,  56});
  oo::FreePixMap(&rgba);

  oo::PixMap gray =
      oo::ScalePixMap(MakePatterned(2, 2, oo::kPixMapGrayscale), 4, 2, false);
  Pinned("gray", gray, 4, 2, {13, 13, 50, 50, 87, 87, 124, 124});
  oo::FreePixMap(&gray);

  oo::PixMap grayAlpha =
      oo::ScalePixMap(MakePatterned(2, 2, oo::kPixMapGrayscaleAlpha), 4, 2, false);
  Pinned(
      "grayAlpha", grayAlpha, 4, 2,
      {13, 50, 13, 50, 87, 124, 87, 124, 161, 198, 161, 198, 235, 16, 235, 16});
  oo::FreePixMap(&grayAlpha);
}

// SqueezeHorizontally(): 4x2 -> 2x2, in place, for each format.
TEST(OOTextureScaling, squeezesHorizontally) {
  oo::PixMap rgba = oo::ScalePixMap(MakePatterned(4, 2, oo::kPixMapRGBA), 2, 2, false);
  Pinned("rgba", rgba, 2, 2,
         {87, 124, 161, 70, 127, 164, 73, 110, 167, 76, 113, 150, 79, 116, 153,
          190});
  oo::FreePixMap(&rgba);

  oo::PixMap gray =
      oo::ScalePixMap(MakePatterned(4, 2, oo::kPixMapGrayscale), 2, 2, false);
  Pinned("gray", gray, 2, 2, {31, 105, 179, 125});
  oo::FreePixMap(&gray);

  oo::PixMap grayAlpha =
      oo::ScalePixMap(MakePatterned(4, 2, oo::kPixMapGrayscaleAlpha), 2, 2, false);
  Pinned("grayAlpha", grayAlpha, 2, 2, {50, 87, 198, 107, 90, 127, 110, 147});
  oo::FreePixMap(&grayAlpha);
}

// StretchVertically() (2x3 -> 2x4 and 2x2 -> 2x4, into a new buffer;
// RGBA and gray+alpha take the 4-byte path StretchVerticallyN_x4, 2-wide
// grayscale the byte path StretchVerticallyN_x1) and the last row of
// StretchHorizontally() (3x1 -> 4x1, one row, so only the last-row loop runs).
// The last output row of a vertical stretch is the last source row and the last
// output pixel of a horizontal one is the last source pixel. 2x3 -> 2x4 and
// every horizontal case gave these same bytes before the fix; 2x2 -> 2x4 copied
// its last row from one row past the source buffer (an ASan
// heap-buffer-overflow READ), as 3x1 -> 4x1 read one pixel past it at weight 0.
TEST(OOTextureScaling, stretchesVerticallyAndKeepsTheEdges) {
  oo::PixMap rgba = oo::ScalePixMap(MakePatterned(2, 3, oo::kPixMapRGBA), 2, 4, false);
  Pinned("rgba 2x3 -> 2x4", rgba, 2, 4,
         {13,  50,  87,  124, 161, 198, 235, 16,  33,  70,  107,
          144, 181, 218, 127, 36,  63,  100, 137, 174, 211, 184,
          29,  66,  93,  130, 167, 204, 241, 22,  59,  96});
  oo::FreePixMap(&rgba);

  oo::PixMap gray =
      oo::ScalePixMap(MakePatterned(2, 3, oo::kPixMapGrayscale), 2, 4, false);
  Pinned("gray 2x3 -> 2x4", gray, 2, 4, {13, 50, 50, 87, 105, 142, 161, 198});
  oo::FreePixMap(&gray);

  oo::PixMap grayAlpha =
      oo::ScalePixMap(MakePatterned(2, 3, oo::kPixMapGrayscaleAlpha), 2, 4, false);
  Pinned(
      "grayAlpha 2x3 -> 2x4", grayAlpha, 2, 4,
      {13, 50, 87, 124, 87, 124, 161, 70, 134, 171, 208, 53, 53, 90, 127, 164});
  oo::FreePixMap(&grayAlpha);

  oo::PixMap rgbaDouble =
      oo::ScalePixMap(MakePatterned(2, 2, oo::kPixMapRGBA), 2, 4, false);
  Pinned("rgba 2x2 -> 2x4", rgbaDouble, 2, 4,
         {13,  50,  87,  124, 161, 198, 235, 16,  13,  50,  87,
          124, 161, 198, 235, 16,  53,  90,  127, 164, 201, 238,
          19,  56,  53,  90,  127, 164, 201, 238, 19,  56});
  oo::FreePixMap(&rgbaDouble);

  oo::PixMap grayDouble =
      oo::ScalePixMap(MakePatterned(2, 2, oo::kPixMapGrayscale), 2, 4, false);
  Pinned("gray 2x2 -> 2x4", grayDouble, 2, 4,
         {13, 50, 13, 50, 87, 124, 87, 124});
  oo::FreePixMap(&grayDouble);

  oo::PixMap grayAlphaDouble =
      oo::ScalePixMap(MakePatterned(2, 2, oo::kPixMapGrayscaleAlpha), 2, 4, false);
  Pinned(
      "grayAlpha 2x2 -> 2x4", grayAlphaDouble, 2, 4,
      {13, 50, 87, 124, 13, 50, 87, 124, 161, 198, 235, 16, 161, 198, 235, 16});
  oo::FreePixMap(&grayAlphaDouble);

  oo::PixMap rgbaRow =
      oo::ScalePixMap(MakePatterned(3, 1, oo::kPixMapRGBA), 4, 1, false);
  Pinned(
      "rgba 3x1 -> 4x1", rgbaRow, 4, 1,
      {13, 50, 87, 124, 87, 124, 161, 70, 134, 171, 208, 53, 53, 90, 127, 164});
  oo::FreePixMap(&rgbaRow);

  oo::PixMap grayRow =
      oo::ScalePixMap(MakePatterned(3, 1, oo::kPixMapGrayscale), 4, 1, false);
  Pinned("gray 3x1 -> 4x1", grayRow, 4, 1, {13, 31, 59, 87});
  oo::FreePixMap(&grayRow);

  oo::PixMap grayAlphaRow =
      oo::ScalePixMap(MakePatterned(3, 1, oo::kPixMapGrayscaleAlpha), 4, 1, false);
  Pinned("grayAlpha 3x1 -> 4x1", grayAlphaRow, 4, 1,
         {13, 50, 50, 87, 105, 142, 161, 198});
  oo::FreePixMap(&grayAlphaRow);
}

// A uniform image stays uniform through every wrapper, in both directions at
// once. Grayscale is squeezed vertically by a whole factor (8 -> 4): by 8 -> 5
// its last row comes out at about 3/8 of the value, a defect of the plain-C
// SqueezeVertically1() that this test does not touch.
TEST(OOTextureScaling, keepsUniformImagesUniform) {
  for (oo::PixMapFormat format : kFormats) {
    const oo::PixMapDimension height = (format == oo::kPixMapGrayscale) ? 4 : 5;
    oo::PixMap squeezed =
        oo::ScalePixMap(MakeUniform(8, 8, format, 0x5A), 3, height, false);
    EXPECT_TRUE(squeezed.pixels != nullptr && squeezed.width == 3 &&
             squeezed.height == height);
    EXPECT_TRUE(AllEqual(squeezed, 0x5A));
    oo::FreePixMap(&squeezed);

    oo::PixMap stretched =
        oo::ScalePixMap(MakeUniform(3, 8, format, 0xC3), 7, height, false);
    EXPECT_TRUE(stretched.pixels != nullptr && stretched.width == 7 &&
             stretched.height == height);
    EXPECT_TRUE(AllEqual(stretched, 0xC3));
    oo::FreePixMap(&stretched);
  }
}

// A uniform grayscale image squeezed vertically by a non-integer
// factor (8 -> 5, and 8x8 -> 3x5 through both squeezers) stays uniform in every
// row, the last included. Before the fix SqueezeVertically1() counted the final
// source row's weight without its value, so the last row of 0x5A came out at 33
// and of 0xC3 at 73; SqueezeVertically2/4 were already right.
TEST(OOTextureScaling, squeezesGrayscaleVerticallyByANonIntegerFactorUniformly) {
  for (uint8_t value :
       {static_cast<uint8_t>(0x5A), static_cast<uint8_t>(0xC3)}) {
    oo::PixMap tall =
        oo::ScalePixMap(MakeUniform(8, 8, oo::kPixMapGrayscale, value), 8, 5, false);
    Pinned("gray 8x8 -> 8x5", tall, 8, 5, std::vector<int>(8 * 5, value));
    oo::FreePixMap(&tall);

    oo::PixMap both =
        oo::ScalePixMap(MakeUniform(8, 8, oo::kPixMapGrayscale, value), 3, 5, false);
    Pinned("gray 8x8 -> 3x5", both, 3, 5, std::vector<int>(3 * 5, value));
    oo::FreePixMap(&both);
  }
}

// A pixmap with no valid format is refused before any wrapper runs: a null
// pixmap and one parameter-error log line, as before.
TEST(OOTextureScaling, refusesAnInvalidPixMap) {
  oo::PixelBuffer invalidBytes = oo::MakePixelBuffer(16);
  oo::PixMap invalid = {invalidBytes->data(), 2, 2, oo::kPixMapInvalidFormat, 8, 16,
                      invalidBytes};
  int before = sParameterErrors;
  oo::PixMap result = oo::ScalePixMap(invalid, 1, 1, false);
  EXPECT_TRUE(result.pixels == nullptr);
  EXPECT_EQ(sParameterErrors, before + 1);
}

// The wrappers' unreachable default arm now raises with OORaiseException(), the
// function form of
// OORaiseException(): the same name and formatted reason (the C++
// oo::Exception, caught as catch (const oo::Exception &)).
TEST(OOTextureScaling, raiseExceptionThrowsTheSameException) {
  const char* name = nullptr;
  std::string reason;
  std::optional<oo::Exception> kept;  // what the autorelease pool kept alive
  try {
    oo::RaiseException(oo::InternalInconsistencyException,
                     "Unsupported pixmap format in scaler: %s",
                     oo::PixMapFormatName(oo::kPixMapInvalidFormat).c_str());
  } catch (const oo::Exception& e)
  {
    kept = e;
    name = kept->name();
    reason = kept->reason();
  }
  EXPECT_TRUE(name != nullptr &&
           std::strcmp(name, oo::InternalInconsistencyException) == 0);
  EXPECT_EQ(reason, std::string("Unsupported pixmap format in scaler: ") +
                          oo::PixMapFormatName(oo::kPixMapInvalidFormat));

  std::string messageReason;
  try {
    oo::RaiseException(oo::InternalInconsistencyException,
                     "Unsupported pixmap format in scaler: %s",
                     oo::PixMapFormatName(oo::kPixMapInvalidFormat)
                         .c_str());
  } catch (const oo::Exception& e)   // was @catch (OOException *e)
  {
    messageReason = e.reason();
  }
  EXPECT_EQ(messageReason, reason);
}
