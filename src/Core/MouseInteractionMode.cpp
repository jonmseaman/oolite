/*

OOMouseInteractionMode.m


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

#include "MouseInteractionMode.h"

#include <format>
#include <string>

namespace oo {

std::string StringFromMouseInteractionMode(MouseInteractionMode mode) {
  switch (mode) {
    case MouseInteractionMode::kUiScreenNoInteraction:
      return "UI_SCREEN_NO_INTERACTION";
    case MouseInteractionMode::kUiScreenWithInteraction:
      return "UI_SCREEN_WITH_INTERACTION";
    case MouseInteractionMode::kFlightNoMouseControl:
      return "FLIGHT_NO_MOUSE_CONTROL";
    case MouseInteractionMode::kFlightWithMouseControl:
      return "FLIGHT_WITH_MOUSE_CONTROL";
  }

  return std::format("<unknown mode {}>", static_cast<unsigned int>(mode));
}

bool MouseInteractionModeIsUIScreen(MouseInteractionMode mode) {
  switch (mode) {
    case MouseInteractionMode::kUiScreenNoInteraction:
      return true;
    case MouseInteractionMode::kUiScreenWithInteraction:
      return true;
    case MouseInteractionMode::kFlightNoMouseControl:
      return false;
    case MouseInteractionMode::kFlightWithMouseControl:
      return false;
  }

  return false;
}

bool MouseInteractionModeIsFlightMode(MouseInteractionMode mode) {
  switch (mode) {
    case MouseInteractionMode::kUiScreenNoInteraction:
      return false;
    case MouseInteractionMode::kUiScreenWithInteraction:
      return false;
    case MouseInteractionMode::kFlightNoMouseControl:
      return true;
    case MouseInteractionMode::kFlightWithMouseControl:
      return true;
  }

  return false;
}

}  // namespace oo
