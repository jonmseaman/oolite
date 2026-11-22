// The classic look: every screen is the text grid, and the host draws and
// consumes nothing. The headless game and an object no host was injected into
// use it.

#pragma once

#include <memory>

#include "UI/UiHost.h"

namespace oo::ui {

class NullUiHost final : public UiHost {
 public:
  // A stateless host for objects built before (or without) injection.
  static NullUiHost& Fallback();

  bool HandleEvent(const SDL_Event& event) override;
  void Resize(int pixel_width, int pixel_height) override;
  bool IsCoveringGui() const override;
  void Render() override;
  std::unique_ptr<ShipLibraryView> MakeShipLibraryView(
      ShipLibraryViewModel& view_model, GridSurface& classic) override;
};

}  // namespace oo::ui
