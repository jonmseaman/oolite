/*	OOCompassMode.h

The values of the compass mode: shaders, saves, JS and callObjC (an int,
ValueType.h), and the names the game reads and writes for them.
It was OOCompassMode.tbl, an X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "OOEnumFlags.h"
#include "OOEnumTable.h"

namespace oo {

enum class CompassMode : std::int32_t {
  kInactive = -1,
  kBasic = 0,
  kPlanet = 1,
  kStation = 3,
  kSun = 4,
  kTarget = 5,
  kBeacons = 6,
};

// What an unknown name reads as.
inline constexpr CompassMode kCompassModeDefault = CompassMode::kBasic;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(CompassMode value) noexcept {
  switch (value) {
    case CompassMode::kInactive:
      return "COMPASS_MODE_INACTIVE";
    case CompassMode::kBasic:
      return "COMPASS_MODE_BASIC";
    case CompassMode::kPlanet:
      return "COMPASS_MODE_PLANET";
    case CompassMode::kStation:
      return "COMPASS_MODE_STATION";
    case CompassMode::kSun:
      return "COMPASS_MODE_SUN";
    case CompassMode::kTarget:
      return "COMPASS_MODE_TARGET";
    case CompassMode::kBeacons:
      return "COMPASS_MODE_BEACONS";
  }
  return {};
}

inline constexpr auto kCompassModeTable =
    oo::MakeEnumTable<CompassMode, CompassMode::kInactive, CompassMode::kBasic,
                      CompassMode::kPlanet, CompassMode::kStation,
                      CompassMode::kSun, CompassMode::kTarget,
                      CompassMode::kBeacons>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::CompassMode::kInactive) == -1);
static_assert(oo::ToUnderlying(oo::CompassMode::kBasic) == 0);
static_assert(oo::ToUnderlying(oo::CompassMode::kPlanet) == 1);
static_assert(oo::ToUnderlying(oo::CompassMode::kStation) == 3);
static_assert(oo::ToUnderlying(oo::CompassMode::kSun) == 4);
static_assert(oo::ToUnderlying(oo::CompassMode::kTarget) == 5);
static_assert(oo::ToUnderlying(oo::CompassMode::kBeacons) == 6);
