// The classic ship library: the view-model written into the text grid row by
// row, as the game has always laid it out (title and exit rows are the
// game's).

#pragma once

#include "UI/ShipLibrary/ShipLibraryView.h"

namespace oo::ui {

class GridSurface;
class ShipLibraryViewModel;

class GridShipLibraryView final : public ShipLibraryView {
 public:
  // The rows the view writes; the screen's own rows (title, exit) are outside.
  static constexpr int kFirstRow = 1;
  static constexpr int kLastRow = 26;

  // Attaches to <view_model>; both must outlive the view.
  GridShipLibraryView(ShipLibraryViewModel& view_model, GridSurface& surface);
  ~GridShipLibraryView() override;
  GridShipLibraryView(const GridShipLibraryView&) = delete;
  GridShipLibraryView& operator=(const GridShipLibraryView&) = delete;

  void Refresh() override;

 private:
  ShipLibraryViewModel& view_model_;
  GridSurface& surface_;
};

}  // namespace oo::ui
