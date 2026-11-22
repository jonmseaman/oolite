// What the ship library's view-model asks of the game. The game owns the demo
// ship and its fly-out and fly-in; the view-model only says which ship to show
// next and when the player leaves.

#pragma once

#include <cstddef>

namespace oo::ui {

class ShipLibraryNavigator {
 public:
  virtual ~ShipLibraryNavigator() = default;

  // Fly the ship on show out and the ship at <index> of <category> in. The game
  // answers with ShipLibraryViewModel::OnShipShown once it is on show.
  virtual void ShowShip(std::size_t category, std::size_t index) = 0;

  // Leave the library for the screen it was opened from.
  virtual void Exit() = 0;
};

}  // namespace oo::ui
