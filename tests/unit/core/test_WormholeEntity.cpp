/*	test_WormholeEntity.mm
        Unit tests for WormholeEntity (src/Core/Entities/WormholeEntity.h), the
   witchspace wormholes ships leave behind.

        The entity reads Universe and PLAYER, so the test links the whole game
   but main (['*']) and uses a Universe that was never initialised, subclassed
   to answer the current system, the system names and a real system description
   manager (with the coordinates of the systems the test uses) and the clock
   format, with no sun, and to record what is removed; PLAYER is an entity that
   answers the galaxy, the clock, its galactic coordinates and its forward
   vector; the ship that opens a wormhole is a plain entity of the mass the test
   sets (a C++ ship under its object, never set up). A wormhole made by a ship
   is a no-mass-yet effect of the wormhole scan class whose size, expiry and
   arrival follow from the ship's mass and the distance; one made from a saved
        dictionary reads its systems, times (crossed times fixed), position and
   misjump back, and -getDict writes them; the misjump setters move the exit and
   the arrival; the scan state; who can collide with it; -update: shrinks
   nothing at no mass, hides it and removes it once it has expired; -suckInShip:
   refuses a missing ship, one already leaving, another system and an expired
        wormhole; -disgorgeShips moves a wormhole that held the player behind
   the player; and the description. Masses and distances are set through the one
   helper below. The cases make the C++ wormhole and ask the C++ class, and the
   facade case pins the crossing: the wormhole is the root Entity. Run: bash
   tools/check-core-tests.sh
*/

#include <cmath>
#include <numbers>
#include <string>

#include <gtest/gtest.h>

#include "EntityOOJavaScriptExtensions.h"
#include "OOJSEntity.h"
#include "OOJSWormhole.h"
#include "SystemDescriptionManager.h"
#include "PlayerEntity.h"  // PLAYER is C++
#include "Universe.h"
#include "WormholeEntity.h"
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
  double _clock = {};
  NSPoint _coordinates = {};

  HPVector viewpointPosition() override { return kZeroHPVector; }
  oo::GalaxyID galaxyNumber() override { return 0; }
  double clockTime() override { return _clock; }
  double clockTimeAdjusted() override { return _clock; }
  NSPoint getGalaxy_coordinates() override { return _coordinates; }
  Vector forwardVector() override { return make_vector(0, 0, 1); }
};

// A C++ player, as PlayerEntity::sharedPlayer() makes
// the game's, retained (+1).
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::RetainEntity(oo::MadeEntity(player));
  }
  return player.get();
}

// An effect, for the collision test (a C++ subclass overriding the member).
class TestEffect : public Entity {
 public:
  bool isEffect() override { return true; }
};

// UNIVERSE: never initialised; answers the system and the system manager, and
// records removals.
class TestUniverse : public Universe {
 public:
  oo::SystemID& _system =
      systemID;  // the universe's own state, which its getter answers
  Entity* _removed = {};

  std::optional<std::string> getSystemName(oo::SystemID sys,
                                           oo::GalaxyID /*gnum*/) override {
    return "System " + std::to_string(sys);
  }  // getSystemName(sys) asks it
  bool removeEntity(Entity* entity) override {
    _removed = entity;
    return true;
  }
};

extern JSContext*
    gOOJSMainThreadContext;  // the engine's (OOJavaScriptEngine.mm)

namespace {

oo::SystemDescriptionManager* sManager = nullptr;

}

namespace {

TestUniverse* sUniverse = nullptr;
TestPlayer* sPlayer = nullptr;

void SetUp() {
  if (sUniverse == nullptr) {
    sUniverse = new TestUniverse();  // never released
    sPlayer = NewTestPlayer<TestPlayer>();
    // Systems 7 at (10, 20) and 9 at (13, 24) of galaxy 0: 5 light-year units
    // apart, 2.0 LY.
    std::shared_ptr<oo::SystemDescriptionManager> manager =
        oo::makeShared<oo::SystemDescriptionManager>();
    manager->setProperties(
        oo::PList(oo::PList::Dict{{"coordinates", oo::PList("10 20")}}), "0 7");
    manager->setProperties(
        oo::PList(oo::PList::Dict{{"coordinates", oo::PList("13 24")}}), "0 9");
    manager->buildRouteCache();    // fills the coordinates cache
    sManager = (*new std::shared_ptr<oo::SystemDescriptionManager>(std::move(manager)))
                   .get();  // never released
    // The universe's own manager and descriptions (the game's clock format, for
    // the description's arrival time).
    sUniverse->systemManager = oo::share(sManager);
    sUniverse->_descriptions = oo::PList(
        oo::PList::Dict{{"clock-format", oo::PList("%07d:%02d:%02d:%02d")}});
  }
  sUniverse->_system = 7;
  sUniverse->_removed = nullptr;
  sPlayer->_clock = 1000.0;
  sPlayer->_coordinates = NSMakePoint(10, 20);
  gSharedUniverse = sUniverse;
  gOOPlayer = sPlayer;
  // The ship's -dealloc sends its (absent) scripts entityDestroyed in a request
  // on the main thread's context, so there is one, with nothing in it.
  if (gOOJSMainThreadContext == nullptr)
    gOOJSMainThreadContext = JS_NewContext(JS_NewRuntime());
}

// --- Ivars the test sets and reads, and nothing else
// ----------------------------------------------

void SetMass(Entity* e, GLfloat mass) { e->mass = mass; }

// --------------------------------------------------------------------------------------------------

bool Near(double a, double b) {
  return std::fabs(a - b) < 1e-3 * std::fmax(1.0, std::fabs(b));
}

// A ship: C++ (the wormhole calls its members), never set up.
Entity* Ship(GLfloat mass) {
  Entity* ship = oo::MadeEntity(oo::makeShared<::ShipEntity>());
  SetMass(ship, mass);
  ship->setPosition(make_HPvector(1, 2, 3));
  return ship;
}

// A new C++ wormhole.
WormholeEntity* ToNine(Entity* ship) {
  std::shared_ptr<WormholeEntity> wh = oo::makeShared<WormholeEntity>();
  (void)oo::MadeEntity(wh);
  wh->initWormholeTo(9, oo::ToShip(ship));
  return wh.get();
}

WormholeEntity* FromDict(const oo::PList& dict) {
  std::shared_ptr<WormholeEntity> wh = oo::makeShared<WormholeEntity>();
  (void)oo::MadeEntity(wh);
  wh->initWithDict(dict);
  return wh.get();
}

oo::PList Saved(double expiry = 1500.0) {
  return oo::PList(oo::PList::Dict{
      {"origin_id", oo::PList(7)},
      {"dest_id", oo::PList(9)},
      {"expiry_time", oo::PList(expiry)},
      {"arrival_time", oo::PList(2000.0)},
      {"position", oo::PList(oo::PList::Array{oo::PList(4.0), oo::PList(5.0),
                                              oo::PList(6.0)})},
      {"misjump", oo::PList(true)},
  });
}

}  // namespace

TEST(WormholeEntity, madeByShip) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    EXPECT_TRUE(wh != nullptr &&
             dynamic_cast<WormholeEntity*>(static_cast<Entity*>(wh)) !=
                 nullptr &&
             wh->getIsWormhole());
    EXPECT_TRUE(wh->status() == oo::EntityStatus::kEffect &&
             wh->getScanClass() == oo::ScanClass::kWormhole);
    EXPECT_TRUE(wh->getOrigin() == 7 && wh->getDestination() == 9);
    NSPoint o = wh->originCoordinates(), d = wh->destinationCoordinates();
    EXPECT_TRUE(o.x == 10 && o.y == 20 && d.x == 13 && d.y == 24);
    EXPECT_TRUE(HPvector_equal(wh->getPosition(), make_HPvector(1, 2, 3)));
    // No player mass (not the player): 200000 tonnes.
    EXPECT_TRUE(Near(wh->collisionRadius(),
                  0.5 * std::numbers::pi * std::pow(200000.0, 1.0 / 3.0)));
    const double distance = distanceBetweenPlanetPositions(10, 20, 13, 24);
    EXPECT_TRUE(Near(wh->travelTime(), distance * distance * 3600));
    EXPECT_TRUE(Near(wh->arrivalTime(), 1000.0 + distance * distance * 3600));
    EXPECT_TRUE(wh->estimatedArrivalTime() == wh->arrivalTime());
    EXPECT_TRUE(Near(wh->expiryTime(), 1000.0 + 200000.0 / WORMHOLE_SHRINK_RATE));
    EXPECT_TRUE(!wh->withMisjump() && !wh->isScanned() &&
             wh->scanInfo() == WORMHOLE_SCANINFO::kNone);
    EXPECT_TRUE(wh->exitSpeed() == 50.0);
    EXPECT_TRUE(wh->canCollide());
    EXPECT_TRUE(wh->getShipsInTransit() == oo::PList(oo::PList::Array{}));
  }
}

TEST(WormholeEntity, misjump) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    const double arrival = wh->arrivalTime();
    const double distance = distanceBetweenPlanetPositions(10, 20, 13, 24);
    wh->setMisjump();
    EXPECT_TRUE(wh->withMisjump());
    EXPECT_TRUE(Near(wh->arrivalTime(), arrival - distance * distance * 900));
    NSPoint d = wh->destinationCoordinates();
    EXPECT_TRUE(d.x == 11.5 && d.y == 22);
    wh->setMisjump();  // only once
    EXPECT_TRUE(Near(wh->arrivalTime(), arrival - distance * distance * 900));

    WormholeEntity* ranged = ToNine(Ship(1000.0f));
    ranged->setMisjumpWithRange(0.25f);
    EXPECT_TRUE(ranged->withMisjump() && ranged->misjumpRange() == 0.25f);
    EXPECT_TRUE(Near(ranged->arrivalTime(),
                  arrival - (distance * 0.75) * (distance * 0.75) * 3600.0));
    d = ranged->destinationCoordinates();
    EXPECT_TRUE(Near(d.x, 10 * 0.75 + 13 * 0.25) &&
             Near(d.y, 20 * 0.75 + 24 * 0.25));

    WormholeEntity* unsafe = ToNine(Ship(1000.0f));
    unsafe->setMisjumpWithRange(2.0f);
    EXPECT_TRUE(unsafe->misjumpRange() == 0.5f);
  }
}

TEST(WormholeEntity, saved) {
  {
    ::SetUp();
    WormholeEntity* wh = FromDict(Saved());
    EXPECT_TRUE(wh->getOrigin() == 7 && wh->getDestination() == 9);
    EXPECT_TRUE(wh->expiryTime() == 1500.0 && wh->arrivalTime() == 2000.0 &&
             wh->estimatedArrivalTime() == 2000.0);
    EXPECT_TRUE(HPvector_equal(wh->getPosition(), make_HPvector(4, 5, 6)));
    EXPECT_TRUE(wh->withMisjump() && wh->isScanned() &&
             wh->scanInfo() == WORMHOLE_SCANINFO::kScanned);
    EXPECT_TRUE(!wh->canCollide());  // no mass

    oo::PList dict = wh->getDict();
    EXPECT_TRUE(dict.get<int>("origin_id") == 7 && dict.get<int>("dest_id") == 9);
    EXPECT_TRUE(dict.get<double>("expiry_time") == 1500.0 &&
             dict.get<double>("arrival_time") == 2000.0);
    EXPECT_TRUE(dict.get<bool>("misjump"));
    EXPECT_TRUE(dict.get<std::string>("origin_coords") == "10.000000 20.000000" &&
             dict.get<std::string>("dest_coords") == "13.000000 24.000000");
    EXPECT_TRUE(dict.find("ships") != nullptr && dict.find("ships")->count() == 0);

    // Crossed times: the expiry is a second before the arrival.
    WormholeEntity* fixed = FromDict(Saved(2500.0));
    EXPECT_TRUE(fixed->expiryTime() == 1999.0);
  }
}

TEST(WormholeEntity, scanning) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    wh->setScannedAt(1234.0);
    EXPECT_TRUE(wh->isScanned() && wh->scanTime() == 1234.0 &&
             wh->scanInfo() == WORMHOLE_SCANINFO::kScanned);
    wh->setScannedAt(5678.0);
    EXPECT_TRUE(wh->scanTime() == 1234.0);
    wh->setScanInfo(WORMHOLE_SCANINFO::kDestination);
    EXPECT_TRUE(wh->scanInfo() == WORMHOLE_SCANINFO::kDestination);
    wh->setExitSpeed(75.0);
    EXPECT_TRUE(wh->exitSpeed() == 75.0);
  }
}

TEST(WormholeEntity, collisions) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    EXPECT_TRUE(wh->canCollide());
    sUniverse->_system = 8;
    EXPECT_TRUE(!wh->canCollide());
    sUniverse->_system = 7;
    sPlayer->_coordinates = NSMakePoint(11, 20);
    EXPECT_TRUE(!wh->canCollide());
    sPlayer->_coordinates = NSMakePoint(10, 20);

    EXPECT_TRUE(wh->checkCloseCollisionWith(Ship(1.0f)));
    EXPECT_TRUE(!wh->checkCloseCollisionWith(
        oo::MadeEntity(oo::makeShared<TestEffect>())));
  }
}

TEST(WormholeEntity, update) {
  {
    ::SetUp();
    WormholeEntity* saved = FromDict(Saved());
    saved->update(0.1);
    EXPECT_TRUE(saved->getScanClass() == oo::ScanClass::kNoDraw);  // no mass
    EXPECT_TRUE(sUniverse->_removed == nullptr);
    sPlayer->_clock = 1600.0;  // past its expiry
    saved->update(0.1);
    EXPECT_TRUE(sUniverse->_removed == saved);

    sPlayer->_clock = 1000.0;
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    wh->update(1.0);
    EXPECT_TRUE(wh->getScanClass() == oo::ScanClass::kWormhole);
    EXPECT_TRUE(Near(
        wh->collisionRadius(),
        0.5 * std::numbers::pi * std::pow(200000.0 - WORMHOLE_SHRINK_RATE, 1.0 / 3.0)));
  }
}

TEST(WormholeEntity, suckInRefused) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    EXPECT_TRUE(!wh->suckInShip(nullptr));
    Entity* leaving = Ship(10.0f);
    leaving->setStatus(oo::EntityStatus::kEnteringWitchspace);
    EXPECT_TRUE(!wh->suckInShip(oo::ToShip(leaving)));
    sUniverse->_system = 8;
    EXPECT_TRUE(!wh->suckInShip(oo::ToShip(Ship(10.0f))));
    sUniverse->_system = 7;
    sPlayer->_clock =
        1000.0 + 200000.0 / WORMHOLE_SHRINK_RATE + 1.0;  // expired
    EXPECT_TRUE(!wh->suckInShip(oo::ToShip(Ship(10.0f))));
    EXPECT_TRUE(wh->getShipsInTransit() == oo::PList(oo::PList::Array{}));
  }
}

TEST(WormholeEntity, disgorgeWithPlayer) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    sPlayer->setPosition(make_HPvector(100, 0, 0));
    wh->disgorgeShips();  // no ships, no player: it stays
    EXPECT_TRUE(HPvector_equal(wh->getPosition(), make_HPvector(1, 2, 3)));
    wh->setContainsPlayer(true);
    wh->disgorgeShips();
    EXPECT_TRUE(HPvector_equal(wh->getPosition(), make_HPvector(100, 0, -500)));
    wh->setExitPosition(make_HPvector(7, 7, 7));
    wh->disgorgeShips();  // once only
    EXPECT_TRUE(HPvector_equal(wh->getPosition(), make_HPvector(7, 7, 7)));
  }
}

TEST(WormholeEntity, description) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    const std::string description = oo::EntityDescription(wh);
    EXPECT_TRUE(description.find(
                 "destination: System 9 ttl: 50.00s arrival: 0000000:") !=
             std::string::npos);
    wh->setMisjump();
    EXPECT_TRUE(oo::EntityDescription(wh).find(
                 "destination: Interstellar Space") != std::string::npos);
  }
}

TEST(WormholeEntity, facade) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    Entity* object = wh;
    EXPECT_TRUE(object != nullptr);  // A C++ entity.
    EXPECT_TRUE(dynamic_cast<WormholeEntity*>(object) == wh);
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
// the engine asks a WormholeEntity for by selector is what OOJSWormhole.mm
// answers.
TEST(WormholeEntity, jsExtensions) {
  {
    ::SetUp();
    WormholeEntity* wh = ToNine(Ship(1000.0f));
    RegisterJSClasses(InitOOJSWormhole);
    Entity* object = wh;  // what the engine asks
    const JSClassID jsClass = OOJSEntityJSClassID(object);
    const JSClassID expectedClass = JSWormholeClassID();
    EXPECT_TRUE(jsClass != 0 && jsClass == expectedClass);
    EXPECT_TRUE(OOJSEntityJSClassName(object) ==
             std::optional<std::string>("Wormhole"));
    EXPECT_TRUE(OOJSEntityIsVisibleToScripts(object) == true);
  }
}
