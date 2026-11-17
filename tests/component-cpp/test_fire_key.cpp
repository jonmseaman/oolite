/*	tests/component-cpp/test_fire_key.cpp
        The player's fire key (spec 011 US6 scenario 4): the player in flight
   with a pulse laser and a target directly ahead; holding the fire key the
   game's key settings name drops the target's energy and heats the player's
   weapon. --without-weapon mounts no weapon, so the test can be seen to fail.
   Run: tests/run.sh component --cpp -k fire_key
*/

#include <chrono>
#include <string>

#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

using namespace std::chrono_literals;
using oo::component::Game;
using oo::component::ShipWithKey;

OO_TEST(holding_the_fire_key_fires_the_forward_laser) {
  Game game = Game::inFlightWithKeys(20260910);
  game.player()->setSpeed(0.0);
  OO_CHECK(game.player()->setWeaponMount(oo::WeaponFacing::kForward,
                                         oo::component::hasArgument("--without-weapon")
                                             ? "EQ_WEAPON_NONE"
                                             : "EQ_WEAPON_PULSE_LASER"));
  oo::component::ShipHandle target =
      game.add(ShipWithKey("asteroid").aheadOf(game.player(), 1'000.0));
  OO_CHECK(target.alive());
  game.step(2);
  const float energyBefore = target->energy;

  const std::string fire = game.keyForControl("key_fire_lasers");
  game.hold(fire, [&] { return !target.alive() || target->energy < energyBefore; }, 2s);
  OO_CHECK(!target.alive() || target->energy < energyBefore);
  OO_CHECK(game.player()->laserHeatLevelForward() > 0.0f);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
