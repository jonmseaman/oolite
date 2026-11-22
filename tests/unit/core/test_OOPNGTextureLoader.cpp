/*	test_OOPNGTextureLoader.mm
        Unit tests for OOPNGTextureLoader
   (src/Core/Materials/OOPNGTextureLoader.h), the texture loader that reads PNG
   files.

        The loaders' factory makes one for a path ending in .png and queues it
   on the game's work manager; on a work thread it reads the file with libpng,
   and the texture takes the pixels on the main thread. The test writes its PNG
   files with libpng's simplified API into a scratch directory under the working
   directory, so no game resource is read. It links the whole game but main
   (tests/unit/core/meson.build entry ['*']), on the hidden GL context of
   oo_gl_test_context.hpp (the loaders' one-time set-up reads the GL texture
   size limit). The loader is made by the factory. The last test pins the C++
   API of the class (a global C++ class). Run: tests/run.sh unit
*/

#include <png.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>

#include "AsyncWorkManager.h"
#include "PNGTextureLoader.h"
#include "oo_gl_test_context.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

const std::filesystem::path& ScratchDirectory() {
  static const std::filesystem::path directory = [] {
    std::filesystem::path result =
        std::filesystem::current_path() / "test_OOPNGTextureLoader.d";
    std::filesystem::remove_all(result);
    std::filesystem::create_directories(result);
    return result;
  }();
  return directory;
}

// A width x height PNG of the given libpng format whose bytes count up from 1;
// its path.
std::string WritePNG(const char* name, uint32_t width, uint32_t height,
                     png_uint_32 format) {
  png_image image = {};
  image.version = PNG_IMAGE_VERSION;
  image.width = width;
  image.height = height;
  image.format = format;
  std::vector<uint8_t> bytes(PNG_IMAGE_SIZE(image));
  for (size_t i = 0; i < bytes.size(); i++) bytes[i] = (uint8_t)(i + 1);

  const std::string path = (ScratchDirectory() / name).generic_string();
  const bool written = png_image_write_to_file(&image, path.c_str(), 0,
                                               bytes.data(), 0, nullptr) != 0;
  png_image_free(&image);
  return written ? path : std::string();
}

void WriteBytes(const std::string& path, const std::vector<char>& bytes) {
  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  file.write(bytes.data(), (std::streamsize)bytes.size());
}

std::vector<char> ReadBytes(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  return std::vector<char>(std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>());
}

// The loaders' sizes come from their one-time set-up, which a path with no
// known extension runs (and then answers nil, reading nothing).
void SetUpLoaders() {
  {
    (void)oo::TextureLoader::loaderWithPath(std::string("unknown.type"), oo::TextureFlags::kNone);
  }
}

struct Result {
  bool ok = false;
  oo::PixMap pixMap = oo::kNullPixMap;
  oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
  uint32_t originalWidth = 0, originalHeight = 0;
};

Result GetResult(oo::TextureLoader* loader) {
  Result result;
  if (std::optional<oo::TextureLoader::Result> loaded = loader->getResult()) {
    result.ok = true;
    result.pixMap = loaded->pixMap;
    result.format = loaded->format;
    result.originalWidth = loaded->originalWidth;
    result.originalHeight = loaded->originalHeight;
  }
  return result;
}

// The loaders made, kept for the test's life (each was autoreleased in its
// case's pool).
std::vector<std::shared_ptr<oo::TextureLoader>> gKeptLoaders;

// The factory's loader for a path, already queued.
oo::TextureLoader* Load(const std::string& path) {
  const std::shared_ptr<oo::TextureLoader> loader =
      oo::TextureLoader::loaderWithPath(path, oo::TextureFlags::kMinFilterLinear);
  if (loader != nullptr) gKeptLoaders.push_back(loader);
  return loader.get();
}

}  // namespace

TEST(OOPNGTextureLoader, theFactoryMakesOneForAPNG) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    const std::string path = WritePNG("made.png", 8, 8, PNG_FORMAT_RGBA);
    EXPECT_TRUE(!path.empty());
    oo::TextureLoader* loader = Load(path);
    EXPECT_TRUE(loader != nullptr);
    EXPECT_TRUE(loader->description().starts_with("<OOPNGTextureLoader 0x"));
    EXPECT_TRUE(loader->path() == std::optional<std::string>(path));
    EXPECT_TRUE(loader->cacheKey() ==
             std::optional<std::string>("made.png:0x0002"));
    Result result = GetResult(loader);
    EXPECT_TRUE(result.ok && loader->isReady());
    oo::FreePixMap(&result.pixMap);

    // The extension is matched without regard to case.
    const std::string upper = WritePNG("UPPER.PNG", 8, 8, PNG_FORMAT_RGBA);
    oo::TextureLoader* upperLoader = Load(upper);
    EXPECT_TRUE(upperLoader != nullptr &&
             upperLoader->description().starts_with("<OOPNGTextureLoader 0x"));
    Result upperResult = GetResult(upperLoader);
    EXPECT_TRUE(upperResult.ok);
    oo::FreePixMap(&upperResult.pixMap);
  }
}

TEST(OOPNGTextureLoader, readsRGBA) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    Result result =
        GetResult(Load(WritePNG("rgba.png", 8, 8, PNG_FORMAT_RGBA)));
    EXPECT_TRUE(result.ok && result.format == oo::kPixMapRGBA);
    EXPECT_TRUE(result.pixMap.width == 8 && result.pixMap.height == 8 &&
             result.pixMap.rowBytes == 32);
    EXPECT_TRUE(result.originalWidth == 8 && result.originalHeight == 8);
    const uint8_t* pixels = (const uint8_t*)result.pixMap.pixels;
    EXPECT_TRUE(pixels != NULL && pixels[0] == 1 && pixels[3] == 4 &&
             pixels[255] == 0);  // byte 256 of the file's image
    oo::FreePixMap(&result.pixMap);
  }
}

TEST(OOPNGTextureLoader, readsRGBWithAnOpaqueAlpha) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    Result result = GetResult(Load(WritePNG("rgb.png", 8, 8, PNG_FORMAT_RGB)));
    EXPECT_TRUE(result.ok && result.format == oo::kPixMapRGBA);
    EXPECT_TRUE(result.pixMap.width == 8 && result.pixMap.rowBytes == 32);
    const uint8_t* pixels = (const uint8_t*)result.pixMap.pixels;
    EXPECT_TRUE(pixels[0] == 1 && pixels[1] == 2 && pixels[2] == 3 &&
             pixels[3] == 0xFF);
    EXPECT_TRUE(pixels[4] == 4 && pixels[7] == 0xFF);
    oo::FreePixMap(&result.pixMap);
  }
}

TEST(OOPNGTextureLoader, readsGrayscale) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    Result gray = GetResult(Load(WritePNG("gray.png", 8, 8, PNG_FORMAT_GRAY)));
    EXPECT_TRUE(gray.ok && gray.format == oo::kPixMapGrayscale);
    EXPECT_TRUE(gray.pixMap.width == 8 && gray.pixMap.height == 8 &&
             gray.pixMap.rowBytes == 8);
    EXPECT_TRUE(((const uint8_t*)gray.pixMap.pixels)[0] == 1 &&
             ((const uint8_t*)gray.pixMap.pixels)[63] == 64);
    oo::FreePixMap(&gray.pixMap);

    Result grayAlpha =
        GetResult(Load(WritePNG("grayalpha.png", 8, 8, PNG_FORMAT_GA)));
    EXPECT_TRUE(grayAlpha.ok && grayAlpha.format == oo::kPixMapGrayscaleAlpha);
    EXPECT_TRUE(grayAlpha.pixMap.width == 8 && grayAlpha.pixMap.rowBytes == 16);
    EXPECT_TRUE(((const uint8_t*)grayAlpha.pixMap.pixels)[0] == 1 &&
             ((const uint8_t*)grayAlpha.pixMap.pixels)[1] == 2);
    oo::FreePixMap(&grayAlpha.pixMap);
  }
}

TEST(OOPNGTextureLoader, sixteenBitsAreStrippedToEight) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    Result result =
        GetResult(Load(WritePNG("deep.png", 8, 8, PNG_FORMAT_LINEAR_Y)));
    EXPECT_TRUE(result.ok && result.format == oo::kPixMapGrayscale);
    EXPECT_TRUE(result.pixMap.width == 8 && result.pixMap.height == 8 &&
             result.pixMap.rowBytes == 8);
    oo::FreePixMap(&result.pixMap);
  }
}

TEST(OOPNGTextureLoader, failures) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    // No file.
    oo::TextureLoader* missing =
        Load((ScratchDirectory() / "missing.png").generic_string());
    EXPECT_TRUE(missing != nullptr);
    Result none = GetResult(missing);
    EXPECT_TRUE(!none.ok && none.format == oo::kPixMapInvalidFormat &&
             missing->isReady());

    // Not a PNG: libpng's error is caught, and nothing is answered.
    const std::string garbage =
        (ScratchDirectory() / "garbage.png").generic_string();
    WriteBytes(garbage, std::vector<char>(64, 'x'));
    oo::TextureLoader* garbageLoader = Load(garbage);
    Result notPNG = GetResult(garbageLoader);
    EXPECT_TRUE(!notPNG.ok && oo::IsNullPixMap(notPNG.pixMap) &&
             garbageLoader->isReady());

    // A file cut short: the read beyond its end is an error, and the pixels
    // read are freed.
    const std::vector<char> whole =
        ReadBytes(WritePNG("whole.png", 8, 8, PNG_FORMAT_RGBA));
    EXPECT_TRUE(whole.size() > 64);
    const std::string truncated =
        (ScratchDirectory() / "truncated.png").generic_string();
    WriteBytes(
        truncated,
        std::vector<char>(whole.begin(),
                          whole.begin() + (std::ptrdiff_t)(whole.size() / 2)));
    oo::TextureLoader* truncatedLoader = Load(truncated);
    Result cutShort = GetResult(truncatedLoader);
    EXPECT_TRUE(!cutShort.ok && oo::IsNullPixMap(cutShort.pixMap) &&
             truncatedLoader->isReady());
    EXPECT_TRUE(truncatedLoader->description().ends_with(">{{" + truncated +
                                                      " -- failed}}"));
  }
}

// --- The C++ API (after the conversion)
// --------------------------------------------------------

TEST(OOPNGTextureLoader, cxxApi) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    const std::string path = WritePNG("cxx.png", 8, 8, PNG_FORMAT_RGBA);
    const std::shared_ptr<oo::PNGTextureLoader> loader =
        oo::makeShared<oo::PNGTextureLoader>();
    EXPECT_TRUE(!loader->initWithPath(std::nullopt, oo::TextureFlags::kNone));
    EXPECT_TRUE(loader->initWithPath(path, oo::TextureFlags::kMinFilterLinear));
    EXPECT_TRUE(loader->path() == std::optional<std::string>(path) &&
             loader->cacheKey() ==
                 std::optional<std::string>("cxx.png:0x0002"));

    // The work manager takes the C++ loader.
    EXPECT_TRUE(loader->description().starts_with("<OOPNGTextureLoader 0x"));
    EXPECT_TRUE(oo::AsyncWorkManager::sharedAsyncWorkManager()->addTask(
        loader.get(), oo::AsyncWorkPriority::kMedium));
    oo::PixMap pixMap = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    std::optional<oo::TextureLoader::Result> loaded = loader->getResult();
    if (loaded) {
      pixMap = loaded->pixMap;
      format = loaded->format;
    }
    EXPECT_TRUE(loaded.has_value() && loader->isReady());
    EXPECT_TRUE(format == oo::kPixMapRGBA && pixMap.width == 8 &&
             ((const uint8_t*)pixMap.pixels)[0] == 1);
    oo::FreePixMap(&pixMap);

    // The factory's loader is one.
    std::shared_ptr<oo::TextureLoader> made =
        oo::TextureLoader::loaderWithPath(path, oo::TextureFlags::kMinFilterLinear);
    EXPECT_TRUE(dynamic_cast<oo::PNGTextureLoader*>(made.get()) != nullptr);
    loaded = made->getResult();
    if (loaded) {
      pixMap = loaded->pixMap;
      format = loaded->format;
    }
    EXPECT_TRUE(loaded.has_value());
    oo::FreePixMap(&pixMap);

    // Reading on its own (as libpng's read callback does): the loader reads its
    // file first.
    const std::shared_ptr<oo::PNGTextureLoader> direct =
        oo::makeShared<oo::PNGTextureLoader>();
    EXPECT_TRUE(
        direct->initWithPath(WritePNG("direct.png", 8, 8, PNG_FORMAT_GA), oo::TextureFlags::kNone));
    direct->loadTexture();
    EXPECT_TRUE(direct->_data != nullptr &&
             direct->_format == oo::kPixMapGrayscaleAlpha &&
             direct->_width == 8 && direct->_rowBytes == 16);
  }
}
