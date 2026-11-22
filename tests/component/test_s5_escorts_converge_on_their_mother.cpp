/*	tests/component/test_s5_escorts_converge_on_their_mother.cpp
        Scenario S5 ("Escorts converge on their mother: A mother ship and her escorts fly without error").
   Each check carries the scenario step it checks.
   Run: tests/run.sh component --cpp -k s5_
*/

#include <chrono>
#include <cstdio>
#include <string>

#include <gtest/gtest.h>

#include "oo_component.hpp"

using oo::component::Game;
using oo::component::ShipsWithRole;
using oo::component::ticks;

TEST(s5_escorts_converge_on_their_mother, a_mother_ship_and_her_escorts_fly_without_error) {
  const auto started = std::chrono::steady_clock::now();

  // Given a universe seeded with 20260910
  Game game = Game::scenario(20260910);
  const double gameStart = game.time();

  // When I spawn 1 ship with role "trader"
  EXPECT_EQ(game.spawn(ShipsWithRole("trader").count(1)).size(), 1u);
  // And I spawn 2 ships with role "escort" within 5 km
  EXPECT_EQ(game.spawn(ShipsWithRole("escort").count(2).withinKm(5)).size(), 2u);
  // And the simulation runs for at most 900 ticks
  game.simulateAtMost(ticks(900));

  // Then a ship with role "escort" survives
  EXPECT_TRUE(game.countWithRole("escort") > 0);
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
