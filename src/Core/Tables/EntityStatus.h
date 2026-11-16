

/*	OOEntityStatus.h

The values of an entity's status: saves, JS and callObjC (an int,
ValueType.h), and the names the game reads and writes for them. It was
OOEntityStatus.tbl, an X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "OOEnumFlags.h"
#include "OOEnumTable.h"

namespace oo {

enum class EntityStatus : std::int32_t {
  kStartGame = -10,
  kRestartGame = -2,
  kDead = -1,
  kInactive = 0,
  kTest = 1,
  kCockpitDisplay = 2,
  kActive = 5,
  kEffect = 10,
  kInFlight = 100,
  kDocked = 200,
  kAutopilotEngaged = 300,
  kDocking = 401,
  kLaunching = 402,
  kWitchspaceCountdown = 410,
  kEnteringWitchspace = 411,
  kExitingWitchspace = 412,
  kEscapeSequence = 500,
  kInHold = 600,
  kBeingScooped = 700,
  kHandlingError = 999,
};

// What an unknown name reads as.
inline constexpr EntityStatus kEntityStatusDefault = EntityStatus::kInactive;

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(EntityStatus value) noexcept {
  switch (value) {
    case EntityStatus::kStartGame:
      return "STATUS_START_GAME";
    case EntityStatus::kRestartGame:
      return "STATUS_RESTART_GAME";
    case EntityStatus::kDead:
      return "STATUS_DEAD";
    case EntityStatus::kInactive:
      return "STATUS_INACTIVE";
    case EntityStatus::kTest:
      return "STATUS_TEST";
    case EntityStatus::kCockpitDisplay:
      return "STATUS_COCKPIT_DISPLAY";
    case EntityStatus::kActive:
      return "STATUS_ACTIVE";
    case EntityStatus::kEffect:
      return "STATUS_EFFECT";
    case EntityStatus::kInFlight:
      return "STATUS_IN_FLIGHT";
    case EntityStatus::kDocked:
      return "STATUS_DOCKED";
    case EntityStatus::kAutopilotEngaged:
      return "STATUS_AUTOPILOT_ENGAGED";
    case EntityStatus::kDocking:
      return "STATUS_DOCKING";
    case EntityStatus::kLaunching:
      return "STATUS_LAUNCHING";
    case EntityStatus::kWitchspaceCountdown:
      return "STATUS_WITCHSPACE_COUNTDOWN";
    case EntityStatus::kEnteringWitchspace:
      return "STATUS_ENTERING_WITCHSPACE";
    case EntityStatus::kExitingWitchspace:
      return "STATUS_EXITING_WITCHSPACE";
    case EntityStatus::kEscapeSequence:
      return "STATUS_ESCAPE_SEQUENCE";
    case EntityStatus::kInHold:
      return "STATUS_IN_HOLD";
    case EntityStatus::kBeingScooped:
      return "STATUS_BEING_SCOOPED";
    case EntityStatus::kHandlingError:
      return "STATUS_HANDLING_ERROR";
  }
  return {};
}

inline constexpr auto kEntityStatusTable = oo::MakeEnumTable<
    EntityStatus, EntityStatus::kStartGame, EntityStatus::kRestartGame,
    EntityStatus::kDead, EntityStatus::kInactive, EntityStatus::kTest,
    EntityStatus::kCockpitDisplay, EntityStatus::kActive, EntityStatus::kEffect,
    EntityStatus::kInFlight, EntityStatus::kDocked,
    EntityStatus::kAutopilotEngaged, EntityStatus::kDocking,
    EntityStatus::kLaunching, EntityStatus::kWitchspaceCountdown,
    EntityStatus::kEnteringWitchspace, EntityStatus::kExitingWitchspace,
    EntityStatus::kEscapeSequence, EntityStatus::kInHold,
    EntityStatus::kBeingScooped, EntityStatus::kHandlingError>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::EntityStatus::kStartGame) == -10);
static_assert(oo::ToUnderlying(oo::EntityStatus::kRestartGame) == -2);
static_assert(oo::ToUnderlying(oo::EntityStatus::kDead) == -1);
static_assert(oo::ToUnderlying(oo::EntityStatus::kInactive) == 0);
static_assert(oo::ToUnderlying(oo::EntityStatus::kTest) == 1);
static_assert(oo::ToUnderlying(oo::EntityStatus::kCockpitDisplay) == 2);
static_assert(oo::ToUnderlying(oo::EntityStatus::kActive) == 5);
static_assert(oo::ToUnderlying(oo::EntityStatus::kEffect) == 10);
static_assert(oo::ToUnderlying(oo::EntityStatus::kInFlight) == 100);
static_assert(oo::ToUnderlying(oo::EntityStatus::kDocked) == 200);
static_assert(oo::ToUnderlying(oo::EntityStatus::kAutopilotEngaged) == 300);
static_assert(oo::ToUnderlying(oo::EntityStatus::kDocking) == 401);
static_assert(oo::ToUnderlying(oo::EntityStatus::kLaunching) == 402);
static_assert(oo::ToUnderlying(oo::EntityStatus::kWitchspaceCountdown) == 410);
static_assert(oo::ToUnderlying(oo::EntityStatus::kEnteringWitchspace) == 411);
static_assert(oo::ToUnderlying(oo::EntityStatus::kExitingWitchspace) == 412);
static_assert(oo::ToUnderlying(oo::EntityStatus::kEscapeSequence) == 500);
static_assert(oo::ToUnderlying(oo::EntityStatus::kInHold) == 600);
static_assert(oo::ToUnderlying(oo::EntityStatus::kBeingScooped) == 700);
static_assert(oo::ToUnderlying(oo::EntityStatus::kHandlingError) == 999);
