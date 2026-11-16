

/*	OOShipDamageType.h

The values of a damage type: JS events carry its name, and the
names the game reads and writes for them. It was OOShipDamageType.tbl, an
X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "EnumTable.h"

namespace oo {

enum class ShipDamageType : std::int32_t {
  kScrape,
  kHeat,
  kEnergy,
  kCascadeWeapon,
  kHitAPlanet,
  kHitASun,
  kRemoved,
};

// What an unknown name reads as.
inline constexpr ShipDamageType kShipDamageTypeDefault =
    ShipDamageType::kEnergy;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(ShipDamageType value) noexcept {
  switch (value) {
    case ShipDamageType::kScrape:
      return "scrape damage";
    case ShipDamageType::kHeat:
      return "heat damage";
    case ShipDamageType::kEnergy:
      return "energy damage";
    case ShipDamageType::kCascadeWeapon:
      return "cascade weapon";
    case ShipDamageType::kHitAPlanet:
      return "hit a planet";
    case ShipDamageType::kHitASun:
      return "hit a sun";
    case ShipDamageType::kRemoved:
      return "removed";
  }
  return {};
}

inline constexpr auto kShipDamageTypeTable =
    oo::MakeEnumTable<ShipDamageType, ShipDamageType::kScrape,
                      ShipDamageType::kHeat, ShipDamageType::kEnergy,
                      ShipDamageType::kCascadeWeapon,
                      ShipDamageType::kHitAPlanet, ShipDamageType::kHitASun,
                      ShipDamageType::kRemoved>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::ShipDamageType::kScrape) == 0);
static_assert(oo::ToUnderlying(oo::ShipDamageType::kHeat) == 1);
static_assert(oo::ToUnderlying(oo::ShipDamageType::kEnergy) == 2);
static_assert(oo::ToUnderlying(oo::ShipDamageType::kCascadeWeapon) == 3);
static_assert(oo::ToUnderlying(oo::ShipDamageType::kHitAPlanet) == 4);
static_assert(oo::ToUnderlying(oo::ShipDamageType::kHitASun) == 5);
static_assert(oo::ToUnderlying(oo::ShipDamageType::kRemoved) == 6);
