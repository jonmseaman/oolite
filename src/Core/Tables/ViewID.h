/*	OOViewID.h

The values of a view direction: JS sees its name. The values are the program's
ordinals; OOViewID.tbl's value column was never used (its macro discarded it),
and the names the game reads and writes for them. It was OOViewID.tbl, an
X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "OOEnumFlags.h"
#include "OOEnumTable.h"

namespace oo {

enum class ViewID : std::int32_t {
  kForward = 0,
  kAft = 1,
  kPort = 2,
  kStarboard = 3,
  kCustom = 4,
  kNone = 5,
  kGUIDisplay = 6,
  kBreakPattern = 7,
};

// What an unknown name reads as.
inline constexpr ViewID kViewIDDefault = ViewID::kNone;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(ViewID value) noexcept {
  switch (value) {
    case ViewID::kForward:
      return "VIEW_FORWARD";
    case ViewID::kAft:
      return "VIEW_AFT";
    case ViewID::kPort:
      return "VIEW_PORT";
    case ViewID::kStarboard:
      return "VIEW_STARBOARD";
    case ViewID::kCustom:
      return "VIEW_CUSTOM";
    case ViewID::kNone:
      return "VIEW_NONE";
    case ViewID::kGUIDisplay:
      return "VIEW_GUI_DISPLAY";
    case ViewID::kBreakPattern:
      return "VIEW_BREAK_PATTERN";
  }
  return {};
}

inline constexpr auto kViewIDTable =
    oo::MakeEnumTable<ViewID, ViewID::kForward, ViewID::kAft, ViewID::kPort,
                      ViewID::kStarboard, ViewID::kCustom, ViewID::kNone,
                      ViewID::kGUIDisplay, ViewID::kBreakPattern>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::ViewID::kForward) == 0);
static_assert(oo::ToUnderlying(oo::ViewID::kAft) == 1);
static_assert(oo::ToUnderlying(oo::ViewID::kPort) == 2);
static_assert(oo::ToUnderlying(oo::ViewID::kStarboard) == 3);
static_assert(oo::ToUnderlying(oo::ViewID::kCustom) == 4);
static_assert(oo::ToUnderlying(oo::ViewID::kNone) == 5);
static_assert(oo::ToUnderlying(oo::ViewID::kGUIDisplay) == 6);
static_assert(oo::ToUnderlying(oo::ViewID::kBreakPattern) == 7);
