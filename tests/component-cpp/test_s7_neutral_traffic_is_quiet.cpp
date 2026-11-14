/*	tests/component-cpp/test_s7_neutral_traffic_is_quiet.cpp
        The headless counterpart of tests/component/features/s7_neutral_traffic_is_quiet.feature
   ("Neutral traffic is quiet: 8 neutral ships run for 900 ticks without incident"): the same seed, the same
   spawns and the same assertions, each through the script of its windowed
   step. Each check carries the line of the feature it ports.
   Run: tests/run.sh component --cpp -k s7_
*/

#include <chrono>
#include <cstdio>
#include <string>

#include "oo_component.hpp"
#include "oo_test.hpp"

using oo::component::Game;
using oo::component::ShipsWithRole;
using oo::component::ticks;

OO_TEST(eight_neutral_ships_run_for_900_ticks_without_incident) {
  const auto started = std::chrono::steady_clock::now();

  // Given a universe seeded with 20260910
  Game game = Game::scenario(20260910);
  const double gameStart = game.time();

  // When I spawn 8 ships with role "shuttle"
  OO_CHECK_EQ(game.spawn(ShipsWithRole("shuttle").count(8)).size(), 8u);
  // And the simulation runs for 900 ticks
  game.simulate(ticks(900));

  // Then a ship with role "shuttle" survives
  OO_CHECK(game.countWithRole("shuttle") > 0);
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
