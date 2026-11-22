/*	tests/component/test_boot.cpp
        The headless game boots: one game, made by the first case, with no
   window, no GL context and no sound device (on Linux and macOS the GL trap
   is linked, so a GL call would fail it by name), the player in flight in a system cleared down
   to the player, add-ons off; and stepping it advances game time without
   waiting on the clock. Later cases reach the same game with
   Game::current().
   Run: meson test -C build/meson_dev test_boot
*/

#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "GameController.h"
#include "Headless.h"
#include "OOJSEngineCore.h"
#include "Sound.h"
#include "PlayerEntity.h"
#include "ResourceManager.h"
#include "Universe.h"
#include "oo_component.hpp"
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

TEST(boot, the_universe_exists) {
  // CI's Linux and macOS steps require the first form in this program's log.
  std::printf("%s\n", oo::component::glTrapState().c_str());
  Game game = Game::scenario(kSeed);
  EXPECT_TRUE(game.universe() != nullptr);
  EXPECT_TRUE(game.player() != nullptr);
}

TEST(boot, the_system_is_empty) {
  Game game = Game::current();
  // The scenarios' measure: only the player is left among the ships.
  EXPECT_EQ(game.eval("system.allShips.length"), 1.0);
}

TEST(boot, the_player_is_in_flight) {
  Game game = Game::current();
  EXPECT_TRUE(game.player()->status() == oo::EntityStatus::kInFlight);
  EXPECT_EQ(game.eval("player.ship.docked ? 1 : 0"), 0.0);
}

TEST(boot, the_resource_search_path_has_one_entry) {
  const std::vector<std::string> paths = ResourceManager::paths();
  EXPECT_EQ(paths.size(), static_cast<std::size_t>(1));
  for (const std::string& path : paths) std::printf("search path: %s\n", path.c_str());
}

TEST(boot, user_directories_are_under_the_scratch_directory) {
  const std::filesystem::path scratch = Game::scratchDirectory();
  EXPECT_TRUE(!scratch.empty());
  const oo::ResourcePaths paths = oo::ResourcePaths::current();
  EXPECT_TRUE(IsUnder(paths.homeDirectory(), scratch));
  EXPECT_TRUE(IsUnder(paths.saveDirectory(), scratch));
  EXPECT_TRUE(IsUnder(paths.snapshotDirectory(), scratch));
  EXPECT_TRUE(IsUnder(paths.logDirectory(), scratch));
  EXPECT_TRUE(IsUnder(paths.managedAddOnsDirectory(), scratch));
  EXPECT_TRUE(IsUnder(paths.extractAddOnsDirectory(), scratch));
  for (const std::filesystem::path& extra : paths.additionalAddOnsDirectories()) {
    EXPECT_TRUE(IsUnder(extra, scratch));
  }
  EXPECT_TRUE(IsUnder(oo::userDefaultsDirectory(paths), scratch));
}

TEST(boot, six_hundred_steps_advance_game_time_by_10_s) {
  Game game = Game::current();
  const double before = game.time();
  const auto started = std::chrono::steady_clock::now();
  game.step(600);
  const double real =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - started)
          .count();
  EXPECT_TRUE(std::fabs(game.time() - before - 10.0) < 1e-6);
  // The measurement spec 011 T015 records (research R12).
  std::printf("test_boot: startup %.2f s, %.0f steps/s, %.0fx real time\n",
              Game::startupSeconds(), 600.0 / real, 10.0 / real);
}

// The scenario game loads the standard save, which sets the ship clock, so
// the clock's start is not visible here (a new commander's is: T045). What
// is: the game clock's time of day comes from the seed, and the ship clock
// follows game time.
TEST(boot, the_ship_clock_starts_from_the_seed) {
  Game game = Game::current();
  const oo::GameClock& clock = game.controller()->clock();
  EXPECT_TRUE(clock.isManual());
  const double expected =
      oo::GameClock::manualTimeOfDayStart(kSeed) + clock.monotonicSeconds();
  EXPECT_TRUE(std::fabs(clock.timeOfDay() - std::fmod(expected, 86400.0)) < 1e-6);
  const double shipBefore = game.player()->clockTime();
  const double gameBefore = game.time();
  game.step(60);
  const double shipElapsed = game.player()->clockTime() - shipBefore;
  const double gameElapsed = game.time() - gameBefore;
  std::printf("ship clock moved %.6f s in %.6f s of game time\n", shipElapsed,
              gameElapsed);
  // The ship clock is about 1.8e11 s, so its doubles resolve ~3e-5 s.
  EXPECT_TRUE(std::fabs(shipElapsed - gameElapsed) < 1e-3);
}

TEST(boot, a_deferred_call_fires_on_game_time) {
  Game game = Game::current();
  bool fired = false;
  oo::ScheduleDeferredCall([&fired] { fired = true; }, 2.0);
  game.step(119);
  EXPECT_TRUE(!fired);
  game.step(1);
  EXPECT_TRUE(fired);
}

// A headless game's Math.random draws from OOSeededRandom seeded with the
// game's seed: the next three values are the generator's next three draws,
// after the draws the core scripts have already taken.
TEST(boot, math_random_draws_from_the_seed) {
  Game game = Game::current();
  const std::uint64_t taken = OOJSSeededMathRandomDraws();
  oo::SeededRandom expected(kSeed);
  for (std::uint64_t i = 0; i < taken; ++i) expected.nextDouble();
  std::printf("Math.random drawn %llu times before this case\n",
              static_cast<unsigned long long>(taken));
  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ(game.eval("Math.random()"), expected.nextDouble());
  }
}

// The observer hears ship script events as they are dispatched.
TEST(boot, a_spawned_ship_raises_shipSpawned) {
  Game game = Game::current();
  const std::size_t mark = game.events().size();
  EXPECT_EQ(game.eval("system.addShips(\"[adder]\", 1, "
                        "player.ship.position.add([0, 0, 5000]), 0).length"),
              1.0);
  game.step();
  const oo::component::Event* spawned = nullptr;
  for (std::size_t i = mark; i < game.events().size(); ++i) {
    if (game.events()[i].name == "shipSpawned") {
      spawned = &game.events()[i];
      break;
    }
  }
  EXPECT_TRUE(spawned != nullptr);
  EXPECT_TRUE(spawned != nullptr && spawned->subjectId != 0);
  EXPECT_TRUE(spawned != nullptr && spawned->subjectKey == "adder");
}

// Run before the case that throws on purpose.
TEST(boot, a_clean_run_has_no_errors) {
  Game game = Game::current();
  game.simulate(oo::component::GameTime{5.0});
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(boot, a_thrown_script_error_is_an_error_entry) {
  Game game = Game::current();
  const std::size_t mark = game.events().size();
  game.eval("(function () { var id = addFrameCallback(function () {"
            " removeFrameCallback(id);"
            " throw new Error('a component test throws on purpose'); });"
            " return 1; })()");
  game.step(2);
  const std::vector<oo::component::Event> errors = game.errorsSince(mark);
  EXPECT_TRUE(!errors.empty());
  bool named = false;
  for (const oo::component::Event& event : errors) {
    std::printf("error entry: [%s] %s\n", event.name.c_str(), event.text.c_str());
    if (event.text.find("a component test throws on purpose") != std::string::npos)
      named = true;
  }
  EXPECT_TRUE(named);
}

// US1 scenario 4: what the game would have drawn and played is recorded.
TEST(boot, frames_and_sounds_asked_for_are_recorded) {
  Game game = Game::current();
  const std::uint64_t frames = game.presentation().framesRequested;
  const std::size_t sounds = game.presentation().soundsRequested.size();
  game.step(10);
  EXPECT_EQ(game.presentation().framesRequested, frames + 10);
  game.eval("(function () { var s = new SoundSource; s.sound = 'beep.ogg';"
            " s.play(); return 1; })()");
  EXPECT_EQ(game.presentation().soundsRequested.size(), sounds + 1);
  if (game.presentation().soundsRequested.size() == sounds + 1) {
    EXPECT_EQ(game.presentation().soundsRequested.back().name,
                std::string("beep.ogg"));
  }
}

// The scenario steps (T023). They spawn traders and pirates, so they come
// last.
TEST(boot, spawn_returns_the_ships_it_added) {
  Game game = Game::current();
  // Each boa comes with escorts, some of which have the trader role as well,
  // so countWithRole("trader") grows by more than two; what spawn hands back
  // is the two ships addShips returned.
  const std::vector<oo::component::ShipHandle> traders =
      game.spawn(oo::component::ShipsWithRole("trader").count(2));
  EXPECT_EQ(traders.size(), static_cast<std::size_t>(2));
  for (const oo::component::ShipHandle& ship : traders) {
    EXPECT_TRUE(ship.alive());
    EXPECT_TRUE(ship.id() != 0);
    EXPECT_EQ(ship.key(), std::string("boa"));
  }
  EXPECT_TRUE(game.countWithRole("trader") >= 2);
}

TEST(boot, spawned_ships_carry_the_scenario_role) {
  Game game = Game::current();
  const std::vector<oo::component::ShipHandle> pirates =
      game.spawn(oo::component::ShipsWithRole("pirate"));
  EXPECT_EQ(pirates.size(), static_cast<std::size_t>(1));
  for (const oo::component::ShipHandle& ship : pirates) {
    EXPECT_TRUE(ship->getPrimaryRole().value_or("") == "pirate");  // not "[sidewinder]"
    EXPECT_EQ(ship.key(), std::string("sidewinder"));     // the pinned key
  }
  EXPECT_EQ(game.eval("system.shipsWithPrimaryRole('pirate')[0].bounty"), 100.0);
}

TEST(boot, within_km_is_measured_from_the_first_spawn) {
  Game game = Game::current();
  EXPECT_TRUE(game.locus().has_value());
  const std::vector<oo::component::ShipHandle> police =
      game.spawn(oo::component::ShipsWithRole("police").count(3).withinKm(10));
  EXPECT_EQ(police.size(), static_cast<std::size_t>(3));
  if (!game.locus().has_value()) return;
  const std::array<double, 3> locus = *game.locus();
  for (const oo::component::ShipHandle& ship : police) {
    const HPVector p = ship->getPosition();
    const double dx = p.x - locus[0], dy = p.y - locus[1], dz = p.z - locus[2];
    const double km = std::sqrt(dx * dx + dy * dy + dz * dz) / 1000.0;
    std::printf("police %u is %.2f km from the locus\n", ship.id(), km);
    EXPECT_TRUE(km <= 10.5);
  }
}

// The plain style (T027): a ship by key, and what a handle does.
TEST(boot, a_removed_ship_is_gone_for_its_handle) {
  Game game = Game::current();
  const std::size_t before = game.entityCount();
  oo::component::ShipHandle adder =
      game.add(oo::component::ShipWithKey("adder").aheadOf(game.player(), 3000.0));
  EXPECT_TRUE(adder.alive());
  EXPECT_TRUE(game.entityCount() > before);
  const std::uint32_t id = adder.id();
  game.remove(adder);
  EXPECT_TRUE(!adder.alive());
  EXPECT_TRUE(static_cast<ShipEntity*>(adder) == nullptr);
  EXPECT_EQ(adder.id(), id);  // kept, so events can still be matched
  EXPECT_EQ(game.entityCount(), before);
}

TEST(boot, a_handle_does_not_keep_a_ship_alive) {
  Game game = Game::current();
  std::shared_ptr<ShipEntity> ship =
      game.spawnKey("adder", game.player()->getPosition());
  EXPECT_TRUE(ship != nullptr);
  if (ship == nullptr) return;
  const long owners = ship.use_count();
  {
    const oo::component::ShipHandle handle(ship);
    EXPECT_EQ(ship.use_count(), owners);
    EXPECT_TRUE(handle.alive());
  }
  game.remove(ship.get());
}

OO_COMPONENT_MAIN()
