/*	OOGalacticHyperspaceBehaviour.h

The values of the galactic hyperspace behaviour: planetinfo.plist, JS and
callObjC (an int, ValueType.h), and the names the game reads and
writes for them. It was OOGalacticHyperspaceBehaviour.tbl, an X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "EnumTable.h"

namespace oo {

enum class GalacticHyperspaceBehaviour : std::int32_t {
  kStandard = 0,
  kAllSystemsReachable = 1,
  kFixedCoordinates = 2,
  kUnknown = -1,
};

// What an unknown name reads as.
inline constexpr GalacticHyperspaceBehaviour
    kGalacticHyperspaceBehaviourDefault = GalacticHyperspaceBehaviour::kUnknown;

// The largest behaviour setGalacticHyperspaceBehaviour() accepts. An alias, so
// it takes no switch case and no row.
inline constexpr GalacticHyperspaceBehaviour kGalacticHyperspaceBehaviourMax =
    GalacticHyperspaceBehaviour::kFixedCoordinates;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(
    GalacticHyperspaceBehaviour value) noexcept {
  switch (value) {
    case GalacticHyperspaceBehaviour::kStandard:
      return "BEHAVIOUR_STANDARD";
    case GalacticHyperspaceBehaviour::kAllSystemsReachable:
      return "BEHAVIOUR_ALL_SYSTEMS_REACHABLE";
    case GalacticHyperspaceBehaviour::kFixedCoordinates:
      return "BEHAVIOUR_FIXED_COORDINATES";
    case GalacticHyperspaceBehaviour::kUnknown:
      return "BEHAVIOUR_UNKNOWN";
  }
  return {};
}

inline constexpr auto kGalacticHyperspaceBehaviourTable =
    oo::MakeEnumTable<GalacticHyperspaceBehaviour,
                      GalacticHyperspaceBehaviour::kStandard,
                      GalacticHyperspaceBehaviour::kAllSystemsReachable,
                      GalacticHyperspaceBehaviour::kFixedCoordinates,
                      GalacticHyperspaceBehaviour::kUnknown>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::GalacticHyperspaceBehaviour::kStandard) ==
              0);
static_assert(oo::ToUnderlying(
                  oo::GalacticHyperspaceBehaviour::kAllSystemsReachable) == 1);
static_assert(
    oo::ToUnderlying(oo::GalacticHyperspaceBehaviour::kFixedCoordinates) == 2);
static_assert(oo::ToUnderlying(oo::GalacticHyperspaceBehaviour::kUnknown) ==
              -1);
