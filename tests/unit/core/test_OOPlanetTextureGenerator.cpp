/*	test_OOPlanetTextureGenerator.mm
        Unit tests for OOPlanetTextureGenerator
   (src/Core/Materials/OOPlanetTextureGenerator.h), the texture generator of a
   planet's surface, with the two helper generators private to its file that
   hand on the normal map and the atmosphere it makes at the same time.

        It reads the planet's parameters (colours as PList::Object nodes holding
   OOColors), sizes its texture by the detail level (no universe here: the
   lowest, so 512 x 512, or 512 x 256 with 3D noise), keys it by its parameters
   and by which helpers it feeds, and fills it from the seed's noise; the class
   methods make the textures, the surface's last so that it is queued once the
        helpers are. It links the whole game but main
   (tests/unit/core/meson.build entry ['*']), on the hidden GL context of
   oo_gl_test_context.hpp. Making a generator and the class methods go through
   the helpers below: the class is a global C++ class. The last test pins the
   C++ API. Run: bash meson test -C build/meson_test
   test_OOPlanetTextureGenerator
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
#include "PlanetTextureGenerator.h"
#include "oo_gl_test_context.hpp"

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
void SetUp() {
  EXPECT_TRUE(OOTestGLContext());
  {
    oo::AutoreleaseScope pool;
    (void)oo::TextureLoader::loaderWithPath(std::string("unknown.type"), oo::TextureFlags::kNone);
  }
}

oo::PList PlanetInfo(bool perlin3d) {
  return oo::PList(oo::PList::Dict{
      {"land_fraction", oo::PList(0.25)},
      {"polar_fraction", oo::PList(0.05)},
      {"land_color",
       oo::ColorObjectNode(
           oo::Color::colorWithRed(0.25f, 0.5f, 0.125f, 1.0f).get())},
      {"sea_color", oo::ColorObjectNode(oo::Color::blueColor().get())},
      {"polar_land_color", oo::ColorObjectNode(oo::Color::whiteColor().get())},
      {"polar_sea_color", oo::ColorObjectNode(oo::Color::cyanColor().get())},
      {"cloud_alpha", oo::PList(1.0)},
      {"cloud_fraction", oo::PList(0.5)},
      {"cloud_color", oo::ColorObjectNode(oo::Color::whiteColor().get())},
      {"polar_cloud_color", oo::ColorObjectNode(oo::Color::lightGrayColor().get())},
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

// Was [[OOPlanetTextureGenerator alloc] initWithPlanetInfo:seed:],
// autoreleased, as a generator.
oo::TextureGenerator* NewGenerator(const oo::PList& info, RANROTSeed seed) {
  return Kept(oo::PlanetTextureGenerator::generatorWithPlanetInfo(info, seed));
}

// Was +planetTextureWithInfo:seed: (autoreleased).
oo::Texture* PlanetTexture(const oo::PList& info, RANROTSeed seed) {
  return Autoreleased(
      oo::PlanetTextureGenerator::planetTextureWithInfo(info, seed));
}

// The three +generatePlanetTexture:... methods: what they answered, and the
// textures they wrote.
struct Textures {
  bool ok = false;
  oo::Texture *texture = nullptr, *secondary = nullptr, *atmosphere = nullptr;
};

Textures GenerateWithAtmosphere(const oo::PList& info, RANROTSeed seed) {
  Textures t;
  std::shared_ptr<oo::Texture> texture, atmosphere;
  t.ok = oo::PlanetTextureGenerator::generatePlanetTextureAndAtmosphere(
      &texture, &atmosphere, info, seed);
  t.texture = Autoreleased(texture);
  t.atmosphere = Autoreleased(atmosphere);
  return t;
}

Textures GenerateWithSecondary(const oo::PList& info, RANROTSeed seed,
                               bool secondary) {
  Textures t;
  std::shared_ptr<oo::Texture> texture, secondaryTexture;
  t.ok = oo::PlanetTextureGenerator::generatePlanetTexture(
      &texture, secondary ? &secondaryTexture : NULL, info, seed);
  t.texture = Autoreleased(texture);
  t.secondary = Autoreleased(secondaryTexture);
  return t;
}

Textures GenerateAll(const oo::PList& info, RANROTSeed seed, bool secondary) {
  Textures t;
  std::shared_ptr<oo::Texture> texture, secondaryTexture, atmosphere;
  t.ok = oo::PlanetTextureGenerator::generatePlanetTexture(
      &texture, secondary ? &secondaryTexture : NULL, &atmosphere, info, seed);
  t.texture = Autoreleased(texture);
  t.secondary = Autoreleased(secondaryTexture);
  t.atmosphere = Autoreleased(atmosphere);
  return t;
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

// A texture loaded: its key starts with the prefix, and it has the size.
bool Loaded(oo::Texture* texture, const std::string& keyPrefix, unsigned width,
            unsigned height) {
  if (!(dynamic_cast<oo::ConcreteTexture*>(texture) != nullptr)) return false;
  texture->ensureFinishedLoading();
  return texture->isFinishedLoading() &&
         texture->cacheKey().value_or("").starts_with(keyPrefix) &&
         texture->dimensions().width == width &&
         texture->dimensions().height == height;
}

void ClearCache() {
  {
    oo::AutoreleaseScope scope;
    oo::Texture::clearCache();
  }
}

}  // namespace

TEST(OOPlanetTextureGenerator, settings) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    oo::TextureGenerator* generator =
        NewGenerator(PlanetInfo(false), (RANROTSeed){12345, 67890});
    EXPECT_TRUE(generator != nullptr &&
             (dynamic_cast<oo::TextureGenerator*>(generator) != nullptr));
    EXPECT_TRUE(generator->textureOptions() ==
             (oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear |
              oo::TextureFlags::kRepeatS | oo::TextureFlags::kNoShrink));
    EXPECT_TRUE(generator->path().value_or("").starts_with("OOPlanetTexture@"));

    // Keyed by what it feeds (nothing yet: baked), the scale (512: 2), the size
    // (not known until it loads), the land fraction, the seed and the colours.
    const std::string key = generator->cacheKey().value_or("");
    EXPECT_TRUE(key.starts_with(
        "OOPlanetTextureGenerator-diffuse-baked@2\n0,0/0.25/12345,67890/"));
    EXPECT_TRUE(generator->description().find("{seed: 12345,67890 land: 0.25}") !=
             std::string::npos);
  }
}

TEST(OOPlanetTextureGenerator, generatesTheSurface) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    Generated a, b;
    Generate(NewGenerator(PlanetInfo(false), (RANROTSeed){1, 1}), a);
    EXPECT_TRUE(a.ok && a.format == oo::kPixMapRGBA && a.pixMap.width == 512 &&
             a.pixMap.height == 512 && a.pixMap.rowBytes == 2048);

    // The same seed and parameters give the same planet.
    Generate(NewGenerator(PlanetInfo(false), (RANROTSeed){1, 1}), b);
    EXPECT_TRUE(b.ok && b.size() == a.size() &&
             std::memcmp(a.bytes(), b.bytes(), a.size()) == 0);

    // 3D noise: twice as wide as high.
    Generated wide;
    Generate(NewGenerator(PlanetInfo(true), (RANROTSeed){1, 1}), wide);
    EXPECT_TRUE(wide.ok && wide.pixMap.width == 512 && wide.pixMap.height == 256);
  }
}

TEST(OOPlanetTextureGenerator, planetTexture) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    EXPECT_TRUE(Loaded(PlanetTexture(PlanetInfo(false), (RANROTSeed){2, 2}),
                    "OOPlanetTextureGenerator-diffuse-baked@2\n", 512, 512));
  }
  ClearCache();
}

TEST(OOPlanetTextureGenerator, withSecondaryTexture) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    // The normal map is filled when the surface is generated, so the surface is
    // loaded first (unlike the atmosphere's, the normal map's generator does
    // not wait for the surface's).
    Textures t =
        GenerateWithSecondary(PlanetInfo(false), (RANROTSeed){3, 3}, true);
    EXPECT_TRUE(t.ok && t.atmosphere == nullptr);
    EXPECT_TRUE(Loaded(t.texture, "OOPlanetTextureGenerator-diffuse-raw@2\n", 512,
                    512));
    EXPECT_TRUE(
        Loaded(t.secondary, "OOPlanetTextureGenerator-normal@2\n", 512, 512));

    Textures alone =
        GenerateWithSecondary(PlanetInfo(false), (RANROTSeed){4, 4}, false);
    EXPECT_TRUE(alone.ok && alone.secondary == nullptr);
    EXPECT_TRUE(Loaded(alone.texture, "OOPlanetTextureGenerator-diffuse-baked@2\n",
                    512, 512));
  }
  ClearCache();
}

TEST(OOPlanetTextureGenerator, withAtmosphere) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    Textures t = GenerateWithAtmosphere(PlanetInfo(false), (RANROTSeed){5, 5});
    EXPECT_TRUE(t.ok && t.secondary == nullptr);
    EXPECT_TRUE(Loaded(t.texture,
                    "OOPlanetTextureGenerator-diffuse-baked-atmo@2\n", 512,
                    512));
    EXPECT_TRUE(
        Loaded(t.atmosphere, "OOPlanetTextureGenerator-atmo@2\n", 512, 512));

    Textures all = GenerateAll(PlanetInfo(false), (RANROTSeed){6, 6}, true);
    EXPECT_TRUE(all.ok);
    EXPECT_TRUE(Loaded(all.texture,
                    "OOPlanetTextureGenerator-diffuse-raw-atmo@2\n", 512, 512));
    EXPECT_TRUE(
        Loaded(all.secondary, "OOPlanetTextureGenerator-normal@2\n", 512, 512));
    EXPECT_TRUE(
        Loaded(all.atmosphere, "OOPlanetTextureGenerator-atmo@2\n", 512, 512));

    Textures noSecondary =
        GenerateAll(PlanetInfo(false), (RANROTSeed){7, 7}, false);
    EXPECT_TRUE(noSecondary.ok && noSecondary.secondary == nullptr);
    EXPECT_TRUE(Loaded(noSecondary.atmosphere, "OOPlanetTextureGenerator-atmo@2\n",
                    512, 512));
    EXPECT_TRUE(Loaded(noSecondary.texture,
                    "OOPlanetTextureGenerator-diffuse-baked-atmo@2\n", 512,
                    512));
  }
  ClearCache();
}

// --- The C++ API (after the conversion)
// --------------------------------------------------------

TEST(OOPlanetTextureGenerator, cxxApi) {
  ::SetUp();
  {
    oo::AutoreleaseScope pool;
    const std::shared_ptr<oo::PlanetTextureGenerator> generator =
        oo::PlanetTextureGenerator::generatorWithPlanetInfo(
            PlanetInfo(false), (RANROTSeed){12345, 67890});
    EXPECT_TRUE(generator &&
             generator->textureOptions() ==
                 (oo::TextureFlags::kMinFilterLinear | oo::TextureFlags::kMagFilterLinear |
                  oo::TextureFlags::kRepeatS | oo::TextureFlags::kNoShrink));
    EXPECT_TRUE(generator->descriptionComponents() ==
             std::optional<std::string>("seed: 12345,67890 land: 0.25"));
    EXPECT_TRUE(generator->cacheKey().value_or("").starts_with(
        "OOPlanetTextureGenerator-diffuse-baked@2\n"));

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

    // The helper generators cross as OOTextureGenerators, named in their
    // descriptions.
    Textures all = GenerateAll(PlanetInfo(false), (RANROTSeed){8, 8}, true);
    EXPECT_TRUE(all.ok &&
             Loaded(all.texture,
                    "OOPlanetTextureGenerator-diffuse-raw-atmo@2\n", 512, 512));
    EXPECT_TRUE(
        Loaded(all.secondary, "OOPlanetTextureGenerator-normal@2\n", 512,
               512) &&
        Loaded(all.atmosphere, "OOPlanetTextureGenerator-atmo@2\n", 512, 512));
  }
  ClearCache();
}
