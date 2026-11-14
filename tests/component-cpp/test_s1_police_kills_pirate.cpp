/*	tests/component-cpp/test_s1_police_kills_pirate.cpp
        The headless counterpart of
   tests/component/features/s1_police_kills_pirate.feature: the same seed, the
   same spawns, the same assertions, each through the script of its windowed
   step (oo_component, from tests/component/steps/world_steps.py). Each check
   carries the line of the feature it ports.

        This is the exemplar for C++ component tests: copy it to write a
   scenario. Its last line says how much faster than real time it ran.
   Run: tests/run.sh component --cpp -k s1_police_kills_pirate
*/

#include <chrono>
#include <cstdio>

#include "oo_component.hpp"
#include "oo_test.hpp"

using oo::component::Game;
using oo::component::ShipsWithRole;
using oo::component::ticks;

OO_TEST(a_police_viper_engages_a_lone_pirate) {
  const auto started = std::chrono::steady_clock::now();

  // Given a universe seeded with 20260910
  Game game = Game::scenario(20260910);
  const double gameStart = game.time();

  // When I spawn 1 ship with role "police"
  OO_CHECK_EQ(game.spawn(ShipsWithRole("police").count(1)).size(), 1u);
  // And I spawn 1 ship with role "pirate" within 10 km
  OO_CHECK_EQ(game.spawn(ShipsWithRole("pirate").count(1).withinKm(10)).size(), 1u);

  // Then within 900 ticks a ship with role "police" engages a ship with role "pirate"
  OO_CHECK(game.simulateUntil([&] { return game.engages("police", "pirate"); }, ticks(900)));
  // And a ship with role "police" survives
  OO_CHECK(game.countWithRole("police") > 0);

  OO_COMPONENT_CHECK_NO_ERRORS(game);

  const double real =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
  const double simulated = game.time() - gameStart;
  std::printf("game %.1f s, real %.2f s, %.0fx\n", simulated, real, simulated / real);
}

OO_COMPONENT_MAIN()
