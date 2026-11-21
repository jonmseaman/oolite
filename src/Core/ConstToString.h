/*

OOConstToString.h

Convert various sets of integer constants to strings.
To consider: replacing the integer constants with string constants.
 See also: OOConstToJSString.h.

This has grown beyond "const-to-string" at this point.

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

#include "Foundation.h"
#include "MyOpenGLView.h"
#include "Types.h"

// Values used for unknown strings.
namespace oo {

inline constexpr CargoType kCargoTypeDefault = CargoType::kNotCargo;
//	kOOCommodityTypeDefault		= COMMODITY_UNDEFINED,
inline constexpr EnergyUnitType kEnergyUnitTypeDefault = EnergyUnitType::kNone;
inline constexpr RouteType kRouteTypeDefault = RouteType::kJumps;

}  // namespace oo

/*

To avoid pulling in unnecessary headers, some functions defined in
OOConstToString.cpp are declared in the header with the appropriate type
declaration, each with its C++ (cxx_) form beside it, in particular:

        Entity.h:
        OOStringFromEntityStatus()
        OOEntityStatusFromString()
        OOStringFromScanClass()
        OOScanClassFromString()

        ShipEntity.h:
        OOStringFromBehaviour()
        OOEquipmentIdentifierFromWeaponType()
        OOWeaponTypeFromEquipmentIdentifierSloppy()
        OOWeaponTypeFromEquipmentIdentifierStrict()
        OOStringFromWeaponType()
        OOWeaponTypeFromString()
        OODisplayStringFromAlertCondition()

        PlayerEntity.h:
        OODisplayRatingStringFromKillCount()
        KillCountToRatingAndKillString()
        OODisplayStringFromLegalStatus()
        OOStringFromGUIScreenID()
        OOGUIScreenIDFromString()
        OOGalacticHyperspaceBehaviourFromString()
        OOStringFromGalacticHyperspaceBehaviour()

        Universe.h:
        OODisplayStringFromGovernmentID()
        OODisplayStringFromEconomyID()

*/

#include <optional>
#include <string>

// C++ forms: std::string results (never nil)
// and const std::string & parameters (the old nil arrived as "" and matched
// nothing: the same defaults).
std::string CargoTypeToString(oo::CargoType cargo);
oo::CargoType StringToCargoType(const std::string& string);
std::string EnergyUnitTypeToString(oo::EnergyUnitType unit);
oo::EnergyUnitType StringToEnergyUnitType(const std::string& string);
std::string RouteTypeToString(oo::RouteType routeType);
oo::RouteType StringToRouteType(const std::string& string);
std::string DockingClearanceStatusToString(
    oo::DockingClearanceStatus dockingClearanceStatus);

namespace oo {

std::string StringFromGraphicsDetail(GraphicsDetail detail);
GraphicsDetail GraphicsDetailFromString(const std::string& string);
std::string StringFromHDRToneMapper(oo::HDRToneMapper toneMapper);
oo::HDRToneMapper HDRToneMapperFromString(const std::string& string);
std::string StringFromSDRToneMapper(oo::SDRToneMapper toneMapper);
oo::SDRToneMapper SDRToneMapperFromString(const std::string& string);

// The .tbl families whose types come from OOTypes.h; the
// others are declared beside their enums in Entity.h, ShipEntity.h and
// PlayerEntity.h.
std::string StringFromCompassMode(oo::CompassMode mode);
oo::CompassMode CompassModeFromString(const std::string& string);
std::string StringFromLongRangeChartMode(oo::LongRangeChartMode chartMode);
oo::LongRangeChartMode LongRangeChartModeFromString(const std::string& string);
std::string StringFromLegalStatusReason(oo::LegalStatusReason reason);

}  // namespace oo

// Commodity display strings. A commodity is named by its
// identifier string.
std::string CommodityDisplayNameForSymbolicName(
    const std::string& symbolicName);
// an array
std::string CommodityDisplayNameForCommodityArray(
    const oo::PList& commodityDefinition);
// nullopt: no description (was nil)
std::optional<std::string> DisplayStringForMassUnit(oo::MassUnit unit);
std::optional<std::string> DisplayStringForMassUnitForCommodity(
    const std::string& commodity);

// Shader settings (OOShaderSetting is OOOpenGL.h's).
// Programmer-readable shader mode strings.
namespace oo {

ShaderSetting ShaderSettingFromString(const std::string& string);
std::string StringFromShaderSetting(ShaderSetting setting);
// Localized shader mode strings; nullopt: no description (was nil).
std::optional<std::string> DisplayStringFromShaderSetting(
    ShaderSetting setting);

}  // namespace oo
