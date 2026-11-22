// The ship library's view-model: the state its views bind to and the commands
// they and the game's key handling call. It owns the model, asks the game to
// change ships through the navigator it is given, and tells its views when
// anything changes.

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "UI/ShipLibrary/ShipLibraryModel.h"

namespace oo::ui {

class ShipLibraryNavigator;
class ShipLibraryView;

class ShipLibraryViewModel {
 public:
  // The classic screen lists this many ships at a time.
  static constexpr std::size_t kPageSize = 5;

  // The ships of the category on show that the classic screen lists, and
  // whether there are more above or below them ("..." rows).
  struct Page {
    std::size_t first = 0;
    std::size_t last = 0;  // inclusive
    bool more_above = false;
    bool more_below = false;
  };

  explicit ShipLibraryViewModel(ShipLibraryNavigator& navigator);
  ShipLibraryViewModel(const ShipLibraryViewModel&) = delete;
  ShipLibraryViewModel& operator=(const ShipLibraryViewModel&) = delete;

  // Views told of every change. A view detaches before it is destroyed.
  void Attach(ShipLibraryView* view);
  void Detach(ShipLibraryView* view);

  // -- From the game --

  // The library's contents and the ship on show when it opens.
  void SetCategories(std::vector<ShipLibraryCategory> categories,
                     std::size_t category, std::size_t ship);
  // The game has put this ship on show.
  void OnShipShown(std::size_t category, std::size_t ship, ShipSheet sheet);

  // -- Commands (keys, joystick, mouse) --

  void NextShip();
  void PreviousShip();
  void NextCategory();
  void PreviousCategory();
  // A ship of the category on show, picked by the pointer.
  void SelectShip(std::size_t index);
  void Exit();

  // -- State --

  const ShipLibraryModel& model() const { return model_; }

  bool empty() const { return model_.categories.empty(); }

  // The category and ship asked for last: the one on show, or the one flying
  // in after a command.
  std::size_t selected_category() const;
  std::size_t selected_ship() const;

  const std::vector<ShipLibraryEntry>& ships() const;
  std::string PreviousCategoryName() const;
  std::string CurrentCategoryName() const;
  std::string NextCategoryName() const;
  Page CurrentPage() const;

 private:
  void Show(std::size_t category, std::size_t ship);
  void NotifyViews();
  std::size_t ShipCount(std::size_t category) const;

  ShipLibraryNavigator& navigator_;
  ShipLibraryModel model_;
  std::optional<std::size_t> pending_category_;
  std::optional<std::size_t> pending_ship_;
  std::vector<ShipLibraryView*> views_;
};

}  // namespace oo::ui
