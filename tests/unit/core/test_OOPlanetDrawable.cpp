/*	test_OOPlanetDrawable.mm
        Unit tests for OOPlanetDrawable (src/Core/OOPlanetDrawable.h), the
   sphere that draws a planet's surface or its atmosphere, a subclass of the
   drawable root OODrawable.

        The drawable reads the universe for its level of detail, so the test
   links the whole game but main (['*']) and uses a Universe that was never
   initialised, subclassed to answer the detail setting and a game view of the
   test's size. The expectations cover: a planet's and an atmosphere's defaults
   (radius 1, half detail), the radius (its magnitude), the level of detail
   (clamped, rounded to the planet data's levels, and computed from the view
   distance and the screen width), which parts are opaque or translucent, the
   bounds, the material and its name, and a copy. Drawing needs OpenGL and is
   left to the integration tests. The drawable is made and read through the one block of
   helpers below. Run: bash tools/check-core-tests.sh
*/

#include <cmath>

#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual references

#include "MyOpenGLView.h"  // the game view is C++
#include "BasicMaterial.h"
#include "PlanetData.h"
#include "PlanetDrawable.h"
#include "Universe.h"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

extern Universe* gSharedUniverse;

// The game view, as far as the drawable asks it.
// The game view: a C++ subclass of the game's, overriding the one member it
// answers (virtual for it).
class TestView : public MyOpenGLView {
 public:
  NSSize _size = {};

  NSSize getViewSize() override { return _size; }
};

// UNIVERSE: never initialised; answers the detail setting and the game view.
class TestUniverse : public Universe {
 public:
  bool _reduced = {};
  TestView* _view = {};

  bool reducedDetail() override { return _reduced; }
};

namespace {

TestUniverse* sUniverse = nullptr;

void SetUp() {
  if (sUniverse == nullptr) {
    sUniverse = new TestUniverse();          // never released
    sUniverse->_view = oo::test::NewRetained<TestView>();       // never released
    sUniverse->gameView = oo::share(sUniverse->_view);  // what -gameView answers
  }
  sUniverse->_reduced = false;
  sUniverse->_view->_size = NSMakeSize(800, 600);
  gSharedUniverse = sUniverse;
}

bool Near(double a, double b) { return std::fabs(a - b) < 1e-5; }

const float kGranularity = (float)(kOOPlanetDataLevels - 1);

float Level(float lod) { return roundf(lod * kGranularity) / kGranularity; }

// A material kept until the pool drains.
oo::Material* NamedMaterial(const char* name) {
  std::shared_ptr<oo::Material> material =
      oo::BasicMaterial::materialWithName(std::string(name), oo::PList());
  oo::Material* result = material.get();
  oo::MaterialAutorelease(std::move(material));
  return result;
}

// --- The drawable, made and read; the only lines the conversion ported
// ------------------------------

using Planet = std::shared_ptr<oo::PlanetDrawable>;

Planet NewPlanet() { return oo::makeShared<oo::PlanetDrawable>(); }
Planet NewAtmosphere() { return oo::PlanetDrawable::initAsAtmosphere(); }
Planet AtmosphereWithRadius(float radius) {
  return oo::PlanetDrawable::atmosphereWithRadius(radius);
}
Planet Copy(const Planet& d) { return d->copy(); }
float Radius(const Planet& d) { return d->radius(); }
void SetRadius(const Planet& d, float radius) { d->setRadius(radius); }
float LevelOfDetail(const Planet& d) { return d->levelOfDetail(); }
void SetLevelOfDetail(const Planet& d, float lod) { d->setLevelOfDetail(lod); }
void CalculateLevelOfDetail(const Planet& d, float distance) {
  d->calculateLevelOfDetailForViewDistance(distance);
}
bool HasOpaqueParts(const Planet& d) { return d->hasOpaqueParts(); }
bool HasTranslucentParts(const Planet& d) { return d->hasTranslucentParts(); }
GLfloat CollisionRadius(const Planet& d) { return d->collisionRadius(); }
GLfloat MaxDrawDistance(const Planet& d) { return d->maxDrawDistance(); }
BoundingBox Bounds(const Planet& d) { return d->boundingBox(); }
oo::Material* Material(const Planet& d) { return d->material(); }
void SetMaterial(const Planet& d, oo::Material* material) {
  d->setMaterial(material);
}
std::optional<std::string> TextureName(const Planet& d) {
  return d->textureName();
}
void SetTextureName(const Planet& d, const std::string& name) {
  d->setTextureName(name);
}

// --------------------------------------------------------------------------------------------------

}  // namespace

TEST(OOPlanetDrawable, planetDefaults) {
  {
    ::SetUp();
    Planet planet = NewPlanet();
    EXPECT_TRUE(Radius(planet) == 1.0f);
    EXPECT_TRUE(CollisionRadius(planet) == 1.0f);
    EXPECT_TRUE(Near(LevelOfDetail(planet), Level(0.5f)));
    EXPECT_TRUE(HasOpaqueParts(planet) && !HasTranslucentParts(planet));
    EXPECT_TRUE(std::isinf(MaxDrawDistance(planet)));
    EXPECT_TRUE(Material(planet) == nullptr);
    EXPECT_TRUE(!TextureName(planet).has_value());
  }
}

TEST(OOPlanetDrawable, atmosphere) {
  {
    ::SetUp();
    Planet atmosphere = NewAtmosphere();
    EXPECT_TRUE(Radius(atmosphere) == 1.0f);
    EXPECT_TRUE(!HasOpaqueParts(atmosphere) && HasTranslucentParts(atmosphere));

    Planet sized = AtmosphereWithRadius(-250.0f);
    EXPECT_TRUE(Radius(sized) == 250.0f);
    EXPECT_TRUE(!HasOpaqueParts(sized) && HasTranslucentParts(sized));
    EXPECT_TRUE(Near(LevelOfDetail(sized), Level(0.5f)));
  }
}

TEST(OOPlanetDrawable, radiusAndBounds) {
  {
    ::SetUp();
    Planet planet = NewPlanet();
    SetRadius(planet, -5000.0f);
    EXPECT_TRUE(Radius(planet) == 5000.0f);
    EXPECT_TRUE(CollisionRadius(planet) == 5000.0f);
    BoundingBox box = Bounds(planet);
    EXPECT_TRUE(box.min.x == -5000.0f && box.min.y == -5000.0f &&
             box.min.z == -5000.0f);
    EXPECT_TRUE(box.max.x == 5000.0f && box.max.y == 5000.0f &&
             box.max.z == 5000.0f);
  }
}

TEST(OOPlanetDrawable, levelOfDetail) {
  {
    ::SetUp();
    Planet planet = NewPlanet();
    SetLevelOfDetail(planet, 0.3f);
    EXPECT_TRUE(Near(LevelOfDetail(planet), Level(0.3f)));
    SetLevelOfDetail(planet, -2.0f);
    EXPECT_TRUE(LevelOfDetail(planet) == 0.0f);
    SetLevelOfDetail(planet, 7.0f);
    EXPECT_TRUE(LevelOfDetail(planet) == 1.0f);
    SetLevelOfDetail(planet, 0.8f);
    EXPECT_TRUE(Near(LevelOfDetail(planet), Level(0.8f)));
  }
}

TEST(OOPlanetDrawable, levelOfDetailForViewDistance) {
  {
    ::SetUp();
    Planet planet = NewPlanet();
    SetRadius(planet, 100.0f);

    // Full detail: sqrt(width / 40 * radius / sqrt(distance) / 4), clamped to
    // [0, 1].
    float distance = 1.0e6f;
    float expected = sqrtf((800.0f / 40.0f) * 100.0f / sqrtf(distance) * 0.25f);
    CalculateLevelOfDetail(planet, distance);
    EXPECT_TRUE(Near(LevelOfDetail(planet), Level(fminf(expected, 1.0f))));

    // Close by: the highest level.
    CalculateLevelOfDetail(planet, 1.0f);
    EXPECT_TRUE(LevelOfDetail(planet) == 1.0f);

    // Far away: the lowest.
    CalculateLevelOfDetail(planet, 1.0e20f);
    EXPECT_TRUE(LevelOfDetail(planet) == 0.0f);

    // Reduced detail: a narrower budget, earlier transitions, and never the
    // highest level.
    sUniverse->_reduced = true;
    CalculateLevelOfDetail(planet, 1.0f);
    EXPECT_TRUE(Near(LevelOfDetail(planet), (kGranularity - 1.0f) / kGranularity));
    expected = sqrtf((800.0f / 100.0f) * 100.0f / sqrtf(distance) * 0.25f) -
               0.5f / kGranularity;
    CalculateLevelOfDetail(planet, distance);
    EXPECT_TRUE(Near(LevelOfDetail(planet),
                  Level(fminf(fmaxf(expected, 0.0f),
                              (kGranularity - 1.0f) / kGranularity))));

    // A wider screen: more detail at the same distance.
    sUniverse->_reduced = false;
    sUniverse->_view->_size = NSMakeSize(1600, 1200);
    expected = sqrtf((1600.0f / 40.0f) * 100.0f / sqrtf(distance) * 0.25f);
    CalculateLevelOfDetail(planet, distance);
    EXPECT_TRUE(Near(LevelOfDetail(planet), Level(fminf(expected, 1.0f))));
  }
}

TEST(OOPlanetDrawable, material) {
  {
    ::SetUp();
    Planet planet = NewPlanet();
    oo::Material* material = NamedMaterial("rock");
    SetMaterial(planet, material);
    EXPECT_TRUE(Material(planet) == material);
    EXPECT_TRUE(TextureName(planet) == std::optional<std::string>("rock"));

    // The same name keeps the material.
    SetTextureName(planet, "rock");
    EXPECT_TRUE(Material(planet) == material);

    SetMaterial(planet, nullptr);
    EXPECT_TRUE(Material(planet) == nullptr);
    EXPECT_TRUE(!TextureName(planet).has_value());
  }
}

TEST(OOPlanetDrawable, copy) {
  {
    ::SetUp();
    Planet atmosphere = AtmosphereWithRadius(321.0f);
    oo::Material* material = NamedMaterial("air");
    SetMaterial(atmosphere, material);
    SetLevelOfDetail(atmosphere, 0.2f);

    Planet copy = Copy(atmosphere);
    EXPECT_TRUE(copy != atmosphere);
    EXPECT_TRUE(Radius(copy) == 321.0f);
    EXPECT_TRUE(Material(copy) == material);
    EXPECT_TRUE(!HasOpaqueParts(copy) && HasTranslucentParts(copy));
    EXPECT_TRUE(Near(LevelOfDetail(copy), Level(0.2f)));
    BoundingBox box = Bounds(copy);
    EXPECT_TRUE(box.max.x == 321.0f);

    // The copy is its own drawable.
    SetRadius(copy, 10.0f);
    EXPECT_TRUE(Radius(atmosphere) == 321.0f && Radius(copy) == 10.0f);
  }
}
