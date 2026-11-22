/*	tests/component/test_s8_destroyed_ships_leave_allships.cpp
        Scenario S8 ("Entity lifetime: Setup with a pirate and police runs cleanly for 900 ticks").
   Each check carries the scenario step it checks.
   Run: tests/run.sh component --cpp -k s8_
*/

#include <chrono>
#include <cstdio>
#include <string>

#include <gtest/gtest.h>

#include "oo_component.hpp"

using oo::component::Game;
using oo::component::ShipsWithRole;
using oo::component::ticks;

TEST(s8_destroyed_ships_leave_allships, setup_with_a_pirate_and_police_runs_cleanly) {
  const auto started = std::chrono::steady_clock::now();

  // Given a universe seeded with 20260910
  Game game = Game::scenario(20260910);
  const double gameStart = game.time();

  // When I spawn 1 ship with role "pirate"
  EXPECT_EQ(game.spawn(ShipsWithRole("pirate").count(1)).size(), 1u);
  // And I spawn 8 ships with role "police" within 10 km
  EXPECT_EQ(game.spawn(ShipsWithRole("police").count(8).withinKm(10)).size(), 8u);
  // And the simulation runs for 900 ticks
  game.simulate(ticks(900));

  // Then a ship with role "police" survives
  EXPECT_TRUE(game.countWithRole("police") > 0);
  // And no ERROR appears in the log
  EXPECT_TRUE(game.logLinesContaining("ERROR").empty());
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
