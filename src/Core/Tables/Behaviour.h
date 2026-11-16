/*	OOBehaviour.h

The values of a ship's behaviour: callObjC (an int, ValueType.h) and
the AI's logs, and the names the game reads and writes for them. It was
OOBehaviour.tbl, an X-macro table.
*/

#pragma once

#include <cstdint>
#include <string_view>

#include "EnumFlags.h"
#include "EnumTable.h"

namespace oo {

enum class Behaviour : std::int32_t {
  kIdle = 0,
  kTrackTarget = 1,
  kTumble = 4,
  kStopStill = 5,
  kStationKeeping = 10,
  kAttackTarget = 101,
  kAttackFlyToTarget = 102,
  kAttackFlyFromTarget = 103,
  kRunningDefense = 104,
  // fleeing
  kFleeTarget = 105,
  // advanced combat...
  kAttackFlyToTargetSix = 106,
  kAttackMiningTarget = 107,
  kEvasiveAction = 108,
  kFleeEvasiveAction = 109,
  kAttackBreakOffTarget = 110,
  kAttackSlowDogfight = 111,
  // further advanced combat...
  kAttackFlyToTargetTwelve = 112,
  kAttackSniper = 113,
  // side weapon combat...
  kAttackBroadside = 115,
  kAttackBroadsideLeft = 116,
  kAttackBroadsideRight = 117,
  kCloseToBroadsideRange = 118,
  kCloseWithTarget = 125,
  kAvoidCollision = 130,
  kTrackAsTurret = 150,
  kFlyRangeFromDestination = 200,
  kFlyToDestination = 201,
  kFlyFromDestination = 202,
  kFaceDestination = 203,
  kFlyThruNavpoints = 210,
  kLandOnPlanet = 250,
  kCollectTarget = 300,
  kInterceptTarget = 350,
  kFormationFormUp = 501,
  kFormationBreak = 502,
  kEnergyBombCountdown = 601,
  kTractored = 701,
  kScriptedAi = 801,
  kScriptedAttackAi = 811,
};

// The name of each value. No default: -Wswitch reports a value added without
// one.
constexpr std::string_view OOEnumName(Behaviour value) noexcept {
  switch (value) {
    case Behaviour::kIdle:
      return "BEHAVIOUR_IDLE";
    case Behaviour::kTrackTarget:
      return "BEHAVIOUR_TRACK_TARGET";
    case Behaviour::kTumble:
      return "BEHAVIOUR_TUMBLE";
    case Behaviour::kStopStill:
      return "BEHAVIOUR_STOP_STILL";
    case Behaviour::kStationKeeping:
      return "BEHAVIOUR_STATION_KEEPING";
    case Behaviour::kAttackTarget:
      return "BEHAVIOUR_ATTACK_TARGET";
    case Behaviour::kAttackFlyToTarget:
      return "BEHAVIOUR_ATTACK_FLY_TO_TARGET";
    case Behaviour::kAttackFlyFromTarget:
      return "BEHAVIOUR_ATTACK_FLY_FROM_TARGET";
    case Behaviour::kRunningDefense:
      return "BEHAVIOUR_RUNNING_DEFENSE";
    case Behaviour::kFleeTarget:
      return "BEHAVIOUR_FLEE_TARGET";
    case Behaviour::kAttackFlyToTargetSix:
      return "BEHAVIOUR_ATTACK_FLY_TO_TARGET_SIX";
    case Behaviour::kAttackMiningTarget:
      return "BEHAVIOUR_ATTACK_MINING_TARGET";
    case Behaviour::kEvasiveAction:
      return "BEHAVIOUR_EVASIVE_ACTION";
    case Behaviour::kFleeEvasiveAction:
      return "BEHAVIOUR_FLEE_EVASIVE_ACTION";
    case Behaviour::kAttackBreakOffTarget:
      return "BEHAVIOUR_ATTACK_BREAK_OFF_TARGET";
    case Behaviour::kAttackSlowDogfight:
      return "BEHAVIOUR_ATTACK_SLOW_DOGFIGHT";
    case Behaviour::kAttackFlyToTargetTwelve:
      return "BEHAVIOUR_ATTACK_FLY_TO_TARGET_TWELVE";
    case Behaviour::kAttackSniper:
      return "BEHAVIOUR_ATTACK_SNIPER";
    case Behaviour::kAttackBroadside:
      return "BEHAVIOUR_ATTACK_BROADSIDE";
    case Behaviour::kAttackBroadsideLeft:
      return "BEHAVIOUR_ATTACK_BROADSIDE_LEFT";
    case Behaviour::kAttackBroadsideRight:
      return "BEHAVIOUR_ATTACK_BROADSIDE_RIGHT";
    case Behaviour::kCloseToBroadsideRange:
      return "BEHAVIOUR_CLOSE_TO_BROADSIDE_RANGE";
    case Behaviour::kCloseWithTarget:
      return "BEHAVIOUR_CLOSE_WITH_TARGET";
    case Behaviour::kAvoidCollision:
      return "BEHAVIOUR_AVOID_COLLISION";
    case Behaviour::kTrackAsTurret:
      return "BEHAVIOUR_TRACK_AS_TURRET";
    case Behaviour::kFlyRangeFromDestination:
      return "BEHAVIOUR_FLY_RANGE_FROM_DESTINATION";
    case Behaviour::kFlyToDestination:
      return "BEHAVIOUR_FLY_TO_DESTINATION";
    case Behaviour::kFlyFromDestination:
      return "BEHAVIOUR_FLY_FROM_DESTINATION";
    case Behaviour::kFaceDestination:
      return "BEHAVIOUR_FACE_DESTINATION";
    case Behaviour::kFlyThruNavpoints:
      return "BEHAVIOUR_FLY_THRU_NAVPOINTS";
    case Behaviour::kLandOnPlanet:
      return "BEHAVIOUR_LAND_ON_PLANET";
    case Behaviour::kCollectTarget:
      return "BEHAVIOUR_COLLECT_TARGET";
    case Behaviour::kInterceptTarget:
      return "BEHAVIOUR_INTERCEPT_TARGET";
    case Behaviour::kFormationFormUp:
      return "BEHAVIOUR_FORMATION_FORM_UP";
    case Behaviour::kFormationBreak:
      return "BEHAVIOUR_FORMATION_BREAK";
    case Behaviour::kEnergyBombCountdown:
      return "BEHAVIOUR_ENERGY_BOMB_COUNTDOWN";
    case Behaviour::kTractored:
      return "BEHAVIOUR_TRACTORED";
    case Behaviour::kScriptedAi:
      return "BEHAVIOUR_SCRIPTED_AI";
    case Behaviour::kScriptedAttackAi:
      return "BEHAVIOUR_SCRIPTED_ATTACK_AI";
  }
  return {};
}

inline constexpr auto kBehaviourTable = oo::MakeEnumTable<
    Behaviour, Behaviour::kIdle, Behaviour::kTrackTarget, Behaviour::kTumble,
    Behaviour::kStopStill, Behaviour::kStationKeeping, Behaviour::kAttackTarget,
    Behaviour::kAttackFlyToTarget, Behaviour::kAttackFlyFromTarget,
    Behaviour::kRunningDefense, Behaviour::kFleeTarget,
    Behaviour::kAttackFlyToTargetSix, Behaviour::kAttackMiningTarget,
    Behaviour::kEvasiveAction, Behaviour::kFleeEvasiveAction,
    Behaviour::kAttackBreakOffTarget, Behaviour::kAttackSlowDogfight,
    Behaviour::kAttackFlyToTargetTwelve, Behaviour::kAttackSniper,
    Behaviour::kAttackBroadside, Behaviour::kAttackBroadsideLeft,
    Behaviour::kAttackBroadsideRight, Behaviour::kCloseToBroadsideRange,
    Behaviour::kCloseWithTarget, Behaviour::kAvoidCollision,
    Behaviour::kTrackAsTurret, Behaviour::kFlyRangeFromDestination,
    Behaviour::kFlyToDestination, Behaviour::kFlyFromDestination,
    Behaviour::kFaceDestination, Behaviour::kFlyThruNavpoints,
    Behaviour::kLandOnPlanet, Behaviour::kCollectTarget,
    Behaviour::kInterceptTarget, Behaviour::kFormationFormUp,
    Behaviour::kFormationBreak, Behaviour::kEnergyBombCountdown,
    Behaviour::kTractored, Behaviour::kScriptedAi,
    Behaviour::kScriptedAttackAi>();

}  // namespace oo

// Every number that leaves the program, pinned.
static_assert(oo::ToUnderlying(oo::Behaviour::kIdle) == 0);
static_assert(oo::ToUnderlying(oo::Behaviour::kTrackTarget) == 1);
static_assert(oo::ToUnderlying(oo::Behaviour::kTumble) == 4);
static_assert(oo::ToUnderlying(oo::Behaviour::kStopStill) == 5);
static_assert(oo::ToUnderlying(oo::Behaviour::kStationKeeping) == 10);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackTarget) == 101);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackFlyToTarget) == 102);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackFlyFromTarget) == 103);
static_assert(oo::ToUnderlying(oo::Behaviour::kRunningDefense) == 104);
static_assert(oo::ToUnderlying(oo::Behaviour::kFleeTarget) == 105);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackFlyToTargetSix) == 106);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackMiningTarget) == 107);
static_assert(oo::ToUnderlying(oo::Behaviour::kEvasiveAction) == 108);
static_assert(oo::ToUnderlying(oo::Behaviour::kFleeEvasiveAction) == 109);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackBreakOffTarget) == 110);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackSlowDogfight) == 111);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackFlyToTargetTwelve) == 112);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackSniper) == 113);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackBroadside) == 115);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackBroadsideLeft) == 116);
static_assert(oo::ToUnderlying(oo::Behaviour::kAttackBroadsideRight) == 117);
static_assert(oo::ToUnderlying(oo::Behaviour::kCloseToBroadsideRange) == 118);
static_assert(oo::ToUnderlying(oo::Behaviour::kCloseWithTarget) == 125);
static_assert(oo::ToUnderlying(oo::Behaviour::kAvoidCollision) == 130);
static_assert(oo::ToUnderlying(oo::Behaviour::kTrackAsTurret) == 150);
static_assert(oo::ToUnderlying(oo::Behaviour::kFlyRangeFromDestination) == 200);
static_assert(oo::ToUnderlying(oo::Behaviour::kFlyToDestination) == 201);
static_assert(oo::ToUnderlying(oo::Behaviour::kFlyFromDestination) == 202);
static_assert(oo::ToUnderlying(oo::Behaviour::kFaceDestination) == 203);
static_assert(oo::ToUnderlying(oo::Behaviour::kFlyThruNavpoints) == 210);
static_assert(oo::ToUnderlying(oo::Behaviour::kLandOnPlanet) == 250);
static_assert(oo::ToUnderlying(oo::Behaviour::kCollectTarget) == 300);
static_assert(oo::ToUnderlying(oo::Behaviour::kInterceptTarget) == 350);
static_assert(oo::ToUnderlying(oo::Behaviour::kFormationFormUp) == 501);
static_assert(oo::ToUnderlying(oo::Behaviour::kFormationBreak) == 502);
static_assert(oo::ToUnderlying(oo::Behaviour::kEnergyBombCountdown) == 601);
static_assert(oo::ToUnderlying(oo::Behaviour::kTractored) == 701);
static_assert(oo::ToUnderlying(oo::Behaviour::kScriptedAi) == 801);
static_assert(oo::ToUnderlying(oo::Behaviour::kScriptedAttackAi) == 811);
