/*	test_OOEnumFlags.cpp
        The flag operators of OOEnumFlags.h, the name table of
        OOEnumTable.h and the exemplar converted enum, OODebugFlags. Header-only; no game
        objects. Run: tests/run.sh unit
*/

#include <cstdint>
#include <string_view>

#include <gtest/gtest.h>

#include "DebugFlags.h"
#include "EnumFlags.h"
#include "EnumTable.h"

namespace {

enum class Plain : std::uint8_t { kA = 1, kB = 2 };  // no OOEnableFlagOperators
static_assert(!oo::FlagEnum<Plain>);
static_assert(oo::FlagEnum<oo::DebugFlags>);

}  // namespace

// Inside namespace oo, beside an unrelated operator|, a global flag enum's operators are still
// found (argument-dependent lookup looks in the global namespace, the enum's own).
namespace oo {
struct Unrelated {};
constexpr Unrelated operator|(Unrelated, Unrelated) { return {}; }
constexpr oo::DebugFlags kInsideOo = oo::DebugFlags::kCollisions | oo::DebugFlags::kDocking;
}  // namespace oo

TEST(OOEnumFlags, flagOperators) {
  constexpr auto both = oo::DebugFlags::kCollisions | oo::DebugFlags::kDocking;
  static_assert(oo::ToUnderlying(both) == 0x0c);
  static_assert(oo::ToUnderlying(oo::kInsideOo) == 0x0c);
  EXPECT_TRUE(oo::Any(both & oo::DebugFlags::kDocking));
  EXPECT_TRUE(!oo::Any(both & oo::DebugFlags::kNoDust));
  EXPECT_TRUE(oo::ToUnderlying(both ^ oo::DebugFlags::kDocking) == 0x04);
  EXPECT_TRUE(oo::ToUnderlying(~oo::DebugFlags::kNone) == 0xffffffffu);
  EXPECT_TRUE(~oo::DebugFlags::kNone == oo::DebugFlags::kAll);

  auto f = oo::DebugFlags::kNone;
  f |= oo::DebugFlags::kMisc;
  EXPECT_TRUE(f == oo::DebugFlags::kMisc);
  f ^= oo::DebugFlags::kLinkedLists;
  EXPECT_TRUE(oo::ToUnderlying(f) == 0x10000001u);
  f &= oo::DebugFlags::kLinkedLists;
  EXPECT_TRUE(f == oo::DebugFlags::kLinkedLists);
  EXPECT_TRUE(!oo::Any(oo::DebugFlags::kNone));
}

TEST(OOEnumFlags, debugFlagNames) {
  EXPECT_TRUE(OOEnumName(oo::DebugFlags::kLinkedLists) == "DEBUG_LINKED_LISTS");
  EXPECT_TRUE(OOEnumName(oo::DebugFlags::kNoShaderFallback) == "DEBUG_NO_SHADER_FALLBACK");
  EXPECT_TRUE(OOEnumName(oo::DebugFlags::kMisc) == "DEBUG_MISC");
  EXPECT_TRUE(OOEnumName(oo::DebugFlags::kNone).empty());
  EXPECT_TRUE(OOEnumName(oo::DebugFlags::kAll).empty());
  // The name is a null-terminated literal, so .data() can name a JS property.
  EXPECT_TRUE(OOEnumName(oo::DebugFlags::kDocking).data()[13] == '\0');
}

TEST(OOEnumFlags, enumTableLooksUpBothWays) {
  constexpr auto table = oo::MakeEnumTable<oo::DebugFlags, oo::DebugFlags::kLinkedLists,
                                           oo::DebugFlags::kCollisions, oo::DebugFlags::kMisc>();
  static_assert(table.entries.size() == 3);
  static_assert(table.name(oo::DebugFlags::kCollisions) == "DEBUG_COLLISIONS");
  static_assert(table.value("DEBUG_MISC") == oo::DebugFlags::kMisc);
  EXPECT_TRUE(table.name(oo::DebugFlags::kDocking).empty());  // not in the table
  EXPECT_TRUE(!table.value("DEBUG_DOCKING").has_value());
  EXPECT_TRUE(!table.value("debug_misc").has_value());  // case-sensitive
  EXPECT_TRUE(!table.value("").has_value());
}
