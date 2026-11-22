/*	test_CollisionRegion.mm
        Unit tests for CollisionRegion (src/Core/CollisionRegion.h).

        CollisionRegion's object reads Entity and Universe ivars, so it links
   the whole game but main (tests/unit/core/meson.build entry ['*']), and
   defines the one global main.mm did, gDebugFlags. The regions are driven with
   plain Entity objects and no Universe (UNIVERSE is nil: no sun, so no
   shadowing work). The tests pin the region tree (a sphere that fits
   goes into the subregion, one that crosses a border is left out), which region
   an entity is filed in (the first subregion within whose borders it lies, else
   the universe), the counts in -debugOut, the description text, the collision
   counters and the shadow geometry of shadowAtPointOcclusion(). Run:
   tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "CollisionRegion.h"
#include "Entity.h"
#include "SunEntity.h"
#include "PlayerEntity.h"  // the stand-in PLAYER
extern PlayerEntity* gOOPlayer;
#include <string>

#include "EntityManualRefs.h"  // the tests' manual entity references

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

namespace {

// PLAYER: the debug build asserts it is set (OOGetPlayer()), so
// the entities' positions, which read it, have a player whose viewpoint is the
// origin: the camera-relative positions are the release build's with no player
// (the origin's). Never released, as the player is not.
class TestPlayer : public PlayerEntity {
 public:
  HPVector viewpointPosition() override { return kZeroHPVector; }
};

void SetUpTestPlayer() {
  static TestPlayer* player = nullptr;
  if (player == nullptr) {
    std::shared_ptr<TestPlayer> made = oo::makeShared<TestPlayer>();
    {
      oo::RetainEntity(oo::MadeEntity(
          made));  // +1, as PlayerEntity::sharedPlayer() makes the game's
    }
    player = made.get();
  }
  gOOPlayer = player;
}

Entity* MakeEntity(HPVector position, GLfloat radius) {
  SetUpTestPlayer();
  Entity* entity = oo::MadeEntity(oo::makeShared<Entity>());
  entity->setPosition(position);
  entity->setCollisionRadius(radius);
  return entity;
}

bool EndsWith(const std::string& text, const std::string& tail) {
  return text.size() >= tail.size() &&
         text.compare(text.size() - tail.size(), tail.size(), tail) == 0;
}

std::shared_ptr<CollisionRegion> MakeUniverse() {
  return oo::makeShared<CollisionRegion>(CollisionRegion::AsUniverse{});
}

// The region an entity is filed in (the entity's C++ part holds it).
CollisionRegion* RegionOf(Entity* entity) {
  return entity->getCollisionRegion();
}

// A universe with three regions: one at x = 1000 (radius 100) holding one at x
// = 1000 (radius 10), and one at x = 5000 (radius 100). A sphere crossing the
// first region's border is left out.
std::shared_ptr<CollisionRegion> MakeTree() {
  std::shared_ptr<CollisionRegion> universe = MakeUniverse();
  universe->addSubregionAtPosition(make_HPvector(1000, 0, 0), 100);
  universe->addSubregionAtPosition(make_HPvector(1000, 0, 0),
                                   10);  // fits: nested
  universe->addSubregionAtPosition(make_HPvector(1050, 0, 0),
                                   100);  // crosses: left out
  universe->addSubregionAtPosition(make_HPvector(5000, 0, 0), 100);
  return universe;
}

}  // namespace

TEST(CollisionRegion, universeAlone) {
  {
    std::shared_ptr<CollisionRegion> universe = MakeUniverse();
    EXPECT_TRUE(universe != nullptr);
    EXPECT_TRUE(universe->debugOut() == std::optional<std::string>("0:"));
    EXPECT_TRUE(universe->collisionDescription() == "p0 - c0");
    EXPECT_TRUE(EndsWith(universe->descriptionComponents().value_or(""),
                      ", 0 subregions, 0 ents"));
  }
}

TEST(CollisionRegion, subregionTree) {
  {
    std::shared_ptr<CollisionRegion> universe = MakeTree();
    EXPECT_TRUE(universe->debugOut() == std::optional<std::string>("0:0:0:0:"));
    EXPECT_TRUE(EndsWith(universe->descriptionComponents().value_or(""),
                      ", 2 subregions, 0 ents"));

    universe->clearSubregions();
    EXPECT_TRUE(universe->debugOut() == std::optional<std::string>("0:"));
    EXPECT_TRUE(EndsWith(universe->descriptionComponents().value_or(""),
                      ", 0 subregions, 0 ents"));
  }
}

TEST(CollisionRegion, entitiesAreFiled) {
  {
    std::shared_ptr<CollisionRegion> universe = MakeTree();
    Entity* inner = MakeEntity(make_HPvector(1000, 0, 0), 5);
    Entity* second = MakeEntity(make_HPvector(5000, 0, 0), 5);
    Entity* distant = MakeEntity(make_HPvector(1.0e7, 0, 0), 5);

    universe->clearEntityList();
    EXPECT_TRUE(universe->checkEntity(inner));
    EXPECT_TRUE(universe->checkEntity(second));
    EXPECT_TRUE(universe->checkEntity(distant));

    // inner lies within the borders (radius + 32 km) of the first region's
    // nested one, and so does second (4 km away); distant is filed in the
    // universe itself.
    EXPECT_TRUE(universe->debugOut() == std::optional<std::string>("1:0:2:0:"));
    EXPECT_TRUE(RegionOf(distant) == universe.get());
    EXPECT_TRUE(RegionOf(inner) != nullptr && RegionOf(inner) != universe.get());
    EXPECT_TRUE(RegionOf(inner) == RegionOf(second));
    EXPECT_TRUE(RegionOf(inner)->debugOut() == std::optional<std::string>("2:"));
    EXPECT_TRUE(EndsWith(RegionOf(inner)->descriptionComponents().value_or(""),
                      ", 0 subregions, 2 ents"));
    EXPECT_TRUE(EndsWith(universe->descriptionComponents().value_or(""),
                      ", 2 subregions, 1 ents"));

    // A region that is not the universe refuses an entity outside its borders.
    EXPECT_TRUE(!RegionOf(inner)->checkEntity(distant));

    // No collision chain (Universe builds it): nothing is checked.
    universe->findCollisions();
    EXPECT_TRUE(universe->collisionDescription() == "p0 - c0");
    universe->findShadowedEntities();  // no Universe, so no sun: nothing to do

    universe->clearEntityList();
    EXPECT_TRUE(universe->debugOut() == std::optional<std::string>("0:0:0:0:"));
  }
}

TEST(CollisionRegion, entityListGrows) {
  {
    std::shared_ptr<CollisionRegion> universe = MakeUniverse();
    for (unsigned i = 0; i < COLLISION_MAX_ENTITIES * 3; i++) {
      universe->addEntity(MakeEntity(make_HPvector(i, 0, 0), 1));
    }
    EXPECT_TRUE(universe->debugOut() ==
             std::optional<std::string>(
                 std::to_string(COLLISION_MAX_ENTITIES * 3) + ":"));
  }
}

TEST(CollisionRegion, shadowGeometry) {
  {
    // The function reads only Entity members of the sun, which is C++: a C++
    // sun with the same position and radius.
    std::shared_ptr<oo::SunEntity> sunRef = oo::makeShared<oo::SunEntity>();
    sunRef->position = make_HPvector(0, 0, 10000);
    sunRef->collision_radius = 10;
    oo::SunEntity* sun = sunRef.get();
    Entity* between = MakeEntity(make_HPvector(0, 0, 100), 50);
    Entity* behind = MakeEntity(make_HPvector(0, 0, -100), 50);
    Entity* small = MakeEntity(make_HPvector(0, 0, 100), 0.5f);
    Occlusion o = shadowAtPointOcclusion(kZeroHPVector, 1, between, sun);
    EXPECT_TRUE(o.occluded);
    EXPECT_TRUE(o.value == 0.0f);

    o = shadowAtPointOcclusion(kZeroHPVector, 1, behind, sun);
    EXPECT_TRUE(!o.occluded);
    EXPECT_TRUE(o.value == 1.5f);

    o = shadowAtPointOcclusion(kZeroHPVector, 1, small,
                               sun);  // smaller can't shade bigger
    EXPECT_TRUE(!o.occluded);
    EXPECT_TRUE(o.value == 1.5f);
  }
}

TEST(CollisionRegion, cxxRegion) {
  {
    std::shared_ptr<CollisionRegion> universe = MakeUniverse();
    universe->addSubregionAtPosition(make_HPvector(1000, 0, 0), 100);
    universe->addSubregionAtPosition(make_HPvector(5000, 0, 0), 100);
    EXPECT_TRUE(universe->debugOut() == std::optional<std::string>("0:0:0:"));
    EXPECT_TRUE(
        EndsWith(*universe->descriptionComponents(), ", 2 subregions, 0 ents"));

    Entity* entity = MakeEntity(make_HPvector(1.0e7, 0, 0), 5);
    EXPECT_TRUE(universe->checkEntity(entity));
    EXPECT_TRUE(RegionOf(entity) == universe.get());
    EXPECT_TRUE(universe->collisionDescription() == "p0 - c0");
  }
}
