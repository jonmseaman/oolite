/*

OODisplaySizeMac.cpp

OOQueryDisplayPhysicalSize on macOS: Core Graphics' CGDisplayScreenSize for
the display under the centre of the SDL display (spec 008 research R7).

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

#include <CoreGraphics/CoreGraphics.h>
#include <SDL3/SDL.h>

#include "OODisplaySize.h"

std::optional<std::pair<double, double>> OOQueryDisplayPhysicalSize(
    int sdlDisplayIndex) {
  SDL_Rect bounds;
  if (!SDL_GetDisplayBounds(static_cast<SDL_DisplayID>(sdlDisplayIndex),
                            &bounds)) {
    return std::nullopt;
  }
  const CGPoint centre =
      CGPointMake(bounds.x + bounds.w / 2.0, bounds.y + bounds.h / 2.0);
  CGDirectDisplayID display = 0;
  uint32_t count = 0;
  if (CGGetDisplaysWithPoint(centre, 1, &display, &count) != kCGErrorSuccess ||
      count == 0) {
    return std::nullopt;
  }
  const CGSize size = CGDisplayScreenSize(display);
  if (size.width <= 0 || size.height <= 0) return std::nullopt;
  return std::make_pair(static_cast<double>(size.width),
                        static_cast<double>(size.height));
}
