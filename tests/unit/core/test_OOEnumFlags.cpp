/*	test_OOEnumFlags.cpp
        The flag operators of OOEnumFlags.h, the name table of
        OOEnumTable.h and the exemplar converted enum, OODebugFlags. Header-only; no game
        objects. Run: tests/run.sh unit
*/

#include <cstdint>
#include <string_view>

#include "DebugFlags.h"
#include "OOEnumFlags.h"
#include "OOEnumTable.h"
#include "oo_test.hpp"

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

OO_TEST(flagOperators) {
  constexpr auto both = oo::DebugFlags::kCollisions | oo::DebugFlags::kDocking;
  static_assert(oo::ToUnderlying(both) == 0x0c);
  static_assert(oo::ToUnderlying(oo::kInsideOo) == 0x0c);
  OO_CHECK(oo::Any(both & oo::DebugFlags::kDocking));
  OO_CHECK(!oo::Any(both & oo::DebugFlags::kNoDust));
  OO_CHECK(oo::ToUnderlying(both ^ oo::DebugFlags::kDocking) == 0x04);
  OO_CHECK(oo::ToUnderlying(~oo::DebugFlags::kNone) == 0xffffffffu);
  OO_CHECK(~oo::DebugFlags::kNone == oo::DebugFlags::kAll);

  auto f = oo::DebugFlags::kNone;
  f |= oo::DebugFlags::kMisc;
  OO_CHECK(f == oo::DebugFlags::kMisc);
  f ^= oo::DebugFlags::kLinkedLists;
  OO_CHECK(oo::ToUnderlying(f) == 0x10000001u);
  f &= oo::DebugFlags::kLinkedLists;
  OO_CHECK(f == oo::DebugFlags::kLinkedLists);
  OO_CHECK(!oo::Any(oo::DebugFlags::kNone));
}

OO_TEST(debugFlagNames) {
  OO_CHECK(OOEnumName(oo::DebugFlags::kLinkedLists) == "DEBUG_LINKED_LISTS");
  OO_CHECK(OOEnumName(oo::DebugFlags::kNoShaderFallback) == "DEBUG_NO_SHADER_FALLBACK");
  OO_CHECK(OOEnumName(oo::DebugFlags::kMisc) == "DEBUG_MISC");
  OO_CHECK(OOEnumName(oo::DebugFlags::kNone).empty());
  OO_CHECK(OOEnumName(oo::DebugFlags::kAll).empty());
  // The name is a null-terminated literal, so .data() can name a JS property.
  OO_CHECK(OOEnumName(oo::DebugFlags::kDocking).data()[13] == '\0');
}

OO_TEST(enumTableLooksUpBothWays) {
  constexpr auto table = oo::MakeEnumTable<oo::DebugFlags, oo::DebugFlags::kLinkedLists,
                                           oo::DebugFlags::kCollisions, oo::DebugFlags::kMisc>();
  static_assert(table.entries.size() == 3);
  static_assert(table.name(oo::DebugFlags::kCollisions) == "DEBUG_COLLISIONS");
  static_assert(table.value("DEBUG_MISC") == oo::DebugFlags::kMisc);
  OO_CHECK(table.name(oo::DebugFlags::kDocking).empty());  // not in the table
  OO_CHECK(!table.value("DEBUG_DOCKING").has_value());
  OO_CHECK(!table.value("debug_misc").has_value());  // case-sensitive
  OO_CHECK(!table.value("").has_value());
}

OO_TEST_MAIN()
