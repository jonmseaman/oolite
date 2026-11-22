/*	tests/component/test_menus.cpp
        The start screen and the menus beneath it, driven by key events pushed
   onto SDL's queue and read by the game's own pollControls() (spec 011 US3).
   The game has a view with no window and no GL context.
   Run: tests/run.sh component --cpp -k menus
*/

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "GameController.h"
#include "MyOpenGLView.h"
#include "LogOutputHandler.h"
#include "oofnd/ResourcePaths.hpp"
#include "oo_component.hpp"

using namespace std::chrono_literals;
using oo::component::Game;

TEST(menus, the_start_screen_shows) {
  Game game = Game::atStartScreen(20260910);
  game.simulate(2s);
  EXPECT_EQ(game.screen(), std::string("GUI_SCREEN_INTRO1"));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(menus, the_default_lookup_is_sdls_own) {
  Game game = Game::current();
  EXPECT_TRUE(game.view() != nullptr);
  if (game.view() == nullptr) return;
  // Run with --layout, the view's lookup is that layout's table instead.
  std::optional<oo::component::Layout> layout;
  if (game.layoutName() != "SDL") layout = oo::component::Layout::load(game.layoutName());
  const SDL_Keymod mods[] = {SDL_KMOD_NONE, SDL_KMOD_SHIFT, SDL_KMOD_CAPS};
  int differ = 0;
  for (int code = 0; code < SDL_SCANCODE_COUNT; ++code) {
    for (SDL_Keymod mod : mods) {
      const auto scancode = static_cast<SDL_Scancode>(code);
      const SDL_Keycode ours = game.view()->keyLookup(scancode, mod);
      const SDL_Keycode sdls = layout.has_value()
                                   ? static_cast<SDL_Keycode>(layout->keyFor(code, mod))
                                   : SDL_GetKeyFromScancode(scancode, mod, false);
      if (ours != sdls) {
        if (differ++ < 5) {
          std::printf("     scancode %d mods 0x%x: 0x%x, SDL 0x%x\n", code,
                      static_cast<unsigned>(mod), static_cast<unsigned>(ours),
                      static_cast<unsigned>(sdls));
        }
      }
    }
  }
  EXPECT_EQ(differ, 0);
}

TEST(menus, us_tsv_agrees_with_sdls_built_in_map) {
  // Under the dummy video driver SDL answers from its own built-in US map.
  const oo::component::Layout us = oo::component::Layout::load("us");
  // The modifier sets the default lookup is checked under (the case above).
  // Shift with Caps Lock is left out: SDL's own built-in map answers it
  // differently by platform (A on macOS, a on Windows and Linux), so no one
  // table can agree with it everywhere (tasks.md, Findings -> Keys).
  const SDL_Keymod mods[] = {SDL_KMOD_NONE, SDL_KMOD_LSHIFT, SDL_KMOD_CAPS};
  int differ = 0;
  int rows = 0;
  for (int code = 1; code < SDL_SCANCODE_COUNT; ++code) {
    for (SDL_Keymod mod : mods) {
      const SDL_Keycode sdls =
          SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(code), mod, false);
      const std::uint32_t table = us.keyFor(code, mod);
      if (sdls != SDLK_UNKNOWN) ++rows;
      if (table != sdls && differ++ < 5) {
        std::printf("     scancode %d mods 0x%x: us.tsv 0x%x, SDL 0x%x\n", code,
                    static_cast<unsigned>(mod), static_cast<unsigned>(table),
                    static_cast<unsigned>(sdls));
      }
    }
  }
  EXPECT_EQ(differ, 0);
  std::size_t tableRows = 0;
  for (const auto& [where, keycode] : us.rows()) {
    if (where.second <= 2) ++tableRows;  // -, shift, caps
  }
  EXPECT_EQ(tableRows, static_cast<std::size_t>(rows));
}

TEST(menus, every_layout_file_loads) {
  using oo::component::Layout;
  for (const char* name : {"us", "fr_azerty", "ru"}) {
    const Layout layout = Layout::load(name);
    EXPECT_TRUE(layout.rows().size() > 100);
  }
  // Letters move on AZERTY: the key in the US Q position gives a.
  EXPECT_EQ(Layout::load("fr_azerty").keyFor(SDL_SCANCODE_Q, SDL_KMOD_NONE), 0x61u);
  // A non-Latin layout: the N key gives a code point above the game's 326 keys.
  EXPECT_TRUE(Layout::load("ru").keyFor(SDL_SCANCODE_N, SDL_KMOD_NONE) > 326u);
}

// US3 scenario 1.
TEST(menus, down_up_and_enter_move_and_open) {
  Game game = Game::current();
  EXPECT_EQ(game.screen(), std::string("GUI_SCREEN_INTRO1"));
  const long first = game.selectedRow();
  EXPECT_TRUE(first >= 0);
  OO_COMPONENT_PRESS_EXPECT(game, "down", game.selectedRow() == first + 1);
  OO_COMPONENT_PRESS_EXPECT(game, "up", game.selectedRow() == first);
  const std::string opens = game.rowText(first);
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() != "GUI_SCREEN_INTRO1");
  std::printf("     enter on \"%s\" opened %s\n", opens.c_str(), game.screen().c_str());
  // The new-game screen is left by its own "exit" row, as a player leaves it.
  EXPECT_EQ(game.screen(), std::string("GUI_SCREEN_NEWGAME"));
  game.selectRowWithKey("exit");
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == "GUI_SCREEN_INTRO1");
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// US3 scenario 5: a control that acts once per press acts once however long
// the key is held. Enter on "Start New Commander" opens the new-game screen;
// were the held key seen again, it would start the scenario selected there.
TEST(menus, a_held_enter_acts_once) {
  Game game = Game::current();
  EXPECT_EQ(game.screen(), std::string("GUI_SCREEN_INTRO1"));
  game.selectRowWithText("Start New Commander");
  game.hold("enter", [] { return false; }, std::chrono::seconds(3));
  const std::string opened = game.screen();
  EXPECT_EQ(opened, std::string("GUI_SCREEN_NEWGAME"));
  game.simulate(std::chrono::seconds(1));
  EXPECT_EQ(game.screen(), opened);
  game.selectRowWithKey("exit");
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == "GUI_SCREEN_INTRO1");
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

namespace {

constexpr const char* kStartScreen = "GUI_SCREEN_INTRO1";
constexpr const char* kGameOptions = "GUI_SCREEN_GAMEOPTIONS";

void PrintRows(Game& game) {
  std::printf("     %s, %zu selectable rows:", game.screen().c_str(),
              game.selectableRows().size());
  for (long row : game.selectableRows()) {
    std::printf(" [%ld %s \"%s\"]", row, game.rowKey(row).c_str(),
                game.rowText(row).c_str());
  }
  std::printf("\n");
}

// Leaves a screen the way a player leaves it, by key: each screen's own way
// out, as the windowed GUI tier found them (tests/gui/test_g5_screen_round_trips.py)
// and as the screens' handlers in PlayerEntityControls.cpp read the keys.
// Never activates any other row (the expansion manager's included: hard rule 4).
bool BackOut(Game& game) {
  const std::string screen = game.screen();
  if (screen == "GUI_SCREEN_NEWGAME") {
    if (!game.selectRowWithKey("exit")) return false;
    game.press("enter");
  } else if (screen == "GUI_SCREEN_SHIPLIBRARY" || screen == "GUI_SCREEN_KEYBOARD") {
    game.press("space");  // both screens' handlers leave on space
  } else if (screen == "GUI_SCREEN_OXZMANAGER") {
    if (!game.selectRowWithKey("_EXIT")) return false;
    game.press("enter");
  } else if (screen == kGameOptions) {
    const std::vector<long> rows = game.selectableRows();
    if (rows.empty() || !game.selectRow(rows.back())) return false;  // Back
    game.press("enter");
  } else {
    game.press("escape");
  }
  return game.screen() != screen;
}

// Opens each selectable row of the showing screen with enter, checks what
// showed, backs out by key and checks the game is back where it was. A row
// whose enter shows nothing new (a value row: left and right change it) is
// counted as visited where it stands. skip: rows left out (the exit rows).
// Returns how many rows were visited.
std::size_t WalkRows(Game& game, const std::vector<long>& skip) {
  const std::string home = game.screen();
  const std::vector<long> rows = game.selectableRows();
  std::size_t visited = 0;
  for (long row : rows) {
    bool skipped = false;
    for (long s : skip) skipped = skipped || s == row;
    if (skipped) continue;
    EXPECT_TRUE(game.selectRow(row));
    const std::string text = game.rowText(row);
    game.press("enter");
    const std::string opened = game.screen();
    std::printf("     %s row %ld \"%s\": %s\n", home.c_str(), row, text.c_str(),
                opened.c_str());
    ++visited;
    if (opened == home) continue;
    PrintRows(game);
    EXPECT_TRUE(BackOut(game));
    if (game.screen() != home) {
      std::printf("     backing out of %s reached %s, not %s\n", opened.c_str(),
                  game.screen().c_str(), home.c_str());
      EXPECT_EQ(game.screen(), home);
      return visited;
    }
  }
  return visited;
}

}  // namespace

// US3 scenario 2 (FR-024, FR-025, SC-006): every row the start screen reports,
// and every row of the options screen beneath it, opened and backed out of by
// key. The rows are the game's own, never listed here.
TEST(menus, every_start_screen_row_opens_and_returns) {
  Game game = Game::current();
  EXPECT_EQ(game.screen(), std::string(kStartScreen));
  PrintRows(game);
  const std::vector<long> rows = game.selectableRows();
  EXPECT_TRUE(rows.size() >= 2);
  if (rows.size() < 2) return;
  const long exitRow = rows.back();  // Exit Game: exit_game_asks_to_quit
  const std::size_t visited = WalkRows(game, {exitRow});
  EXPECT_EQ(visited + 1, rows.size());
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(menus, every_game_options_row_opens_and_returns) {
  Game game = Game::current();
  EXPECT_EQ(game.screen(), std::string(kStartScreen));
  bool opened = false;
  for (long row : game.selectableRows()) {
    game.selectRow(row);
    game.press("enter");
    if (game.screen() == kGameOptions) {
      opened = true;
      break;
    }
    BackOut(game);
  }
  EXPECT_TRUE(opened);
  if (!opened) return;
  PrintRows(game);
  const std::vector<long> rows = game.selectableRows();
  const long backRow = rows.back();  // returns to the start screen
  const std::size_t visited = WalkRows(game, {backRow});
  EXPECT_EQ(visited + 1, rows.size());
  EXPECT_TRUE(BackOut(game));
  EXPECT_EQ(game.screen(), std::string(kStartScreen));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// Every user directory the game resolved is in the test's scratch directory
// (US3 scenario 7: the developer's own saves and settings are untouched).
TEST(menus, every_user_directory_is_under_the_scratch_directory) {
  Game game = Game::current();
  const std::filesystem::path scratch =
      std::filesystem::weakly_canonical(Game::scratchDirectory());
  const oo::ResourcePaths& paths = oo::ResourcePaths::current();
  std::vector<std::pair<const char*, std::filesystem::path>> resolved = {
      {"home", paths.homeDirectory()},
      {"saves", paths.saveDirectory()},
      {"snapshots", paths.snapshotDirectory()},
      {"logs", paths.logDirectory()},
      {"user library", paths.userLibraryDirectory()},
      {"application support", paths.applicationSupportDirectory()},
      {"caches", paths.cachesDirectory()},
      {"managed add-ons", paths.managedAddOnsDirectory()},
      {"add-on extraction", paths.extractAddOnsDirectory()},
  };
  for (const auto& extra : paths.additionalAddOnsDirectories()) {
    resolved.emplace_back("additional add-ons", extra);
  }
  if (const auto logs = oo::LogHandlerGetLogBasePath()) {
    resolved.emplace_back("log base", std::filesystem::path(*logs));
  }
  if (const auto saves = game.controller()->playerFileDirectory()) {
    resolved.emplace_back("player files", std::filesystem::path(*saves));
  }
  for (const auto& [what, path] : resolved) {
    const std::filesystem::path full = std::filesystem::weakly_canonical(path);
    const auto [end, rest] =
        std::mismatch(scratch.begin(), scratch.end(), full.begin(), full.end());
    const bool under = end == scratch.end();
    if (!under) {
      std::printf("     %s: %s is not under %s\n", what, full.generic_string().c_str(),
                  scratch.generic_string().c_str());
    }
    EXPECT_TRUE(under);
  }
}

TEST(menus, exit_game_asks_to_quit) {
  Game game = Game::current();
  EXPECT_EQ(game.screen(), std::string(kStartScreen));
  const std::vector<long> rows = game.selectableRows();
  EXPECT_TRUE(!rows.empty());
  if (rows.empty()) return;
  EXPECT_TRUE(game.selectRow(rows.back()));
  EXPECT_EQ(game.rowText(rows.back()), std::string("Exit Game"));
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.exitRequested());
}

OO_COMPONENT_MAIN()
