// One open ship library: its view-model and the view the host made for it.
// Made when the library opens and destroyed when the player leaves it; the
// view goes first, so it never outlives the view-model it is attached to.

#pragma once

#include <memory>

#include "UI/ShipLibrary/ShipLibraryView.h"
#include "UI/ShipLibrary/ShipLibraryViewModel.h"

namespace oo::ui {

class GridSurface;
class ShipLibraryNavigator;
class UiHost;

class ShipLibraryScreen {
 public:
  // <navigator>, <host> and <classic> must outlive the screen.
  ShipLibraryScreen(ShipLibraryNavigator& navigator, UiHost& host,
                    GridSurface& classic);
  ~ShipLibraryScreen();
  ShipLibraryScreen(const ShipLibraryScreen&) = delete;
  ShipLibraryScreen& operator=(const ShipLibraryScreen&) = delete;

  ShipLibraryViewModel& view_model() { return view_model_; }

  // Where the demo ship goes on this screen.
  ShipStage stage() const {
    return view_ != nullptr ? view_->Stage() : ShipStage{};
  }

 private:
  ShipLibraryViewModel view_model_;
  std::unique_ptr<ShipLibraryView> view_;
};

}  // namespace oo::ui
