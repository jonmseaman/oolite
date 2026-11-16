/*	OOLongRangeChartMode.h

The values of the long-range chart's colour mode: saved games hold it as
chart_colour_mode (PlayerEntity.cpp), so the numbers are the program's ordinals,
pinned below, and not OOLongRangeChartMode.tbl's value column, which its macro
discarded. JS and callObjC (an int, ValueType.h) see it too; and the
names the game reads and writes for them. It was OOLongRangeChartMode.tbl, an
X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "EnumTable.h"

namespace oo {

enum class LongRangeChartMode : std::int32_t {
  kUnknown = 0,
  kSunColor = 1,
  kEconomy = 2,
  kGovernment = 3,
  kTechLevel = 4,
};

// What an unknown name reads as.
inline constexpr LongRangeChartMode kLongRangeChartModeDefault =
    LongRangeChartMode::kUnknown;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(LongRangeChartMode value) noexcept {
  switch (value) {
    case LongRangeChartMode::kUnknown:
      return "OOLRC_MODE_UNKNOWN";
    case LongRangeChartMode::kSunColor:
      return "OOLRC_MODE_SUNCOLOR";
    case LongRangeChartMode::kEconomy:
      return "OOLRC_MODE_ECONOMY";
    case LongRangeChartMode::kGovernment:
      return "OOLRC_MODE_GOVERNMENT";
    case LongRangeChartMode::kTechLevel:
      return "OOLRC_MODE_TECHLEVEL";
  }
  return {};
}

inline constexpr auto kLongRangeChartModeTable = oo::MakeEnumTable<
    LongRangeChartMode, LongRangeChartMode::kUnknown,
    LongRangeChartMode::kSunColor, LongRangeChartMode::kEconomy,
    LongRangeChartMode::kGovernment, LongRangeChartMode::kTechLevel>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::LongRangeChartMode::kUnknown) == 0);
static_assert(oo::ToUnderlying(oo::LongRangeChartMode::kSunColor) == 1);
static_assert(oo::ToUnderlying(oo::LongRangeChartMode::kEconomy) == 2);
static_assert(oo::ToUnderlying(oo::LongRangeChartMode::kGovernment) == 3);
static_assert(oo::ToUnderlying(oo::LongRangeChartMode::kTechLevel) == 4);
