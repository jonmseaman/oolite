/*	test_OOPixMapTextureLoader.mm
        Unit tests for OOPixMapTextureLoader
   (src/Core/Materials/OOPixMapTextureLoader.h), the texture generator whose
   pixels are a pixmap made in memory (the planets' generated surface and cloud
   textures).

        It takes over the pixmap it is given, answers its texture options with
   the defaults applied, and hands the pixels to the texture as they are, after
   making room for and generating mip-maps when its options ask for them. It
   links the whole game but main (tests/unit/core/meson.build entry ['*']), on
   the hidden GL context of oo_gl_test_context.hpp. Making a loader goes through
   the helper below: the class is a global C++ class made by its factory. The
   last test pins the C++ API. Run: tests/run.sh unit
*/

#include <cstdlib>

#include <gtest/gtest.h>

#include "ConcreteTexture.h"
#include "PixMapTextureLoader.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/AutoreleaseScope.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

// The C++ texture, kept until the current autorelease scope drains
// (oo::autorelease).
static oo::Texture* Autoreleased(const std::shared_ptr<oo::Texture>& texture) {
  if (texture == nullptr) return nullptr;
  oo::autorelease(std::shared_ptr<oo::Texture>(texture));
  return texture.get();
}

namespace {

// The loaders' sizes come from their one-time set-up, which a path with no
// known extension runs (and then answers nil, reading nothing).
void SetUpLoaders() {
  {
    oo::AutoreleaseScope pool;
    (void)oo::TextureLoader::loaderWithPath(std::string("unknown.type"), oo::TextureFlags::kNone);
  }
}

// An 8 x 8 RGBA pixmap whose bytes count up from 1, allocated with OOAllocatePixMap().
oo::PixMap TestPixMap() {
  oo::PixMap pixMap = oo::AllocatePixMap(8, 8, oo::kPixMapRGBA, 0, 0);
  uint8_t* bytes = (uint8_t*)pixMap.pixels;
  for (unsigned i = 0; i < 8 * 8 * 4; i++) bytes[i] = (uint8_t)(i + 1);
  return pixMap;
}

// The generators made, kept for the test's life (each was autoreleased in its
// case's pool).
std::vector<std::shared_ptr<oo::TextureGenerator>> gKeptGenerators;

oo::TextureGenerator* Kept(std::shared_ptr<oo::TextureGenerator> generator) {
  if (generator != nullptr) gKeptGenerators.push_back(generator);
  return generator.get();
}

// Was [[OOPixMapTextureLoader alloc] initWithPixMap:...], autoreleased: the
// loader as a generator.
oo::TextureGenerator* NewLoader(oo::PixMap pixMap, oo::TextureFlags options,
                              bool freeWhenDone) {
  return Kept(
      oo::PixMapTextureLoader::loaderWithPixMap(pixMap, options, freeWhenDone));
}

void ClearCache() {
  {
    oo::AutoreleaseScope scope;
    oo::Texture::clearCache();
  }
}

}  // namespace

TEST(OOPixMapTextureLoader, takesThePixMap) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* loader = NewLoader(
        TestPixMap(), oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kRepeatS, true);
    EXPECT_TRUE(loader != nullptr &&
             dynamic_cast<oo::TextureGenerator*>(loader) != nullptr);
    EXPECT_TRUE(loader->textureOptions() ==
             oo::ApplyTextureOptionDefaults(oo::TextureFlags::kMinFilterLinear |
                                          oo::TextureFlags::kRepeatS));
    EXPECT_TRUE(loader->path().value_or("").starts_with("OOPixMap@"));
    EXPECT_TRUE(!loader->cacheKey().has_value());  // a generator: not cached

    EXPECT_TRUE(loader->enqueue());
    oo::PixMap result = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    uint32_t width = 0, height = 0;
    const std::optional<oo::TextureLoader::Result> loaded = loader->getResult();
    EXPECT_TRUE(loaded.has_value());
    if (loaded) {
      result = loaded->pixMap;
      format = loaded->format;
      width = loaded->originalWidth;
      height = loaded->originalHeight;
    }
    EXPECT_TRUE(format == oo::kPixMapRGBA && result.width == 8 &&
             result.height == 8 && result.rowBytes == 32);
    EXPECT_TRUE(width == 8 && height == 8);
    EXPECT_TRUE(((const uint8_t*)result.pixels)[0] == 1 &&
             ((const uint8_t*)result.pixels)[255] == 0);
    oo::FreePixMap(&result);
  }
}

TEST(OOPixMapTextureLoader, refusals) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    // Not a pixmap.
    EXPECT_TRUE(NewLoader(oo::kNullPixMap, oo::TextureFlags::kMinFilterLinear, true) ==
             nullptr);

    // Not taken over: the initialiser duplicates its own (still empty) pixmap,
    // not the one it was given, so it answers nil and the caller keeps its
    // pixels. (As it has since 2010; kept.)
    oo::PixMap kept = TestPixMap();
    EXPECT_TRUE(NewLoader(kept, oo::TextureFlags::kMinFilterLinear, false) == nullptr);
    EXPECT_TRUE(oo::IsValidPixMap(kept) && ((const uint8_t*)kept.pixels)[0] == 1);
    oo::FreePixMap(&kept);
  }
}

TEST(OOPixMapTextureLoader, loadsWithMipMaps) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* loader =
        NewLoader(TestPixMap(), oo::TextureFlags::kMinFilterMipMap, true);
    EXPECT_TRUE(loader->textureOptions() ==
             oo::ApplyTextureOptionDefaults(oo::TextureFlags::kMinFilterMipMap));
    EXPECT_TRUE(loader->enqueue());
    oo::PixMap result = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    const std::optional<oo::TextureLoader::Result> loaded = loader->getResult();
    EXPECT_TRUE(loaded.has_value());
    if (loaded) {
      result = loaded->pixMap;
      format = loaded->format;
    }
    EXPECT_TRUE(result.width == 8 && result.height == 8 &&
             format == oo::kPixMapRGBA);
    EXPECT_TRUE(((const uint8_t*)result.pixels)[0] ==
             1);  // the base level as given
    oo::FreePixMap(&result);
  }
}

TEST(OOPixMapTextureLoader, aTextureOfTheGenerator) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* loader = NewLoader(
        TestPixMap(), oo::TextureFlags::kDefaultOptions | oo::TextureFlags::kRepeatS, true);
    oo::Texture* texture = Autoreleased(oo::Texture::textureWithGenerator(loader));
    EXPECT_TRUE((dynamic_cast<oo::ConcreteTexture*>(texture) != nullptr));
    texture->ensureFinishedLoading();
    EXPECT_TRUE(texture->dimensions().width == 8 &&
             texture->dimensions().height == 8);
    EXPECT_TRUE(texture->isFinishedLoading());
  }
  ClearCache();
}

// --- The C++ API (after the conversion)
// --------------------------------------------------------

TEST(OOPixMapTextureLoader, cxxApi) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(!oo::PixMapTextureLoader::loaderWithPixMap(oo::kNullPixMap, oo::TextureFlags::kNone, true));

    const std::shared_ptr<oo::PixMapTextureLoader> loader =
        oo::PixMapTextureLoader::loaderWithPixMap(
            TestPixMap(), oo::TextureFlags::kMinFilterLinear, true);
    EXPECT_TRUE(loader &&
             loader->textureOptions() ==
                 oo::ApplyTextureOptionDefaults(oo::TextureFlags::kMinFilterLinear));
    EXPECT_TRUE(loader->path().value_or("").starts_with("OOPixMap@") &&
             !loader->cacheKey().has_value());

    // Loading hands the pixels over as they are.
    loader->loadTexture();
    EXPECT_TRUE(loader->_data != nullptr && loader->_width == 8 &&
             loader->_height == 8 && loader->_rowBytes == 32);
    EXPECT_TRUE(loader->_format == oo::kPixMapRGBA &&
             loader->_data->data()[0] == 1);
  }
}
