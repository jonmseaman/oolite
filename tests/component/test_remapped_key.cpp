/*	tests/component/test_remapped_key.cpp
        A remapped key (spec 011 US3 scenario 10, FR-039): with saved settings
   that move "fire lasers" from a to k, as the keyboard settings screen saves
   them, holding k fires the forward laser at a target ahead (the weapon heats
   and the target loses energy); holding the old key a does neither.
   Run: tests/run.sh component --cpp -k remapped_key
*/

#include <chrono>

#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

using namespace std::chrono_literals;
using oo::component::Game;
using oo::component::Settings;
using oo::component::ShipWithKey;

OO_TEST(the_remapped_fire_key_fires_and_the_old_one_does_not) {
  Game game = Game::inFlightWithKeys(20260910,
                                     Settings().remapKey("key_fire_lasers", "k"));
  OO_CHECK_EQ(game.keyForControl("key_fire_lasers"), std::string("k"));
  game.player()->setSpeed(0.0);
  oo::component::ShipHandle target =
      game.add(ShipWithKey("asteroid").aheadOf(game.player(), 1'000.0));
  OO_CHECK(target.alive());
  game.step(2);

  // The old key: nothing.
  const float energyBefore = target->energy;
  game.hold("a", [] { return false; }, 1s);
  OO_CHECK(game.player()->laserHeatLevelForward() == 0.0f);
  OO_CHECK(target.alive() && target->energy == energyBefore);

  // The new key: the forward laser fires.
  game.hold("k", [&] { return target.alive() && target->energy < energyBefore; }, 2s);
  OO_CHECK(game.player()->laserHeatLevelForward() > 0.0f);
  OO_CHECK(!target.alive() || target->energy < energyBefore);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
