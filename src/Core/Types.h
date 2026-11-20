/*

OOTypes.h

Various simple types that don't require us to pull in the associated class
headers.

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

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "Foundation.h"
#include "FunctionAttributes.h"

namespace oo {

enum class RouteType : std::int32_t { kNone, kJumps, kTime };

}  // namespace oo

// The numbers JS and the saved game's chart_ana_mode use.
static_assert(oo::ToUnderlying(oo::RouteType::kNone) == 0);
static_assert(oo::ToUnderlying(oo::RouteType::kJumps) == 1);
static_assert(oo::ToUnderlying(oo::RouteType::kTime) == 2);

namespace oo {

// The names the game writes for OORouteType (saves, logs and plists).
constexpr std::string_view OOEnumName(RouteType value) noexcept {
  switch (value) {
    case RouteType::kNone:
      return "OPTIMIZED_BY_NONE";
    case RouteType::kJumps:
      return "OPTIMIZED_BY_JUMPS";
    case RouteType::kTime:
      return "OPTIMIZED_BY_TIME";
  }
  return {};
}

}  // namespace oo

#include "CompassMode.h"
#include "LongRangeChartMode.h"
#include "ViewID.h"

// An int to a by-name call (ValueType.h): it answers it as a number.
namespace oo {

enum class AegisStatus : std::int32_t {
  kNone,
  kCloseToAnyPlanet,
  kCloseToMainPlanet,
  kInDockingRange
};

enum class CargoType : std::int32_t {
  kNotCargo = -1,
  kSlaves = 3,
  kAlloy = 9,
  kMinerals = 12,
  kThargoid = 16,
  kRandom = 100,
  kScriptedItem = 200,
  kCharacter = 300
};

}  // namespace oo

// The numbers shipdata's cargo_type uses.
static_assert(oo::ToUnderlying(oo::CargoType::kNotCargo) == -1);
static_assert(oo::ToUnderlying(oo::CargoType::kSlaves) == 3);
static_assert(oo::ToUnderlying(oo::CargoType::kAlloy) == 9);
static_assert(oo::ToUnderlying(oo::CargoType::kMinerals) == 12);
static_assert(oo::ToUnderlying(oo::CargoType::kThargoid) == 16);
static_assert(oo::ToUnderlying(oo::CargoType::kRandom) == 100);
static_assert(oo::ToUnderlying(oo::CargoType::kScriptedItem) == 200);
static_assert(oo::ToUnderlying(oo::CargoType::kCharacter) == 300);

namespace oo {

// The names the game writes for OOCargoType (saves, logs and plists).
constexpr std::string_view OOEnumName(CargoType value) noexcept {
  switch (value) {
    case CargoType::kNotCargo:
      return "CARGO_NOT_CARGO";
    case CargoType::kSlaves:
      return "CARGO_SLAVES";
    case CargoType::kAlloy:
      return "CARGO_ALLOY";
    case CargoType::kMinerals:
      return "CARGO_MINERALS";
    case CargoType::kThargoid:
      return "CARGO_THARGOID";
    case CargoType::kRandom:
      return "CARGO_RANDOM";
    case CargoType::kScriptedItem:
      return "CARGO_SCRIPTED_ITEM";
    case CargoType::kCharacter:
      return "CARGO_CHARACTER";
  }
  return {};
}

}  // namespace oo

/*
enum
{
        COMMODITY_UNDEFINED		= -1,

        // FIXME: hard-coded commodity types are used in OOJSManifest.
Everything else is data-driven. COMMODITY_FOOD,		//	=  0
        COMMODITY_TEXTILES,
        COMMODITY_RADIOACTIVES,
        COMMODITY_SLAVES,
        COMMODITY_LIQUOR_WINES,
        COMMODITY_LUXURIES,
        COMMODITY_NARCOTICS,
        COMMODITY_COMPUTERS,
        COMMODITY_MACHINERY,
        COMMODITY_ALLOYS,
        COMMODITY_FIREARMS,
        COMMODITY_FURS,
        COMMODITY_MINERALS,
        COMMODITY_GOLD,
        COMMODITY_PLATINUM,
        COMMODITY_GEM_STONES,
        COMMODITY_ALIEN_ITEMS
};
typedef std::ptrdiff_t OOCommodityType;
*/

namespace oo {

enum class CargoFlag : int {
  kNone = 400,
  kFullPlentiful = 501,
  kFullScarce = 502,
  kFullMedical = 503,
  kFullContraband = 504,
  kPirate = 505,
  kFullUniform = 510,
  kCanisters = 600,
  kFullPassengers = 700
};

// trade-goods.plist's quantity_unit numbers (OOMassUnitFromNumber).
enum class MassUnit : unsigned {
  kTons,
  kKilograms,
  kGrams,
  kUnknown  // a quantity_unit that is none of the above; displays as "??"
};

}  // namespace oo

static_assert(oo::ToUnderlying(oo::MassUnit::kTons) == 0);
static_assert(oo::ToUnderlying(oo::MassUnit::kKilograms) == 1);
static_assert(oo::ToUnderlying(oo::MassUnit::kGrams) == 2);

namespace oo {

enum class EnergyUnitType : std::int32_t {
  kNone,
  kNormalDamaged,
  kNavalDamaged,
  kOldNormal = 15,
  kOldNaval = 20,
  kNormal = 8,
  kNaval = 16
};

}  // namespace oo

// The numbers a legacy saved game's energy_unit uses.
static_assert(oo::ToUnderlying(oo::EnergyUnitType::kNone) == 0);
static_assert(oo::ToUnderlying(oo::EnergyUnitType::kNormalDamaged) == 1);
static_assert(oo::ToUnderlying(oo::EnergyUnitType::kNavalDamaged) == 2);
static_assert(oo::ToUnderlying(oo::EnergyUnitType::kOldNormal) == 15);
static_assert(oo::ToUnderlying(oo::EnergyUnitType::kOldNaval) == 20);
static_assert(oo::ToUnderlying(oo::EnergyUnitType::kNormal) == 8);
static_assert(oo::ToUnderlying(oo::EnergyUnitType::kNaval) == 16);

namespace oo {

// The names the game writes for OOEnergyUnitType (saves, logs and plists).
constexpr std::string_view OOEnumName(EnergyUnitType value) noexcept {
  switch (value) {
    case EnergyUnitType::kNone:
      return "ENERGY_UNIT_NONE";
    case EnergyUnitType::kNormal:
      return "ENERGY_UNIT_NORMAL";
    case EnergyUnitType::kNaval:
      return "ENERGY_UNIT_NAVAL";
    case EnergyUnitType::kNormalDamaged:
      return "ENERGY_UNIT_NORMAL_DAMAGED";
    case EnergyUnitType::kNavalDamaged:
      return "ENERGY_UNIT_NAVAL_DAMAGED";
    case EnergyUnitType::kOldNormal:  // a legacy save's number only: no name
    case EnergyUnitType::kOldNaval:
      return {};
  }
  return {};
}

}  // namespace oo

#include "LegalStatusReason.h"

namespace oo {

enum class DockingClearanceStatus : std::int32_t {
  kNone,
  kNotRequired,
  kRequested,
  kGranted,
  kTimingOut,
};

}  // namespace oo

// The numbers JS sees.
static_assert(oo::ToUnderlying(oo::DockingClearanceStatus::kNone) == 0);
static_assert(oo::ToUnderlying(oo::DockingClearanceStatus::kNotRequired) == 1);
static_assert(oo::ToUnderlying(oo::DockingClearanceStatus::kRequested) == 2);
static_assert(oo::ToUnderlying(oo::DockingClearanceStatus::kGranted) == 3);
static_assert(oo::ToUnderlying(oo::DockingClearanceStatus::kTimingOut) == 4);

namespace oo {

// The names the game writes for OODockingClearanceStatus (saves, logs and
// plists).
constexpr std::string_view OOEnumName(DockingClearanceStatus value) noexcept {
  switch (value) {
    case DockingClearanceStatus::kNone:
      return "DOCKING_CLEARANCE_STATUS_NONE";
    case DockingClearanceStatus::kRequested:
      return "DOCKING_CLEARANCE_STATUS_REQUESTED";
    case DockingClearanceStatus::kNotRequired:
      return "DOCKING_CLEARANCE_STATUS_NOT_REQUIRED";
    case DockingClearanceStatus::kGranted:
      return "DOCKING_CLEARANCE_STATUS_GRANTED";
    case DockingClearanceStatus::kTimingOut:
      return "DOCKING_CLEARANCE_STATUS_TIMING_OUT";
  }
  return {};
}

using CargoQuantity = uint32_t;
using CargoQuantityDelta = int32_t;

using FuelQuantity = uint16_t;

using CreditsQuantity = uint64_t;

}  // namespace oo

#define kOOMaxCredits ULLONG_MAX

namespace oo {

using KeyCode = uint16_t;

using UniversalID = uint16_t;

}  // namespace oo
   // Valid IDs range from 100 to 1000.

inline constexpr int UNIVERSE_MAX_ENTITIES = 2048;
inline constexpr int NO_TARGET = 0;
inline constexpr int MIN_ENTITY_UID = 100;
inline constexpr int MAX_ENTITY_UID =
    MIN_ENTITY_UID + UNIVERSE_MAX_ENTITIES + 1;

namespace oo {

inline constexpr int kVariableTechLevel = 99;
using TechLevelID = std::size_t;  // 0..14, 99 is special. oo::kNotFound is
                                  // used, so std::size_t required.

using GovernmentID = uint8_t;  // 0..7
using EconomyID = uint8_t;     // 0..7

using GalaxyID = uint8_t;  // 0..7
using SystemID = int16_t;  // 0..255, -1 for interstellar space (?)

inline constexpr int kMaximumGalaxyID = 7;
inline constexpr int kMaximumSystemID = 255;
inline constexpr int kMinimumSystemID = -1;
inline constexpr int kSystemIDInterstellarSpace = kMinimumSystemID;

using TimeAbsolute = double;
using TimeDelta = double;

// One weapon mount. An int to a by-name call (ValueType.h): it answers it as
// a number.
enum class WeaponFacing : std::uint8_t {
  kNone = 0,
  kForward = 1,
  kAft = 2,
  kPort = 4,
  kStarboard = 8
};

constexpr std::true_type OOEnableFlagOperators(WeaponFacing);

}  // namespace oo

// The numbers of shipdata's weapon_facings bits and of JS's weaponFacings.
static_assert(oo::ToUnderlying(oo::WeaponFacing::kNone) == 0);
static_assert(oo::ToUnderlying(oo::WeaponFacing::kForward) == 1);
static_assert(oo::ToUnderlying(oo::WeaponFacing::kAft) == 2);
static_assert(oo::ToUnderlying(oo::WeaponFacing::kPort) == 4);
static_assert(oo::ToUnderlying(oo::WeaponFacing::kStarboard) == 8);

// A set of mounts, as shipdata, shipyard.plist and JS give it: a number with an
// OOWeaponFacing bit per mount.
namespace oo {

using WeaponFacingSet = std::uint8_t;

inline constexpr WeaponFacingSet kValidWeaponFacings = oo::ToUnderlying(
    WeaponFacing::kNone | WeaponFacing::kForward | WeaponFacing::kAft |
    WeaponFacing::kPort | WeaponFacing::kStarboard);

// Takes any unsigned number of facings: the sets read from plists are unsigned.
inline bool WeaponFacingIsIn(unsigned facings, WeaponFacing facing) noexcept {
  return (facings & oo::ToUnderlying(facing)) != 0;
}

enum class GraphicsDetail : std::int32_t {
  kMinimum = 0,
  kNormal = 1,
  kShaders = 2,
  kExtras = 3
};
inline constexpr GraphicsDetail kGraphicsDetailMaximum =
    GraphicsDetail::kExtras;

}  // namespace oo

// The numbers the preferences' detailLevel, the debug console and the
// description keys use.
static_assert(oo::ToUnderlying(oo::GraphicsDetail::kMinimum) == 0);
static_assert(oo::ToUnderlying(oo::GraphicsDetail::kNormal) == 1);
static_assert(oo::ToUnderlying(oo::GraphicsDetail::kShaders) == 2);
static_assert(oo::ToUnderlying(oo::GraphicsDetail::kExtras) == 3);

namespace oo {

// The names the game writes for OOGraphicsDetail (saves, logs and plists).
constexpr std::string_view OOEnumName(GraphicsDetail value) noexcept {
  switch (value) {
    case GraphicsDetail::kMinimum:
      return "DETAIL_LEVEL_MINIMUM";
    case GraphicsDetail::kNormal:
      return "DETAIL_LEVEL_NORMAL";
    case GraphicsDetail::kShaders:
      return "DETAIL_LEVEL_SHADERS";
    case GraphicsDetail::kExtras:
      return "DETAIL_LEVEL_EXTRAS";
  }
  return {};
}

}  // namespace oo
