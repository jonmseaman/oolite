#include "UI/ShipLibrary/GridShipLibraryView.h"

#include <format>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>

#include "UI/GridSurface.h"
#include "UI/ShipLibrary/ShipLibraryViewModel.h"

namespace oo::ui {
namespace {

constexpr int kSummaryRow = 1;
constexpr int kFirstStatsRow = 3;
constexpr int kDescriptionRow = 7;
constexpr int kDescriptionRowWithoutStats = 3;
constexpr int kCategoryRow = 19;
constexpr int kMoreAboveRow = 20;
constexpr int kFirstShipRow = 21;
constexpr int kMoreBelowRow = 26;

// The fields up to the first absent one, as the screen has always cut a row.
std::vector<std::string> FieldsUpToAbsent(
    std::initializer_list<std::optional<std::string>> fields) {
  std::vector<std::string> result;
  for (const std::optional<std::string>& field : fields) {
    if (!field.has_value()) break;
    result.push_back(*field);
  }
  return result;
}

}  // namespace

GridShipLibraryView::GridShipLibraryView(ShipLibraryViewModel& view_model,
                                         GridSurface& surface)
    : view_model_(view_model), surface_(surface) {
  view_model_.Attach(this);
}

GridShipLibraryView::~GridShipLibraryView() { view_model_.Detach(this); }

void GridShipLibraryView::Refresh() {
  if (view_model_.empty()) return;

  surface_.SetTabStops({0, 170, 340});
  for (int row = kFirstRow; row <= kLastRow; ++row) surface_.SetText(row, "");

  const ShipSheet& sheet = view_model_.model().sheet;
  surface_.SetFields(
      kSummaryRow, FieldsUpToAbsent({sheet.category_singular, sheet.class_name,
                                     sheet.summary}));
  surface_.SetColor(kSummaryRow, GridColor::kGreen);

  int description_row = kDescriptionRow;
  if (sheet.has_stats) {
    surface_.SetFields(kFirstStatsRow,
                       {sheet.speed, sheet.turn_rate, sheet.cargo});
    surface_.SetFields(kFirstStatsRow + 1,
                       {sheet.generator, sheet.shields, sheet.witchspace});
    surface_.SetFields(kFirstStatsRow + 2,
                       {sheet.weapons, sheet.turrets, sheet.size});
  } else {
    description_row = kDescriptionRowWithoutStats;
  }
  if (sheet.has_description) {
    surface_.AddLongText(sheet.description, description_row);
  }

  surface_.SetFields(kCategoryRow,
                     {std::format("<-- {}", view_model_.PreviousCategoryName()),
                      view_model_.CurrentCategoryName(),
                      std::format("{} -->", view_model_.NextCategoryName())});
  surface_.SetColor(kCategoryRow, GridColor::kGreen);

  const std::vector<ShipLibraryEntry>& ships = view_model_.ships();
  const ShipLibraryViewModel::Page page = view_model_.CurrentPage();
  if (ships.empty()) return;
  int row = kFirstShipRow;
  for (std::size_t i = page.first; i <= page.last; ++i, ++row) {
    surface_.SetFields(
        row, FieldsUpToAbsent({std::string(), ships[i].name, std::string()}));
    surface_.SetColor(row, i == view_model_.model().ship ? GridColor::kYellow
                                                         : GridColor::kWhite);
  }
  if (page.more_above) {
    surface_.SetFields(kMoreAboveRow, {std::string(), "...", std::string()});
    surface_.SetColor(kMoreAboveRow, GridColor::kWhite);
  }
  if (page.more_below) {
    surface_.SetFields(kMoreBelowRow, {std::string(), "...", std::string()});
    surface_.SetColor(kMoreBelowRow, GridColor::kWhite);
  }
}

}  // namespace oo::ui
