/*	test_Entity.mm
        Unit tests for Entity (src/Core/Entities/Entity.h), the root of the
   entity hierarchy.

        Entity's object reads Universe and PlayerEntity, so the test links the
   whole game but main (tests/unit/core/meson.build entry ['*']) and defines
   the one global main.mm did, gDebugFlags. UNIVERSE is a Universe that was
   never initialised (zeroed by the runtime: session 0, time 0, no entities),
   whose ivars the test sets where a case needs them, and PLAYER is a plain
   entity that answers -viewpointPosition. The tests cover the defaults an
   entity starts with, the accessors and what they derive (camera ranges,
   speed, matrices), movement and rotation, -update: in the cockpit, in flight
   and as a subentity, owners (weak) and parents, the position-sorted linked
   lists, the session check, the shader bindings, the description, and which
   methods reach a subclass's overrides. Ivars the game reads directly (the
   lists, the motion flags) are read through the small block of helpers below,
   the one place that knows where they live. The tests after the first ones
   pin the crossing, as test_OODrawable.mm does: a C++ entity and the
   dump-state lines. Run: tests/run.sh unit
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "Entity.h"
#include "EntityManualRefs.h"  // the tests' manual entity references
#include "DebugFlags.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Log.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

class PlayerEntity;
extern PlayerEntity* gOOPlayer;
extern Universe* gSharedUniverse;

// PLAYER: a player whose viewpoint is set by the test (PlayerEntity's is its
// position plus an offset).
class TestPlayer : public PlayerEntity {
 public:
  HPVector _viewpoint = kZeroHPVector;

  HPVector viewpointPosition() override { return _viewpoint; }
};

// A C++ player made as PlayerEntity::sharedPlayer() makes the game's,
// retained (+1) as +alloc's object was.
template <class T>
T* NewTestPlayer() {
  std::shared_ptr<T> player = oo::makeShared<T>();
  {
    oo::AutoreleaseScope scope;
    oo::RetainEntity(oo::MadeEntity(player));
  }
  return player.get();
}

// A subclass whose overrides count, so the test sees which of the root's
// members reach it: a C++ subclass overriding the virtual members, with the
// same counts. Global and keeping its name, which the expected log and dump
// texts print.
class TestObjCEntity : public Entity {
 public:
  bool isPlanet() override { return true; }
  bool canCollide() override { return false; }
  GLfloat frustumRadius() override { return _frustumRadius; }
  std::size_t sessionID() override { return _session; }
  bool isVisible() override { return _visible; }
  void updateCameraRelativePosition() override {
    _cameraUpdates++;
    Entity::updateCameraRelativePosition();
  }
  void orientationChanged() override {
    _orientationChanges++;
    Entity::orientationChanged();
  }
  void dumpSelfState() override {
    Entity::dumpSelfState();
    _selfStateDumps++;
  }
  void drawImmediate(bool, bool) override { _draws++; }

  int _cameraUpdates = 0;
  int _orientationChanges = 0;
  int _selfStateDumps = 0;
  int _draws = 0;
  GLfloat _frustumRadius = 0;
  std::size_t _session = 0;
  bool _visible = false;
};

// A converted entity: a C++ subclass. Global, as a game class is, so that its
// description names it as the game's would.
class TestCxxEntity : public Entity {
 public:
  bool isPlanet() override { return true; }
  GLfloat frustumRadius() override { return 2.0f; }
  void updateCameraRelativePosition() override {
    cameraUpdates++;
    Entity::updateCameraRelativePosition();
  }
  std::optional<std::string> descriptionComponents() const override {
    return "test";
  }

  int cameraUpdates = 0;
};

namespace {

// --- Ivars the game reads directly (ent->x_next, ent->hasMoved), and nothing
// else -----------------

Entity* XPrevious(Entity* e) { return e->x_previous; }
Entity* XNext(Entity* e) { return e->x_next; }
Entity* YNext(Entity* e) { return e->y_next; }
Entity* ZNext(Entity* e) { return e->z_next; }
bool HasMoved(Entity* e) { return e->hasMoved; }
bool HasRotated(Entity* e) { return e->hasRotated; }
void SetSubEntity(Entity* e, bool value) { e->isSubEntity = value; }
void SetIsShip(Entity* e, bool value) { e->isShip = value; }
GLfloat NoDrawDistance(Entity* e) { return e->no_draw_distance; }
Vector CameraRelativePosition(Entity* e) { return e->cameraRelativePosition; }

// --------------------------------------------------------------------------------------------------

Universe* sUniverse = nullptr;

// What the entity logs: every class displayed, every line kept.
std::vector<std::string> sLog;

void CaptureLine(std::string_view line) {
  sLog.emplace_back(line);
  if (std::getenv("OO_TEST_ECHO_LOG") != nullptr)
    std::fprintf(stderr, "log: %s\n", sLog.back().c_str());
}

void CaptureLog() {
  oo::log::Logger& logger = oo::log::logger();
  logger.setInitialized(true);
  logger.setSink(CaptureLine);
  sLog.clear();
}

bool Logged(const std::string& text) {
  for (const std::string& line : sLog) {
    if (line.find(text) != std::string::npos) return true;
  }
  return false;
}

// Universe's ivars are members of its C++ part.
template <class T>
T& UniverseIvar(const char* name) {
  Universe* part = sUniverse;
  // Each name answers only the ivar's own type (GCC -Wstrict-aliasing on the
  // other cast).
  if constexpr (std::is_same_v<T, decltype(part->_sessionID)>) {
    if (std::strcmp(name, "_sessionID") == 0) return part->_sessionID;
  }
  if constexpr (std::is_same_v<T, decltype(part->universal_time)>) {
    if (std::strcmp(name, "universal_time") == 0) return part->universal_time;
  }
  std::abort();
}

// A Universe that was never initialised: the runtime zeroed it (session 0, time
// 0, no entities).
void SetUpUniverse() {
  if (sUniverse == nullptr) {
    sUniverse = new Universe();  // never released
  }
  gSharedUniverse = sUniverse;
  UniverseIvar<std::size_t>("_sessionID") = 0;
  UniverseIvar<oo::TimeAbsolute>("universal_time") = 0;
  sUniverse->n_entities = 0;
  sUniverse->x_list_start = sUniverse->y_list_start = sUniverse->z_list_start =
      nullptr;
}

TestPlayer* SetUpPlayer(HPVector position, HPVector viewpoint) {
  static TestPlayer* player = nullptr;
  if (player == nullptr)
    player =
        NewTestPlayer<TestPlayer>();  // never released, as the player is not
  gOOPlayer = player;  // before its position, which reads PLAYER (the debug
                       // build asserts it is set)
  player->setPosition(position);
  player->_viewpoint = viewpoint;
  return player;
}

Entity* MakeEntity(HPVector position, GLfloat radius) {
  Entity* entity = oo::MadeEntity(oo::makeShared<Entity>());
  entity->setPosition(position);
  entity->setCollisionRadius(radius);
  return entity;
}

bool VectorEqual(Vector a, Vector b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool Near(double a, double b) { return a - b < 1e-5 && b - a < 1e-5; }

bool QuaternionNear(Quaternion a, Quaternion b) {
  return Near(a.w, b.w) && Near(a.x, b.x) && Near(a.y, b.y) && Near(a.z, b.z);
}

}  // namespace

TEST(Entity, defaults) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);
    UniverseIvar<std::size_t>("_sessionID") = 7;
    UniverseIvar<oo::TimeAbsolute>("universal_time") = 12.5;

    Entity* entity = oo::MadeEntity(oo::makeShared<Entity>());
    EXPECT_TRUE(entity->sessionID() == 7);
    EXPECT_TRUE(entity->getSpawnTime() == 12.5f);
    EXPECT_TRUE(!entity->getIsShip() && !entity->isDock() &&
             !entity->getIsStation() && !entity->getIsSubEntity() &&
             !entity->getIsPlayer());
    EXPECT_TRUE(!entity->isPlanet() && !entity->isSun() &&
             !entity->isStellarObject() && !entity->isSky() &&
             !entity->getIsWormhole());
    EXPECT_TRUE(!entity->isEffect() && !entity->getIsVisualEffect() &&
             !entity->isWaypoint() && !entity->getIsImmuneToBreakPatternHide());
    EXPECT_TRUE(entity->getIsSunlit());
    EXPECT_TRUE(entity->status() == oo::EntityStatus::kCockpitDisplay &&
             entity->getScanClass() == oo::ScanClass::kNotSet);
    EXPECT_TRUE(!entity->isInSpace());
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), kZeroHPVector));
    EXPECT_TRUE(quaternion_equal(entity->getOrientation(), kIdentityQuaternion));
    EXPECT_TRUE(oo::MatrixEqual(entity->rotationMatrix(), kIdentityMatrix));
    EXPECT_TRUE(entity->getEnergy() == 0 && entity->getMaxEnergy() == 0 &&
             entity->getMass() == 0);
    EXPECT_TRUE(entity->collisionRadius() == 0 && entity->frustumRadius() == 0);
    EXPECT_TRUE(entity->getUniversalID() == NO_TARGET &&
             entity->getLastDrawCounter() == 0);
    EXPECT_TRUE(entity->owner() == nullptr && entity->parentEntity() == nullptr &&
             entity->rootShipEntity() == nullptr);
    EXPECT_TRUE(entity->getCollisionRegion() == nullptr);
    EXPECT_TRUE(entity->getCollidingEntities() != nullptr &&
             entity->getCollidingEntities()->empty());
    EXPECT_TRUE(entity->canCollide() && entity->isVisible() &&
             !entity->throwingSparks());
    EXPECT_TRUE(NoDrawDistance(entity) == 100000.0f);

    oo::Color* fog = entity->fogUniform().get();
    EXPECT_TRUE(fog != nullptr);
    EXPECT_TRUE(fog->redComponent() == 0 && fog->greenComponent() == 0 &&
             fog->blueComponent() == 0 && fog->alphaComponent() == 0);

    // The components %@ prints.
    EXPECT_TRUE(oo::EntityDescription(entity).ends_with(
        "{position: (0, 0, 0) scanClass: CLASS_NOT_SET status: "
        "STATUS_COCKPIT_DISPLAY}"));
    EXPECT_TRUE(oo::EntityDescription(entity).starts_with("<Entity 0x"));
  }
}

TEST(Entity, accessors) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(make_HPvector(1, 1, 1), make_HPvector(0, 0, 10));

    Entity* entity = MakeEntity(make_HPvector(3, 4, 10), 2.0f);
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), make_HPvector(3, 4, 10)));
    // -setPosition: updates the camera-relative position from PLAYER's
    // viewpoint.
    EXPECT_TRUE(
        VectorEqual(entity->getCameraRelativePosition(), make_vector(3, 4, 0)));
    EXPECT_TRUE(entity->cameraRangeFront() == 3.0f &&
             entity->cameraRangeBack() == 7.0f);
    EXPECT_TRUE(VectorEqual(entity->relativePosition(), make_vector(2, 3, 9)));

    entity->setPositionX(-1, -2, -3);
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), make_HPvector(-1, -2, -3)));
    EXPECT_TRUE(VectorEqual(entity->getCameraRelativePosition(),
                         make_vector(-1, -2, -13)));

    Entity* other = MakeEntity(make_HPvector(2, 2, 2), 1.0f);
    EXPECT_TRUE(VectorEqual(entity->vectorTo(other), make_vector(3, 4, 5)));

    entity->setVelocity(make_vector(3, 0, 4));
    EXPECT_TRUE(VectorEqual(entity->getVelocity(), make_vector(3, 0, 4)) &&
             entity->speed() == 5.0);
    entity->setEnergy(40);
    entity->setMaxEnergy(100);
    EXPECT_TRUE(entity->getEnergy() == 40 && entity->getMaxEnergy() == 100);
    entity->setDistanceTravelled(9);
    EXPECT_TRUE(entity->getDistanceTravelled() == 9);
    entity->setStatus(oo::EntityStatus::kInFlight);
    EXPECT_TRUE(entity->status() == oo::EntityStatus::kInFlight && entity->isInSpace());
    entity->setStatus(oo::EntityStatus::kDead);
    EXPECT_TRUE(!entity->isInSpace());
    entity->setScanClass(oo::ScanClass::kBuoy);
    EXPECT_TRUE(entity->getScanClass() == oo::ScanClass::kBuoy);
    entity->setUniversalID(42);
    EXPECT_TRUE(entity->getUniversalID() == 42);
    entity->setUniversalID(
        NO_TARGET);  // so that -dealloc does not ask the universe to remove it
    entity->setThrowSparks(true);
    EXPECT_TRUE(entity->throwingSparks());
    entity->throwSparks();
    entity->setLastDrawCounter(17);
    EXPECT_TRUE(entity->getLastDrawCounter() == 17);

    // -compareZeroDistance: sorts the nearer entity later.
    SetUpPlayer(kZeroHPVector, kZeroHPVector);
    entity->update(0);
    other->update(0);
    EXPECT_TRUE(entity->zeroDistance() == 14.0 && other->zeroDistance() == 12.0);
    EXPECT_TRUE(entity->compareZeroDistance(other) == oo::ComparisonResult::kAscending);
    EXPECT_TRUE(other->compareZeroDistance(entity) == oo::ComparisonResult::kDescending);
    EXPECT_TRUE(entity->compareZeroDistance(nullptr) == oo::ComparisonResult::kDescending);

    EXPECT_TRUE(!entity->checkCloseCollisionWith(nullptr) &&
             entity->checkCloseCollisionWith(other));
    entity->takeEnergyDamage(10, other, other, "EQ_WEAPON_TEST");
    EXPECT_TRUE(entity->getEnergy() == 40);  // the root takes no damage
    entity->warnAboutHostiles();
    entity->wasAddedToUniverse();
    entity->wasRemovedFromUniverse();
  }
}

TEST(Entity, orientationAndMovement) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);

    Entity* entity = MakeEntity(kZeroHPVector, 1.0f);
    Quaternion q =
        make_quaternion(2, 0, 0, 0);  // normalised by -setOrientation:
    entity->setOrientation(q);
    EXPECT_TRUE(quaternion_equal(entity->getOrientation(), kIdentityQuaternion));
    EXPECT_TRUE(
        quaternion_equal(entity->normalOrientation(), kIdentityQuaternion));

    // No rotation and no rotation last frame: nothing happens.
    entity->applyRoll(0, 0);
    EXPECT_TRUE(quaternion_equal(entity->getOrientation(), kIdentityQuaternion));

    Quaternion expected = kIdentityQuaternion;
    quaternion_rotate_about_z(&expected, -0.25f);
    quaternion_rotate_about_x(&expected, -0.5f);
    quaternion_normalize(&expected);
    entity->applyRoll(0.25f, 0.5f);
    EXPECT_TRUE(QuaternionNear(entity->getOrientation(), expected));
    EXPECT_TRUE(
        oo::MatrixEqual(entity->rotationMatrix(),
                      oo::MatrixForQuaternionRotation(entity->getOrientation())));
    EXPECT_TRUE(
        oo::MatrixEqual(entity->drawRotationMatrix(), entity->rotationMatrix()));

    quaternion_rotate_about_z(&expected, -0.1f);
    quaternion_rotate_about_x(&expected, -0.2f);
    quaternion_rotate_about_y(&expected, -0.3f);
    quaternion_normalize(&expected);
    entity->applyRoll(0.1f, 0.2f, 0.3f);
    EXPECT_TRUE(QuaternionNear(entity->getOrientation(), expected));

    entity->setNormalOrientation(kIdentityQuaternion);
    EXPECT_TRUE(quaternion_equal(entity->getOrientation(), kIdentityQuaternion));

    // Forward is +z for the identity orientation.
    entity->moveForward(5);
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), make_HPvector(0, 0, 5)));
    EXPECT_TRUE(entity->getDistanceTravelled() == 5);

    oo::Matrix translated =
        oo::MatrixHPTranslate(kIdentityMatrix, make_HPvector(0, 0, 5));
    EXPECT_TRUE(oo::MatrixEqual(entity->transformationMatrix(), translated));
    EXPECT_TRUE(oo::MatrixEqual(entity->drawTransformationMatrix(), translated));

    // With no parent, the absolute position of an offset is position + offset
    // rotated.
    EXPECT_TRUE(HPvector_equal(entity->absolutePositionForSubentity(),
                            make_HPvector(0, 0, 5)));
    EXPECT_TRUE(HPvector_equal(
        entity->absolutePositionForSubentityOffset(make_HPvector(1, 2, 3)),
        make_HPvector(1, 2, 8)));
  }
}

TEST(Entity, update) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(make_HPvector(0, 0, 1), make_HPvector(0, 0, 2));

    // In the cockpit display, the distances are from the origin and the entity
    // does not move.
    Entity* entity = MakeEntity(make_HPvector(3, 0, 4), 1.0f);
    entity->setVelocity(make_vector(1, 0, 0));
    entity->update(2);
    EXPECT_TRUE(entity->zeroDistance() == 25.0 &&
             entity->camZeroDistance() == 25.0);
    EXPECT_TRUE(VectorEqual(CameraRelativePosition(entity), make_vector(3, 0, 4)));
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), make_HPvector(3, 0, 4)));
    EXPECT_TRUE(
        HasMoved(entity) &&
        HasRotated(entity));  // from the zeroed last position and orientation
    entity->update(2);
    EXPECT_TRUE(!HasMoved(entity) && !HasRotated(entity));

    // In flight, from PLAYER's position and viewpoint, then the velocity
    // applies.
    entity->setStatus(oo::EntityStatus::kInFlight);
    entity->update(2);
    EXPECT_TRUE(entity->zeroDistance() == 18.0 &&
             entity->camZeroDistance() == 13.0);
    EXPECT_TRUE(VectorEqual(CameraRelativePosition(entity), make_vector(3, 0, 2)));
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), make_HPvector(5, 0, 4)));
    EXPECT_TRUE(HasMoved(entity));

    // A subentity takes its owner's distances.
    Entity* sub = MakeEntity(make_HPvector(100, 0, 0), 1.0f);
    sub->setOwner(entity);
    SetSubEntity(sub, true);
    sub->setStatus(oo::EntityStatus::kInFlight);
    sub->update(0);
    EXPECT_TRUE(sub->zeroDistance() == 18.0 && sub->camZeroDistance() == 13.0);
    EXPECT_TRUE(sub->getIsSubEntity());
  }
}

TEST(Entity, ownersAndParents) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);

    Entity* entity = MakeEntity(kZeroHPVector, 1.0f);
    {
      oo::AutoreleaseScope scope;
      Entity* owner = oo::RetainEntity(oo::MadeEntity(oo::makeShared<Entity>()));
      entity->setOwner(owner);
      EXPECT_TRUE(entity->owner() == owner);
      // Not a ship with this as a subentity: no parent, and not a ship itself.
      EXPECT_TRUE(entity->parentEntity() == nullptr &&
               entity->rootShipEntity() == nullptr);
      EXPECT_TRUE(entity->superShaderBindingTarget() == nullptr);
      oo::ReleaseEntity(owner);
    }
    EXPECT_TRUE(entity->owner() == nullptr);  // held weakly

    entity->setOwner(entity);
    EXPECT_TRUE(entity->owner() == entity);
    entity->setOwner(nullptr);
    EXPECT_TRUE(entity->owner() == nullptr);
    CaptureLog();
    entity->dumpState();
    EXPECT_TRUE(Logged("State for <Entity 0x") && Logged("Universal ID: 0") &&
             Logged("Scan class: CLASS_NOT_SET"));
    EXPECT_TRUE(Logged("Status: STATUS_COCKPIT_DISPLAY") &&
             Logged("Position: (0, 0, 0)") &&
             Logged("Orientation: (1 + 0i + 0j + 0k)"));
    EXPECT_TRUE(Logged("Distance travelled: 0") && Logged("Energy: 0 of 0") &&
             Logged("Mass: 0"));
    // The owner line and those after it are not pinned here: the Objective-C
    // body described the literal @"none", and a tiny string's -description
    // could raise, which -dumpState swallowed, ending the dump.

    SetIsShip(entity, true);
    EXPECT_TRUE(entity->getIsShip() && entity->rootShipEntity() == entity);
    SetIsShip(entity, false);
  }
}

TEST(Entity, linkedLists) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);

    // Sorted by the low edge of each entity's sphere on each axis.
    Entity* a = MakeEntity(make_HPvector(10, 30, 20), 1.0f);
    Entity* b = MakeEntity(make_HPvector(20, 10, 30), 1.0f);
    Entity* c = MakeEntity(make_HPvector(30, 20, 10), 1.0f);

    a->updateLinkedLists();  // not in the lists: nothing happens
    EXPECT_TRUE(XNext(a) == nullptr && sUniverse->x_list_start == nullptr);

    a->addToLinkedLists();
    b->addToLinkedLists();
    c->addToLinkedLists();
    sUniverse->n_entities = 3;
    EXPECT_TRUE(sUniverse->x_list_start == a && XNext(a) == b && XNext(b) == c &&
             XNext(c) == nullptr);
    EXPECT_TRUE(XPrevious(a) == nullptr && XPrevious(b) == a && XPrevious(c) == b);
    EXPECT_TRUE(sUniverse->y_list_start == b && YNext(b) == c && YNext(c) == a &&
             YNext(a) == nullptr);
    EXPECT_TRUE(sUniverse->z_list_start == c && ZNext(c) == a && ZNext(a) == b &&
             ZNext(b) == nullptr);

    // Moving b to the far end of x re-sorts it, and the lists still check.
#ifndef NDEBUG
    gDebugFlags = oo::ToUnderlying(oo::DebugFlags::kLinkedLists);
    CaptureLog();
#endif
    b->setPosition(make_HPvector(40, 10, 30));
    b->updateLinkedLists();
    EXPECT_TRUE(sUniverse->x_list_start == a && XNext(a) == c && XNext(c) == b &&
             XNext(b) == nullptr);
    EXPECT_TRUE(XPrevious(b) == c && XPrevious(c) == a);
    EXPECT_TRUE(sUniverse->y_list_start == b && YNext(b) == c);

    c->removeFromLinkedLists();
    sUniverse->n_entities = 2;
    EXPECT_TRUE(sUniverse->x_list_start == a && XNext(a) == b &&
             XPrevious(b) == a);
    EXPECT_TRUE(sUniverse->z_list_start == a && ZNext(a) == b);
    EXPECT_TRUE(XNext(c) == nullptr && XPrevious(c) == nullptr);
    c->removeFromLinkedLists();  // removed already: nothing happens
#ifndef NDEBUG
    EXPECT_TRUE(Logged("DEBUG removing entity <Entity 0x") &&
             Logged("from linked lists"));
    EXPECT_TRUE(!Logged("Broken") && !Logged("problem encountered"));
    gDebugFlags = 0;
#endif

    // The head of a list that moves up the list stays the head the universe
    // knows (the universe's own check rebuilds it): pinned as it is.
    a->setPosition(make_HPvector(50, 30, 20));
    a->updateLinkedLists();
    EXPECT_TRUE(sUniverse->x_list_start == a && XNext(b) == a &&
             XPrevious(a) == b && XPrevious(b) == nullptr);

    a->removeFromLinkedLists();
    b->removeFromLinkedLists();
    sUniverse->n_entities = 0;
  }
}

TEST(Entity, sessionAndShaderBindings) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(make_HPvector(0, 1, 0), kZeroHPVector);
    UniverseIvar<oo::TimeAbsolute>("universal_time") = 100;

    Entity* entity = MakeEntity(make_HPvector(0, 0, 3), 1.0f);
    EXPECT_TRUE(entity->validForAddToUniverse());
    UniverseIvar<std::size_t>("_sessionID") = 1;  // a new game started since
    EXPECT_TRUE(!entity->validForAddToUniverse());

    UniverseIvar<oo::TimeAbsolute>("universal_time") = 104.5;
    EXPECT_TRUE(entity->universalTime() == 104.5f &&
             entity->getSpawnTime() == 100.0f);
    EXPECT_TRUE(entity->timeElapsedSinceSpawn() == 4.5f);
    EXPECT_TRUE(VectorEqual(entity->relativePosition(), make_vector(0, -1, 3)));

    entity->setAtmosphereFogging(
        oo::Color::colorWithRed(0.25f, 0.5f, 0.75f, 1.0f).get());
    EXPECT_TRUE(entity->fogUniform().get()->greenComponent() == 0.5f &&
             entity->fogUniform().get()->alphaComponent() == 1.0f);
    entity->setAtmosphereFogging(nullptr);
    EXPECT_TRUE(entity->fogUniform().get() == nullptr);

#ifndef NDEBUG
    EXPECT_TRUE(entity->allTextures().empty());
    EXPECT_TRUE(entity->descriptionForObjDumpBasic() ==
             std::optional<std::string>(
                 "Entity position: (0, 0, 3) scanClass: CLASS_NOT_SET status: "
                 "STATUS_COCKPIT_DISPLAY"));
    EXPECT_TRUE(entity->descriptionForObjDump() ==
             std::optional<std::string>(
                 "Entity position: (0, 0, 3) scanClass: CLASS_NOT_SET status: "
                 "STATUS_COCKPIT_DISPLAY range: 3.16228 (visible: yes)"));
#endif
  }
}

TEST(Entity, objCSubclassOverrides) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);

    TestObjCEntity* entity = static_cast<TestObjCEntity*>(
        oo::MadeEntity(oo::makeShared<TestObjCEntity>()));
    entity->_frustumRadius = 1.5f;
    entity->_session = 3;

    // The root's own members call the virtual members, so they reach the
    // subclass's overrides.
    EXPECT_TRUE(entity->isStellarObject());
    entity->setPosition(make_HPvector(0, 0, 10));
    EXPECT_TRUE(entity->_cameraUpdates == 1);
    EXPECT_TRUE(entity->cameraRangeFront() == 8.5f &&
             entity->cameraRangeBack() == 11.5f);
    entity->setOrientation(kIdentityQuaternion);
    entity->applyRoll(0.5f, 0);
    EXPECT_TRUE(entity->_orientationChanges == 2);
    CaptureLog();
    EXPECT_TRUE(
        !entity->validForAddToUniverse());  // session 3 is not the universe's 0
    EXPECT_TRUE(Logged("from session 3 cannot be added to universe in session 0"));
    entity->_session = 0;
    EXPECT_TRUE(entity->validForAddToUniverse());
    entity->dumpState();
    EXPECT_TRUE(entity->_selfStateDumps == 1);
    EXPECT_TRUE(Logged("State for <TestObjCEntity 0x") && Logged("Mass: 0"));
    entity->setStatus(oo::EntityStatus::kInFlight);
    entity->update(0);
    EXPECT_TRUE(entity->_cameraUpdates == 2);
    EXPECT_TRUE(!entity->canCollide());
    entity->drawImmediate(false, false);
    EXPECT_TRUE(entity->_draws == 1);
#ifndef NDEBUG
    entity->_visible = false;
    EXPECT_TRUE(
        entity->descriptionForObjDump()->ends_with("range: 10 (visible: no)"));
    EXPECT_TRUE(entity->descriptionForObjDumpBasic()->starts_with(
        "TestObjCEntity position: "));
#endif
  }
}

TEST(Entity, rootDefaultsForSubclassResponsibilities) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);

    Entity* entity = MakeEntity(kZeroHPVector, 1.0f);
    CaptureLog();
    EXPECT_TRUE(entity->findCollisionRadius() ==
             0);  // logs the subclass responsibility
    entity->drawImmediate(true, false);
    EXPECT_TRUE(sLog.size() == 2);
    entity->warnAboutHostiles();
    EXPECT_TRUE(Logged("***** Entity does nothing in warnAboutHostiles"));
    entity->subEntityReallyDied(nullptr);
    EXPECT_TRUE(Logged("called for non-ship entity"));
  }
}

#ifndef NDEBUG
TEST(Entity, liveEntityCount) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);

    const uint32_t count = gLiveEntityCount;
    const size_t memory = gTotalEntityMemory;
    Entity* entity = nullptr;
    {
      oo::AutoreleaseScope scope;
      entity = oo::RetainEntity(oo::MadeEntity(
          oo::makeShared<TestObjCEntity>()));  // +1, as +alloc's was
    }
    EXPECT_TRUE(gLiveEntityCount == count + 1);
    EXPECT_TRUE(gTotalEntityMemory ==
             memory + oo::EntityObjectSize(entity));  // its object's size
    oo::ReleaseEntity(entity);
    EXPECT_TRUE(gLiveEntityCount == count && gTotalEntityMemory == memory);
  }
}
#endif

// --- The crossing
// ---------------------------------------------------------

TEST(Entity, cxxEntityBehindItsFacade) {
#ifndef NDEBUG
  const uint32_t count = gLiveEntityCount;
#endif
  {
    oo::AutoreleaseScope scope;
    // The test's reference goes with the pool (the object's lifetime is the
    // entity's, so the live count below falls when the entity goes).
    const std::shared_ptr<TestCxxEntity> entity = oo::makeShared<TestCxxEntity>();
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);

    Entity* facade = oo::MadeEntity(entity);
    EXPECT_TRUE(facade == entity.get());
#ifndef NDEBUG
    EXPECT_TRUE(gLiveEntityCount == count + 1);
#endif

    // Messages reach the C++ overrides, and the root's members call them.
    EXPECT_TRUE(facade->isPlanet() && facade->isStellarObject() &&
             !facade->isSun());
    facade->setPosition(make_HPvector(0, 0, 10));
    EXPECT_TRUE(entity->cameraUpdates == 1);
    EXPECT_TRUE(facade->cameraRangeFront() == 8.0f &&
             facade->cameraRangeBack() == 12.0f);
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), make_HPvector(0, 0, 10)));
    EXPECT_TRUE(oo::EntityDescription(facade).starts_with("<TestCxxEntity 0x") &&
             oo::EntityDescription(facade).ends_with(">{test}"));
#ifndef NDEBUG
    EXPECT_TRUE(facade->descriptionForObjDumpBasic() ==
             std::optional<std::string>("TestCxxEntity test"));
#endif

    // The lists link its facade, among Objective-C entities.
    Entity* other = MakeEntity(make_HPvector(20, 0, 0), 1.0f);
    facade->addToLinkedLists();
    other->addToLinkedLists();
    sUniverse->n_entities = 2;
    EXPECT_TRUE(sUniverse->x_list_start == facade && XNext(facade) == other &&
             XPrevious(other) == facade);
    facade->removeFromLinkedLists();
    other->removeFromLinkedLists();
    sUniverse->n_entities = 0;
  }
  // The facade was the entity's identity and owner: gone with the pool.
#ifndef NDEBUG
  EXPECT_TRUE(gLiveEntityCount == count);
#endif
}

// PlayerEntity's -deferredInit sends -init again (through -[ShipEntity
// initWithKey:...]) to an entity that is already initialised and has a
// script object: the body runs again over the same state, and what it does not
// set stays.
TEST(Entity, initSentAgain) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);
    TestObjCEntity* entity = static_cast<TestObjCEntity*>(
        oo::MadeEntity(oo::makeShared<TestObjCEntity>()));
    entity->setPosition(make_HPvector(1, 2, 3));
    entity->setStatus(oo::EntityStatus::kInFlight);
    entity->setEnergy(42);
    entity->setUniversalID(9);
    entity->init();  // the body -init ran again
    EXPECT_TRUE(HPvector_equal(entity->getPosition(), kZeroHPVector) &&
             entity->status() == oo::EntityStatus::kCockpitDisplay);
    EXPECT_TRUE(entity->getEnergy() == 42 && entity->getUniversalID() == 9);
    entity->setUniversalID(NO_TARGET);
  }
}

// The Objective-C body described the literals @"self" and @"none", which could
// raise and end the dump; the C++ body prints them.
TEST(Entity, dumpStateOwnerLines) {
  {
    oo::AutoreleaseScope scope;
    SetUpUniverse();
    SetUpPlayer(kZeroHPVector, kZeroHPVector);
    Entity* entity = MakeEntity(kZeroHPVector, 1.0f);
    CaptureLog();
    entity->dumpState();
    EXPECT_TRUE(Logged("Owner: none") && Logged("Flags: isSunlit") &&
             Logged("Collision Test Filter: 0"));
    entity->setOwner(entity);
    sLog.clear();
    entity->dumpState();
    EXPECT_TRUE(Logged("Owner: self") && Logged("Flags: isSunlit"));
    entity->setOwner(nullptr);
  }
}
