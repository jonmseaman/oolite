

/*	OOScanClass.h

The values of a scan class: shipdata, saves, JS and callObjC (an int,
ValueType.h), and the names the game reads and writes for them. It was
OOScanClass.tbl, an X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "EnumTable.h"

namespace oo {

enum class ScanClass : std::int32_t {
  kNotSet = -1,
  kNoDraw = 0,
  kNeutral = 1,
  kStation = 3,
  kTarget = 4,
  kCargo = 5,
  kMissile = 6,
  kRock = 7,
  kMine = 8,
  kThargoid = 9,
  kBuoy = 10,
  kVisualEffect = 11,
  kWormhole = 444,
  kPlayer = 100,
  kPolice = 999,
  kMilitary = 333,
};

// What an unknown name reads as.
inline constexpr ScanClass kScanClassDefault = ScanClass::kNotSet;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(ScanClass value) noexcept {
  switch (value) {
    case ScanClass::kNotSet:
      return "CLASS_NOT_SET";
    case ScanClass::kNoDraw:
      return "CLASS_NO_DRAW";
    case ScanClass::kNeutral:
      return "CLASS_NEUTRAL";
    case ScanClass::kStation:
      return "CLASS_STATION";
    case ScanClass::kTarget:
      return "CLASS_TARGET";
    case ScanClass::kCargo:
      return "CLASS_CARGO";
    case ScanClass::kMissile:
      return "CLASS_MISSILE";
    case ScanClass::kRock:
      return "CLASS_ROCK";
    case ScanClass::kMine:
      return "CLASS_MINE";
    case ScanClass::kThargoid:
      return "CLASS_THARGOID";
    case ScanClass::kBuoy:
      return "CLASS_BUOY";
    case ScanClass::kVisualEffect:
      return "CLASS_VISUAL_EFFECT";
    case ScanClass::kWormhole:
      return "CLASS_WORMHOLE";
    case ScanClass::kPlayer:
      return "CLASS_PLAYER";
    case ScanClass::kPolice:
      return "CLASS_POLICE";
    case ScanClass::kMilitary:
      return "CLASS_MILITARY";
  }
  return {};
}

inline constexpr auto kScanClassTable = oo::MakeEnumTable<
    ScanClass, ScanClass::kNotSet, ScanClass::kNoDraw, ScanClass::kNeutral,
    ScanClass::kStation, ScanClass::kTarget, ScanClass::kCargo,
    ScanClass::kMissile, ScanClass::kRock, ScanClass::kMine,
    ScanClass::kThargoid, ScanClass::kBuoy, ScanClass::kVisualEffect,
    ScanClass::kWormhole, ScanClass::kPlayer, ScanClass::kPolice,
    ScanClass::kMilitary>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::ScanClass::kNotSet) == -1);
static_assert(oo::ToUnderlying(oo::ScanClass::kNoDraw) == 0);
static_assert(oo::ToUnderlying(oo::ScanClass::kNeutral) == 1);
static_assert(oo::ToUnderlying(oo::ScanClass::kStation) == 3);
static_assert(oo::ToUnderlying(oo::ScanClass::kTarget) == 4);
static_assert(oo::ToUnderlying(oo::ScanClass::kCargo) == 5);
static_assert(oo::ToUnderlying(oo::ScanClass::kMissile) == 6);
static_assert(oo::ToUnderlying(oo::ScanClass::kRock) == 7);
static_assert(oo::ToUnderlying(oo::ScanClass::kMine) == 8);
static_assert(oo::ToUnderlying(oo::ScanClass::kThargoid) == 9);
static_assert(oo::ToUnderlying(oo::ScanClass::kBuoy) == 10);
static_assert(oo::ToUnderlying(oo::ScanClass::kVisualEffect) == 11);
static_assert(oo::ToUnderlying(oo::ScanClass::kWormhole) == 444);
static_assert(oo::ToUnderlying(oo::ScanClass::kPlayer) == 100);
static_assert(oo::ToUnderlying(oo::ScanClass::kPolice) == 999);
static_assert(oo::ToUnderlying(oo::ScanClass::kMilitary) == 333);
