/*	tests/component-cpp/test_menus.cpp
        The start screen and the menus beneath it, driven by key events pushed
   onto SDL's queue and read by the game's own pollControls() (spec 011 US3).
   The game has a view with no window and no GL context.
   Run: tests/run.sh component --cpp -k menus
*/

#include <SDL3/SDL.h>

#include <chrono>
#include <cstdio>

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

OO_COMPONENT_MAIN()
