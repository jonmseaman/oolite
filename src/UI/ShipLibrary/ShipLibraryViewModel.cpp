#include "UI/ShipLibrary/ShipLibraryViewModel.h"

#include <algorithm>
#include <utility>

#include "UI/ShipLibrary/ShipLibraryNavigator.h"
#include "UI/ShipLibrary/ShipLibraryView.h"

namespace oo::ui {

ShipLibraryViewModel::ShipLibraryViewModel(ShipLibraryNavigator& navigator)
    : navigator_(navigator) {}

void ShipLibraryViewModel::Attach(ShipLibraryView* view) {
  if (view != nullptr &&
      std::find(views_.begin(), views_.end(), view) == views_.end()) {
    views_.push_back(view);
  }
}

void ShipLibraryViewModel::Detach(ShipLibraryView* view) {
  std::erase(views_, view);
}

void ShipLibraryViewModel::SetCategories(
    std::vector<ShipLibraryCategory> categories, std::size_t category,
    std::size_t ship) {
  model_.categories = std::move(categories);
  model_.category = category;
  model_.ship = ship;
  pending_category_.reset();
  pending_ship_.reset();
  // Nothing to show until the game puts a ship on show (OnShipShown).
}

void ShipLibraryViewModel::OnShipShown(std::size_t category, std::size_t ship,
                                       ShipSheet sheet) {
  model_.category = category;
  model_.ship = ship;
  model_.sheet = std::move(sheet);
  if (pending_category_ == category && pending_ship_ == ship) {
    pending_category_.reset();
    pending_ship_.reset();
  }
  NotifyViews();
}

void ShipLibraryViewModel::NextShip() {
  if (empty()) return;
  const std::size_t category = selected_category();
  const std::size_t count = ShipCount(category);
  if (count == 0) return;
  Show(category, (selected_ship() + 1) % count);
}

void ShipLibraryViewModel::PreviousShip() {
  if (empty()) return;
  const std::size_t category = selected_category();
  const std::size_t count = ShipCount(category);
  if (count == 0) return;
  Show(category, (selected_ship() + count - 1) % count);
}

void ShipLibraryViewModel::NextCategory() {
  if (empty()) return;
  const std::size_t count = model_.categories.size();
  Show((selected_category() + 1) % count, 0);
}

void ShipLibraryViewModel::PreviousCategory() {
  if (empty()) return;
  const std::size_t count = model_.categories.size();
  Show((selected_category() + count - 1) % count, 0);
}

void ShipLibraryViewModel::SelectShip(std::size_t index) {
  if (empty()) return;
  const std::size_t category = selected_category();
  if (index >= ShipCount(category) || index == selected_ship()) return;
  Show(category, index);
}

void ShipLibraryViewModel::Exit() { navigator_.Exit(); }

std::size_t ShipLibraryViewModel::selected_category() const {
  return pending_category_.value_or(model_.category);
}

std::size_t ShipLibraryViewModel::selected_ship() const {
  return pending_ship_.value_or(model_.ship);
}

const std::vector<ShipLibraryEntry>& ShipLibraryViewModel::ships() const {
  static const std::vector<ShipLibraryEntry> kNone;
  if (model_.category >= model_.categories.size()) return kNone;
  return model_.categories[model_.category].entries;
}

std::string ShipLibraryViewModel::PreviousCategoryName() const {
  if (empty()) return std::string();
  const std::size_t count = model_.categories.size();
  return model_.categories[(model_.category + count - 1) % count].plural_name;
}

std::string ShipLibraryViewModel::CurrentCategoryName() const {
  if (model_.category >= model_.categories.size()) return std::string();
  return model_.categories[model_.category].plural_name;
}

std::string ShipLibraryViewModel::NextCategoryName() const {
  if (empty()) return std::string();
  const std::size_t count = model_.categories.size();
  return model_.categories[(model_.category + 1) % count].plural_name;
}

ShipLibraryViewModel::Page ShipLibraryViewModel::CurrentPage() const {
  Page page;
  const std::size_t count = ships().size();
  if (count == 0) return page;
  page.first = model_.ship - (model_.ship % kPageSize);
  page.last = std::min(page.first + kPageSize - 1, count - 1);
  page.more_above = page.first > 0;
  page.more_below = page.last < count - 1;
  return page;
}

void ShipLibraryViewModel::Show(std::size_t category, std::size_t ship) {
  pending_category_ = category;
  pending_ship_ = ship;
  navigator_.ShowShip(category, ship);
  NotifyViews();
}

void ShipLibraryViewModel::NotifyViews() {
  // A copy: a view may detach while it is told.
  const std::vector<ShipLibraryView*> views = views_;
  for (ShipLibraryView* view : views) view->Refresh();
}

std::size_t ShipLibraryViewModel::ShipCount(std::size_t category) const {
  return category < model_.categories.size()
             ? model_.categories[category].entries.size()
             : 0;
}

}  // namespace oo::ui
