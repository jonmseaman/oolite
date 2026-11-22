/*	tests/unit/ui/test_grid_ship_library_view.cpp
	The classic ship library: the rows, fields and colours the grid view writes
	for a view-model, as Universe::setLibraryTextForDemoShip wrote them.
	Run: meson test -C build/meson_dev --suite ui
*/

#include <array>
#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "UI/GridSurface.h"
#include "UI/ShipLibrary/GridShipLibraryView.h"
#include "UI/ShipLibrary/ShipLibraryModel.h"
#include "UI/ShipLibrary/ShipLibraryNavigator.h"
#include "UI/ShipLibrary/ShipLibraryViewModel.h"

using oo::ui::GridColor;
using oo::ui::ShipLibraryCategory;
using oo::ui::ShipLibraryViewModel;
using oo::ui::ShipSheet;

namespace {

class NoNavigator : public oo::ui::ShipLibraryNavigator {
 public:
  void ShowShip(std::size_t, std::size_t) override {}
  void Exit() override {}
};

// The grid: the last fields and colour written to each row.
class FakeGrid : public oo::ui::GridSurface {
 public:
  void SetTabStops(const std::array<int, kTabStopCount>& stops) override { tab_stops = stops; }
  void SetText(int row, const std::string& text) override { fields[row] = {text}; }
  void SetFields(int row, const std::vector<std::string>& row_fields) override {
    fields[row] = row_fields;
  }
  void SetColor(int row, GridColor color) override { colors[row] = color; }
  void AddLongText(const std::optional<std::string>& text, int row) override {
    long_text_row = row;
    long_text = text;
  }

  std::array<int, kTabStopCount> tab_stops = {};
  std::map<int, std::vector<std::string>> fields;
  std::map<int, GridColor> colors;
  std::optional<int> long_text_row;
  std::optional<std::string> long_text;
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

ShipSheet Sheet() {
  ShipSheet sheet;
  sheet.category_singular = "Ship";
  sheet.class_name = "Cobra Mark III";
  sheet.summary = "A trader";
  sheet.has_stats = true;
  sheet.speed = "Speed: 0.350 LS";
  sheet.turn_rate = "Turn: 1";
  sheet.cargo = "Cargo: 20 TC";
  sheet.generator = "Gen: 4";
  sheet.shields = "Shields: 2";
  sheet.witchspace = "Hyperspace";
  sheet.weapons = "Weapons: 2";
  sheet.turrets = "Turrets: 0";
  sheet.size = "Size: 130m";
  sheet.has_description = true;
  sheet.description = "The ship.";
  return sheet;
}

struct Screen {
  explicit Screen(std::size_t ship, ShipSheet sheet = Sheet()) {
    view_model.SetCategories({Category("ship", 7), Category("station", 2)}, 0, ship);
    view_model.OnShipShown(0, ship, std::move(sheet));
  }
  NoNavigator navigator;
  ShipLibraryViewModel view_model{navigator};
  FakeGrid grid;
  oo::ui::GridShipLibraryView view{view_model, grid};
};

}  // namespace

TEST(GridShipLibraryView, writes_nothing_until_told) {
  NoNavigator navigator;
  ShipLibraryViewModel view_model(navigator);
  FakeGrid grid;
  oo::ui::GridShipLibraryView view(view_model, grid);
  EXPECT_TRUE(grid.fields.empty());
}

TEST(GridShipLibraryView, lays_out_the_sheet_as_the_classic_screen) {
  Screen screen(1);
  screen.view.Refresh();
  const FakeGrid& grid = screen.grid;
  EXPECT_EQ(grid.tab_stops, (std::array<int, 3>{0, 170, 340}));
  EXPECT_EQ(grid.fields.at(1), (std::vector<std::string>{"Ship", "Cobra Mark III", "A trader"}));
  EXPECT_EQ(grid.colors.at(1), GridColor::kGreen);
  EXPECT_EQ(grid.fields.at(2), (std::vector<std::string>{""}));
  EXPECT_EQ(grid.fields.at(3),
            (std::vector<std::string>{"Speed: 0.350 LS", "Turn: 1", "Cargo: 20 TC"}));
  EXPECT_EQ(grid.fields.at(4), (std::vector<std::string>{"Gen: 4", "Shields: 2", "Hyperspace"}));
  EXPECT_EQ(grid.fields.at(5),
            (std::vector<std::string>{"Weapons: 2", "Turrets: 0", "Size: 130m"}));
  EXPECT_EQ(grid.long_text_row, 7);
  EXPECT_EQ(grid.long_text, std::optional<std::string>("The ship."));
  EXPECT_EQ(grid.fields.at(19),
            (std::vector<std::string>{"<-- stations", "ships", "stations -->"}));
  EXPECT_EQ(grid.colors.at(19), GridColor::kGreen);
}

TEST(GridShipLibraryView, lists_the_page_with_the_ship_on_show_in_yellow) {
  Screen screen(1);
  screen.view.Refresh();
  const FakeGrid& grid = screen.grid;
  for (int row = 21; row <= 25; ++row) {
    const std::string name = "ship " + std::to_string(row - 21);
    EXPECT_EQ(grid.fields.at(row), (std::vector<std::string>{"", name, ""}));
    EXPECT_EQ(grid.colors.at(row), row == 22 ? GridColor::kYellow : GridColor::kWhite);
  }
  EXPECT_EQ(grid.fields.at(20), (std::vector<std::string>{""}));  // first page: no "..."
  EXPECT_EQ(grid.fields.at(26), (std::vector<std::string>{"", "...", ""}));
}

TEST(GridShipLibraryView, the_last_page_has_more_above_only) {
  Screen screen(6);
  screen.view.Refresh();
  const FakeGrid& grid = screen.grid;
  EXPECT_EQ(grid.fields.at(20), (std::vector<std::string>{"", "...", ""}));
  EXPECT_EQ(grid.fields.at(21), (std::vector<std::string>{"", "ship 5", ""}));
  EXPECT_EQ(grid.fields.at(22), (std::vector<std::string>{"", "ship 6", ""}));
  EXPECT_EQ(grid.colors.at(22), GridColor::kYellow);
  EXPECT_EQ(grid.fields.at(23), (std::vector<std::string>{""}));
  EXPECT_EQ(grid.fields.at(26), (std::vector<std::string>{""}));
}

TEST(GridShipLibraryView, without_ship_data_the_description_moves_up) {
  ShipSheet sheet = Sheet();
  sheet.has_stats = false;
  Screen screen(0, sheet);
  screen.view.Refresh();
  EXPECT_EQ(screen.grid.fields.at(3), (std::vector<std::string>{""}));
  EXPECT_EQ(screen.grid.long_text_row, 3);
}

TEST(GridShipLibraryView, an_absent_field_ends_its_row) {
  ShipSheet sheet = Sheet();
  sheet.class_name.reset();
  sheet.has_description = false;
  Screen screen(0, sheet);
  screen.view.Refresh();
  EXPECT_EQ(screen.grid.fields.at(1), (std::vector<std::string>{"Ship"}));
  EXPECT_FALSE(screen.grid.long_text_row.has_value());
}
