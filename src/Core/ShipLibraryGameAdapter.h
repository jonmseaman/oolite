// The game side of the ship library: carries the view-model's requests to the
// universe (which ship flies in) and the player (which screen to leave for).

#pragma once

#include <cstddef>

#include "UI/ShipLibrary/ShipLibraryNavigator.h"

class Universe;

namespace oo {

class ShipLibraryGameAdapter final : public oo::ui::ShipLibraryNavigator {
 public:
  // <universe> owns the adapter.
  explicit ShipLibraryGameAdapter(Universe& universe);

  void ShowShip(std::size_t category, std::size_t index) override;
  // Asks to leave. The library's views may be running an input handler, and
  // leaving destroys them, so the universe leaves on its next update
  // (RunPendingExit).
  void Exit() override;

  // Leaves if asked to since the last update: docked (the library was opened
  // from the interfaces screen) back to the interfaces, else back to the start
  // screen.
  void RunPendingExit();

 private:
  Universe& universe_;
  bool exit_requested_ = false;
};

}  // namespace oo
