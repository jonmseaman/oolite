/*	OOLegalStatusReason.h

The values of the reason a legal status changed: JS sees its name, and the names
the game reads and writes for them. It was OOLegalStatusReason.tbl, an X-macro
table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "EnumTable.h"

namespace oo {

enum class LegalStatusReason : std::int32_t {
  kUnknown,
  kSetup,
  kByScript,
  kAttackedPolice,
  kAttackedMainStation,
  kAttackedInnocent,
  kSeenByPolice,
  kDistressCall,
  kIllegalExports,
  kIllegalImports,
  kAssistingOffender,
  kNewGalaxy,
  kNewSystem,
  kPaidFine,
  kEscapePod,
  kAssistingPolice,
  kPoliceAreClean,
  // kThereCanBeOnlyOne,
};

// What an unknown name reads as.
inline constexpr LegalStatusReason kLegalStatusReasonDefault =
    LegalStatusReason::kUnknown;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(LegalStatusReason value) noexcept {
  switch (value) {
    case LegalStatusReason::kUnknown:
      return "unknown";
    case LegalStatusReason::kSetup:
      return "setup actions";
    case LegalStatusReason::kByScript:
      return "scripted";
    case LegalStatusReason::kAttackedPolice:
      return "attacked police";
    case LegalStatusReason::kAttackedMainStation:
      return "attacked main station";
    case LegalStatusReason::kAttackedInnocent:
      return "attacked innocent";
    case LegalStatusReason::kSeenByPolice:
      return "seen by police";
    case LegalStatusReason::kDistressCall:
      return "distress call";
    case LegalStatusReason::kIllegalExports:
      return "illegal exports";
    case LegalStatusReason::kIllegalImports:
      return "illegal imports";
    case LegalStatusReason::kAssistingOffender:
      return "assisting offenders";
    case LegalStatusReason::kNewGalaxy:
      return "new galaxy";
    case LegalStatusReason::kNewSystem:
      return "new system";
    case LegalStatusReason::kPaidFine:
      return "paid fine";
    case LegalStatusReason::kEscapePod:
      return "escape pod";
    case LegalStatusReason::kAssistingPolice:
      return "assisting police";
    case LegalStatusReason::kPoliceAreClean:
      return "police are clean";
      // case OOLegalStatusReason::kThereCanBeOnlyOne:
      //   return "there can be only one";
  }
  return {};
}

inline constexpr auto kLegalStatusReasonTable = oo::MakeEnumTable<
    LegalStatusReason, LegalStatusReason::kUnknown, LegalStatusReason::kSetup,
    LegalStatusReason::kByScript, LegalStatusReason::kAttackedPolice,
    LegalStatusReason::kAttackedMainStation,
    LegalStatusReason::kAttackedInnocent, LegalStatusReason::kSeenByPolice,
    LegalStatusReason::kDistressCall, LegalStatusReason::kIllegalExports,
    LegalStatusReason::kIllegalImports, LegalStatusReason::kAssistingOffender,
    LegalStatusReason::kNewGalaxy, LegalStatusReason::kNewSystem,
    LegalStatusReason::kPaidFine, LegalStatusReason::kEscapePod,
    LegalStatusReason::kAssistingPolice, LegalStatusReason::kPoliceAreClean>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kUnknown) == 0);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kSetup) == 1);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kByScript) == 2);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kAttackedPolice) == 3);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kAttackedMainStation) ==
              4);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kAttackedInnocent) == 5);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kSeenByPolice) == 6);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kDistressCall) == 7);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kIllegalExports) == 8);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kIllegalImports) == 9);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kAssistingOffender) ==
              10);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kNewGalaxy) == 11);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kNewSystem) == 12);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kPaidFine) == 13);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kEscapePod) == 14);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kAssistingPolice) == 15);
static_assert(oo::ToUnderlying(oo::LegalStatusReason::kPoliceAreClean) == 16);
