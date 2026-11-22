/*	test_OOFlasherEntity.mm
        Unit tests for OOFlasherEntity (src/Core/Entities/OOFlasherEntity.h),
   the blinking lights that ships and visual effects carry as subentities.

        The entity reads Universe and PLAYER, so the test links the whole game
   but main (['*']) and uses a Universe that was never initialised and a plain
   entity as PLAYER. The tests cover: a flasher reads its size, frequency
   (doubled), phase, bright fraction, colours and initial state from its
   dictionary (defaults 1, 1, 0, 0.5, none, on); its accessors; its colour is
   its current components; -update: cycles its colours on the wave's falling
   half and sets its brightness, or keeps it opaque at frequency 0; it draws
   only while active; it is a flasher (and an entity is not); its collision
   radius is half its diameter; -rescaleBy: scales the diameter and
        -rescaleBy:writeToCache: does nothing. The colour components, an ivar of
   OOLightParticleEntity, are read and set through the one block of helpers
   below. The flashers are made as the callers make them
   (flasherWithDictionary()). The cases ask the C++ class; the facade case pins
   the crossing (the object is OOLightParticleEntity's, whose C++ part is the
   flasher), and jsExtensions pins that the engine's selectors reach the C++
   class's overrides through the root. Run: tests/run.sh unit
*/

#include <cmath>
#include <numbers>

#include <gtest/gtest.h>

#include "EntityOOJavaScriptExtensions.h"
#include "Color.h"
#include "FlasherEntity.h"
#include "OOJSEntity.h"
#include "OOJSFlasher.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "EntityManualRefs.h"  // the tests' manual entity references

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

class PlayerEntity;
extern PlayerEntity* gOOPlayer;
extern Universe* gSharedUniverse;

class TestPlayer : public PlayerEntity {
 public:
  HPVector viewpointPosition() override { return kZeroHPVector; }
};

// A C++ player under the ship's facade, as PlayerEntity::sharedPlayer() makes
// the game's, retained (+1) as +alloc's object was.
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::RetainEntity(oo::MadeEntity(player));
  }
  return player.get();
}

// UNIVERSE: never initialised.
class TestUniverse : public Universe {
 public:
};

namespace {

void SetUp() {
  static Universe* universe = nullptr;
  if (universe == nullptr) {
    universe = new TestUniverse();  // never released
  }
  gSharedUniverse = universe;
  static TestPlayer* player = nullptr;
  if (player == nullptr) player = NewTestPlayer<TestPlayer>();
  gOOPlayer = player;
}

// --- Ivars the test reads and sets, and nothing else
// -----------------------------------------------

const GLfloat* ColorComponents(oo::FlasherEntity* e) {
  return e->_colorComponents;
}
GLfloat CamZeroDistance(Entity* e) { return e->cam_zero_distance; }
void SetCamZeroDistance(Entity* e, GLfloat value) {
  e->cam_zero_distance = value;
}

// --------------------------------------------------------------------------------------------------

// A flasher, made as the ships and the visual effects make one: its Objective-C
// object is autoreleased in the test's pool and holds it.
oo::FlasherEntity* Flasher(const oo::PList& dictionary) {
  return static_cast<oo::FlasherEntity*>(
      oo::MadeEntity(oo::FlasherEntity::flasherWithDictionary(dictionary)));
}

bool Near(double a, double b) { return std::fabs(a - b) < 1e-4; }

bool RGBIs(oo::FlasherEntity* e, GLfloat r, GLfloat g, GLfloat b) {
  const GLfloat* c = ColorComponents(e);
  return Near(c[0], r) && Near(c[1], g) && Near(c[2], b);
}

oo::PList Dict(oo::PList::Dict entries) {
  return oo::PList(std::move(entries));
}

oo::PList WhiteBlack(float frequency, float fraction) {
  return Dict({
      {"size", oo::PList(4.0)},
      {"frequency", oo::PList(double(frequency))},
      {"bright_fraction", oo::PList(double(fraction))},
      {"colors",
       oo::PList(oo::PList::Array{oo::PList(std::string("whiteColor")),
                                  oo::PList(std::string("blackColor"))})},
  });
}

}  // namespace

TEST(OOFlasherEntity, defaults) {
  {
    ::SetUp();
    oo::FlasherEntity* flasher = Flasher(Dict({}));
    EXPECT_TRUE(flasher != nullptr);
    EXPECT_TRUE(dynamic_cast<oo::LightParticleEntity*>(flasher) != nullptr);
    EXPECT_TRUE(flasher->diameter() == 1.0f);
    EXPECT_TRUE(flasher->frequency() == 2.0f && flasher->phase() == 0.0f &&
             flasher->fraction() == 0.5f);
    EXPECT_TRUE(flasher->isActive());
    EXPECT_TRUE(flasher->status() == oo::EntityStatus::kEffect &&
             flasher->getScanClass() == oo::ScanClass::kNoDraw);
    // No colours: the current colour is nil, which left the components white.
    EXPECT_TRUE(RGBIs(flasher, 1, 1, 1) && ColorComponents(flasher)[3] == 1.0f);
    EXPECT_TRUE(flasher->isFlasher());
    EXPECT_TRUE(flasher->findCollisionRadius() == 0.5);
  }
}

TEST(OOFlasherEntity, dictionary) {
  {
    ::SetUp();
    oo::FlasherEntity* flasher = Flasher(Dict({
        {"size", oo::PList(6.0)},
        {"frequency", oo::PList(1.5)},
        {"phase", oo::PList(0.25)},
        {"bright_fraction", oo::PList(0.75)},
        {"initially_on", oo::PList(false)},
        {"colors",
         oo::PList(oo::PList::Array{oo::PList(std::string("blackColor")),
                                    oo::PList(std::string("whiteColor"))})},
    }));
    EXPECT_TRUE(flasher->diameter() == 6.0f &&
             flasher->findCollisionRadius() == 3.0);
    EXPECT_TRUE(flasher->frequency() == 3.0f && flasher->phase() == 0.25f &&
             flasher->fraction() == 0.75f);
    EXPECT_TRUE(!flasher->isActive());
    EXPECT_TRUE(RGBIs(flasher, 0, 0, 0) &&
             ColorComponents(flasher)[3] == 1.0f);  // the first colour

    std::shared_ptr<oo::Color> color = flasher->color();
    float r = -1, g = -1, b = -1, a = -1;
    color->getRed(&r, &g, &b, &a);
    EXPECT_TRUE(r == 0 && g == 0 && b == 0 && a == 1);
  }
}

TEST(OOFlasherEntity, saturationFactor) {
  {
    ::SetUp();
    oo::FlasherEntity* flasher = Flasher(Dict({
        {"colors",
         oo::PList(oo::PList::Array{oo::PList(std::string("redColor"))})},
    }));
    // Red at three quarters of its saturation.
    EXPECT_TRUE(RGBIs(flasher, 1.0f, 0.25f, 0.25f));
  }
}

TEST(OOFlasherEntity, accessors) {
  {
    ::SetUp();
    oo::FlasherEntity* flasher = Flasher(WhiteBlack(1, 0.5));
    flasher->setActive(false);
    EXPECT_TRUE(!flasher->isActive());
    flasher->setActive(true);
    EXPECT_TRUE(flasher->isActive());
    flasher->setFrequency(7.0f);
    EXPECT_TRUE(flasher->frequency() == 7.0f);
    flasher->setPhase(0.5f);
    EXPECT_TRUE(flasher->phase() == 0.5f);
    flasher->setFraction(0.125f);
    EXPECT_TRUE(flasher->fraction() == 0.125f);
    flasher->setColor(oo::Color::colorWithRed(0.5f, 0.25f, 0.125f, 0.75f).get(),
                      0.375f);
    float r = -1, g = -1, b = -1, a = -1;
    flasher->color()->getRed(&r, &g, &b, &a);
    EXPECT_TRUE(r == 0.5f && g == 0.25f && b == 0.125f && a == 0.375f);
  }
}

TEST(OOFlasherEntity, updateCyclesColours) {
  {
    ::SetUp();
    // frequency 1 (stored 2): wave = sin(2 pi t), fraction 0.5: threshold
    // cos(pi / 2) = 0.
    oo::FlasherEntity* flasher = Flasher(WhiteBlack(1, 0.5));
    EXPECT_TRUE(RGBIs(flasher, 1, 1, 1));

    flasher->update(0.125);  // t 0.125: wave sin(pi/4), rising half: brightness
                             // above the fraction
    double wave = std::sin(std::numbers::pi / 4);
    EXPECT_TRUE(RGBIs(flasher, 1, 1, 1));
    EXPECT_TRUE(Near(ColorComponents(flasher)[3], 0.5 + (0.5 / 1.0) * wave));

    flasher->update(
        0.5);  // t 0.625: wave -sin(pi/4), falling: no switch (wave < _wave)
    EXPECT_TRUE(RGBIs(flasher, 1, 1, 1));
    EXPECT_TRUE(Near(ColorComponents(flasher)[3], 0.5 + (0.5 / 1.0) * -wave));

    flasher->update(
        0.3125);  // t 0.9375: wave still negative and rising: the next colour
    EXPECT_TRUE(RGBIs(flasher, 0, 0, 0));

    flasher->update(0.03125);  // t 0.96875: just switched, no second switch
    EXPECT_TRUE(RGBIs(flasher, 0, 0, 0));

    flasher->update(0.0625);   // t 1.03125: positive again, the switch resets
    flasher->update(0.59375);  // t 1.625: negative but falling: no switch
    EXPECT_TRUE(RGBIs(flasher, 0, 0, 0));
    flasher->update(
        0.3125);  // t 1.9375: rising below zero: back to the first colour
    EXPECT_TRUE(RGBIs(flasher, 1, 1, 1));
  }
}

TEST(OOFlasherEntity, frequencyZeroIsOpaque) {
  {
    ::SetUp();
    oo::FlasherEntity* flasher = Flasher(WhiteBlack(0, 0.5));
    flasher->setColor(oo::Color::whiteColor().get(), 0.25f);
    flasher->update(0.3);
    EXPECT_TRUE(ColorComponents(flasher)[3] == 1.0f);
    EXPECT_TRUE(RGBIs(flasher, 1, 1, 1));
  }
}

TEST(OOFlasherEntity, drawsOnlyWhileActive) {
  {
    ::SetUp();
    Entity* owner = oo::MadeEntity(oo::makeShared<Entity>());
    SetCamZeroDistance(
        owner,
        1e30f);  // beyond any draw distance: the draw stops at the cut-off
    oo::FlasherEntity* flasher = Flasher(WhiteBlack(1, 0.5));
    flasher->setOwner(owner);
    SetCamZeroDistance(flasher, 5.0f);

    flasher->setActive(false);
    flasher->drawSubEntityImmediate(true, true);
    EXPECT_TRUE(CamZeroDistance(flasher) == 5.0f);  // not drawn

    flasher->setActive(true);
    flasher->drawSubEntityImmediate(true, true);
    EXPECT_TRUE(CamZeroDistance(flasher) ==
             1e30f);  // OOLightParticleEntity's draw read the owner's distance
  }
}

TEST(OOFlasherEntity, rescale) {
  {
    ::SetUp();
    oo::FlasherEntity* flasher = Flasher(WhiteBlack(1, 0.5));
    flasher->rescaleBy(2.5f);
    EXPECT_TRUE(flasher->diameter() == 10.0f);
    flasher->rescaleBy(3.0f, true);
    EXPECT_TRUE(flasher->diameter() == 10.0f);
  }
}

TEST(OOFlasherEntity, isFlasher) {
  {
    ::SetUp();
    // Callers ask the C++ part.
    Entity* plain = oo::MadeEntity(oo::makeShared<Entity>());
    EXPECT_TRUE(dynamic_cast<oo::FlasherEntity*>(plain) == nullptr);
    const std::shared_ptr<oo::LightParticleEntity> lightParticle =
        oo::makeShared<oo::LightParticleEntity>();
    Entity* particle = oo::MadeEntity(lightParticle);
    lightParticle->initWithDiameter(1.0f);
    EXPECT_TRUE(dynamic_cast<oo::FlasherEntity*>(particle) == nullptr);
    oo::FlasherEntity* flasher = Flasher(Dict({}));
    EXPECT_TRUE(flasher->isFlasher() &&
             dynamic_cast<oo::FlasherEntity*>(flasher) == flasher);
  }
}

TEST(OOFlasherEntity, facade) {
  {
    ::SetUp();
    // The object is the root's facade, whose C++ part is the flasher: a C++
    // entity.
    Entity* object = oo::MadeEntity(
        oo::FlasherEntity::flasherWithDictionary(WhiteBlack(1, 0.5)));
    EXPECT_TRUE(dynamic_cast<oo::FlasherEntity*>(object) != nullptr);
    EXPECT_TRUE(object == object);
  }
}

// Registers Entity's JS class and the given class (a child of Entity) in the
// test's context, as the engine's start does; the context is made here if the
// test has none yet.
static void RegisterJSClasses(void (*initClass)(JSContext*, JSValueConst)) {
  extern JSContext* gOOJSMainThreadContext;
  if (gOOJSMainThreadContext == nullptr)
    gOOJSMainThreadContext = JS_NewContext(JS_NewRuntime());
  JSContext* context = OOJSAcquireContext();
  JSValue global = JS_GetGlobalObject(context);
  InitOOJSEntity(context, global);
  initClass(context, global);
  JS_FreeValue(context, global);
}

// The C++ class's overrides of the root's JS members: what the engine asks a
// flasher's object for by selector is what OOJSFlasher.mm answers.
TEST(OOFlasherEntity, jsExtensions) {
  {
    ::SetUp();
    Entity* flasher = Flasher(WhiteBlack(1, 0.5));
    RegisterJSClasses(InitOOJSFlasher);
    const JSClassID jsClass = OOJSEntityJSClassID(flasher);
    const JSClassID expectedClass = JSFlasherClassID();
    EXPECT_TRUE(jsClass != 0 && jsClass == expectedClass);
    EXPECT_TRUE(OOJSEntityJSClassName(flasher) ==
             std::optional<std::string>("Flasher"));
    EXPECT_TRUE(OOJSEntityIsVisibleToScripts(flasher) == true);
  }
}
