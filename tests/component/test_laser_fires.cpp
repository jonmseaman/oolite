/*	tests/component/test_laser_fires.cpp
        One ship, one behaviour (spec 011 US6 scenario 1): a ship with a pulse
   laser on its forward mount fires it at a target directly ahead and in
   range; the weapon heats, the target loses energy, and the game raises
   shipBeingAttacked for the target. --without-weapon mounts no weapon
   instead (an adder carries a pulse laser by its definition, so skipping the
   mount would change nothing), so the test can be seen to fail.
   Run: tests/run.sh component --cpp -k laser_fires
*/

#include <chrono>

#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

using namespace std::chrono_literals;
using oo::component::Game;
using oo::component::ShipWithKey;

OO_TEST(a_ship_fires_its_forward_laser_and_hits_what_is_ahead) {
  Game game = Game::scenario(20260910);

  oo::component::ShipHandle shooter =
      game.add(ShipWithKey("adder").near(game.player(), 20'000.0));
  OO_CHECK(shooter.alive());
  shooter->setAITo("nullAI.plist");  // it sits still: this is about the gun, not the pilot
  OO_CHECK(shooter->setWeaponMount(OOWeaponFacing::kForward,
                                   oo::component::hasArgument("--without-weapon")
                                       ? "EQ_WEAPON_NONE"
                                       : "EQ_WEAPON_PULSE_LASER"));

  oo::component::ShipHandle target =
      game.add(ShipWithKey("asteroid").aheadOf(shooter, 1'000.0));
  OO_CHECK(target.alive());
  const float energyBefore = target->energy;

  shooter->addTarget(target);
  shooter->fireMainWeapon(1'000.0);  // the game's own "fire the forward weapon"
  game.simulate(1s);

  OO_CHECK(shooter->laserHeatLevelForward() > 0.0f);         // the gun was used
  OO_CHECK(target.alive() && target->energy < energyBefore);  // the shot landed
  OO_CHECK(game.events().has("shipBeingAttacked", target));  // and the game said so
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
