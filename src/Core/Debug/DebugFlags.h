#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

#include "EnumFlags.h"

// The debug console's flags. Their names and values are console.DEBUG_* in
// JavaScript.
namespace oo {

enum class DebugFlags : std::uint32_t {
  kNone = 0,
  kLinkedLists = 0x00000001,
  //	UNUSED						= 0x00000002,
  kCollisions = 0x00000004,
  kDocking = 0x00000008,
  kOctreeLogging = 0x00000010,
  //	UNUSED						= 0x00000020,
  kBoundingBoxes = 0x00000040,
  kOctreeDraw = 0x00000080,
  kDrawNormals = 0x00000100,
  kNoDust = 0x00000200,
  kNoShaderFallback = 0x00000400,
  kShaderValidation = 0x00000800,

  // Flag for temporary use, always last in list.
  kMisc = 0x10000000,

  kAll = 0xffffffff,
};

constexpr std::true_type OOEnableFlagOperators(DebugFlags);

// The JavaScript name of each flag, written out: never derived from the
// identifier.
constexpr std::string_view OOEnumName(DebugFlags f) noexcept {
  switch (f) {
    case DebugFlags::kLinkedLists:
      return "DEBUG_LINKED_LISTS";
    case DebugFlags::kCollisions:
      return "DEBUG_COLLISIONS";
    case DebugFlags::kDocking:
      return "DEBUG_DOCKING";
    case DebugFlags::kOctreeLogging:
      return "DEBUG_OCTREE_LOGGING";
    case DebugFlags::kBoundingBoxes:
      return "DEBUG_BOUNDING_BOXES";
    case DebugFlags::kOctreeDraw:
      return "DEBUG_OCTREE_DRAW";
    case DebugFlags::kDrawNormals:
      return "DEBUG_DRAW_NORMALS";
    case DebugFlags::kNoDust:
      return "DEBUG_NO_DUST";
    case DebugFlags::kNoShaderFallback:
      return "DEBUG_NO_SHADER_FALLBACK";
    case DebugFlags::kShaderValidation:
      return "DEBUG_SHADER_VALIDATION";
    case DebugFlags::kMisc:
      return "DEBUG_MISC";
    case DebugFlags::kNone:
    case DebugFlags::kAll:
      return {};
  }
  return {};
}

}  // namespace oo

// Every value scripts see, pinned.
static_assert(oo::ToUnderlying(oo::DebugFlags::kLinkedLists) == 0x00000001);
static_assert(oo::ToUnderlying(oo::DebugFlags::kCollisions) == 0x00000004);
static_assert(oo::ToUnderlying(oo::DebugFlags::kDocking) == 0x00000008);
static_assert(oo::ToUnderlying(oo::DebugFlags::kOctreeLogging) == 0x00000010);
static_assert(oo::ToUnderlying(oo::DebugFlags::kBoundingBoxes) == 0x00000040);
static_assert(oo::ToUnderlying(oo::DebugFlags::kOctreeDraw) == 0x00000080);
static_assert(oo::ToUnderlying(oo::DebugFlags::kDrawNormals) == 0x00000100);
static_assert(oo::ToUnderlying(oo::DebugFlags::kNoDust) == 0x00000200);
static_assert(oo::ToUnderlying(oo::DebugFlags::kNoShaderFallback) ==
              0x00000400);
static_assert(oo::ToUnderlying(oo::DebugFlags::kShaderValidation) ==
              0x00000800);
static_assert(oo::ToUnderlying(oo::DebugFlags::kMisc) == 0x10000000);
static_assert(oo::ToUnderlying(oo::DebugFlags::kAll) == 0xffffffff);

#ifdef OO_DEBUG_MONITOR_ENABLED

extern std::uint32_t gDebugFlags;
extern std::uint32_t gLiveEntityCount;
extern std::size_t gTotalEntityMemory;

#else

#define gDebugFlags (0)

#endif

// True if the debug console has turned flag on. Always false without the debug
// monitor.
namespace oo {

inline bool DebugFlagIsOn(DebugFlags flag) noexcept {
  return (gDebugFlags & oo::ToUnderlying(flag)) != 0;
}

}  // namespace oo
