/*

OOMouseInteractionMode.h


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

#pragma once

#ifndef OO_PLAIN_CPP_UNIT
#include "Foundation.h"
#else
/*	Plain C++ translation unit (OOMouseInteractionMode.cpp): only
   BOOL is needed, and OOFoundation.h imports Foundation, which does not compile
   in a plain C++ unit.
*/
#include "oofnd/objc/FoundationTypes.h"  // bool
#endif
#include <cstdint>
#include <string>
#include <utility>

#include "EnumFlags.h"

/*
        OOMouseInteractionMode
        Mouse interaction states, defined in game-centric terms. The precise
        semantics may vary across platforms.

        The primary distinction is between UI screens and flight screens. Flight
        screens are screens 1-4 when neither docked nor paused, or the break
        pattern screen. Every other screen is a UI screen, including screens 1-4
        when paused in flight.

        UI screens are divided between ones with clickable controls (like the
        star chart, outfitting screen and config screen), and ones without (like
        the manifest screen, system data screen and pause screen).

        Flight screens have two modes, one for mouse control enabled and one for
        mouse control disabled.
*/
namespace oo {

enum class MouseInteractionMode : std::int32_t {
  kUiScreenNoInteraction,
  kUiScreenWithInteraction,
  kFlightNoMouseControl,
  kFlightWithMouseControl,
};

}  // namespace oo

static_assert(
    std::to_underlying(oo::MouseInteractionMode::kUiScreenNoInteraction) == 0);
static_assert(std::to_underlying(
                  oo::MouseInteractionMode::kUiScreenWithInteraction) == 1);
static_assert(
    std::to_underlying(oo::MouseInteractionMode::kFlightNoMouseControl) == 2);
static_assert(
    std::to_underlying(oo::MouseInteractionMode::kFlightWithMouseControl) == 3);

namespace oo {

std::string StringFromMouseInteractionMode(MouseInteractionMode mode);
bool MouseInteractionModeIsUIScreen(MouseInteractionMode mode);
bool MouseInteractionModeIsFlightMode(MouseInteractionMode mode);

}  // namespace oo
