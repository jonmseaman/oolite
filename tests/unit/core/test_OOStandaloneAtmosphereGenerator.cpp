/*	test_OOStandaloneAtmosphereGenerator.mm
        Unit tests for OOStandaloneAtmosphereGenerator
   (src/Core/Materials/OOStandaloneAtmosphereGenerator.h), the texture generator
   of a planet's cloud layer when its surface is not generated.

        It reads the cloud parameters from the planet's material parameters
   (colours as PList::Object nodes holding OOColors), sizes its texture by the
   detail level (no universe here: the lowest, so 512 x 512, or 512 x 256 with
   3D noise), keys it by its parameters, and fills it with clouds from the
   seed's noise, their alpha scaled by cloud_alpha. It links the whole game but
   main (tests/unit/core/meson.build entry ['*']), on the hidden GL context of
   oo_gl_test_context.hpp. Making a generator and the class methods go through
   the helpers below. The class is a global C++ class. The last test pins the
   C++ API. Run: bash tools/check-core-tests.sh
   test_OOStandaloneAtmosphereGenerator
*/

#include <cstdlib>
#include <cstring>
#include <string>

#include <gtest/gtest.h>

#include "Color.h"
#include "ConcreteTexture.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/PList.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "StandaloneAtmosphereGenerator.h"
#include "oo_gl_test_context.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

// What an autoreleased texture is: the C++ texture, kept until the current
// autorelease scope drains (oo::autorelease).
static oo::Texture* Autoreleased(const std::shared_ptr<oo::Texture>& texture) {
  if (texture == nullptr) return nullptr;
  oo::autorelease(std::shared_ptr<oo::Texture>(texture));
  return texture.get();
}

namespace {

const RANROTSeed kSeed = {12345, 67890};

// The loaders' sizes come from their one-time set-up, which a path with no
// known extension runs (and then answers nil, reading nothing).
void SetUp() {
  EXPECT_TRUE(OOTestGLContext());
  {
    oo::AutoreleaseScope pool;
    (void)oo::TextureLoader::loaderWithPath(std::string("unknown.type"), oo::TextureFlags::kNone);
  }
}

oo::PList PlanetInfo(float cloudAlpha, bool perlin3d) {
  return oo::PList(oo::PList::Dict{
      {"cloud_alpha", oo::PList(cloudAlpha)},
      {"cloud_fraction", oo::PList(0.5)},
      {"cloud_color",
       oo::ColorObjectNode(oo::Color::colorWithRed(1.0f, 0.5f, 0.25f, 1.0f).get())},
      {"polar_cloud_color", oo::ColorObjectNode(oo::Color::whiteColor().get())},
      {"perlin_3d", oo::PList(perlin3d)},
  });
}

// --- The only lines a conversion ports
// ----------------------------------------------------------

// The generators made, kept for the test's life (each was autoreleased in its
// case's pool).
std::vector<std::shared_ptr<oo::TextureGenerator>> gKeptGenerators;

oo::TextureGenerator* Kept(std::shared_ptr<oo::TextureGenerator> generator) {
  if (generator != nullptr) gKeptGenerators.push_back(generator);
  return generator.get();
}

// Was [[OOStandaloneAtmosphereGenerator alloc] initWithPlanetInfo:seed:],
// autoreleased, as a generator.
oo::TextureGenerator* NewGenerator(const oo::PList& info, RANROTSeed seed) {
  return Kept(
      oo::StandaloneAtmosphereGenerator::generatorWithPlanetInfo(info, seed));
}

// Was +generateAtmosphereTexture:withInfo:seed:; the texture (autoreleased), or
// nil where it answered NO.
oo::Texture* GenerateAtmosphere(const oo::PList& info, RANROTSeed seed) {
  std::shared_ptr<oo::Texture> texture;
  return oo::StandaloneAtmosphereGenerator::generateAtmosphereTexture(&texture,
                                                                    info, seed)
             ? Autoreleased(texture)
             : nullptr;
}

// Was +planetTextureWithInfo:seed: (autoreleased).
oo::Texture* PlanetTexture(const oo::PList& info, RANROTSeed seed) {
  return Autoreleased(
      oo::StandaloneAtmosphereGenerator::planetTextureWithInfo(info, seed));
}

// --------------------------------------------------------------------------------------------------

struct Generated {
  bool ok = false;
  oo::PixMap pixMap = oo::kNullPixMap;
  oo::TextureDataFormat format = oo::kPixMapInvalidFormat;

  ~Generated() { oo::FreePixMap(&pixMap); }

  const uint8_t* bytes() const { return (const uint8_t*)pixMap.pixels; }
  size_t size() const { return (size_t)pixMap.rowBytes * pixMap.height; }
};

void Generate(oo::TextureGenerator* generator, Generated& generated) {
  EXPECT_TRUE(generator->enqueue());
  if (std::optional<oo::TextureLoader::Result> loaded = generator->getResult()) {
    generated.ok = true;
    generated.pixMap = loaded->pixMap;
    generated.format = loaded->format;
  }
}

void ClearCache() {
  {
    oo::AutoreleaseScope scope;
    oo::Texture::clearCache();
  }
}

}  // namespace

TEST(OOStandaloneAtmosphereGenerator, settings) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* generator =
        NewGenerator(PlanetInfo(0.75f, false), kSeed);
    EXPECT_TRUE(generator != nullptr &&
             (dynamic_cast<oo::TextureGenerator*>(generator) != nullptr));
    EXPECT_TRUE(generator->textureOptions() ==
             (oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear |
              oo::TextureFlags::kRepeatS | oo::TextureFlags::kNoShrink));
    EXPECT_TRUE(generator->path().value_or("").starts_with(
        "OOStandaloneAtmosphereTexture@"));

    // Keyed by the scale (512: 2), the size (not known until it loads), the
    // seed and the clouds; the air colour is never set.
    const std::string key = generator->cacheKey().value_or("");
    EXPECT_TRUE(
        key.starts_with("OOStandaloneAtmosphereGenerator-@2\n0,0/12345,67890/"
                        "0.750000/0.500000/0.000000,0.000000,0.000000/"));
    EXPECT_TRUE(NewGenerator(PlanetInfo(0.75f, false), kSeed)->cacheKey() ==
             std::optional<std::string>(key));
    EXPECT_TRUE(NewGenerator(PlanetInfo(0.75f, false), (RANROTSeed){1, 2})
                 ->cacheKey() != std::optional<std::string>(key));

    EXPECT_TRUE(generator->description().find("{seed: 12345,67890}") !=
             std::string::npos);
  }
}

TEST(OOStandaloneAtmosphereGenerator, generatesClouds) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    Generated a, b;
    Generate(NewGenerator(PlanetInfo(1.0f, false), kSeed), a);
    EXPECT_TRUE(a.ok && a.format == oo::kPixMapRGBA && a.pixMap.width == 512 &&
             a.pixMap.height == 512 && a.pixMap.rowBytes == 2048);

    // The same seed and parameters give the same clouds.
    Generate(NewGenerator(PlanetInfo(1.0f, false), kSeed), b);
    EXPECT_TRUE(b.ok && b.size() == a.size() &&
             std::memcmp(a.bytes(), b.bytes(), a.size()) == 0);

    // Some cloud, some clear sky.
    bool someCloud = false, someClear = false;
    for (size_t i = 3; i < a.size(); i += 4) {
      if (a.bytes()[i] > 128) someCloud = true;
      if (a.bytes()[i] < 128) someClear = true;
    }
    EXPECT_TRUE(someCloud && someClear);

    // No cloud alpha: transparent everywhere.
    Generated clear;
    Generate(NewGenerator(PlanetInfo(0.0f, false), kSeed), clear);
    bool allClear = clear.ok;
    for (size_t i = 3; i < clear.size(); i += 4)
      allClear = allClear && clear.bytes()[i] == 0;
    EXPECT_TRUE(allClear);

    // 3D noise: twice as wide as high.
    Generated wide;
    Generate(NewGenerator(PlanetInfo(1.0f, true), kSeed), wide);
    EXPECT_TRUE(wide.ok && wide.pixMap.width == 512 && wide.pixMap.height == 256);
  }
}

TEST(OOStandaloneAtmosphereGenerator, textures) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    oo::Texture* atmosphere = GenerateAtmosphere(PlanetInfo(1.0f, false), kSeed);
    EXPECT_TRUE((dynamic_cast<oo::ConcreteTexture*>(atmosphere) != nullptr));
    atmosphere->ensureFinishedLoading();
    EXPECT_TRUE(atmosphere->isFinishedLoading() &&
             atmosphere->dimensions().width == 512 &&
             atmosphere->dimensions().height == 512);

    oo::Texture* texture =
        PlanetTexture(PlanetInfo(1.0f, true), (RANROTSeed){3, 4});
    EXPECT_TRUE((dynamic_cast<oo::ConcreteTexture*>(texture) != nullptr));
    texture->ensureFinishedLoading();
    EXPECT_TRUE(texture->dimensions().width == 512 &&
             texture->dimensions().height == 256);
  }
  ClearCache();
}

// --- The C++ API (after the conversion)
// --------------------------------------------------------

TEST(OOStandaloneAtmosphereGenerator, cxxApi) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    const std::shared_ptr<oo::StandaloneAtmosphereGenerator> generator =
        oo::StandaloneAtmosphereGenerator::generatorWithPlanetInfo(
            PlanetInfo(1.0f, false), kSeed);
    EXPECT_TRUE(generator &&
             generator->textureOptions() ==
                 (oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear |
                  oo::TextureFlags::kRepeatS | oo::TextureFlags::kNoShrink));
    EXPECT_TRUE(generator->descriptionComponents() ==
             std::optional<std::string>("seed: 12345,67890"));
    EXPECT_TRUE(generator->cacheKey().value_or("").starts_with(
        "OOStandaloneAtmosphereGenerator-@2\n"));

    // Loading fills the root's state; the old -getResult:format:width:height:
    // answers the result.
    generator->loadTexture();
    EXPECT_TRUE(generator->_width == 512 && generator->_height == 512 &&
             generator->_data != nullptr);
    oo::PixMap pixMap = oo::kNullPixMap;
    oo::TextureDataFormat format = oo::kPixMapInvalidFormat;
    generator->completeAsyncTask();
    const std::optional<oo::TextureLoader::Result> loaded =
        generator->getResultFormatWidthHeight();
    if (loaded) {
      pixMap = loaded->pixMap;
      format = loaded->format;
    }
    EXPECT_TRUE(loaded.has_value() && format == oo::kPixMapRGBA &&
             pixMap.width == 512);
    oo::FreePixMap(&pixMap);

    std::shared_ptr<oo::Texture> texture;
    EXPECT_TRUE(oo::StandaloneAtmosphereGenerator::generateAtmosphereTexture(
                 &texture, PlanetInfo(1.0f, false), (RANROTSeed){9, 9}) &&
             (dynamic_cast<oo::ConcreteTexture*>(texture.get()) != nullptr));
    EXPECT_TRUE((dynamic_cast<oo::ConcreteTexture*>(
                  oo::StandaloneAtmosphereGenerator::planetTextureWithInfo(
                      PlanetInfo(1.0f, false), (RANROTSeed){9, 10})
                      .get()) != nullptr));
  }
  ClearCache();
}
