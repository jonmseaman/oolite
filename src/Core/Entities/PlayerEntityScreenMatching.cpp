/*

PlayerEntityScreenMatching.cpp

The screen-matching settings (spec 008, contracts/settings.md): the player's
screen (size, aspect, flat or curved), where they sit, and the field of view
worked out from them, a custom horizontal angle and the wide-angle look. It
opens from the Field Of View row of Game Options; every change applies at
once.


Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

#include <algorithm>
#include <cmath>
#include <string>

#include "GameController.h"
#include "GuiDisplayGen.h"
#include "MyOpenGLView.h"
#include "OODisplayGeometry.h"
#include "OOJoystickManager.h"
#include "PlayerEntity.h"
#include "Universe.h"
#include "oofnd/Defaults.hpp"
#include "oofnd/String.hpp"

namespace {

enum : OOGUIRow {
  kRowMatching = 4,
  kRowSize = 6,
  kRowAspect,
  kRowShape,
  kRowCurvature,
  kRowDistance,
  kRowUnits,
  kRowFovMode = 13,
  kRowHorizontal,
  kRowVertical,
  kRowLook,
  kRowNotice = 18,
  kRowHint = 20,
  kRowBack = 22,
};

constexpr double kMmPerInch = 25.4;
const char* const kDegrees = "\xC2\xB0";

struct AspectPreset {
  double value;
  const char* label;
};
constexpr AspectPreset kAspectPresets[] = {
    {16.0 / 9.0, "16:9"},
    {16.0 / 10.0, "16:10"},
    {64.0 / 27.0, "21:9"},
    {32.0 / 9.0, "32:9"},
};

// Key repeat, as Game Options' sliders do it.
bool sArrowHeld = false;
double sLastArrowTime = 0.0;
bool sSelectHeld = true;

oo::Defaults& Prefs() { return oo::Defaults::standard(); }

bool InCentimetres() {
  return Prefs().stringForKey("screen-units").value_or("in") == "cm";
}

std::string Length(double mm, bool centimetres) {
  return centimetres ? oo::str::format("%.1f cm", mm / 10.0)
                     : oo::str::format("%.1f\"", mm / kMmPerInch);
}

std::string AspectLabel(double aspect) {
  for (const AspectPreset& preset : kAspectPresets) {
    if (std::fabs(preset.value - aspect) < 0.005) return preset.label;
  }
  return OO_DESC("gameoptions-screen-aspect-custom") +
         oo::str::format("%.2f", aspect);
}

// The projection the current settings give, matching on or off, so the
// screen can show the physical angles before the player turns it on.
OODisplayGeometry PreviewGeometry(MyOpenGLView* view) {
  const NSSize size = view != nullptr ? view->backingViewSize() : NSSize{};
  const bool shaders = UNIVERSE == nullptr || UNIVERSE->useShaders();
  if (view == nullptr) return OODisplayGeometry();
  return OODisplayGeometry(view->screenDescription(), view->viewerDistanceMm(),
                           view->fovChoice(), static_cast<int>(size.width),
                           static_cast<int>(size.height), 1.0, 1.0, shaders);
}

void Apply(MyOpenGLView* view) {
  if (view != nullptr) view->reloadScreenMatchingPreferences();
}

void SetSizeEntered() {
  Prefs().setObject("screen-size-source", oo::PList("entered"));
}

}  // namespace

void PlayerEntity::setGuiToScreenMatchingScreen(OOGUIRow selectedRow) {
  MyOpenGLView* gameView =
      UNIVERSE != nullptr ? UNIVERSE->getGameView() : nullptr;
  GuiDisplayGen* gui = UNIVERSE != nullptr ? UNIVERSE->getGui() : nullptr;
  if (gui == nullptr || gameView == nullptr) return;

  // As Game Options: the mouse picks rows on this screen.
  if (gui_screen != GUI_SCREEN_SCREENMATCHING) {
    sSelectHeld = true;
    gameView->clearMouse();
    if (UNIVERSE->gameController() != nullptr) {
      UNIVERSE->gameController()
          ->setMouseInteractionModeForUIWithMouseInteraction(true);
    }
  }
  gui_screen = GUI_SCREEN_SCREENMATCHING;

  const OOScreenDescription& screen = gameView->screenDescription();
  const OOFieldOfViewChoice& choice = gameView->fovChoice();
  const bool on = gameView->screenMatching();
  const bool cm = InCentimetres();
  const OODisplayGeometry preview = PreviewGeometry(gameView);
  const bool curved = screen.curvatureMm.has_value();
  const bool custom = choice.mode == OOFovMode::kCustom;
  const bool wider = custom && preview.horizontalDeg() >
                                   preview.physicalHorizontalDeg() + 0.05;
  const bool shaders = UNIVERSE == nullptr || UNIVERSE->useShaders();

  gui->clear();
  gui->setTitle(OO_DESC("gameoptions-screen-title"));

  auto row = [&](OOGUIRow r, const std::string& text, bool enabled) {
    gui->setText(text, r, GUI_ALIGN_CENTER);
    if (enabled) {
      gui->setKey(std::string(GUI_KEY_OK), r);
    } else {
      gui->setColor(OOColor::grayColor().get(), r);
    }
  };

  row(kRowMatching,
      OO_DESC("gameoptions-screen-matching") +
          (on ? OO_DESC("gameoptions-screen-on")
              : OO_DESC("gameoptions-screen-off")),
      true);

  std::string sizeText =
      OO_DESC("gameoptions-screen-size") + Length(screen.diagonalMm, cm);
  if (screen.source == OOScreenSizeSource::kDetected) {
    sizeText += OO_DESC("gameoptions-screen-detected");
  } else if (screen.source == OOScreenSizeSource::kDefault) {
    sizeText += OO_DESC("gameoptions-screen-enter-size");
  }
  row(kRowSize, sizeText, true);
  row(kRowAspect,
      OO_DESC("gameoptions-screen-aspect") + AspectLabel(screen.aspect), true);
  row(kRowShape,
      OO_DESC("gameoptions-screen-shape") +
          (curved ? OO_DESC("gameoptions-screen-curved")
                  : OO_DESC("gameoptions-screen-flat")),
      true);
  row(kRowCurvature,
      OO_DESC("gameoptions-screen-curvature") +
          (curved ? oo::str::format("%dR", (int)lround(*screen.curvatureMm))
                  : std::string("-")),
      curved);
  row(kRowDistance,
      OO_DESC("gameoptions-screen-distance") +
          Length(gameView->viewerDistanceMm(), cm),
      true);
  row(kRowUnits,
      OO_DESC("gameoptions-screen-units") +
          (cm ? OO_DESC("gameoptions-screen-cm")
              : OO_DESC("gameoptions-screen-inches")),
      true);

  row(kRowFovMode,
      OO_DESC("gameoptions-screen-fov") +
          (custom ? OO_DESC("gameoptions-screen-custom")
                  : OO_DESC("gameoptions-screen-physical")),
      true);
  row(kRowHorizontal,
      OO_DESC("gameoptions-screen-horizontal") +
          oo::str::format("%.1f%s", preview.horizontalDeg(), kDegrees) +
          (custom
               ? std::string()
               : oo::str::format(
                     " (%s)", OO_DESC("gameoptions-screen-physical").c_str())),
      custom);
  row(kRowVertical,
      OO_DESC("gameoptions-screen-vertical") +
          oo::str::format("%.1f%s", preview.verticalDeg(), kDegrees),
      false);
  if (wider) {
    const std::string look = choice.look == OOWideLook::kEvenSpacing
                                 ? OO_DESC("gameoptions-screen-even")
                             : choice.look == OOWideLook::kClassicFlat
                                 ? OO_DESC("gameoptions-screen-classic")
                                 : OO_DESC("gameoptions-screen-panini");
    row(kRowLook, OO_DESC("gameoptions-screen-look") + look, true);
  }
  if (!shaders &&
      (curved || (wider && choice.look != OOWideLook::kClassicFlat))) {
    gui->setText(OO_DESC("gameoptions-screen-needs-shaders"), kRowNotice,
                 GUI_ALIGN_CENTER);
    gui->setColor(OOColor::yellowColor().get(), kRowNotice);
  }
  gui->setText(OO_DESC("gameoptions-screen-hint"), kRowHint, GUI_ALIGN_CENTER);
  gui->setColor(OOColor::grayColor().get(), kRowHint);

  row(kRowBack, OO_DESC("gui-back"), true);

  gui->setSelectableRange(
      NSMakeRange(kRowMatching, kRowBack - kRowMatching + 1));
  if (selectedRow < 0 || !gui->setSelectedRow(selectedRow)) {
    gui->setSelectedRow(kRowMatching);
  }
  gui->setShowTextCursor(false);
  gui->setForegroundTextureKey(std::optional<std::string>(
      status() == STATUS_DOCKED ? "docked_overlay" : "paused_overlay"));
  gui->setBackgroundTextureKey("settings");

  setShowDemoShips(false);
  if (UNIVERSE != nullptr) UNIVERSE->enterGUIViewModeWithMouseInteraction(true);
}

void PlayerEntity::handleScreenMatchingScreenKeys() {
  MyOpenGLView* gameView =
      UNIVERSE != nullptr ? UNIVERSE->getGameView() : nullptr;
  GuiDisplayGen* gui = UNIVERSE != nullptr ? UNIVERSE->getGui() : nullptr;
  if (gui == nullptr || gameView == nullptr) return;
  const bool* joyButtonState =
      OOJoystickManager::sharedStickHandler()->getAllButtonStates();

  handleGUIUpDownArrowKeys();
  const OOGUIRow selected = gui->getSelectedRow();

  const bool select = checkKeyPress(n_key_gui_select) ||
                      gameView->isDown(gvMouseDoubleClick) ||
                      joyButtonState[BUTTON_GUISELECT];
  if (gameView->isDown(gvMouseDoubleClick)) gameView->clearMouse();
  // The arrow keys themselves, whatever the modifiers: Shift asks for bigger
  // steps, and checkKeyPress() only matches the binding's exact modifiers.
  const bool right = checkKeyPress(n_key_gui_arrow_right) ||
                     gameView->isDown(gvArrowKeyRight) ||
                     joyButtonState[BUTTON_GUIRIGHT];
  const bool left = checkKeyPress(n_key_gui_arrow_left) ||
                    gameView->isDown(gvArrowKeyLeft) ||
                    joyButtonState[BUTTON_GUILEFT];

  // Enter on Back, or on a two-way row, acts once per press.
  if (select && !sSelectHeld) {
    sSelectHeld = true;
    if (selected == kRowBack) {
      // The Enter that chose Back is still down on Game Options' first frame,
      // where Enter on Field Of View would reopen this screen.
      gameView->clearKeys();
      setGuiToGameOptionsScreen();
      gui->setSelectedRow(GUI_ROW(GAME, FOV));
      return;
    }
  }
  if (!select) sSelectHeld = false;

  if (!(left || right)) {
    sArrowHeld = false;
    return;
  }
  if (sArrowHeld && script_time < sLastArrowTime + KEY_REPEAT_INTERVAL) return;
  sArrowHeld = true;
  sLastArrowTime = script_time;

  const int direction = right ? 1 : -1;
  const bool big = MyOpenGLView::pollShiftKey();
  oo::Defaults& prefs = Prefs();
  const OOScreenDescription& screen = gameView->screenDescription();
  const OOFieldOfViewChoice& choice = gameView->fovChoice();
  const bool cm = InCentimetres();

  switch (selected) {
    case kRowMatching:
      prefs.setBool("screen-matching", !gameView->screenMatching());
      break;
    case kRowSize: {
      // 0.5" or 1 cm steps, ten times that with Shift.
      const double step = (cm ? 10.0 : kMmPerInch / 2) * (big ? 10 : 1);
      prefs.setDouble(
          "screen-diagonal-mm",
          std::clamp(screen.diagonalMm + direction * step, 254.0, 3048.0));
      SetSizeEntered();
      break;
    }
    case kRowAspect: {
      double aspect = screen.aspect;
      if (big) {
        aspect = std::clamp(aspect + direction * 0.01, 1.0, 4.0);  // custom
      } else {
        // The next preset in that direction.
        const AspectPreset* next = nullptr;
        for (const AspectPreset& preset : kAspectPresets) {
          const bool beyond = direction > 0 ? preset.value > aspect + 0.005
                                            : preset.value < aspect - 0.005;
          if (beyond &&
              (next == nullptr || std::fabs(preset.value - aspect) <
                                      std::fabs(next->value - aspect))) {
            next = &preset;
          }
        }
        if (next != nullptr) aspect = next->value;
      }
      prefs.setDouble("screen-aspect", aspect);
      SetSizeEntered();
      break;
    }
    case kRowShape:
      if (screen.curvatureMm) {
        prefs.setDouble("screen-curvature-mm", 0.0);
      } else {
        prefs.setDouble("screen-curvature-mm", 1800.0);
      }
      break;
    case kRowCurvature:
      if (screen.curvatureMm) {
        prefs.setDouble(
            "screen-curvature-mm",
            std::clamp(*screen.curvatureMm + direction * 100.0 * (big ? 10 : 1),
                       800.0, 4000.0));
      }
      break;
    case kRowDistance: {
      const double step = (cm ? 10.0 : kMmPerInch / 2) * (big ? 10 : 1);
      prefs.setDouble(
          "viewer-distance-mm",
          std::clamp(gameView->viewerDistanceMm() + direction * step, 200.0,
                     3000.0));
      break;
    }
    case kRowUnits:
      prefs.setObject("screen-units", oo::PList(cm ? "in" : "cm"));
      break;
    case kRowFovMode:
      if (choice.mode == OOFovMode::kCustom) {
        prefs.setObject("fov-mode", oo::PList("physical"));
      } else {
        // Start the custom angle at the physical one.
        prefs.setObject("fov-mode", oo::PList("custom"));
        prefs.setDouble(
            "fov-custom-horizontal",
            std::round(PreviewGeometry(gameView).physicalHorizontalDeg()));
      }
      break;
    case kRowHorizontal:
      if (choice.mode == OOFovMode::kCustom) {
        prefs.setDouble("fov-custom-horizontal",
                        std::clamp(choice.customHorizontalDeg +
                                       direction * (big ? 10.0 : 1.0),
                                   20.0, 170.0));
      }
      break;
    case kRowLook: {
      static const char* const kLooks[] = {"panini", "even", "classic"};
      int index = choice.look == OOWideLook::kEvenSpacing   ? 1
                  : choice.look == OOWideLook::kClassicFlat ? 2
                                                            : 0;
      index = (index + direction + 3) % 3;
      prefs.setObject("fov-wide-look", oo::PList(kLooks[index]));
      break;
    }
    default:
      return;
  }
  Apply(gameView);
  setGuiToScreenMatchingScreen(selected);
}
