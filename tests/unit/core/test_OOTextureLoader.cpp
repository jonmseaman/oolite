/*	test_OOTextureLoader.mm
        Unit tests for OOTextureLoader (src/Core/Materials/OOTextureLoader.h),
   the root of the texture loaders.

        A loader is a task on the game's work manager: its subclass's
   -loadTexture fills the pixels on a work thread, the root then applies the
   options (channel extraction, scaling to a power of two, mip-maps), and the
   texture takes the result on the main thread. OOPNGTextureLoader,
        OOPixMapTextureLoader and the generators derive from it in their own
   files. The test's loader is a subclass that makes its pixels itself, so no
   file is read; the class factory is asked only for paths it refuses. It links
   the whole game but main (tests/unit/core/meson.build entry ['*']), on the
   hidden GL context of oo_gl_test_context.hpp (the loaders' one-time set-up
        reads the GL texture size limit). The test's loader is a C++ subclass,
   and the cases call its members. The last test pins
   the C++ API (OOTextureLoader). Run: tests/run.sh unit
*/

#include <cstdlib>

#include <gtest/gtest.h>

#include "AsyncWorkManager.h"
#include "Texture.h"
#include "TextureLoader.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/Exception.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

static int gLoads = 0;

// A loader whose pixels are a width x height image of the given format whose
// bytes count up from 1, or none, or an exception.
class TestLoader : public oo::TextureLoader {
 public:
  void loadTexture() override {
    gLoads++;
    if (_testRaise)
      oo::RaiseException("TestLoaderException", "test loader raised");
    if (_testEmpty) return;
    const size_t size = (size_t)_testWidth * _testHeight *
                        oo::TextureComponentsForFormat(_testFormat);
    oo::PixelBuffer buffer = oo::MakePixelBuffer(size);
    uint8_t* bytes = buffer->data();
    for (size_t i = 0; i < size; i++) bytes[i] = (uint8_t)(i + 1);
    _data = buffer;
    _width = _testWidth;
    _height = _testHeight;
    _format = _testFormat;
  }

  // The root's state, as the subclasses read it (the one block the conversion
  // ports).
  oo::TextureFlags testOptions() { return _options; }
  uint32_t testMaxSize() { return _maxSize; }
  uint32_t testShrinkThreshold() { return _shrinkThreshold; }
  bool testGenerateMipMaps() { return _generateMipMaps; }
  bool testAvoidShrinking() { return _avoidShrinking; }
  bool testNoScaling() { return _noScalingWhatsoever; }
  bool testExtractChannel() { return _extractChannel; }
  uint8_t testExtractChannelIndex() { return _extractChannelIndex; }
  bool testAllowCubeMap() { return _allowCubeMap; }
  std::string testPath() { return _path; }

  uint32_t _testWidth = 0, _testHeight = 0;
  oo::TextureDataFormat _testFormat = {};
  bool _testEmpty = false, _testRaise = false;
};

namespace {

// The loaders' sizes come from their one-time set-up, which a path with no
// known extension runs (and then answers nil, reading nothing).
void SetUpLoaders() {
  {
    (void)oo::TextureLoader::loaderWithPath(std::string("unknown.type"), oo::TextureFlags::kNone);
  }
}

// The loaders made, kept for the test's life (each was autoreleased in its
// case's pool).
std::vector<std::shared_ptr<TestLoader>> gKeptLoaders;

TestLoader* Loader(const char* path, oo::TextureFlags options, uint32_t width,
                   uint32_t height, oo::TextureDataFormat format) {
  const std::shared_ptr<TestLoader> made = oo::makeShared<TestLoader>();
  TestLoader* loader =
      made->initWithPath(std::string(path), options) ? made.get() : nullptr;
  if (loader != nullptr) {
    gKeptLoaders.push_back(made);
    loader->_testWidth = width;
    loader->_testHeight = height;
    loader->_testFormat = format;
  }
  return loader;
}

bool Queue(oo::TextureLoader* loader) {
  // The work manager's task is the C++ loader.
  return oo::AsyncWorkManager::sharedAsyncWorkManager()->addTask(
      loader, oo::AsyncWorkPriority::kMedium);
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

}  // namespace

TEST(OOTextureLoader, factoryRefusals) {
  EXPECT_TRUE(OOTestGLContext());
  {
    EXPECT_TRUE(oo::TextureLoader::loaderWithPath(std::nullopt, oo::TextureFlags::kNone) == nullptr);
    EXPECT_TRUE(oo::TextureLoader::loaderWithPath(
                 std::string("textures/unknown.type"), oo::TextureFlags::kNone) == nullptr);
    EXPECT_TRUE(oo::TextureLoader::loaderWithTextureSpecifier(
                 oo::PList(3.0), oo::TextureFlags::kNone, std::string("Textures")) == nullptr);
    EXPECT_TRUE(oo::TextureLoader::loaderWithTextureSpecifier(
                 oo::PList(), oo::TextureFlags::kNone, std::string("Textures")) == nullptr);
    EXPECT_TRUE(!oo::makeShared<TestLoader>()->initWithPath(std::nullopt, oo::TextureFlags::kNone));
  }
}

TEST(OOTextureLoader, optionsDecoded) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    TestLoader* plain =
        Loader("dir/plain.png", oo::TextureFlags::kMinFilterLinear, 4, 4, oo::kPixMapRGBA);
    EXPECT_TRUE(plain->testOptions() == oo::TextureFlags::kMinFilterLinear);
    EXPECT_TRUE(!plain->testGenerateMipMaps() && !plain->testAvoidShrinking() &&
             !plain->testNoScaling());
    EXPECT_TRUE(plain->testShrinkThreshold() == 512 && plain->testMaxSize() >= 64);
    EXPECT_TRUE(!plain->testExtractChannel() && !plain->testAllowCubeMap());
    EXPECT_TRUE(plain->testPath() == "dir/plain.png" &&
             plain->path() == std::optional<std::string>("dir/plain.png"));
    EXPECT_TRUE(plain->cacheKey() ==
             std::optional<std::string>("plain.png:0x0002"));
    EXPECT_TRUE(!plain->isReady());

    EXPECT_TRUE(Loader("a", oo::TextureFlags::kMinFilterMipMap, 1, 1, oo::kPixMapRGBA)
                 ->testGenerateMipMaps());
    TestLoader* noShrink = Loader("b", oo::TextureFlags::kNoShrink, 1, 1, oo::kPixMapRGBA);
    EXPECT_TRUE(noShrink->testAvoidShrinking() &&
             noShrink->testShrinkThreshold() == UINT32_MAX);
    TestLoader* never = Loader("c", oo::TextureFlags::kNeverScale, 1, 1, oo::kPixMapRGBA);
    EXPECT_TRUE(never->testNoScaling() &&
             never->testShrinkThreshold() == UINT32_MAX);
    TestLoader* extra = Loader("d", oo::TextureFlags::kExtraShrink, 1, 1, oo::kPixMapRGBA);
    EXPECT_TRUE(extra->testShrinkThreshold() == 128 &&
             extra->testMaxSize() <= 256);
    EXPECT_TRUE(Loader("e", oo::TextureFlags::kAllowCubeMap, 1, 1, oo::kPixMapRGBA)
                 ->testAllowCubeMap());
    TestLoader* green =
        Loader("f", oo::TextureFlags::kExtractChannelG, 1, 1, oo::kPixMapRGBA);
    EXPECT_TRUE(green->testExtractChannel() &&
             green->testExtractChannelIndex() == 1);
    TestLoader* alpha =
        Loader("g", oo::TextureFlags::kExtractChannelA, 1, 1, oo::kPixMapRGBA);
    EXPECT_TRUE(alpha->testExtractChannel() &&
             alpha->testExtractChannelIndex() == 3);
  }
}

TEST(OOTextureLoader, descriptions) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    TestLoader* loader =
        Loader("dir/desc.png", oo::TextureFlags::kMinFilterLinear, 4, 4, oo::kPixMapRGBA);
    EXPECT_TRUE(loader->description().starts_with("<TestLoader 0x"));
    EXPECT_TRUE(loader->description().ends_with(">{{dir/desc.png -- loading}}"));
    EXPECT_TRUE(loader->shortDescription().ends_with(">{desc.png}"));
    EXPECT_TRUE(Queue(loader));
    Result result = GetResult(loader);
    EXPECT_TRUE(result.ok && loader->isReady());
    EXPECT_TRUE(loader->description().ends_with(
        ">{{dir/desc.png -- failed}}"));  // the pixels were handed over
    oo::FreePixMap(&result.pixMap);
  }
}

TEST(OOTextureLoader, loadsScalesAndHandsOver) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    const int loads = gLoads;
    TestLoader* loader =
        Loader("square.png", oo::TextureFlags::kMinFilterLinear, 8, 8, oo::kPixMapRGBA);
    EXPECT_TRUE(Queue(loader));
    Result result = GetResult(loader);  // waits for the work thread
    EXPECT_TRUE(gLoads == loads + 1);
    EXPECT_TRUE(result.ok && result.format == oo::kPixMapRGBA);
    EXPECT_TRUE(result.originalWidth == 8 && result.originalHeight == 8);
    EXPECT_TRUE(result.pixMap.width == 8 && result.pixMap.height == 8 &&
             result.pixMap.rowBytes == 32);  // a power of two: as loaded
    EXPECT_TRUE(result.pixMap.pixels != NULL &&
             ((uint8_t*)result.pixMap.pixels)[0] == 1);
    oo::FreePixMap(&result.pixMap);

    // The pixels were handed over: a second request has none.
    Result again = GetResult(loader);
    EXPECT_TRUE(!again.ok && again.format == oo::kPixMapInvalidFormat &&
             oo::IsNullPixMap(again.pixMap));

    // A 4 x 2 image is scaled to 2 x 2 (a power of two near two thirds of each
    // side).
    TestLoader* wide =
        Loader("wide.png", oo::TextureFlags::kMinFilterLinear, 4, 2, oo::kPixMapRGBA);
    EXPECT_TRUE(Queue(wide));
    Result scaled = GetResult(wide);
    EXPECT_TRUE(scaled.ok && scaled.pixMap.width == 2 &&
             scaled.pixMap.height == 2);
    EXPECT_TRUE(scaled.originalWidth == 4 && scaled.originalHeight == 2);
    EXPECT_TRUE(((uint8_t*)scaled.pixMap.pixels)[0] == 3);
    oo::FreePixMap(&scaled.pixMap);

    // Never scaled: as loaded.
    TestLoader* never =
        Loader("never.png", oo::TextureFlags::kNeverScale, 4, 2, oo::kPixMapRGBA);
    EXPECT_TRUE(Queue(never));
    Result asLoaded = GetResult(never);
    EXPECT_TRUE(asLoaded.ok && asLoaded.pixMap.width == 4 &&
             asLoaded.pixMap.height == 2);
    oo::FreePixMap(&asLoaded.pixMap);
  }
}

TEST(OOTextureLoader, extractsAChannel) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    TestLoader* loader = Loader(
        "green.png", oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kExtractChannelG, 8,
        8, oo::kPixMapRGBA);
    EXPECT_TRUE(Queue(loader));
    Result result = GetResult(loader);
    EXPECT_TRUE(result.ok && result.format == oo::kPixMapGrayscale);
    EXPECT_TRUE(result.pixMap.width == 8 && result.pixMap.height == 8 &&
             result.pixMap.rowBytes == 8);
    EXPECT_TRUE(((uint8_t*)result.pixMap.pixels)[0] ==
             2);  // the first pixel's green byte
    oo::FreePixMap(&result.pixMap);
  }
}

TEST(OOTextureLoader, mipMaps) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    TestLoader* loader =
        Loader("mip.png", oo::TextureFlags::kMinFilterMipMap, 8, 8, oo::kPixMapRGBA);
    EXPECT_TRUE(Queue(loader));
    Result result = GetResult(loader);
    EXPECT_TRUE(result.ok && result.pixMap.width == 8 &&
             result.pixMap.height == 8);
    EXPECT_TRUE(((uint8_t*)result.pixMap.pixels)[0] ==
             1);  // the base level as loaded
    oo::FreePixMap(&result.pixMap);
  }
}

TEST(OOTextureLoader, failures) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    TestLoader* empty =
        Loader("empty.png", oo::TextureFlags::kMinFilterLinear, 4, 4, oo::kPixMapRGBA);
    empty->_testEmpty = true;
    EXPECT_TRUE(Queue(empty));
    Result none = GetResult(empty);
    EXPECT_TRUE(!none.ok && none.format == oo::kPixMapInvalidFormat &&
             empty->isReady());
    EXPECT_TRUE(empty->description().ends_with(">{{empty.png -- failed}}"));

    TestLoader* raising =
        Loader("raise.png", oo::TextureFlags::kMinFilterLinear, 4, 4, oo::kPixMapRGBA);
    raising->_testRaise = true;
    EXPECT_TRUE(Queue(raising));
    Result raised =
        GetResult(raising);  // the exception is caught on the work thread
    EXPECT_TRUE(!raised.ok && raising->isReady());
  }
}

// --- The C++ API (after the conversion)
// --------------------------------------------------------

// A converted loader, as OOPNGTextureLoader will be one: a C++ subclass that
// fills the pixels.
class TestCxxLoader final : public oo::TextureLoader {
 public:
  void loadTexture() override {
    loads++;
    oo::PixelBuffer buffer = oo::MakePixelBuffer(8 * 8 * 4);
    uint8_t* bytes = buffer->data();
    for (unsigned i = 0; i < 8 * 8 * 4; i++) bytes[i] = (uint8_t)(i + 1);
    _data = buffer;
    _width = 8;
    _height = 8;
    _format = oo::kPixMapRGBA;
  }

  std::optional<std::string> descriptionComponents() const override {
    return std::string("cxx");
  }

  int loads = 0;
};

TEST(OOTextureLoader, cxxApi) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    EXPECT_TRUE(!oo::TextureLoader::loaderWithPath(std::nullopt, oo::TextureFlags::kNone));
    EXPECT_TRUE(!oo::TextureLoader::loaderWithPath(std::string("unknown.type"), oo::TextureFlags::kNone));
    EXPECT_TRUE(!oo::TextureLoader::loaderWithTextureSpecifier(
        oo::PList(3.0), oo::TextureFlags::kNone, std::string("Textures")));

    const std::shared_ptr<TestCxxLoader> loader = oo::makeShared<TestCxxLoader>();
    EXPECT_TRUE(!loader->initWithPath(std::nullopt, oo::TextureFlags::kNone));
    EXPECT_TRUE(loader->initWithPath(std::string("dir/cxx.png"),
                                  oo::TextureFlags::kMinFilterLinear));
    EXPECT_TRUE(loader->path() == std::optional<std::string>("dir/cxx.png") &&
             loader->_shrinkThreshold == 512);
    EXPECT_TRUE(loader->cacheKey() ==
             std::optional<std::string>("cxx.png:0x0002"));
    EXPECT_TRUE(loader->shortDescriptionComponents() ==
             std::optional<std::string>("cxx.png"));

    // Queued, loaded on a work thread, handed over on this one.
    EXPECT_TRUE(Queue(loader.get()));
    oo::PixMap pixMap = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    uint32_t width = 0, height = 0;
    const std::optional<oo::TextureLoader::Result> loaded = loader->getResult();
    EXPECT_TRUE(loaded.has_value());
    if (loaded) {
      pixMap = loaded->pixMap;
      format = loaded->format;
      width = loaded->originalWidth;
      height = loaded->originalHeight;
    }
    EXPECT_TRUE(loader->loads == 1 && loader->isReady() &&
             format == oo::kPixMapRGBA);
    EXPECT_TRUE(pixMap.width == 8 && pixMap.height == 8 && width == 8 &&
             height == 8);
    EXPECT_TRUE(((uint8_t*)pixMap.pixels)[0] == 1);
    oo::FreePixMap(&pixMap);

    // The C++ loader's overrides and the root's defaults, as callers
    // reach them.
    EXPECT_TRUE(loader->description().starts_with("<TestCxxLoader 0x") &&
             loader->description().ends_with(">{cxx}"));
    EXPECT_TRUE(loader->cacheKey() ==
                 std::optional<std::string>("cxx.png:0x0002") &&
             loader->isReady());
  }
}
