/*

OODisplaySize.h

The physical size of a display, where the platform can tell (spec 008 research
R7): Windows reads the monitor's EDID, macOS asks Core Graphics, and Linux
reports nothing, so the player enters the size.

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
#include <optional>
#include <utility>

// Physical width and height in millimetres of the SDL display, or empty if
// unknown.
std::optional<std::pair<double, double>> OOQueryDisplayPhysicalSize(
    int sdlDisplayIndex);
