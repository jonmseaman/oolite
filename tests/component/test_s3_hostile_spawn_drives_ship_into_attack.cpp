/*	tests/component/test_s3_hostile_spawn_drives_ship_into_attack.cpp
        Scenario S3 ("Ship-to-ship combat: A hostile spawn drives a ship into ATTACK; the setup runs cleanly").
   Each check carries the scenario step it checks.
   Run: tests/run.sh component --cpp -k s3_
*/

#include <chrono>
#include <cstdio>
#include <string>

#include <gtest/gtest.h>

#include "oo_component.hpp"

using oo::component::Game;
using oo::component::ShipsWithRole;
using oo::component::ticks;

TEST(s3_hostile_spawn_drives_ship_into_attack, a_hostile_spawn_drives_a_ship_into_attack) {
  const auto started = std::chrono::steady_clock::now();

  // Given a universe seeded with 20260918
  Game game = Game::scenario(20260918);
  const double gameStart = game.time();

  // When I spawn 1 ship with role "trader"
  EXPECT_EQ(game.spawn(ShipsWithRole("trader").count(1)).size(), 1u);
  // And I spawn 1 ship with role "pirate" within 10 km
  EXPECT_EQ(game.spawn(ShipsWithRole("pirate").count(1).withinKm(10)).size(), 1u);
  // And the simulation runs for at most 900 ticks
  game.simulateAtMost(ticks(900));

  // Then a ship with role "trader" survives
  EXPECT_TRUE(game.countWithRole("trader") > 0);
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
