#include "UI/ShipLibrary/ShipLibraryScreen.h"

#include "UI/ShipLibrary/ShipLibraryView.h"
#include "UI/UiHost.h"

namespace oo::ui {

ShipLibraryScreen::ShipLibraryScreen(ShipLibraryNavigator& navigator,
                                     UiHost& host, GridSurface& classic)
    : view_model_(navigator),
      view_(host.MakeShipLibraryView(view_model_, classic)) {}

ShipLibraryScreen::~ShipLibraryScreen() { view_.reset(); }

}  // namespace oo::ui
