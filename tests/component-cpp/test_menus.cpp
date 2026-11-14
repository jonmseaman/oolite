/*	tests/component-cpp/test_menus.cpp
        The start screen and the menus beneath it, driven by key events pushed
   onto SDL's queue and read by the game's own pollControls() (spec 011 US3).
   The game has a view with no window and no GL context.
   Run: tests/run.sh component --cpp -k menus
*/

#include <chrono>

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

OO_COMPONENT_MAIN()
