/*	tests/component-cpp/test_menus.cpp
        The start screen and the menus beneath it, driven by key events pushed
   onto SDL's queue and read by the game's own pollControls() (spec 011 US3).
   The game has a view with no window and no GL context.
   Run: tests/run.sh component --cpp -k menus
*/

#include <SDL3/SDL.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string>

#include "MyOpenGLView.h"
#include "oo_component.hpp"
#include "oo_test.hpp"

using namespace std::chrono_literals;
using oo::component::Game;

OO_TEST(the_start_screen_shows) {
  Game game = Game::atStartScreen(20260910);
  game.simulate(2s);
  OO_CHECK_EQ(game.screen(), std::string("GUI_SCREEN_INTRO1"));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_TEST(the_default_lookup_is_sdls_own) {
  Game game = Game::current();
  OO_CHECK(game.view() != nullptr);
  if (game.view() == nullptr) return;
  const SDL_Keymod mods[] = {SDL_KMOD_NONE, SDL_KMOD_SHIFT, SDL_KMOD_CAPS};
  int differ = 0;
  for (int code = 0; code < SDL_SCANCODE_COUNT; ++code) {
    for (SDL_Keymod mod : mods) {
      const auto scancode = static_cast<SDL_Scancode>(code);
      const SDL_Keycode ours = game.view()->keyLookup(scancode, mod);
      const SDL_Keycode sdls = SDL_GetKeyFromScancode(scancode, mod, false);
      if (ours != sdls) {
        if (differ++ < 5) {
          std::printf("     scancode %d mods 0x%x: 0x%x, SDL 0x%x\n", code,
                      static_cast<unsigned>(mod), static_cast<unsigned>(ours),
                      static_cast<unsigned>(sdls));
        }
      }
    }
  }
  OO_CHECK_EQ(differ, 0);
}

OO_TEST(us_tsv_agrees_with_sdls_built_in_map) {
  // Under the dummy video driver SDL answers from its own built-in US map.
  const oo::component::Layout us = oo::component::Layout::load("us");
  const SDL_Keymod mods[] = {SDL_KMOD_NONE, SDL_KMOD_LSHIFT, SDL_KMOD_CAPS,
                             static_cast<SDL_Keymod>(SDL_KMOD_LSHIFT | SDL_KMOD_CAPS),
                             SDL_KMOD_MODE};
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
  OO_CHECK_EQ(differ, 0);
  OO_CHECK_EQ(us.rows().size(), static_cast<std::size_t>(rows));
}

// US3 scenario 1.
OO_TEST(down_up_and_enter_move_and_open) {
  Game game = Game::current();
  OO_CHECK_EQ(game.screen(), std::string("GUI_SCREEN_INTRO1"));
  const long first = game.selectedRow();
  OO_CHECK(first >= 0);
  OO_COMPONENT_PRESS_EXPECT(game, "down", game.selectedRow() == first + 1);
  OO_COMPONENT_PRESS_EXPECT(game, "up", game.selectedRow() == first);
  const std::string opens = game.rowText(first);
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() != "GUI_SCREEN_INTRO1");
  std::printf("     enter on \"%s\" opened %s\n", opens.c_str(), game.screen().c_str());
  // The new-game screen is left by its own "exit" row, as a player leaves it.
  OO_CHECK_EQ(game.screen(), std::string("GUI_SCREEN_NEWGAME"));
  game.selectRowWithKey("exit");
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == "GUI_SCREEN_INTRO1");
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// US3 scenario 5: a control that acts once per press acts once however long
// the key is held. Enter on "Start New Commander" opens the new-game screen;
// were the held key seen again, it would start the scenario selected there.
OO_TEST(a_held_enter_acts_once) {
  Game game = Game::current();
  OO_CHECK_EQ(game.screen(), std::string("GUI_SCREEN_INTRO1"));
  game.selectRowWithText("Start New Commander");
  game.hold("enter", [] { return false; }, std::chrono::seconds(3));
  const std::string opened = game.screen();
  OO_CHECK_EQ(opened, std::string("GUI_SCREEN_NEWGAME"));
  game.simulate(std::chrono::seconds(1));
  OO_CHECK_EQ(game.screen(), opened);
  game.selectRowWithKey("exit");
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == "GUI_SCREEN_INTRO1");
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
