/*	tests/component/test_launch_key.cpp
        The launch key (spec 011 US3 scenarios 8 and 9, FR-038): a new
   commander docked at the main station holds the game's own launch key until
   the launch is over; it launches exactly once. Pressed again in flight, it
   launches nothing and the launch's break pattern does not play again. First,
   text typed by key on the long-range chart arrives (Game::type).
   Run: tests/run.sh component --cpp -k launch_key
*/

#include <chrono>
#include <cstddef>
#include <string>

#include <gtest/gtest.h>

#include "MyOpenGLView.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "oo_component.hpp"

using namespace std::chrono_literals;
using oo::component::EventKind;
using oo::component::Game;

namespace {

std::size_t Launches(const Game& game) {
  std::size_t count = 0;
  for (const auto& event : game.events().entries()) {
    if (event.kind == EventKind::kWorldEvent && event.name == "shipLaunchedFromStation") {
      ++count;
    }
  }
  return count;
}

bool LaunchIsOver(Game& game) {
  return game.player()->status() == oo::EntityStatus::kInFlight &&
         game.universe()->breakPatternOver();
}

}  // namespace

// The long-range chart takes typed text for its system search: the first
// text-entry screen a docked game reaches by key.
TEST(launch_key, text_typed_on_the_long_range_chart_arrives) {
  Game game = Game::docked(20260910);
  EXPECT_TRUE(game.player()->isDocked());
  const std::string charts = game.keyForControl("key_gui_chart_screens");
  for (int i = 0; i < 2 && game.screen() != "GUI_SCREEN_LONG_RANGE_CHART"; ++i) {
    OO_COMPONENT_PRESS_EXPECT(game, charts, game.screen().find("CHART") != std::string::npos);
  }
  EXPECT_EQ(game.screen(), std::string("GUI_SCREEN_LONG_RANGE_CHART"));
  game.type("lave");
  EXPECT_EQ(game.view()->getTypedString().value_or(""), std::string("lave"));
  const std::string status = game.keyForControl("key_gui_screen_status");
  OO_COMPONENT_PRESS_EXPECT(game, status, game.screen() == "GUI_SCREEN_STATUS");
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// US3 scenario 8.
TEST(launch_key, holding_the_launch_key_launches_once) {
  Game game = Game::current();
  EXPECT_TRUE(game.player()->isDocked());
  EXPECT_EQ(Launches(game), 0u);
  const std::string launch = game.keyForControl("key_launch_ship");
  EXPECT_TRUE(game.hold(launch, [&] { return LaunchIsOver(game); }, 60s));
  EXPECT_TRUE(LaunchIsOver(game));
  EXPECT_EQ(Launches(game), 1u);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

// US3 scenario 9.
TEST(launch_key, pressing_the_launch_key_again_launches_nothing) {
  Game game = Game::current();
  EXPECT_TRUE(LaunchIsOver(game));
  const std::string launch = game.keyForControl("key_launch_ship");
  game.press(launch);
  game.simulate(5s);
  EXPECT_TRUE(game.player()->status() == oo::EntityStatus::kInFlight);
  EXPECT_TRUE(game.universe()->breakPatternOver());
  EXPECT_EQ(game.evalString("player.ship.viewDirection"), std::string("VIEW_FORWARD"));
  EXPECT_EQ(Launches(game), 1u);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
