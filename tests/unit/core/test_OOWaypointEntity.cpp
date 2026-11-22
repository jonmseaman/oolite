/*	test_OOWaypointEntity.mm
        Unit tests for OOWaypointEntity (src/Core/Entities/OOWaypointEntity.h),
   the scripted beacon waypoints.

        The entity reads Universe and PLAYER, so the test links the whole game
   but main (['*']) and uses a Universe that was never initialised and a plain
   entity as PLAYER. A waypoint reads its position, orientation, size and beacon
   code and label from its dictionary (defaults: the origin, the identity, 1000,
   "W" and "Waypoint"; an empty dictionary reads zeros and no beacon); a zero
        orientation is the identity and marks it unoriented; its size sets its
   draw distance and ignores a size that is not positive; it is a no-draw effect
   and a waypoint; the beacon code and label setters (an empty string is none; a
   code gives a blank label its text) and the case-insensitive comparison; its
   beacon icon is kept until the code changes; and its neighbours in the beacon
        list are weak references. The draw distance is read through the one
   helper below. The waypoints are made as the universe makes them: by the C++
   factory. The object still answers the beacon selectors (the root's category
   Entity (OOWaypointBeacon)), so the beacon checks send them; the class's own
   selectors are member calls; the last tests pin that the object is a C++
        entity's, that only a waypoint's object (or an entity that implements
   them) answers the beacon selectors, and the JS answers. Run: bash
   tools/check-core-tests.sh
*/


#include <gtest/gtest.h>

#include "EntityManualRefs.h"  // the tests' manual entity references
#include "EntityOOJavaScriptExtensions.h"
#include "OOJSEntity.h"
#include "OOJSWaypoint.h"
#include "WaypointEntity.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "oofnd/AutoreleaseScope.hpp"

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

// NewTestPlayer<TestPlayer>(): a C++ player, as PlayerEntity::sharedPlayer()
// makes the game's, retained (+1).
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::AutoreleaseScope scope;
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

// --- Ivars the test reads, and nothing else
// ---------------------------------------------------------

GLfloat NoDrawDistance(Entity* e) { return e->no_draw_distance; }

// --------------------------------------------------------------------------------------------------

oo::PList Dict(oo::PList::Dict entries) {
  return oo::PList(std::move(entries));
}

oo::PList Vec(double x, double y, double z) {
  return oo::PList(oo::PList::Array{oo::PList(x), oo::PList(y), oo::PList(z)});
}

bool QuatIs(Quaternion q, float w, float x, float y, float z) {
  return q.w == w && q.x == x && q.y == y && q.z == z;
}

// The waypoint's object, as the universe makes it.
Entity* WaypointObject(const oo::PList& info) {
  return (Entity*)oo::MadeEntity(
      oo::WaypointEntity::waypointWithDictionary(info));
}

oo::WaypointEntity* Part(Entity* object) {
  return dynamic_cast<oo::WaypointEntity*>(object);
}

Entity* Waypoint(const std::string& code) {
  return WaypointObject(Dict({{"beaconCode", oo::PList(code)}}));
}

}  // namespace

TEST(OOWaypointEntity, defaults) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = WaypointObject(Dict({{"unused", oo::PList(1)}}));
    EXPECT_TRUE(wp != nullptr && Part(wp) != nullptr);
    EXPECT_TRUE(HPvector_equal(wp->getPosition(), kZeroHPVector));
    EXPECT_TRUE(QuatIs(wp->getOrientation(), 1, 0, 0, 0) &&
             Part(wp)->getOriented());
    EXPECT_TRUE(Part(wp)->size() == 1000.0);
    EXPECT_TRUE(NoDrawDistance(wp) ==
             (GLfloat)(1000.0 * 1000.0 * NO_DRAW_DISTANCE_FACTOR *
                       NO_DRAW_DISTANCE_FACTOR * 2));
    EXPECT_TRUE(oo::BeaconCode(wp) == std::optional<std::string>("W"));
    EXPECT_TRUE(oo::BeaconLabel(wp) == std::optional<std::string>("Waypoint"));
    EXPECT_TRUE(oo::EntityIsBeacon(wp) && !oo::BeaconIsJammingScanning(wp));
    EXPECT_TRUE(wp->status() == oo::EntityStatus::kEffect &&
             wp->getScanClass() == oo::ScanClass::kNoDraw);
    EXPECT_TRUE(wp->isEffect() && wp->isWaypoint());
  }
}

TEST(OOWaypointEntity, dictionary) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = WaypointObject(Dict({
        {"position", Vec(1, 2, 3)},
        {"orientation",
         oo::PList(oo::PList::Array{oo::PList(0.0), oo::PList(1.0),
                                    oo::PList(0.0), oo::PList(0.0)})},
        {"size", oo::PList(50.0)},
        {"beaconCode", oo::PList("AB")},
        {"beaconLabel", oo::PList("Somewhere")},
    }));
    EXPECT_TRUE(HPvector_equal(wp->getPosition(), make_HPvector(1, 2, 3)));
    EXPECT_TRUE(QuatIs(wp->getOrientation(), 0, 1, 0, 0) &&
             Part(wp)->getOriented());
    EXPECT_TRUE(Part(wp)->size() == 50.0);
    EXPECT_TRUE(oo::BeaconCode(wp) == std::optional<std::string>("AB"));
    EXPECT_TRUE(oo::BeaconLabel(wp) == std::optional<std::string>("Somewhere"));
  }
}

TEST(OOWaypointEntity, emptyDictionary) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    // A nil dictionary read zero-filled values and nil strings: the zero
    // orientation is unoriented.
    Entity* wp = WaypointObject(oo::PList());
    EXPECT_TRUE(HPvector_equal(wp->getPosition(), kZeroHPVector));
    EXPECT_TRUE(QuatIs(wp->getOrientation(), 1, 0, 0, 0) &&
             !Part(wp)->getOriented());
    EXPECT_TRUE(!oo::BeaconCode(wp).has_value() &&
             !oo::BeaconLabel(wp).has_value() && !oo::EntityIsBeacon(wp));
  }
}

TEST(OOWaypointEntity, orientation) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = Waypoint("W");
    wp->setOrientation(kZeroQuaternion);
    EXPECT_TRUE(!Part(wp)->getOriented() &&
             QuatIs(wp->getOrientation(), 1, 0, 0, 0));
    Quaternion q = {0, 0, 1, 0};
    wp->setOrientation(q);
    EXPECT_TRUE(Part(wp)->getOriented() &&
             QuatIs(wp->getOrientation(), 0, 0, 1, 0));
  }
}

TEST(OOWaypointEntity, size) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = Waypoint("W");
    Part(wp)->setSize(10.0);
    EXPECT_TRUE(Part(wp)->size() == 10.0);
    EXPECT_TRUE(NoDrawDistance(wp) ==
             (GLfloat)(10.0 * 10.0 * NO_DRAW_DISTANCE_FACTOR *
                       NO_DRAW_DISTANCE_FACTOR * 2));
    Part(wp)->setSize(0.0);
    Part(wp)->setSize(-5.0);
    EXPECT_TRUE(Part(wp)->size() == 10.0);
  }
}

TEST(OOWaypointEntity, beaconStrings) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = WaypointObject(Dict(
        {{"beaconCode", oo::PList("X")}, {"beaconLabel", oo::PList("Label")}}));
    oo::SetBeaconCode(wp, std::string(""));
    EXPECT_TRUE(!oo::BeaconCode(wp).has_value() && !oo::EntityIsBeacon(wp));
    EXPECT_TRUE(oo::BeaconLabel(wp) == std::optional<std::string>("Label"));

    static_cast<oo::WaypointEntity*>(wp)->setBeaconLabel(
        std::string(""));  // the waypoint's, which its object's
                           // -setBeaconLabel: forwarded to
    EXPECT_TRUE(!oo::BeaconLabel(wp).has_value());
    oo::SetBeaconCode(wp, std::string("Q"));
    EXPECT_TRUE(oo::BeaconCode(wp) == std::optional<std::string>("Q") &&
             oo::BeaconLabel(wp) == std::optional<std::string>("Q"));
    oo::SetBeaconCode(wp, std::string("R"));
    EXPECT_TRUE(oo::BeaconLabel(wp) ==
             std::optional<std::string>("Q"));  // a label is kept
  }
}

TEST(OOWaypointEntity, compareBeaconCodes) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    EXPECT_TRUE(Part(Waypoint("a"))->compareBeaconCodeWith(Waypoint("B")) ==
             oo::ComparisonResult::kAscending);
    EXPECT_TRUE(Part(Waypoint("b"))->compareBeaconCodeWith(Waypoint("A")) ==
             oo::ComparisonResult::kDescending);
    EXPECT_TRUE(Part(Waypoint("abc"))->compareBeaconCodeWith(Waypoint("ABC")) ==
             oo::ComparisonResult::kSame);
  }
}

TEST(OOWaypointEntity, beaconDrawable) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = Waypoint("W");
    // A C++ OOHUDBeaconIcon, held as the test retained it.
    std::shared_ptr<oo::HUDBeaconIcon> icon(oo::share(
        static_cast<oo::HUDBeaconIcon*>(oo::BeaconDrawable(wp))));
    EXPECT_TRUE(icon != nullptr && oo::BeaconDrawable(wp) == icon.get());
    oo::SetBeaconCode(wp, std::string("V"));
    EXPECT_TRUE(oo::BeaconDrawable(wp) != nullptr &&
             oo::BeaconDrawable(wp) != icon.get());
  }
}

TEST(OOWaypointEntity, neighbours) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = Waypoint("W");
    EXPECT_TRUE(oo::PrevBeacon(wp) == nullptr && oo::NextBeacon(wp) == nullptr);
    Entity* other = nullptr;
    {
      oo::AutoreleaseScope scope;
      other = oo::RetainEntity(Waypoint("V"));
    }
    oo::SetPrevBeacon(wp, other);
    oo::SetNextBeacon(wp, other);
    EXPECT_TRUE(oo::PrevBeacon(wp) == other && oo::NextBeacon(wp) == other);
    oo::ReleaseEntity(other);  // weak: they read nil once it is gone
    EXPECT_TRUE(oo::PrevBeacon(wp) == nullptr && oo::NextBeacon(wp) == nullptr);
  }
}

TEST(OOWaypointEntity, facade) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = Waypoint("W");  // the root entity (the protocol type would
                                 // pick a subclass's crossing)
    // A C++ entity.
    EXPECT_TRUE(dynamic_cast<oo::WaypointEntity*>(wp) != nullptr);
    EXPECT_TRUE(wp == wp);
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

// The beacon selectors on the root entity: answered for a waypoint's part,
// and not for another entity's, whose object did not answer them. The beacon
// questions a plain entity is asked answer none.
TEST(OOWaypointEntity, beaconSelectorsForAWaypointOnly) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* plain = oo::MadeEntity(oo::makeShared<Entity>());
    EXPECT_TRUE(!oo::EntityIsBeacon(plain) && !oo::BeaconCode(plain).has_value());
    EXPECT_TRUE(oo::NextBeacon(plain) == nullptr);
  }
}

// The binding's category: what the engine asks a waypoint's object for by
// selector is what OOJSWaypoint.mm answers.
TEST(OOWaypointEntity, jsExtensions) {
  {
    oo::AutoreleaseScope scope;
    ::SetUp();
    Entity* wp = Waypoint("W");
    RegisterJSClasses(InitOOJSWaypoint);
    const JSClassID jsClass = OOJSEntityJSClassID(wp);
    const JSClassID expectedClass = JSWaypointClassID();
    EXPECT_TRUE(jsClass != 0 && jsClass == expectedClass);
    EXPECT_TRUE(OOJSEntityJSClassName(wp) ==
             std::optional<std::string>("Waypoint"));
    EXPECT_TRUE(OOJSEntityIsVisibleToScripts(wp) == true);
  }
}
