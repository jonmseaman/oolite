/*	tests/component/test_key_faults.cpp
        The things in the key path that look wrong (spec 011 US4, research
   H1-H5), one case each, each saying what a player should see. Each case runs
   in a program of its own (--gtest_filter=key_faults.<name>; meson registers key_fault_h1 ...
   key_fault_h5). A failing case is a finding, recorded in the spec's tasks
   (Findings -> Keys), not a verdict, and is not rewritten to pass.
   H1-H3 use the tutorial's first steps, where the activate-equipment key has
   a visible effect: the tutorial moves on.
   Run: tests/run.sh component --cpp -k key_fault
*/

#include <SDL3/SDL.h>

#include <chrono>
#include <string>

#include <gtest/gtest.h>

#include "MyOpenGLView.h"
#include "oo_component.hpp"
#include "tutorial_steps.hpp"

using namespace std::chrono_literals;
using oo::component::Game;

namespace {

// The physical key that carries the activate-equipment key's letter on a US
// board: the key a player presses, whatever the layout makes of it.
int ActivateScancode(Game& game) {
  const std::string key = game.keyForControl("key_activate_equipment");
  SDL_Keymod mods = SDL_KMOD_NONE;
  return key.size() == 1 ? SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(key[0]), &mods)
                         : SDL_SCANCODE_UNKNOWN;
}

// Presses the activate key with these modifiers and reports whether the
// tutorial moved.
bool ActivateMovesTutorial(Game& game, std::uint16_t mods) {
  const Stage before = Progress(game);
  game.pressScancode(ActivateScancode(game), mods);
  game.simulate(1s);
  return Progress(game) != before;
}

}  // namespace

// H1: with Caps Lock on, the n key still activates.
TEST(key_faults, h1_n_activates_with_caps_lock_on) {
  Game game = Game::atStartScreen(20261027);
  StartTutorial(game);
  EXPECT_TRUE(ActivateMovesTutorial(game, SDL_KMOD_CAPS));
}

// H2: under a Russian layout, the n key still activates.
TEST(key_faults, h2_n_activates_under_ru) {
  Game game = Game::atStartScreen(20261027);
  game.useLayout("ru");
  StartTutorial(game);
  EXPECT_TRUE(ActivateMovesTutorial(game, SDL_KMOD_NONE));
}

// H3: a modifier that is not shift, ctrl or alt in the key setting's sense
// (Num Lock on; AltGr's mode bit) does not stop the n key activating.
TEST(key_faults, h3_n_activates_with_num_lock_or_altgr_bits) {
  Game game = Game::atStartScreen(20261027);
  StartTutorial(game);
  EXPECT_TRUE(ActivateMovesTutorial(game, SDL_KMOD_NUM));
  EXPECT_TRUE(ActivateMovesTutorial(game, SDL_KMOD_MODE));
}

// H4: Caps Lock or Num Lock pressed alone is not seen as another key (9, S).
TEST(key_faults, h4_caps_lock_and_num_lock_are_not_other_keys) {
  Game game = Game::inFlightWithKeys(20260910);
  game.keyDownScancode(SDL_SCANCODE_CAPSLOCK, SDL_KMOD_CAPS);
  game.step(2);
  EXPECT_TRUE(!game.view()->isDown('9'));
  game.keyUpScancode(SDL_SCANCODE_CAPSLOCK, SDL_KMOD_CAPS);
  game.step(2);
  game.keyDownScancode(SDL_SCANCODE_NUMLOCKCLEAR, SDL_KMOD_NUM);
  game.step(2);
  EXPECT_TRUE(!game.view()->isDown('S'));
  game.keyUpScancode(SDL_SCANCODE_NUMLOCKCLEAR, SDL_KMOD_NUM);
  game.step(2);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// H5: every physical key can be pressed and released, scancodes above 326
// included; under the sanitizer build a read outside scancode2Unicode fails
// this case by itself.
TEST(key_faults, h5_every_scancode_can_be_pressed_and_released) {
  Game game = Game::inFlightWithKeys(20260910);
  for (int code = 1; code < SDL_SCANCODE_COUNT; ++code) {
    if (code == SDL_SCANCODE_ESCAPE) continue;  // shift-less escape is the pause key
    game.keyDownScancode(code, SDL_KMOD_NONE);
    game.step();
    game.keyUpScancode(code, SDL_KMOD_NONE);
    game.step();
  }
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
