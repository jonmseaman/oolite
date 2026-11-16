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

#include "ConcreteTexture.h"
#include "PixMapTextureLoader.h"
#include "oo_gl_test_context.hpp"
#include "oo_test.hpp"
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

OO_TEST(takesThePixMap) {
  OO_CHECK(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* loader = NewLoader(
        TestPixMap(), oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kRepeatS, true);
    OO_CHECK(loader != nullptr &&
             dynamic_cast<oo::TextureGenerator*>(loader) != nullptr);
    OO_CHECK(loader->textureOptions() ==
             oo::ApplyTextureOptionDefaults(oo::TextureFlags::kMinFilterLinear |
                                          oo::TextureFlags::kRepeatS));
    OO_CHECK(loader->path().value_or("").starts_with("OOPixMap@"));
    OO_CHECK(!loader->cacheKey().has_value());  // a generator: not cached

    OO_CHECK(loader->enqueue());
    oo::PixMap result = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    uint32_t width = 0, height = 0;
    const std::optional<oo::TextureLoader::Result> loaded = loader->getResult();
    OO_CHECK(loaded.has_value());
    if (loaded) {
      result = loaded->pixMap;
      format = loaded->format;
      width = loaded->originalWidth;
      height = loaded->originalHeight;
    }
    OO_CHECK(format == oo::kPixMapRGBA && result.width == 8 &&
             result.height == 8 && result.rowBytes == 32);
    OO_CHECK(width == 8 && height == 8);
    OO_CHECK(((const uint8_t*)result.pixels)[0] == 1 &&
             ((const uint8_t*)result.pixels)[255] == 0);
    oo::FreePixMap(&result);
  }
}

OO_TEST(refusals) {
  OO_CHECK(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    // Not a pixmap.
    OO_CHECK(NewLoader(oo::kNullPixMap, oo::TextureFlags::kMinFilterLinear, true) ==
             nullptr);

    // Not taken over: the initialiser duplicates its own (still empty) pixmap,
    // not the one it was given, so it answers nil and the caller keeps its
    // pixels. (As it has since 2010; kept.)
    oo::PixMap kept = TestPixMap();
    OO_CHECK(NewLoader(kept, oo::TextureFlags::kMinFilterLinear, false) == nullptr);
    OO_CHECK(oo::IsValidPixMap(kept) && ((const uint8_t*)kept.pixels)[0] == 1);
    oo::FreePixMap(&kept);
  }
}

OO_TEST(loadsWithMipMaps) {
  OO_CHECK(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* loader =
        NewLoader(TestPixMap(), oo::TextureFlags::kMinFilterMipMap, true);
    OO_CHECK(loader->textureOptions() ==
             oo::ApplyTextureOptionDefaults(oo::TextureFlags::kMinFilterMipMap));
    OO_CHECK(loader->enqueue());
    oo::PixMap result = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    const std::optional<oo::TextureLoader::Result> loaded = loader->getResult();
    OO_CHECK(loaded.has_value());
    if (loaded) {
      result = loaded->pixMap;
      format = loaded->format;
    }
    OO_CHECK(result.width == 8 && result.height == 8 &&
             format == oo::kPixMapRGBA);
    OO_CHECK(((const uint8_t*)result.pixels)[0] ==
             1);  // the base level as given
    oo::FreePixMap(&result);
  }
}

OO_TEST(aTextureOfTheGenerator) {
  OO_CHECK(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* loader = NewLoader(
        TestPixMap(), oo::TextureFlags::kDefaultOptions | oo::TextureFlags::kRepeatS, true);
    oo::Texture* texture = Autoreleased(oo::Texture::textureWithGenerator(loader));
    OO_CHECK((dynamic_cast<oo::ConcreteTexture*>(texture) != nullptr));
    texture->ensureFinishedLoading();
    OO_CHECK(texture->dimensions().width == 8 &&
             texture->dimensions().height == 8);
    OO_CHECK(texture->isFinishedLoading());
  }
  ClearCache();
}

// --- The C++ API (after the conversion)
// --------------------------------------------------------

OO_TEST(cxxApi) {
  OO_CHECK(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    OO_CHECK(!oo::PixMapTextureLoader::loaderWithPixMap(oo::kNullPixMap, oo::TextureFlags::kNone, true));

    const std::shared_ptr<oo::PixMapTextureLoader> loader =
        oo::PixMapTextureLoader::loaderWithPixMap(
            TestPixMap(), oo::TextureFlags::kMinFilterLinear, true);
    OO_CHECK(loader &&
             loader->textureOptions() ==
                 oo::ApplyTextureOptionDefaults(oo::TextureFlags::kMinFilterLinear));
    OO_CHECK(loader->path().value_or("").starts_with("OOPixMap@") &&
             !loader->cacheKey().has_value());

    // Loading hands the pixels over as they are.
    loader->loadTexture();
    OO_CHECK(loader->_data != nullptr && loader->_width == 8 &&
             loader->_height == 8 && loader->_rowBytes == 32);
    OO_CHECK(loader->_format == oo::kPixMapRGBA &&
             loader->_data->data()[0] == 1);
  }
}

OO_TEST_MAIN()
