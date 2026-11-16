/*

OOShipLibraryDescriptions.h

Default descriptions for ships

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

#include <string>

#include "ShipEntity.h"

// shiplibrary.plist keys, UTF-8.

namespace oo {

static constexpr const char* kDemoShipKey = "ship";
static constexpr const char* kDemoShipName = "name";  // set internally only
static constexpr const char* kDemoShipClass = "class";
static constexpr const char* kDemoShipSummary = "summary";
static constexpr const char* kDemoShipDescription = "description";
static constexpr const char* kDemoShipShipData = "ship_data";
static constexpr const char* kDemoShipSpeed = "speed";
static constexpr const char* kDemoShipTurnRate = "turn_rate";
static constexpr const char* kDemoShipCargo = "cargo";
static constexpr const char* kDemoShipGenerator = "generator";
static constexpr const char* kDemoShipShields = "shields";
static constexpr const char* kDemoShipWitchspace = "witchspace";
static constexpr const char* kDemoShipWeapons = "weapons";
static constexpr const char* kDemoShipTurrets = "turrets";
static constexpr const char* kDemoShipSize = "size";
static constexpr const char* kDemoShipConditions = "condition_script";

std::string ShipLibraryCategorySingular(const std::string& category);
std::string ShipLibraryCategoryPlural(const std::string& category);

std::string ShipLibrarySpeed(ShipEntity* demo_ship);
std::string ShipLibraryTurnRate(ShipEntity* demo_ship);
std::string ShipLibraryCargo(ShipEntity* demo_ship);
std::string ShipLibraryGenerator(ShipEntity* demo_ship);
std::string ShipLibraryShields(ShipEntity* demo_ship);
std::string ShipLibraryWitchspace(ShipEntity* demo_ship);
std::string ShipLibraryWeapons(ShipEntity* demo_ship);
std::string ShipLibraryTurrets(ShipEntity* demo_ship);
std::string ShipLibrarySize(ShipEntity* demo_ship);

}  // namespace oo
