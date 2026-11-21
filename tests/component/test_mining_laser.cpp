/*	tests/component/test_mining_laser.cpp
        One ship, one behaviour (spec 011 US6 scenarios 3 and 6): an adder with
   a mining laser, set to mine an asteroid 2 km away, breaks it up within two
   minutes; the asteroid's handle is no longer alive, at least two boulders
   exist where there were none, and the game raised shipDied for the
   asteroid (a handle matches by id after the ship is gone).
   The miner flies the mining behaviour it is given, with nullAI.plist so
   that its own AI script does not choose another. --without-weapon mounts no
   weapon.
   Run: tests/run.sh component --cpp -k mining_laser
*/

#include <chrono>

#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

using namespace std::chrono_literals;
using oo::component::Game;
using oo::component::ShipWithKey;

OO_TEST(a_mining_laser_breaks_an_asteroid_into_boulders) {
  Game game = Game::scenario(20260910);

  oo::component::ShipHandle asteroid =
      game.add(ShipWithKey("asteroid").near(game.player(), 20'000.0));
  OO_CHECK(asteroid.alive());
  oo::component::ShipHandle miner = game.add(ShipWithKey("adder").near(asteroid, 2'000.0));
  OO_CHECK(miner.alive());
  miner->setAITo("nullAI.plist");
  OO_CHECK(miner->setWeaponMount(OOWeaponFacing::kForward,
                                 oo::component::hasArgument("--without-weapon")
                                     ? "EQ_WEAPON_NONE"
                                     : "EQ_WEAPON_MINING_LASER"));
  OO_CHECK_EQ(game.shipsWithRole("boulder").size(), 0u);

  miner->addTarget(asteroid);
  miner->performMining();

  OO_CHECK(game.simulateUntil([&] { return !asteroid.alive(); }, 2min));

  // A mined asteroid always leaves at least two boulders.
  OO_CHECK(game.shipsWithRole("boulder").size() >= 2u);
  OO_CHECK(game.events().has("shipDied", asteroid));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
