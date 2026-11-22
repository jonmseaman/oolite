/*	test_OOTextureGenerator.mm
        Unit tests for OOTextureGenerator
   (src/Core/Materials/OOTextureGenerator.h), a texture loader that needs no
   file.

        A generator chooses its own texture options, anisotropy, LOD bias and
   cache key, and queues itself on the game's work manager;
   OOPixMapTextureLoader and the planet, atmosphere and emission-map generators
   derive from it in their own files. The test's generators are C++ subclasses:
   a bare one that answers the class's defaults, and one that overrides
   them. It links the whole game but main (tests/unit/core/meson.build entry
   ['*']), on the hidden GL context of oo_gl_test_context.hpp. The last test
   pins the C++ API (OOTextureGenerator). Run: tests/run.sh unit
*/


#include <gtest/gtest.h>

#include "ConcreteTexture.h"
#include "TextureGenerator.h"
#include "oo_gl_test_context.hpp"
#include "oofnd/AutoreleaseScope.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

static int gLoads = 0;

// A generator of an 8 x 8 RGBA image whose bytes count up from 1. Its root
// state is written in one block.
class BareGenerator : public oo::TextureGenerator {
 public:
  void loadTexture() override {
    gLoads++;
    oo::PixelBuffer buffer = oo::MakePixelBuffer(8 * 8 * 4);
    uint8_t* bytes = buffer->data();
    for (unsigned i = 0; i < 8 * 8 * 4; i++) bytes[i] = (uint8_t)(i + 1);
    _data = buffer;
    _width = 8;
    _height = 8;
    _format = oo::kPixMapRGBA;
  }
};

// A generator that chooses its own settings and cache key.
class ChoosyGenerator : public BareGenerator {
 public:
  oo::TextureFlags textureOptions() override {
    return oo::TextureFlags::kMinFilterNearest | oo::TextureFlags::kMagFilterNearest;
  }
  GLfloat anisotropy() override { return 0.25f; }
  GLfloat lodBias() override { return 0.5f; }
  std::optional<std::string> cacheKey() override {
    return std::string("test:choosy");
  }
};

// What an autoreleased texture is: the C++ texture, kept until the current
// autorelease scope drains (oo::autorelease).
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

// The generators made, kept for the test's life (each was autoreleased in its
// case's pool).
std::vector<std::shared_ptr<oo::TextureGenerator>> gKeptGenerators;

template <class G>
G* Generator(const char* path, oo::TextureFlags options) {
  const std::shared_ptr<G> made = oo::makeShared<G>();
  if (!made->initWithPath(std::string(path), options)) return nullptr;
  gKeptGenerators.push_back(made);
  return made.get();
}

void ClearCache() {
  {
    oo::AutoreleaseScope scope;
    oo::Texture::clearCache();
  }
}

}  // namespace

TEST(OOTextureGenerator, defaults) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    BareGenerator* generator =
        Generator<BareGenerator>("bare generator", oo::TextureFlags::kMinFilterLinear);
    EXPECT_TRUE(generator->textureOptions() == oo::TextureFlags::kDefaultOptions);
    EXPECT_TRUE(generator->anisotropy() == (GLfloat)kOOTextureDefaultAnisotropy &&
             generator->lodBias() == (GLfloat)kOOTextureDefaultLODBias);
    EXPECT_TRUE(!generator->cacheKey()
                  .has_value());  // a generator is not cached unless it says so
    EXPECT_TRUE(generator->path() == std::optional<std::string>("bare generator"));
    EXPECT_TRUE(dynamic_cast<oo::TextureLoader*>(generator) != nullptr);
    EXPECT_TRUE(
        generator->description().ends_with(">{{bare generator -- loading}}"));
    EXPECT_TRUE(!oo::makeShared<BareGenerator>()->initWithPath(std::nullopt, oo::TextureFlags::kNone));
  }
}

TEST(OOTextureGenerator, enqueueLoadsOnTheWorkManager) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    const int loads = gLoads;
    BareGenerator* generator =
        Generator<BareGenerator>("queued generator", oo::TextureFlags::kMinFilterLinear);
    EXPECT_TRUE(generator->enqueue());
    oo::PixMap pixMap = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    uint32_t width = 0;
    const std::optional<oo::TextureLoader::Result> loaded =
        generator->getResult();
    EXPECT_TRUE(loaded.has_value());
    if (loaded) {
      pixMap = loaded->pixMap;
      format = loaded->format;
      width = loaded->originalWidth;
    }
    EXPECT_TRUE(gLoads == loads + 1 && generator->isReady());
    EXPECT_TRUE(pixMap.width == 8 && pixMap.height == 8 && width == 8 &&
             format == oo::kPixMapRGBA);
    EXPECT_TRUE(((uint8_t*)pixMap.pixels)[0] == 1);
    oo::FreePixMap(&pixMap);
  }
}

TEST(OOTextureGenerator, aTextureTakesTheGeneratorsSettings) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    ChoosyGenerator* generator =
        Generator<ChoosyGenerator>("choosy generator", oo::TextureFlags::kNone);
    EXPECT_TRUE(generator->textureOptions() ==
             (oo::TextureFlags::kMinFilterNearest | oo::TextureFlags::kMagFilterNearest));
    EXPECT_TRUE(generator->anisotropy() == 0.25f && generator->lodBias() == 0.5f);

    oo::Texture* texture =
        Autoreleased(oo::Texture::textureWithGenerator(generator));
    EXPECT_TRUE((dynamic_cast<oo::ConcreteTexture*>(texture) != nullptr));
    EXPECT_TRUE(texture->cacheKey() == std::optional<std::string>("test:choosy"));
    EXPECT_TRUE(oo::Texture::textureWithGenerator(generator).get() ==
             texture);  // cached by the generator's key
    texture->ensureFinishedLoading();
    EXPECT_TRUE(texture->dimensions().width == 8 &&
             !texture->isMipMapped());  // nearest: no mip-maps
#ifndef NDEBUG
    EXPECT_TRUE(texture->name() ==
             std::optional<std::string>("<ChoosyGenerator>"));
#endif
  }
  ClearCache();
}

// --- The C++ API (after the conversion)
// --------------------------------------------------------

// A converted generator, as OOPixMapTextureLoader will be one: a global C++
// subclass.
class TestCxxGenerator final : public oo::TextureGenerator {
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

  oo::TextureFlags textureOptions() override {
    return oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear;
  }
  std::optional<std::string> cacheKey() override {
    return std::string("test:cxxgen");
  }

  int loads = 0;
};

TEST(OOTextureGenerator, cxxApi) {
  EXPECT_TRUE(OOTestGLContext());
  SetUpLoaders();
  {
    oo::AutoreleaseScope pool;
    const std::shared_ptr<TestCxxGenerator> generator = oo::makeShared<TestCxxGenerator>();
    EXPECT_TRUE(generator->initWithPath(std::string("cxx generator"), oo::TextureFlags::kNone));
    EXPECT_TRUE(generator->anisotropy() == (GLfloat)kOOTextureDefaultAnisotropy &&
             generator->lodBias() == (GLfloat)kOOTextureDefaultLODBias);

    // A texture takes it.
    EXPECT_TRUE(generator->textureOptions() ==
             (oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear));
    EXPECT_TRUE(generator->cacheKey() ==
             std::optional<std::string>("test:cxxgen"));
    oo::Texture* texture =
        Autoreleased(oo::Texture::textureWithGenerator(generator.get()));
    EXPECT_TRUE(texture->cacheKey() == std::optional<std::string>("test:cxxgen"));
    texture->ensureFinishedLoading();
    EXPECT_TRUE(generator->loads == 1 && texture->dimensions().width == 8);

    // The bare class's own answers.
    const std::shared_ptr<oo::TextureGenerator> bare = oo::makeShared<oo::TextureGenerator>();
    EXPECT_TRUE(bare->textureOptions() == oo::TextureFlags::kDefaultOptions &&
             !bare->cacheKey().has_value());
  }
  ClearCache();
}
