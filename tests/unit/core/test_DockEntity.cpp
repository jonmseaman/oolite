/*	test_DockEntity.mm
        Unit tests for DockEntity (src/Core/Entities/DockEntity.h), a station's
   dock: the class shell, with the dock's state and its flags, geometry and
   lifecycle. The dock is made in C++ (its object is the ship's facade) and the
   cases call its members.

        As test_StationEntity's, the dock's object needs the game graph, so the
   test links the whole game but main (['*']) and uses a Universe that was never
   initialised and a plain entity as PLAYER (answering the one player query a
   dock makes when it aborts its dockings). A dock's station is a StationEntity
   whose virtual dock only records its definition, with the dock added as its
   subentity. Run: bash meson test -C build/meson_test test_DockEntity
*/

#include <cmath>
#include <numbers>
#include <string>

#include <gtest/gtest.h>

#include "DockEntity.h"
#include "EntityManualRefs.h"  // the tests' manual entity references
#include "PlayerEntity.h"
#include "StationEntity.h"
#include "Universe.h"
#include "oofnd/AutoreleaseScope.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

class PlayerEntity;
extern PlayerEntity* gOOPlayer;
extern Universe* gSharedUniverse;
extern JSContext* gOOJSMainThreadContext;

// PLAYER: docked at no station.
class TestDockPlayer : public PlayerEntity {
 public:
  HPVector viewpointPosition() override { return kZeroHPVector; }
  ::StationEntity* getTargetDockStation() override { return nullptr; }
};

// [[TestPlayer alloc] init]: a C++ player under the ship's facade, as
// PlayerEntity::sharedPlayer() makes the game's, retained (+1) as +alloc's
// object was.
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::RetainEntity(oo::MadeEntity(player));
  }
  return player.get();
}

// A station whose virtual dock is not made (the dock is a shipdata entry). A
// C++ subclass.
class TestDockStation : public StationEntity {
 public:
  bool setUpOneStandardSubentity(const oo::PList& /*subentDict*/,
                                 bool /*asTurret*/) override {
    return true;
  }
};

namespace {

void SetUp() {
  static Universe* universe = nullptr;
  if (universe == nullptr) {
    universe = new Universe();  // never released
  }
  gSharedUniverse = universe;
  static TestDockPlayer* player = nullptr;
  if (player == nullptr) player = NewTestPlayer<TestDockPlayer>();
  gOOPlayer = player;
  // The ship's -dealloc sends its (absent) scripts entityDestroyed in a request
  // on the main thread's context, so there is one, with nothing in it.
  if (gOOJSMainThreadContext == nullptr)
    gOOJSMainThreadContext = JS_NewContext(JS_NewRuntime());
}

// Unpiloted (a crewed ship would ask the universe for a pilot).
oo::PList Definition() {
  return oo::PList(oo::PList::Dict{{"unpiloted", oo::PList(true)}});
}

// [[[DockEntity alloc] initWithKey:definition:] autorelease]: the dock
// oo::RetainedEntity(DockEntity::newDockObject()) makes, its object (the ship's facade)
// autoreleased.
DockEntity* MakeDock(const std::string& key) {
  ::ShipEntity* ship = oo::RetainedEntity(DockEntity::newDockObject(key, Definition()));
  oo::AutoreleaseEntity(ship);
  return static_cast<DockEntity*>(ship);
}

// A dock that is a subentity of a station, so its parent is the station.
DockEntity* MakeDockOfStation(TestDockStation** outStation) {
  // [[[TestDockStation alloc] initWithKey:definition:] autorelease]: made
  // as oo::RetainedEntity(StationEntity::newStationObject()) makes a station, its object (the
  // ship's facade) autoreleased.
  ::ShipEntity* object = oo::ToShip(oo::autorelease(oo::NewShipObject(
      oo::makeShared<TestDockStation>(), "dock-station", Definition())));
  TestDockStation* station = static_cast<TestDockStation*>(object);
  station->initStationDefaults();
  DockEntity* dock = MakeDock("dock");
  if (object != nullptr) object->addSubEntity(dock);
  *outStation = station;
  return dock;
}

bool Near(Vector a, Vector b) {
  return std::fabs(a.x - b.x) < 1e-5 && std::fabs(a.y - b.y) < 1e-5 &&
         std::fabs(a.z - b.z) < 1e-5;
}

BoundingBox Box(float width, float height) {
  BoundingBox bb;
  bb.min = make_vector(0, 0, 0);
  bb.max = make_vector(width, height, 1);
  return bb;
}

}  // namespace

// --- The class shell
// --------------------------------------------------------

// The initialiser and the set-up from the definition: a dock is a ship that is
// not a station, open for docking and launching, with empty queues.
TEST(DockEntity, initAndSetUp) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("dock");
    EXPECT_TRUE(dock != nullptr);
    EXPECT_TRUE((dock != nullptr ? dock->isDock() : false) &&
             (dock != nullptr ? dock->getIsShip() : false) &&
             !(dock != nullptr ? dock->getIsStation() : false) &&
             !(dock != nullptr ? dock->getIsPlayer() : false));
    EXPECT_TRUE((dock != nullptr ? dock->shipDataKey()
                              : std::optional<std::string>()) ==
             std::optional<std::string>("dock"));
    EXPECT_TRUE((dock != nullptr ? dock->allowsDocking() : false) &&
             (dock != nullptr ? dock->allowsLaunching() : false) &&
             !(dock != nullptr ? dock->disallowedDockingCollides() : false));
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInDockingQueue() : 0) == 0 &&
             (dock != nullptr ? dock->countOfShipsInLaunchQueue() : 0) == 0);
    EXPECT_TRUE((dock != nullptr ? dock->parentEntity() : (Entity*)nullptr) ==
             nullptr);
  }
}

// The flags: closing to docking or launching aborts what is queued (nothing
// here).
TEST(DockEntity, flags) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("flags");
    if (dock != nullptr) dock->setAllowsDocking(false);
    EXPECT_TRUE(!(dock != nullptr ? dock->allowsDocking() : false));
    if (dock != nullptr) dock->setAllowsDocking(true);
    EXPECT_TRUE((dock != nullptr ? dock->allowsDocking() : false));
    if (dock != nullptr) dock->setAllowsLaunching(false);
    EXPECT_TRUE(!(dock != nullptr ? dock->allowsLaunching() : false));
    if (dock != nullptr) dock->setAllowsLaunching(true);
    EXPECT_TRUE((dock != nullptr ? dock->allowsLaunching() : false));
    if (dock != nullptr) dock->setDisallowedDockingCollides(true);
    EXPECT_TRUE((dock != nullptr ? dock->disallowedDockingCollides() : false));
    if (dock != nullptr) dock->clear();
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInDockingQueue() : 0) == 0 &&
             (dock != nullptr ? dock->countOfShipsInLaunchQueue() : 0) == 0);
  }
}

// Off centre: away from the station's axis, or not facing along it.
TEST(DockEntity, isOffCentre) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("centre");
    EXPECT_TRUE(!(dock != nullptr ? dock->isOffCentre() : false));
    if (dock != nullptr) dock->setPosition(make_HPvector(0, 0, 500));
    EXPECT_TRUE(!(dock != nullptr ? dock->isOffCentre() : false));
    if (dock != nullptr) dock->setPosition(make_HPvector(3, 3, 0));
    EXPECT_TRUE((dock != nullptr ? dock->isOffCentre() : false));
    if (dock != nullptr) dock->setPosition(kZeroHPVector);
    Quaternion q = kIdentityQuaternion;
    quaternion_rotate_about_y(&q, std::numbers::pi / 2);
    if (dock != nullptr) dock->setOrientation(q);
    EXPECT_TRUE((dock != nullptr ? dock->isOffCentre() : false));
  }
}

// The geometry from the station: a virtual dock takes the station's virtual
// port; the port's up vector turns with a ship whose box is the other way
// round.
TEST(DockEntity, dimensionsAndPortUpVector) {
  {
    ::SetUp();
    TestDockStation* station = nullptr;
    DockEntity* dock = MakeDockOfStation(&station);
    EXPECT_TRUE((dock != nullptr ? dock->parentEntity() : (Entity*)nullptr) ==
             station);

    if (dock != nullptr) dock->setVirtual();
    if (dock != nullptr) dock->setDimensionsAndCorridor(false, true, false);
    EXPECT_TRUE(!(dock != nullptr ? dock->allowsDocking() : false) &&
             (dock != nullptr ? dock->disallowedDockingCollides() : false) &&
             !(dock != nullptr ? dock->allowsLaunching() : false));

    // The virtual port is 69 x 69: square, so only the ship's box decides.
    EXPECT_TRUE(Near(
        (dock != nullptr ? dock->portUpVectorForShipsBoundingBox(Box(20, 10))
                         : Vector{}),
        make_vector(0, 1, 0)));
    EXPECT_TRUE(Near(
        (dock != nullptr ? dock->portUpVectorForShipsBoundingBox(Box(10, 20))
                         : Vector{}),
        make_vector(1, 0, 0)));

    if (dock != nullptr) dock->setDimensionsAndCorridor(true, false, true);
    EXPECT_TRUE((dock != nullptr ? dock->allowsDocking() : false) &&
             !(dock != nullptr ? dock->disallowedDockingCollides() : false) &&
             (dock != nullptr ? dock->allowsLaunching() : false));
  }
}

// A virtual dock takes no damage.
TEST(DockEntity, virtualDockTakesNoDamage) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("virtual");
    if (dock != nullptr) dock->setVirtual();
    const double energy = (dock != nullptr ? dock->getEnergy() : 0.0f);
    if (dock != nullptr) dock->takeEnergyDamage(50.0, nullptr, nullptr, "");
    EXPECT_TRUE((dock != nullptr ? dock->getEnergy() : 0.0f) == energy);
    if (dock != nullptr)
      dock->noteTakingDamage(50.0, nullptr, oo::ShipDamageType::kEnergy);
    EXPECT_TRUE((dock != nullptr ? dock->getEnergy() : 0.0f) == energy);
    if (dock != nullptr)
      dock->drawImmediate(
          false, false);  // not drawn: nothing to check but that it returns
  }
}

// --- Docking guidance
// --------------------------------------------------- These cases reach only
// the approach queue and the dock's flags: no player, no ship in the universe,
// no clock.

namespace {

// A ship with the universal ID the approach queue keys it by.
ShipEntity* MakeQueuedShip(oo::UniversalID shipID) {
  ShipEntity* ship = oo::ToShip(oo::autorelease(oo::NewShipObject(oo::makeShared<::ShipEntity>(), "queued", Definition())));
  ship->universalID = shipID;
  return ship;
}

}  // namespace

// A dock that is closed rejects a ship outright, before it asks the ship or its
// script anything (the later tests of an open dock call the script engine,
// which this test does not start).
TEST(DockEntity, canAcceptShipForDocking) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("accept");
    ShipEntity* ship = MakeQueuedShip(11);
    if (dock != nullptr) dock->setAllowsDocking(false);
    EXPECT_TRUE((dock != nullptr ? dock->canAcceptShipForDocking(ship)
                              : std::optional<std::string>()) ==
             std::optional<std::string>("DOCK_CLOSED"));
    EXPECT_TRUE(dock->canAcceptShipForDocking(ship) ==
             std::optional<std::string>("DOCK_CLOSED"));
  }
}

// The approach queue: a ship is in it by its ID, and aborting its docking takes
// it out again.
TEST(DockEntity, approachQueue) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("queue");
    ShipEntity* ship = MakeQueuedShip(12);
    EXPECT_TRUE(!(dock != nullptr ? dock->shipIsInDockingQueue(nullptr) : false) &&
             !(dock != nullptr ? dock->shipIsInDockingQueue(ship) : false));
    dock->shipsOnApproach[12] = std::vector<oo::PList>();
    EXPECT_TRUE((dock != nullptr ? dock->shipIsInDockingQueue(ship) : false));
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInDockingQueue() : 0) == 1);
    if (dock != nullptr) dock->abortDockingForShip(ship);
    EXPECT_TRUE(!(dock != nullptr ? dock->shipIsInDockingQueue(ship) : false));
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInDockingQueue() : 0) == 0);
  }
}

// Docking instructions for no ship are none.
TEST(DockEntity, dockingInstructionsForNoShip) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("instructions");
    EXPECT_TRUE((dock != nullptr ? dock->dockingInstructionsForShip(nullptr)
                              : oo::PList())
                 .isNull());
  }
}

// Nothing in the queue: nothing to pull in, and the queue stays empty. The C++
// part does the same.
TEST(DockEntity, autoDockEmptyQueue) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("autodock");
    if (dock != nullptr) dock->autoDockShipsOnApproach();
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInDockingQueue() : 0) == 0);
    DockEntity* part = dock;
    part->shipsOnApproach[13] = std::vector<oo::PList>();
    part->autoDockShipsOnApproach();  // no ship has ID 13 in the (never
                                      // initialised) universe's lookup: dropped
    EXPECT_TRUE(part->countOfShipsInDockingQueue() == 0);
  }
}

// --- The docking corridor and launching
// ----------------------------- These cases reach only the launch queue and the
// checks that refuse a non-ship: no player, no script engine, no ship in the
// universe.

// Not a ship: not in the corridor, not allowed to launch.
TEST(DockEntity, corridorAndLaunchRefuseNoShip) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("refuse");
    EXPECT_TRUE(
        !(dock != nullptr ? dock->shipIsInDockingCorridor(nullptr) : false));
    EXPECT_TRUE(!(dock != nullptr ? dock->allowsLaunchingOf(nullptr) : false));
    EXPECT_TRUE(!dock->shipIsInDockingCorridor(nullptr));
    EXPECT_TRUE(!dock->allowsLaunchingOf(nullptr));
  }
}

// The launch queue: a ship joins at the back, or at the front with priority;
// nothing joins for no ship; aborting the launches empties it.
TEST(DockEntity, launchQueue) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("launchqueue");
    ShipEntity* first = MakeQueuedShip(21);
    ShipEntity* second = MakeQueuedShip(22);
    if (dock != nullptr) dock->addShipToLaunchQueue(nullptr, false);
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInLaunchQueue() : 0) == 0);
    if (dock != nullptr) dock->addShipToLaunchQueue(first, false);
    dock->addShipToLaunchQueue(second, true);
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInLaunchQueue() : 0) == 2);
    DockEntity* part = dock;
    EXPECT_TRUE(part->launchQueue[0].get() == second &&
             part->launchQueue[1].get() ==
                 first);  // the queue holds the C++ ships
    EXPECT_TRUE((first != nullptr ? first->status() : oo::EntityStatus{}) ==
                 oo::EntityStatus::kDocked &&
             (second != nullptr ? second->status() : oo::EntityStatus{}) ==
                 oo::EntityStatus::kDocked);
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInLaunchQueueWithPrimaryRole(
                                    "no-such-role")
                              : 0) == 0);
    part->no_docking_while_launching = true;
    if (dock != nullptr) dock->abortAllLaunches();
    EXPECT_TRUE((dock != nullptr ? dock->countOfShipsInLaunchQueue() : 0) == 0 &&
             !part->no_docking_while_launching);
  }
}

// --- The crossing (after the conversion)
// ---------------------------------------------------------

// A dock's object holds its C++ part, a DockEntity, and from C++ the entity's
// virtual members reach the dock's.
TEST(DockEntity, objCDockPartIsADock) {
  {
    ::SetUp();
    DockEntity* dock = MakeDock("crossing");
    DockEntity* part = dock;
    EXPECT_TRUE(part != nullptr);
    EXPECT_TRUE(oo::ToDock(part) == part);

    Entity* asEntity = part;
    EXPECT_TRUE(asEntity->isDock());
    EXPECT_TRUE(part->allowsDocking() && part->allowsLaunching() &&
             !part->disallowedDockingCollides());
    part->setAllowsDocking(false);
    EXPECT_TRUE(!(dock != nullptr ? dock->allowsDocking() : false));
    EXPECT_TRUE(part->countOfShipsInDockingQueue() == 0 &&
             part->countOfShipsInLaunchQueue() == 0);

    part->setVirtual();
    EXPECT_TRUE(part->virtual_dock);
    const double energy = (dock != nullptr ? dock->getEnergy() : 0.0f);
    asEntity->takeEnergyDamage(10.0, nullptr, nullptr, "");
    EXPECT_TRUE((dock != nullptr ? dock->getEnergy() : 0.0f) == energy);
  }
}
