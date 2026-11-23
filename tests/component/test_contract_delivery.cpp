/*	tests/component/test_contract_delivery.cpp
        A cargo contract, accepted by script, flown to another system and
   settled when the player docks at its main station
   (PlayerEntity::checkPassengerContracts, run by docked()). The first case
   delivers on time and is paid; the second, in the same game, delivers back
   at Lave after its deadline and is not.

        Each outcome is read from a playerCompletedContract handler put on the
   core world script oolite-contracts-cargo, which has none of its own: world
   script events look their handler up by name each time they fire
   (OOJSScript::callMethod), so a handler added by the test is called.

        Reputation is read as the save stores it, the contracts_fulfilled and
   contracts_expired counts. player.contractReputation is not used: it mixes
   in the market's random factor, which every jump resets
   (PlayerEntity::contractReputation), so it can fall after a success.
   Run: tests/run.sh component --cpp -k contract_delivery
*/

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <format>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "PlayerEntity.h"
#include "Universe.h"
#include "oo_component.hpp"
#include "oofnd/PList.hpp"

using namespace std::chrono_literals;
using oo::component::Game;

namespace {

// Lave, where the standard scenario's commander starts.
constexpr int kLave = 7;

// Each playerCompletedContract as "type:result:amount" (amount in tenths of
// a credit), in order.
constexpr const char* kInstallListenerJs =
    "(function () { var ws = worldScripts['oolite-contracts-cargo'];"
    " ws.$completed = []; var old = ws.playerCompletedContract;"
    " ws.playerCompletedContract = function (type, result, amount) {"
    " this.$completed.push(type + ':' + result + ':' + amount);"
    " if (old) old.apply(this, arguments); }; return true; })()";
constexpr const char* kCompletedJs =
    "worldScripts['oolite-contracts-cargo'].$completed.join(',')";

// Out of the main station's mass lock: 300 km from it, away from the planet
// (tests/run_js_tests.py, MOVE_CLEAR).
constexpr const char* kMoveClearJs =
    "(function () { var st = system.mainStation;"
    " player.ship.position = st.position.add(st.position.subtract("
    "system.mainPlanet.position).direction().multiply(300000));"
    " return true; })()";

// The commander's cargo-contract record, as the save stores it.
struct ContractRecord {
  int fulfilled = 0;
  int expired = 0;
};

ContractRecord Record(const Game& game) {
  const oo::PList saved = game.player()->commanderDataDictionary();
  const oo::PList* reputation = saved.get<oo::PList>("reputation");
  EXPECT_TRUE(reputation != nullptr && reputation->isDict());
  if (reputation == nullptr) return {};
  return {reputation->get<int>("contracts_fulfilled"),
          reputation->get<int>("contracts_expired")};
}

bool HappenedSince(const Game& game, std::size_t mark, std::string_view name) {
  const auto& entries = game.events().entries();
  for (std::size_t i = mark; i < entries.size(); ++i) {
    if (entries[i].name == name) return true;
  }
  return false;
}

// The most stable system within 7 light years, lowest id first on a tie
// (tests/run_js_tests.py, the flight phase).
int Destination(Game& game) {
  return static_cast<int>(game.eval(
      "system.info.systemsInRange(7).sort(function (a, b) {"
      " return (b.government - a.government) || (a.systemID - b.systemID);"
      " })[0].systemID"));
}

// Docked, to in flight in systemId: target it, launch, leave the mass lock
// and jump.
void FlyTo(Game& game, int systemId) {
  EXPECT_TRUE(game.player()->isDocked());
  EXPECT_TRUE(game.evalBool(std::format(
      "player.ship.targetSystem = {0}, player.ship.targetSystem === {0}",
      systemId)));
  EXPECT_TRUE(game.evalBool("player.ship.launch(), true"));
  EXPECT_TRUE(game.simulateUntil(
      [&] {
        return game.player()->status() == oo::EntityStatus::kInFlight &&
               game.universe()->breakPatternOver();
      },
      120s));
  EXPECT_TRUE(game.evalBool(kMoveClearJs));
  EXPECT_TRUE(game.evalBool("player.ship.beginHyperspaceCountdown(5)"));
  EXPECT_TRUE(game.simulateUntil(
      [&] {
        return game.evalBool(std::format(
            "system.ID === {} && player.ship.status === 'STATUS_IN_FLIGHT'",
            systemId));
      },
      180s));
}

// In flight, to docked at this system's main station, once
// shipDockedWithStation has been sent (player.ship.docked is true a frame
// before it).
void DockAtMainStation(Game& game) {
  const std::size_t mark = game.events().size();
  EXPECT_TRUE(game.evalBool("system.mainStation.dockPlayer(), true"));
  EXPECT_TRUE(game.simulateUntil(
      [&] {
        return game.player()->isDocked() &&
               HappenedSince(game, mark, "shipDockedWithStation");
      },
      60s));
}

void PrintSpeed(std::chrono::steady_clock::time_point started,
                double simulated) {
  const double real =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - started)
          .count();
  std::printf("game %.1f s, real %.2f s, %.0fx\n", simulated, real,
              simulated / real);
}

}  // namespace

TEST(contract_delivery, a_cargo_contract_delivered_on_time_is_paid) {
  const auto started = std::chrono::steady_clock::now();

  // Given a new commander docked at Lave, seeded 20260910
  Game game = Game::docked(20260910);
  const double gameStart = game.time();
  EXPECT_TRUE(game.player()->isDocked());
  EXPECT_EQ(game.eval("system.ID"), kLave);
  EXPECT_TRUE(game.evalBool(kInstallListenerJs));
  const int destination = Destination(game);
  EXPECT_NE(destination, kLave);
  const double credits = game.eval("player.credits");
  const ContractRecord record = Record(game);
  const double food = game.eval("player.ship.manifest.food");
  const std::size_t mark = game.events().size();

  // And a contract for 5 t of food to the most stable neighbour, due in 30
  // days, for 200 Cr
  EXPECT_TRUE(game.evalBool(std::format(
      "player.ship.awardContract(5, 'food', system.ID, {}, "
      "clock.seconds + 30 * 86400, 200, 0)",
      destination)));
  EXPECT_EQ(game.eval("player.ship.contracts.length"), 1.0);
  EXPECT_EQ(game.eval("player.ship.manifest.food"), food + 5);

  // When I fly there and dock at its main station
  FlyTo(game, destination);
  DockAtMainStation(game);

  // Then the contract is complete and paid
  EXPECT_TRUE(HappenedSince(game, mark, "playerCompletedContract"));
  EXPECT_EQ(game.evalString(kCompletedJs), std::string("cargo:success:2000"));
  EXPECT_NEAR(game.eval("player.credits"), credits + 200, 0.05);
  // And the goods were taken
  EXPECT_EQ(game.eval("player.ship.contracts.length"), 0.0);
  EXPECT_EQ(game.eval("player.ship.manifest.food"), food);
  // And the commander's contract record gained (increaseContractReputation(10))
  EXPECT_GT(Record(game).fulfilled, record.fulfilled);
  EXPECT_EQ(Record(game).expired, record.expired);

  OO_COMPONENT_CHECK_NO_ERRORS(game);
  PrintSpeed(started, game.time() - gameStart);
}

TEST(contract_delivery, a_cargo_contract_delivered_late_is_not_paid) {
  const auto started = std::chrono::steady_clock::now();

  // Given the commander docked where the last case delivered, with a full tank
  Game game = Game::current();
  const double gameStart = game.time();
  EXPECT_TRUE(game.player()->isDocked());
  EXPECT_NE(game.eval("system.ID"), kLave);
  EXPECT_TRUE(game.evalBool("player.ship.fuel = 7, player.ship.fuel === 7"));
  EXPECT_TRUE(game.evalBool(
      "worldScripts['oolite-contracts-cargo'].$completed = [], true"));
  const double credits = game.eval("player.credits");
  const ContractRecord record = Record(game);
  const double food = game.eval("player.ship.manifest.food");
  const std::size_t mark = game.events().size();

  // And a contract for 5 t of food to Lave, due in 60 s: the jump alone
  // takes hours of ship time
  EXPECT_TRUE(game.evalBool(std::format(
      "player.ship.awardContract(5, 'food', system.ID, {}, "
      "clock.seconds + 60, 200, 0)",
      kLave)));
  EXPECT_EQ(game.eval("player.ship.contracts.length"), 1.0);

  // When I fly to Lave and dock at its main station
  FlyTo(game, kLave);
  DockAtMainStation(game);

  // Then the contract is settled as late and not paid
  EXPECT_TRUE(HappenedSince(game, mark, "playerCompletedContract"));
  EXPECT_EQ(game.evalString(kCompletedJs), std::string("cargo:late:0"));
  EXPECT_NEAR(game.eval("player.credits"), credits, 0.05);
  // And the goods stay in the hold
  EXPECT_EQ(game.eval("player.ship.contracts.length"), 0.0);
  EXPECT_EQ(game.eval("player.ship.manifest.food"), food + 5);
  // And the commander's contract record lost (decreaseContractReputation(10)):
  // the last case's good marks go first
  EXPECT_LT(Record(game).fulfilled, record.fulfilled);

  OO_COMPONENT_CHECK_NO_ERRORS(game);
  PrintSpeed(started, game.time() - gameStart);
}

OO_COMPONENT_MAIN()
