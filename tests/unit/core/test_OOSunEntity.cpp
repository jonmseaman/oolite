/*	test_OOSunEntity.mm
        Unit tests for OOSunEntity (src/Core/Entities/OOSunEntity.h), a system's
   star.

        The entity reads Universe and PLAYER, so the test links the whole game
   but main (['*']) and uses a Universe that was never initialised (minimum
   detail, so the corona never moves), subclassed to record the main light
   position, the sky colour and the system data the sun sets, and an entity that
   answers the viewpoint the test sets as PLAYER. A sun's radius, name and
   corona come from its dictionary, it is a no-draw, visible, collidable sun;
   its light colours are its colour blended with white;
   -changeSunProperty:withDictionary: takes the radius, the name, the corona,
        and going nova or not, and refuses anything else; a new position moves
   the main light; going nova counts down, then whitens the sky and records the
   nova and the growing radius in the system data; and the camera-relative
   position is pulled in to 1e9 for a far sun. The camera-relative position is
   read through the one helper below. The cases make the C++ sun and ask the C++
        class, and the facade case pins the crossing: the
        sun is the root Entity. Run: bash tools/check-core-tests.sh
*/

#include <cmath>
#include <map>
#include <string>

#include <gtest/gtest.h>

#include "EntityOOJavaScriptExtensions.h"
#include "Color.h"
#include "OOJSEntity.h"
#include "OOJSSun.h"
#include "SunEntity.h"
#include "PlayerEntity.h"  // PLAYER is C++
#include "Universe.h"
#include "oofnd/PListGet.hpp"
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
  HPVector _viewpoint = {};

  HPVector viewpointPosition() override { return _viewpoint; }
};

// NewTestPlayer<TestPlayer>(): a C++ player, as PlayerEntity::sharedPlayer()
// makes the game's, retained (+1).
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::RetainEntity(oo::MadeEntity(player));
  }
  return player.get();
}

// UNIVERSE: never initialised; records what the sun sets.
class TestUniverse : public Universe {
 public:
  Vector _mainLight = {};
  GLfloat _sky[4];
  int _skySets = {};
  std::map<std::string, oo::PList> _systemData = {};

  void setMainLightPosition(Vector sunPos) override { _mainLight = sunPos; }
  void setSkyColorRed(GLfloat red, GLfloat green, GLfloat blue,
                      GLfloat alpha) override {
    _sky[0] = red;
    _sky[1] = green;
    _sky[2] = blue;
    _sky[3] = alpha;
    _skySets++;
  }
  void setSystemDataKey(
      const std::string& key, const oo::PList& value,
      const std::optional<std::string>& /*manifest*/) override {
    _systemData[key] = value;
  }
};

namespace {

TestUniverse* sUniverse = nullptr;
TestPlayer* sPlayer = nullptr;

void SetUp() {
  if (sUniverse == nullptr) {
    sUniverse = new TestUniverse();  // never released
    new (&sUniverse->_systemData) std::map<std::string, oo::PList>();
    sPlayer = NewTestPlayer<TestPlayer>();
  }
  sUniverse->_skySets = 0;
  sUniverse->_systemData.clear();
  sUniverse->_mainLight = kZeroVector;
  gSharedUniverse = sUniverse;
  gOOPlayer = sPlayer;
  sPlayer->_viewpoint = kZeroHPVector;
}

// --- Ivars the test reads, and nothing else
// ---------------------------------------------------------

Vector CameraRelativePosition(Entity* e) {
  return e->cameraRelativePosition;
}  // --------------------------------------------------------------------------------------------------

oo::PList Dict(oo::PList::Dict entries) {
  return oo::PList(std::move(entries));
}

bool Near(double a, double b) { return std::fabs(a - b) < 1e-4; }

// A new C++ sun.
oo::SunEntity* Sun(oo::Color* color, double radius) {
  std::shared_ptr<oo::SunEntity> sun = oo::makeShared<oo::SunEntity>();
  (void)oo::MadeEntity(sun);
  sun->initSunWithColor(color, Dict({
                                   {"sun_name", oo::PList("Sol")},
                                   {"sun_radius", oo::PList(radius)},
                                   {"corona_flare", oo::PList(0.5)},
                                   {"corona_shimmer", oo::PList(0.5)},
                               }));
  return sun.get();
}

oo::SunEntity* RedSun() {
  return Sun(oo::Color::colorWithRed(1.0f, 0.0f, 0.0f, 1.0f).get(), 1000.0);
}

}  // namespace

TEST(OOSunEntity, made) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    EXPECT_TRUE(sun != nullptr &&
             dynamic_cast<oo::SunEntity*>(static_cast<Entity*>(sun)) != nullptr);
    EXPECT_TRUE(sun->radius() == 1000.0 && sun->collisionRadius() == 1000.0f);
    EXPECT_TRUE(sun->name() == std::optional<std::string>("Sol"));
    EXPECT_TRUE(sun->planetType() == oo::StellarBodyType::kSun);
    EXPECT_TRUE(sun->getScanClass() == oo::ScanClass::kNoDraw);
    EXPECT_TRUE(sun->isSun() && sun->isVisible() && sun->canCollide());
    EXPECT_TRUE(!sun->willGoNova() && !sun->goneNova());
  }
}

TEST(OOSunEntity, lightColours) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    // Red blended 30% with white: (1, 0.3, 0.3).
    GLfloat diffuse[4] = {-1, -1, -1, -1};
    GLfloat specular[4] = {-1, -1, -1, -1};
    sun->getDiffuseComponents(diffuse);
    sun->getSpecularComponents(specular);
    EXPECT_TRUE(Near(specular[0], 1.0) && Near(specular[1], 0.3) &&
             Near(specular[2], 0.3) && specular[3] == 1.0f);
    EXPECT_TRUE(Near(diffuse[0], 1.0) && Near(diffuse[1], 0.65) &&
             Near(diffuse[2], 0.65) && diffuse[3] == 1.0f);

    EXPECT_TRUE(sun->setSunColor(oo::Color::blueColor().get()));
    sun->getSpecularComponents(specular);
    EXPECT_TRUE(Near(specular[0], 0.3) && Near(specular[1], 0.3) &&
             Near(specular[2], 1.0));
    EXPECT_TRUE(!sun->setSunColor(nullptr));
    sun->getSpecularComponents(specular);
    EXPECT_TRUE(Near(specular[2], 1.0));
  }
}

TEST(OOSunEntity, changeSunProperty) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    EXPECT_TRUE(sun->changeSunProperty("sun_radius",
                                    Dict({{"sun_radius", oo::PList(2500.0)}})));
    EXPECT_TRUE(sun->radius() == 2500.0);
    EXPECT_TRUE(sun->changeSunProperty("sun_name",
                                    Dict({{"sun_name", oo::PList("Alpha")}})));
    EXPECT_TRUE(sun->name() == std::optional<std::string>("Alpha"));
    EXPECT_TRUE(sun->changeSunProperty("sun_name", Dict({})));
    EXPECT_TRUE(!sun->name().has_value());
    EXPECT_TRUE(sun->changeSunProperty("corona_flare",
                                    Dict({{"corona_flare", oo::PList(0.25)}})));
    EXPECT_TRUE(sun->radius() == 2500.0);
    EXPECT_TRUE(sun->changeSunProperty(
        "corona_shimmer", Dict({{"corona_shimmer", oo::PList(0.5)}})));
    EXPECT_TRUE(sun->changeSunProperty("corona_hues",
                                    Dict({{"corona_hues", oo::PList(0.5)}})));

    EXPECT_TRUE(sun->changeSunProperty(
        "sun_gone_nova", Dict({{"sun_gone_nova", oo::PList(true)}})));
    EXPECT_TRUE(sun->willGoNova() && sun->goneNova());
    EXPECT_TRUE(sun->changeSunProperty("sun_gone_nova",
                                    Dict({{"sun_gone_nova", oo::PList(false)},
                                          {"corona_flare", oo::PList(0.1)}})));
    EXPECT_TRUE(!sun->willGoNova() && !sun->goneNova());
    EXPECT_TRUE(sun->radius() == 2500.0);  // the radius it had before going nova

    EXPECT_TRUE(!sun->changeSunProperty(
        "sun_color", Dict({{"sun_color", oo::PList("redColor")}})));
  }
}

TEST(OOSunEntity, radiusAndPosition) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    sun->setRadius(300.0f, 0.0f);
    EXPECT_TRUE(sun->radius() == 300.0);
    sun->setPosition(make_HPvector(10, 20, 30));
    EXPECT_TRUE(HPvector_equal(sun->getPosition(), make_HPvector(10, 20, 30)));
    EXPECT_TRUE(sUniverse->_mainLight.x == 10 && sUniverse->_mainLight.y == 20 &&
             sUniverse->_mainLight.z == 30);
  }
}

TEST(OOSunEntity, goingNova) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    sun->setGoingNova(true, 1.0);
    EXPECT_TRUE(sun->willGoNova() && !sun->goneNova());
    sun->update(0.5);
    EXPECT_TRUE(!sun->goneNova() && sUniverse->_skySets == 0);
    sun->update(0.625);
    EXPECT_TRUE(sun->goneNova() && sUniverse->_skySets == 0);

    // The expansion: the sky goes white and the nova is recorded.
    sun->update(0.125);
    EXPECT_TRUE(sUniverse->_skySets == 1 && sUniverse->_sky[0] == 1.0f &&
             sUniverse->_sky[3] == 1.0f);
    EXPECT_TRUE(sUniverse->_systemData["sun_gone_nova"] ==
             oo::PList(static_cast<bool>(
                 true)));  // a boolean (OOCocoa.h defines true as 1)
    EXPECT_TRUE(sUniverse->_systemData.count("corona_flare") == 1 &&
             sUniverse->_systemData.count("corona_hues") == 1);
    EXPECT_TRUE(Near(
        oo::PListGet<double>::from(&sUniverse->_systemData["sun_radius"], -1.0),
        1000.0 + 0.125 * 10000.0));

    // Later in the minute: the sky fades, nothing more recorded but the radius.
    sUniverse->_systemData.clear();
    sun->update(1.0);
    EXPECT_TRUE(sUniverse->_skySets == 2 && sUniverse->_sky[0] == 0.8125f &&
             sUniverse->_sky[3] == 1.0f);
    EXPECT_TRUE(sUniverse->_systemData.count("sun_gone_nova") == 0 &&
             sUniverse->_systemData.count("sun_radius") == 1);
    // Then it goes black.
    sun->update(0.125);
    EXPECT_TRUE(sUniverse->_skySets == 3 && sUniverse->_sky[0] == 0.0f &&
             sUniverse->_sky[3] == 0.0f);

    sun->resetNova();
    EXPECT_TRUE(sun->willGoNova() &&
             sun->goneNova());  // reset keeps the higher temperature
    sun->setGoingNova(false, 0);
    EXPECT_TRUE(!sun->willGoNova());
  }
}

TEST(OOSunEntity, description) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    sun->setGoingNova(true, 10.0);
    const std::string description = oo::EntityDescription(sun);
    EXPECT_TRUE(description.find("radius: 1.000km") != std::string::npos);
    EXPECT_TRUE(description.find("(will go nova)") != std::string::npos);
  }
}

TEST(OOSunEntity, cameraRelativePosition) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    sun->setPosition(make_HPvector(100, 0, 0));
    sPlayer->_viewpoint = make_HPvector(40, 0, 0);
    sun->updateCameraRelativePosition();
    Vector v = CameraRelativePosition(sun);
    EXPECT_TRUE(v.x == 60.0f && v.y == 0.0f && v.z == 0.0f);

    sun->setPosition(make_HPvector(4e9, 0, 3e9));
    sPlayer->_viewpoint = kZeroHPVector;
    sun->updateCameraRelativePosition();
    v = CameraRelativePosition(sun);
    EXPECT_TRUE(Near(v.x / 1e9, 0.8) && Near(v.z / 1e9, 0.6));
  }
}

TEST(OOSunEntity, facade) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    Entity* object = sun;
    EXPECT_TRUE(object != nullptr);  // A C++ entity.
    EXPECT_TRUE(dynamic_cast<oo::SunEntity*>(object) == sun);
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

// The binding's category: what
// the engine asks a OOSunEntity for by selector is what OOJSSun.mm answers.
TEST(OOSunEntity, jsExtensions) {
  {
    ::SetUp();
    oo::SunEntity* sun = RedSun();
    RegisterJSClasses(InitOOJSSun);
    Entity* object = sun;  // what the engine asks
    const JSClassID jsClass = OOJSEntityJSClassID(object);
    const JSClassID expectedClass = JSSunClassID();
    EXPECT_TRUE(jsClass != 0 && jsClass == expectedClass);
    EXPECT_TRUE(OOJSEntityJSClassName(object) ==
             std::optional<std::string>("Sun"));
    EXPECT_TRUE(OOJSEntityIsVisibleToScripts(object) == true);
  }
}
