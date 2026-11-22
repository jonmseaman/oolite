/*	tests/component/test_remapped_key.cpp
        A remapped key (spec 011 US3 scenario 10, FR-039): with saved settings
   that move "fire lasers" from a to k, as the keyboard settings screen saves
   them, holding k fires the forward laser at a target ahead (the weapon heats
   and the target loses energy); holding the old key a does neither.
   Run: tests/run.sh component --cpp -k remapped_key
*/

#include <chrono>

#include <gtest/gtest.h>

#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "oo_component.hpp"

using namespace std::chrono_literals;
using oo::component::Game;
using oo::component::Settings;
using oo::component::ShipWithKey;

TEST(remapped_key, the_remapped_fire_key_fires_and_the_old_one_does_not) {
  Game game = Game::inFlightWithKeys(20260910,
                                     Settings().remapKey("key_fire_lasers", "k"));
  EXPECT_EQ(game.keyForControl("key_fire_lasers"), std::string("k"));
  game.player()->setSpeed(0.0);
  oo::component::ShipHandle target =
      game.add(ShipWithKey("asteroid").aheadOf(game.player(), 1'000.0));
  EXPECT_TRUE(target.alive());
  game.step(2);

  // The old key: nothing.
  const float energyBefore = target->energy;
  game.hold("a", [] { return false; }, 1s);
  EXPECT_TRUE(game.player()->laserHeatLevelForward() == 0.0f);
  EXPECT_TRUE(target.alive() && target->energy == energyBefore);

  // The new key: the forward laser fires.
  game.hold("k", [&] { return target.alive() && target->energy < energyBefore; }, 2s);
  EXPECT_TRUE(game.player()->laserHeatLevelForward() > 0.0f);
  EXPECT_TRUE(!target.alive() || target->energy < energyBefore);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
