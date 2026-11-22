#include "UI/NullUiHost.h"

#include "UI/ShipLibrary/GridShipLibraryView.h"

namespace oo::ui {

NullUiHost& NullUiHost::Fallback() {
  static NullUiHost host;
  return host;
}

bool NullUiHost::HandleEvent(const SDL_Event& /*event*/) { return false; }

void NullUiHost::Resize(int /*pixel_width*/, int /*pixel_height*/) {}

bool NullUiHost::IsCoveringGui() const { return false; }

void NullUiHost::Render() {}

std::unique_ptr<ShipLibraryView> NullUiHost::MakeShipLibraryView(
    ShipLibraryViewModel& view_model, GridSurface& classic) {
  return std::make_unique<GridShipLibraryView>(view_model, classic);
}

}  // namespace oo::ui
