/*	tests/component-cpp/test_boot.cpp
        The headless game boots: one game, made by the first case, with no
   window and no sound device, the player in flight in a system cleared down
   to the player, add-ons off; and stepping it advances game time without
   waiting on the clock. Later cases reach the same game with
   Game::current().
   Run: meson test -C build/meson_dev test_boot
*/

#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include "PlayerEntity.h"
#include "ResourceManager.h"
#include "Universe.h"
#include "oo_component.hpp"
#include "oo_test.hpp"
#include "oofnd/Defaults.hpp"
#include "oofnd/ResourcePaths.hpp"

using oo::component::Game;

namespace {

constexpr std::uint32_t kSeed = 20260910;

// Whether path is inside directory (both made absolute and normal).
bool IsUnder(const std::filesystem::path& path,
             const std::filesystem::path& directory) {
  const std::filesystem::path p =
      std::filesystem::absolute(path).lexically_normal();
  const std::filesystem::path d =
      std::filesystem::absolute(directory).lexically_normal();
  const std::string ps = p.generic_string(), ds = d.generic_string();
  return ps.size() >= ds.size() && ps.compare(0, ds.size(), ds) == 0;
}

}  // namespace

OO_TEST(the_universe_exists) {
  Game game = Game::scenario(kSeed);
  OO_CHECK(game.universe() != nullptr);
  OO_CHECK(game.player() != nullptr);
}

OO_TEST(the_system_is_empty) {
  Game game = Game::current();
  // The windowed steps' measure: only the player is left among the ships.
  OO_CHECK_EQ(game.eval("system.allShips.length"), 1.0);
}

OO_TEST(the_player_is_in_flight) {
  Game game = Game::current();
  OO_CHECK(game.player()->status() == OOEntityStatus::kInFlight);
  OO_CHECK_EQ(game.eval("player.ship.docked ? 1 : 0"), 0.0);
}

OO_TEST(the_resource_search_path_has_one_entry) {
  const std::vector<std::string> paths = ResourceManager::paths();
  OO_CHECK_EQ(paths.size(), static_cast<std::size_t>(1));
  for (const std::string& path : paths) std::printf("search path: %s\n", path.c_str());
}

OO_TEST(user_directories_are_under_the_scratch_directory) {
  const std::filesystem::path scratch = Game::scratchDirectory();
  OO_CHECK(!scratch.empty());
  const oo::ResourcePaths paths = oo::ResourcePaths::current();
  OO_CHECK(IsUnder(paths.homeDirectory(), scratch));
  OO_CHECK(IsUnder(paths.saveDirectory(), scratch));
  OO_CHECK(IsUnder(paths.snapshotDirectory(), scratch));
  OO_CHECK(IsUnder(paths.logDirectory(), scratch));
  OO_CHECK(IsUnder(paths.managedAddOnsDirectory(), scratch));
  OO_CHECK(IsUnder(paths.extractAddOnsDirectory(), scratch));
  for (const std::filesystem::path& extra : paths.additionalAddOnsDirectories()) {
    OO_CHECK(IsUnder(extra, scratch));
  }
  OO_CHECK(IsUnder(oo::userDefaultsDirectory(paths), scratch));
}

OO_TEST(six_hundred_steps_advance_game_time_by_10_s) {
  Game game = Game::current();
  const double before = game.time();
  game.step(600);
  OO_CHECK(std::fabs(game.time() - before - 10.0) < 1e-6);
}

OO_COMPONENT_MAIN()
