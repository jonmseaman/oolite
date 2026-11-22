/*	tests/unit/ui/test_ship_library_view_model.cpp
	The ship library's view-model: which ship each command asks the game for,
	what it reports, and when it tells its views. The game is a fake navigator.
	Run: meson test -C build/meson_dev --suite ui
*/

#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "UI/ShipLibrary/ShipLibraryModel.h"
#include "UI/ShipLibrary/ShipLibraryNavigator.h"
#include "UI/ShipLibrary/ShipLibraryView.h"
#include "UI/ShipLibrary/ShipLibraryViewModel.h"

using oo::ui::ShipLibraryCategory;
using oo::ui::ShipLibraryEntry;
using oo::ui::ShipLibraryViewModel;
using oo::ui::ShipSheet;

namespace {

struct Shown {
  std::size_t category;
  std::size_t ship;
  bool operator==(const Shown&) const = default;
};

// The game: records what it was asked to show, and shows it when told to.
class FakeNavigator : public oo::ui::ShipLibraryNavigator {
 public:
  void ShowShip(std::size_t category, std::size_t index) override {
    asked.push_back({category, index});
  }
  void Exit() override { ++exits; }

  std::vector<Shown> asked;
  int exits = 0;
};

class CountingView : public oo::ui::ShipLibraryView {
 public:
  void Refresh() override { ++refreshes; }
  int refreshes = 0;
};

ShipLibraryCategory Category(const std::string& id, std::size_t ships) {
  ShipLibraryCategory category;
  category.id = id;
  category.plural_name = id + "s";
  for (std::size_t i = 0; i < ships; ++i) {
    category.entries.push_back({id + std::to_string(i), id + " " + std::to_string(i)});
  }
  return category;
}

// Three categories of 12, 1 and 3 ships, showing <category>, <ship>.
struct Library {
  explicit Library(std::size_t category = 0, std::size_t ship = 0) {
    view_model.SetCategories({Category("ship", 12), Category("station", 1), Category("thing", 3)},
                             category, ship);
    view_model.OnShipShown(category, ship, ShipSheet{});
    view_model.Attach(&view);
  }
  // The game finishes flying in the ship it was asked for last.
  void Arrive() {
    ASSERT_FALSE(navigator.asked.empty());
    const Shown last = navigator.asked.back();
    view_model.OnShipShown(last.category, last.ship, ShipSheet{});
  }

  FakeNavigator navigator;
  ShipLibraryViewModel view_model{navigator};
  CountingView view;
};

}  // namespace

TEST(ShipLibraryViewModel, next_asks_for_the_following_ship) {
  Library library(0, 4);
  library.view_model.NextShip();
  EXPECT_EQ(library.navigator.asked, (std::vector<Shown>{{0, 5}}));
}

TEST(ShipLibraryViewModel, next_wraps_to_the_first_ship) {
  Library library(0, 11);
  library.view_model.NextShip();
  EXPECT_EQ(library.navigator.asked, (std::vector<Shown>{{0, 0}}));
}

TEST(ShipLibraryViewModel, previous_wraps_to_the_last_ship) {
  Library library(0, 0);
  library.view_model.PreviousShip();
  EXPECT_EQ(library.navigator.asked, (std::vector<Shown>{{0, 11}}));
}

TEST(ShipLibraryViewModel, a_category_of_one_ship_shows_it_again) {
  Library library(1, 0);
  library.view_model.NextShip();
  library.view_model.PreviousShip();
  EXPECT_EQ(library.navigator.asked, (std::vector<Shown>{{1, 0}, {1, 0}}));
}

TEST(ShipLibraryViewModel, a_new_category_starts_at_its_first_ship) {
  Library library(0, 7);
  library.view_model.NextCategory();
  EXPECT_EQ(library.navigator.asked.back(), (Shown{1, 0}));
  library.Arrive();
  library.view_model.PreviousCategory();
  EXPECT_EQ(library.navigator.asked.back(), (Shown{0, 0}));
}

TEST(ShipLibraryViewModel, categories_wrap_both_ways) {
  Library library(2, 1);
  library.view_model.NextCategory();
  EXPECT_EQ(library.navigator.asked.back(), (Shown{0, 0}));
  library.Arrive();
  library.view_model.PreviousCategory();
  EXPECT_EQ(library.navigator.asked.back(), (Shown{2, 0}));
}

// Two presses before the first ship has flown in move two ships, not one.
TEST(ShipLibraryViewModel, commands_count_from_the_ship_asked_for) {
  Library library(0, 3);
  library.view_model.NextShip();
  library.view_model.NextShip();
  EXPECT_EQ(library.navigator.asked, (std::vector<Shown>{{0, 4}, {0, 5}}));
  EXPECT_EQ(library.view_model.selected_ship(), 5u);
  EXPECT_EQ(library.view_model.model().ship, 3u);  // still on show
  library.Arrive();
  EXPECT_EQ(library.view_model.model().ship, 5u);
  EXPECT_EQ(library.view_model.selected_ship(), 5u);
}

TEST(ShipLibraryViewModel, select_ship_asks_for_that_ship) {
  Library library(0, 0);
  library.view_model.SelectShip(9);
  EXPECT_EQ(library.navigator.asked, (std::vector<Shown>{{0, 9}}));
}

TEST(ShipLibraryViewModel, select_ship_ignores_the_ship_on_show_and_strangers) {
  Library library(0, 2);
  library.view_model.SelectShip(2);
  library.view_model.SelectShip(12);
  EXPECT_TRUE(library.navigator.asked.empty());
}

TEST(ShipLibraryViewModel, exit_is_the_navigators) {
  Library library;
  library.view_model.Exit();
  EXPECT_EQ(library.navigator.exits, 1);
}

TEST(ShipLibraryViewModel, views_are_told_of_commands_and_arrivals) {
  Library library(0, 0);
  library.view_model.NextShip();
  EXPECT_EQ(library.view.refreshes, 1);
  library.Arrive();
  EXPECT_EQ(library.view.refreshes, 2);
  library.view_model.Detach(&library.view);
  library.view_model.NextShip();
  EXPECT_EQ(library.view.refreshes, 2);
}

TEST(ShipLibraryViewModel, category_names_are_the_neighbours_of_the_one_on_show) {
  Library library(0, 0);
  EXPECT_EQ(library.view_model.PreviousCategoryName(), "things");
  EXPECT_EQ(library.view_model.CurrentCategoryName(), "ships");
  EXPECT_EQ(library.view_model.NextCategoryName(), "stations");
}

TEST(ShipLibraryViewModel, the_page_is_the_five_around_the_ship_on_show) {
  Library first_page(0, 3);
  ShipLibraryViewModel::Page page = first_page.view_model.CurrentPage();
  EXPECT_EQ(page.first, 0u);
  EXPECT_EQ(page.last, 4u);
  EXPECT_FALSE(page.more_above);
  EXPECT_TRUE(page.more_below);

  Library middle_page(0, 7);
  page = middle_page.view_model.CurrentPage();
  EXPECT_EQ(page.first, 5u);
  EXPECT_EQ(page.last, 9u);
  EXPECT_TRUE(page.more_above);
  EXPECT_TRUE(page.more_below);

  Library last_page(0, 11);
  page = last_page.view_model.CurrentPage();
  EXPECT_EQ(page.first, 10u);
  EXPECT_EQ(page.last, 11u);
  EXPECT_TRUE(page.more_above);
  EXPECT_FALSE(page.more_below);
}

TEST(ShipLibraryViewModel, an_empty_library_asks_for_nothing) {
  FakeNavigator navigator;
  ShipLibraryViewModel view_model(navigator);
  view_model.NextShip();
  view_model.PreviousShip();
  view_model.NextCategory();
  view_model.PreviousCategory();
  view_model.SelectShip(0);
  EXPECT_TRUE(navigator.asked.empty());
  EXPECT_TRUE(view_model.ships().empty());
  EXPECT_EQ(view_model.CurrentCategoryName(), "");
}
