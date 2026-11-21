/*	tests/component/test_s4_missile_armed_ship_kills_by_missile.cpp
        Scenario S4 ("Ship-to-ship combat: A missile-armed ship kills by missile (smoke)").
   Each check carries the scenario step it checks.
   Run: tests/run.sh component --cpp -k s4_
*/

#include <chrono>
#include <cstdio>
#include <string>

#include "oo_component.hpp"
#include "oo_test.hpp"

using oo::component::Game;
using oo::component::ShipsWithRole;
using oo::component::ticks;

OO_TEST(a_missile_armed_ship_kills_by_missile_smoke) {
  const auto started = std::chrono::steady_clock::now();

  // Given a universe seeded with 20260918
  Game game = Game::scenario(20260918);
  const double gameStart = game.time();

  // When I spawn 1 ship with role "pirate"
  OO_CHECK_EQ(game.spawn(ShipsWithRole("pirate").count(1)).size(), 1u);
  // And I spawn 1 ship with role "trader" within 10 km
  OO_CHECK_EQ(game.spawn(ShipsWithRole("trader").count(1).withinKm(10)).size(), 1u);
  // And the simulation runs for at most 900 ticks
  game.simulateAtMost(ticks(900));

  // Then a ship with role "pirate" survives
  OO_CHECK(game.countWithRole("pirate") > 0);
  // And no ERROR appears in the log
  OO_CHECK(game.logLinesContaining("ERROR").empty());
  for (const std::string& line : game.logLinesContaining("ERROR")) {
    std::printf("     %s\n", line.c_str());
  }

  OO_COMPONENT_CHECK_NO_ERRORS(game);

  const double real =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
  const double simulated = game.time() - gameStart;
  std::printf("game %.1f s, real %.2f s, %.0fx\n", simulated, real, simulated / real);
}

OO_COMPONENT_MAIN()
