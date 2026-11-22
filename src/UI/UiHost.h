// The interface layer the game is given. GameController makes one (the
// composition root) and hands it to the view, which passes it input, and to
// the universe, which has it draw and asks it for screen views. NullUiHost is
// the classic look and the headless game; RmlUiHost is the RmlUi look.

#pragma once

#include <memory>

union SDL_Event;

namespace oo::ui {

class GridSurface;
class ShipLibraryView;
class ShipLibraryViewModel;

class UiHost {
 public:
  virtual ~UiHost() = default;

  // An input event, before the game reads it. True: the interface used it,
  // and the game ignores it.
  virtual bool HandleEvent(const SDL_Event& event) = 0;
  // The drawable's size in pixels changed (window resize, full screen).
  virtual void Resize(int pixel_width, int pixel_height) = 0;
  // A document of this host replaces the text grid on screen.
  virtual bool IsCoveringGui() const = 0;
  // Draw the host's documents over the frame drawn so far.
  virtual void Render() = 0;

  // The view of the ship library for <view_model>. <classic> is the text grid,
  // for a host whose look is the grid or whose own document failed to load.
  virtual std::unique_ptr<ShipLibraryView> MakeShipLibraryView(
      ShipLibraryViewModel& view_model, GridSurface& classic) = 0;
};

}  // namespace oo::ui
