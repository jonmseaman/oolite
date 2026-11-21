/*	tests/component/test_ship_attacks.cpp
        One ship, one behaviour (spec 011 US6 scenario 2): a ship with a laser,
   told to attack a target a few kilometres away, turns, closes and opens
   fire by itself; within 60 s of game time the target has been attacked and
   has lost energy. The hunter flies the attack behaviour it is given, with
   nullAI.plist so that its own AI script does not choose another.
   --without-weapon mounts no weapon (an adder has a laser by definition).
   Run: tests/run.sh component --cpp -k ship_attacks
*/

#include <chrono>

#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

using namespace std::chrono_literals;
using oo::component::Game;
using oo::component::ShipWithKey;

OO_TEST(a_ship_told_to_attack_opens_fire) {
  Game game = Game::scenario(20260910);

  oo::component::ShipHandle target =
      game.add(ShipWithKey("cobra3-trader").near(game.player(), 20'000.0));
  OO_CHECK(target.alive());
  target->setAITo("nullAI.plist");  // it does not run or fight back
  oo::component::ShipHandle hunter = game.add(ShipWithKey("adder").near(target, 5'000.0));
  OO_CHECK(hunter.alive());
  hunter->setAITo("nullAI.plist");
  OO_CHECK(hunter->setWeaponMount(OOWeaponFacing::kForward,
                                  oo::component::hasArgument("--without-weapon")
                                      ? "EQ_WEAPON_NONE"
                                      : "EQ_WEAPON_PULSE_LASER"));

  hunter->addTarget(target);
  hunter->performAttack();

  // It has to turn, close the range and line up: give it time, and assert that it happened.
  OO_CHECK(game.simulateUntil(
      [&] { return game.events().has("shipBeingAttacked", target); }, 60s));
  OO_CHECK(target.alive() && target->energy < target->maxEnergy);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
