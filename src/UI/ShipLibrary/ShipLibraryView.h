// A presentation of the ship library's view-model. A view is made with the
// view-model it shows (UiHost::MakeShipLibraryView) and is told when the
// view-model changes; it never changes the model itself, only calls the
// view-model's commands.

#pragma once

namespace oo::ui {

// Where the view wants the demo ship, which the game draws behind it. The
// distance is a multiple of the classic resting distance (smaller: closer and
// larger on screen); the offsets move it across the screen, as fractions of
// that distance.
struct ShipStage {
  double distance_scale = 1.0;
  double x_offset = 0.0;  // + right
  double y_offset = 0.0;  // + up
};

class ShipLibraryView {
 public:
  virtual ~ShipLibraryView() = default;

  // The view-model changed: show its state.
  virtual void Refresh() = 0;

  // The classic grid leaves the ship where it has always been.
  virtual ShipStage Stage() const { return ShipStage{}; }
};

}  // namespace oo::ui
