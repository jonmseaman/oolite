/*	tests/component/test_ship_library.cpp
	The ship library through the game: opened from the start screen, driven by
	its keys, its view-model (src/UI) and the classic grid agreeing, and left
	with space. The game has no window, so the classic grid is the view.
	Run: tests/run.sh component -k ship_library
*/

#include <chrono>
#include <cstddef>
#include <string>

#include <gtest/gtest.h>

#include "Universe.h"
#include "UI/ShipLibrary/ShipLibraryViewModel.h"
#include "oo_component.hpp"

using namespace std::chrono_literals;
using oo::component::Game;

namespace {

constexpr const char* kLibrary = "GUI_SCREEN_SHIPLIBRARY";

oo::ui::ShipLibraryViewModel* Library() {
  return UNIVERSE != nullptr ? UNIVERSE->shipLibrary() : nullptr;
}

// Waits for the ship asked for last to fly in (fly-out 0.25 s, fly-in 0.4 s).
bool Arrives(Game& game, std::size_t category, std::size_t ship) {
  return game.simulateUntil(
      [&] {
        const oo::ui::ShipLibraryViewModel* library = Library();
        return library != nullptr && library->model().category == category &&
               library->model().ship == ship;
      },
      3s);
}

}  // namespace

TEST(ship_library, opens_from_the_start_screen_with_a_view_model) {
  Game game = Game::atStartScreen(20261122);
  game.simulate(1s);
  EXPECT_EQ(game.screen(), std::string("GUI_SCREEN_INTRO1"));
  EXPECT_EQ(Library(), nullptr);
  ASSERT_TRUE(game.selectRowWithText("View Ship Library"));
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == kLibrary);
  const oo::ui::ShipLibraryViewModel* library = Library();
  ASSERT_NE(library, nullptr);
  EXPECT_FALSE(library->empty());
  // It opens on the ships, at the Cobra III where the library has one.
  EXPECT_EQ(library->model().categories[library->model().category].id, "ship");
  EXPECT_FALSE(library->model().sheet.category_singular.empty());
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(ship_library, the_grid_shows_the_view_models_sheet) {
  Game game = Game::current();
  ASSERT_EQ(game.screen(), std::string(kLibrary));
  const oo::ui::ShipLibraryViewModel* library = Library();
  ASSERT_NE(library, nullptr);
  EXPECT_EQ(game.rowText(1), library->model().sheet.category_singular);
  EXPECT_EQ(game.rowText(19), "<-- " + library->PreviousCategoryName());
}

TEST(ship_library, down_and_up_fly_in_the_next_and_previous_ship) {
  Game game = Game::current();
  const oo::ui::ShipLibraryViewModel* library = Library();
  ASSERT_NE(library, nullptr);
  const std::size_t category = library->model().category;
  const std::size_t count = library->ships().size();
  ASSERT_GT(count, 1u);
  const std::size_t first = library->model().ship;
  game.press("down");
  EXPECT_TRUE(Arrives(game, category, (first + 1) % count));
  game.press("up");
  EXPECT_TRUE(Arrives(game, category, first));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(ship_library, right_and_left_change_category_at_its_first_ship) {
  Game game = Game::current();
  const oo::ui::ShipLibraryViewModel* library = Library();
  ASSERT_NE(library, nullptr);
  const std::size_t categories = library->model().categories.size();
  ASSERT_GT(categories, 1u);
  const std::size_t category = library->model().category;
  game.press("right");
  EXPECT_TRUE(Arrives(game, (category + 1) % categories, 0));
  game.press("left");
  EXPECT_TRUE(Arrives(game, category, 0));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(ship_library, up_from_the_first_ship_wraps_to_the_last) {
  Game game = Game::current();
  const oo::ui::ShipLibraryViewModel* library = Library();
  ASSERT_NE(library, nullptr);
  ASSERT_EQ(library->model().ship, 0u);
  const std::size_t category = library->model().category;
  const std::size_t count = library->ships().size();
  game.press("up");
  EXPECT_TRUE(Arrives(game, category, count - 1));
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(ship_library, the_view_models_exit_returns_to_the_start_screen) {
  Game game = Game::current();
  oo::ui::ShipLibraryViewModel* library = Library();
  ASSERT_NE(library, nullptr);
  library->Exit();  // the RmlUi look's Back control
  EXPECT_TRUE(game.simulateUntil([&] { return game.screen() == "GUI_SCREEN_INTRO1"; }, 1s));
  EXPECT_EQ(Library(), nullptr);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

TEST(ship_library, space_leaves_and_closes_the_view_model) {
  Game game = Game::current();
  ASSERT_TRUE(game.selectRowWithText("View Ship Library"));
  OO_COMPONENT_PRESS_EXPECT(game, "enter", game.screen() == kLibrary);
  ASSERT_NE(Library(), nullptr);
  OO_COMPONENT_PRESS_EXPECT(game, "space", game.screen() == "GUI_SCREEN_INTRO1");
  EXPECT_EQ(Library(), nullptr);
  OO_COMPONENT_CHECK_NO_ERRORS(game);
}

OO_COMPONENT_MAIN()
