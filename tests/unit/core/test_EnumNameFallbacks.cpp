/*	test_EnumNameFallbacks.cpp
        The names the game reads and writes for its enums: saved games, scripts and expansions
   depend on them. For every enum with a name lookup in OOConstToString.cpp and
   OOConstToJSString.cpp: each value's exact string, each known name's value, and the value an
   unknown name falls back to. Every string here is written out: written from today's
   OOConstToString.cpp and the .tbl tables of the time by a script (the fallbacks resolved to the enumerators they name today), and only ever added to. Run:
   tests/run.sh unit
*/

#include <quickjs.h>

#include <string>

#include <gtest/gtest.h>

#include "OOConstToJSString.h"
#include "ConstToString.h"
#include "OpenGL.h"
#include "Types.h"
#include "PlayerEntity.h"
#include "ShipEntity.h"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

TEST(EnumNameFallbacks, OOStringFromEntityStatus) {
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kStartGame) == "STATUS_START_GAME");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kRestartGame) == "STATUS_RESTART_GAME");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kDead) == "STATUS_DEAD");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kInactive) == "STATUS_INACTIVE");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kTest) == "STATUS_TEST");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kCockpitDisplay) == "STATUS_COCKPIT_DISPLAY");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kActive) == "STATUS_ACTIVE");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kEffect) == "STATUS_EFFECT");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kInFlight) == "STATUS_IN_FLIGHT");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kDocked) == "STATUS_DOCKED");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kAutopilotEngaged) == "STATUS_AUTOPILOT_ENGAGED");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kDocking) == "STATUS_DOCKING");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kLaunching) == "STATUS_LAUNCHING");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kWitchspaceCountdown) == "STATUS_WITCHSPACE_COUNTDOWN");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kEnteringWitchspace) == "STATUS_ENTERING_WITCHSPACE");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kExitingWitchspace) == "STATUS_EXITING_WITCHSPACE");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kEscapeSequence) == "STATUS_ESCAPE_SEQUENCE");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kInHold) == "STATUS_IN_HOLD");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kBeingScooped) == "STATUS_BEING_SCOOPED");
  EXPECT_TRUE(oo::StringFromEntityStatus(oo::EntityStatus::kHandlingError) == "STATUS_HANDLING_ERROR");
}

TEST(EnumNameFallbacks, OOStringFromBehaviour) {
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kIdle) == "BEHAVIOUR_IDLE");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kTrackTarget) == "BEHAVIOUR_TRACK_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kTumble) == "BEHAVIOUR_TUMBLE");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kStopStill) == "BEHAVIOUR_STOP_STILL");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kStationKeeping) == "BEHAVIOUR_STATION_KEEPING");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackTarget) == "BEHAVIOUR_ATTACK_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackFlyToTarget) == "BEHAVIOUR_ATTACK_FLY_TO_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackFlyFromTarget) == "BEHAVIOUR_ATTACK_FLY_FROM_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kRunningDefense) == "BEHAVIOUR_RUNNING_DEFENSE");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFleeTarget) == "BEHAVIOUR_FLEE_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackFlyToTargetSix) == "BEHAVIOUR_ATTACK_FLY_TO_TARGET_SIX");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackMiningTarget) == "BEHAVIOUR_ATTACK_MINING_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kEvasiveAction) == "BEHAVIOUR_EVASIVE_ACTION");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFleeEvasiveAction) == "BEHAVIOUR_FLEE_EVASIVE_ACTION");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackBreakOffTarget) == "BEHAVIOUR_ATTACK_BREAK_OFF_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackSlowDogfight) == "BEHAVIOUR_ATTACK_SLOW_DOGFIGHT");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackFlyToTargetTwelve) == "BEHAVIOUR_ATTACK_FLY_TO_TARGET_TWELVE");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackSniper) == "BEHAVIOUR_ATTACK_SNIPER");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackBroadside) == "BEHAVIOUR_ATTACK_BROADSIDE");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackBroadsideLeft) == "BEHAVIOUR_ATTACK_BROADSIDE_LEFT");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAttackBroadsideRight) == "BEHAVIOUR_ATTACK_BROADSIDE_RIGHT");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kCloseToBroadsideRange) == "BEHAVIOUR_CLOSE_TO_BROADSIDE_RANGE");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kCloseWithTarget) == "BEHAVIOUR_CLOSE_WITH_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kAvoidCollision) == "BEHAVIOUR_AVOID_COLLISION");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kTrackAsTurret) == "BEHAVIOUR_TRACK_AS_TURRET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFlyRangeFromDestination) == "BEHAVIOUR_FLY_RANGE_FROM_DESTINATION");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFlyToDestination) == "BEHAVIOUR_FLY_TO_DESTINATION");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFlyFromDestination) == "BEHAVIOUR_FLY_FROM_DESTINATION");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFaceDestination) == "BEHAVIOUR_FACE_DESTINATION");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFlyThruNavpoints) == "BEHAVIOUR_FLY_THRU_NAVPOINTS");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kLandOnPlanet) == "BEHAVIOUR_LAND_ON_PLANET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kCollectTarget) == "BEHAVIOUR_COLLECT_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kInterceptTarget) == "BEHAVIOUR_INTERCEPT_TARGET");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFormationFormUp) == "BEHAVIOUR_FORMATION_FORM_UP");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kFormationBreak) == "BEHAVIOUR_FORMATION_BREAK");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kEnergyBombCountdown) == "BEHAVIOUR_ENERGY_BOMB_COUNTDOWN");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kTractored) == "BEHAVIOUR_TRACTORED");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kScriptedAi) == "BEHAVIOUR_SCRIPTED_AI");
  EXPECT_TRUE(oo::StringFromBehaviour(oo::Behaviour::kScriptedAttackAi) == "BEHAVIOUR_SCRIPTED_ATTACK_AI");
}

TEST(EnumNameFallbacks, OOStringFromCompassMode) {
  EXPECT_TRUE(oo::StringFromCompassMode(oo::CompassMode::kInactive) == "COMPASS_MODE_INACTIVE");
  EXPECT_TRUE(oo::StringFromCompassMode(oo::CompassMode::kBasic) == "COMPASS_MODE_BASIC");
  EXPECT_TRUE(oo::StringFromCompassMode(oo::CompassMode::kPlanet) == "COMPASS_MODE_PLANET");
  EXPECT_TRUE(oo::StringFromCompassMode(oo::CompassMode::kStation) == "COMPASS_MODE_STATION");
  EXPECT_TRUE(oo::StringFromCompassMode(oo::CompassMode::kSun) == "COMPASS_MODE_SUN");
  EXPECT_TRUE(oo::StringFromCompassMode(oo::CompassMode::kTarget) == "COMPASS_MODE_TARGET");
  EXPECT_TRUE(oo::StringFromCompassMode(oo::CompassMode::kBeacons) == "COMPASS_MODE_BEACONS");
}

TEST(EnumNameFallbacks, OOStringFromLongRangeChartMode) {
  EXPECT_TRUE(oo::StringFromLongRangeChartMode(oo::LongRangeChartMode::kUnknown) == "OOLRC_MODE_UNKNOWN");
  EXPECT_TRUE(oo::StringFromLongRangeChartMode(oo::LongRangeChartMode::kSunColor) == "OOLRC_MODE_SUNCOLOR");
  EXPECT_TRUE(oo::StringFromLongRangeChartMode(oo::LongRangeChartMode::kEconomy) == "OOLRC_MODE_ECONOMY");
  EXPECT_TRUE(oo::StringFromLongRangeChartMode(oo::LongRangeChartMode::kGovernment) == "OOLRC_MODE_GOVERNMENT");
  EXPECT_TRUE(oo::StringFromLongRangeChartMode(oo::LongRangeChartMode::kTechLevel) == "OOLRC_MODE_TECHLEVEL");
}

TEST(EnumNameFallbacks, OOStringFromGalacticHyperspaceBehaviour) {
  EXPECT_TRUE(oo::StringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kStandard) == "BEHAVIOUR_STANDARD");
  EXPECT_TRUE(oo::StringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kAllSystemsReachable) == "BEHAVIOUR_ALL_SYSTEMS_REACHABLE");
  EXPECT_TRUE(oo::StringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kFixedCoordinates) == "BEHAVIOUR_FIXED_COORDINATES");
  EXPECT_TRUE(oo::StringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kUnknown) == "BEHAVIOUR_UNKNOWN");
}

TEST(EnumNameFallbacks, OOStringFromGUIScreenID) {
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kMain) == "GUI_SCREEN_MAIN");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kIntro1) == "GUI_SCREEN_INTRO1");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kShipLibrary) == "GUI_SCREEN_SHIPLIBRARY");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kKeyboard) == "GUI_SCREEN_KEYBOARD");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kNewGame) == "GUI_SCREEN_NEWGAME");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kOXZManager) == "GUI_SCREEN_OXZMANAGER");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kStatus) == "GUI_SCREEN_STATUS");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kManifest) == "GUI_SCREEN_MANIFEST");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kEquipShip) == "GUI_SCREEN_EQUIP_SHIP");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kShipyard) == "GUI_SCREEN_SHIPYARD");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kLongRangeChart) == "GUI_SCREEN_LONG_RANGE_CHART");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kShortRangeChart) == "GUI_SCREEN_SHORT_RANGE_CHART");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kSystemData) == "GUI_SCREEN_SYSTEM_DATA");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kMarket) == "GUI_SCREEN_MARKET");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kMarketInfo) == "GUI_SCREEN_MARKETINFO");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kOptions) == "GUI_SCREEN_OPTIONS");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kGameOptions) == "GUI_SCREEN_GAMEOPTIONS");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kLoad) == "GUI_SCREEN_LOAD");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kSave) == "GUI_SCREEN_SAVE");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kSaveOverwrite) == "GUI_SCREEN_SAVE_OVERWRITE");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kStickMapper) == "GUI_SCREEN_STICKMAPPER");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kStickProfile) == "GUI_SCREEN_STICKPROFILE");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kMission) == "GUI_SCREEN_MISSION");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kReport) == "GUI_SCREEN_REPORT");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kInterfaces) == "GUI_SCREEN_INTERFACES");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kKeyboardConfirmClear) == "GUI_SCREEN_KEYBOARD_CONFIRMCLEAR");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kKeyboardConfig) == "GUI_SCREEN_KEYBOARD_CONFIG");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kKeyboardEntry) == "GUI_SCREEN_KEYBOARD_ENTRY");
  EXPECT_TRUE(oo::StringFromGUIScreenID(oo::GUIScreenID::kKeyboardLayout) == "GUI_SCREEN_KEYBOARD_LAYOUT");
}

TEST(EnumNameFallbacks, OOStringFromScanClass) {
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kNotSet) == "CLASS_NOT_SET");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kNoDraw) == "CLASS_NO_DRAW");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kNeutral) == "CLASS_NEUTRAL");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kStation) == "CLASS_STATION");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kTarget) == "CLASS_TARGET");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kCargo) == "CLASS_CARGO");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kMissile) == "CLASS_MISSILE");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kRock) == "CLASS_ROCK");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kMine) == "CLASS_MINE");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kThargoid) == "CLASS_THARGOID");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kBuoy) == "CLASS_BUOY");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kVisualEffect) == "CLASS_VISUAL_EFFECT");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kWormhole) == "CLASS_WORMHOLE");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kPlayer) == "CLASS_PLAYER");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kPolice) == "CLASS_POLICE");
  EXPECT_TRUE(oo::StringFromScanClass(oo::ScanClass::kMilitary) == "CLASS_MILITARY");
}

TEST(EnumNameFallbacks, OOStringFromShipDamageType) {
  EXPECT_TRUE(oo::StringFromShipDamageType(oo::ShipDamageType::kScrape) == "scrape damage");
  EXPECT_TRUE(oo::StringFromShipDamageType(oo::ShipDamageType::kHeat) == "heat damage");
  EXPECT_TRUE(oo::StringFromShipDamageType(oo::ShipDamageType::kEnergy) == "energy damage");
  EXPECT_TRUE(oo::StringFromShipDamageType(oo::ShipDamageType::kCascadeWeapon) == "cascade weapon");
  EXPECT_TRUE(oo::StringFromShipDamageType(oo::ShipDamageType::kHitAPlanet) == "hit a planet");
  EXPECT_TRUE(oo::StringFromShipDamageType(oo::ShipDamageType::kHitASun) == "hit a sun");
  EXPECT_TRUE(oo::StringFromShipDamageType(oo::ShipDamageType::kRemoved) == "removed");
}

TEST(EnumNameFallbacks, OOStringFromLegalStatusReason) {
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kUnknown) == "unknown");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kSetup) == "setup actions");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kByScript) == "scripted");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kAttackedPolice) == "attacked police");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kAttackedMainStation) == "attacked main station");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kAttackedInnocent) == "attacked innocent");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kSeenByPolice) == "seen by police");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kDistressCall) == "distress call");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kIllegalExports) == "illegal exports");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kIllegalImports) == "illegal imports");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kAssistingOffender) == "assisting offenders");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kNewGalaxy) == "new galaxy");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kNewSystem) == "new system");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kPaidFine) == "paid fine");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kEscapePod) == "escape pod");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kAssistingPolice) == "assisting police");
  EXPECT_TRUE(oo::StringFromLegalStatusReason(oo::LegalStatusReason::kPoliceAreClean) == "police are clean");
}

TEST(EnumNameFallbacks, OOEntityStatusFromString) {
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_START_GAME") == oo::EntityStatus::kStartGame);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_RESTART_GAME") == oo::EntityStatus::kRestartGame);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_DEAD") == oo::EntityStatus::kDead);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_INACTIVE") == oo::EntityStatus::kInactive);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_TEST") == oo::EntityStatus::kTest);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_COCKPIT_DISPLAY") == oo::EntityStatus::kCockpitDisplay);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_ACTIVE") == oo::EntityStatus::kActive);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_EFFECT") == oo::EntityStatus::kEffect);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_IN_FLIGHT") == oo::EntityStatus::kInFlight);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_DOCKED") == oo::EntityStatus::kDocked);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_AUTOPILOT_ENGAGED") == oo::EntityStatus::kAutopilotEngaged);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_DOCKING") == oo::EntityStatus::kDocking);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_LAUNCHING") == oo::EntityStatus::kLaunching);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_WITCHSPACE_COUNTDOWN") == oo::EntityStatus::kWitchspaceCountdown);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_ENTERING_WITCHSPACE") == oo::EntityStatus::kEnteringWitchspace);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_EXITING_WITCHSPACE") == oo::EntityStatus::kExitingWitchspace);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_ESCAPE_SEQUENCE") == oo::EntityStatus::kEscapeSequence);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_IN_HOLD") == oo::EntityStatus::kInHold);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_BEING_SCOOPED") == oo::EntityStatus::kBeingScooped);
  EXPECT_TRUE(oo::EntityStatusFromString("STATUS_HANDLING_ERROR") == oo::EntityStatus::kHandlingError);
  EXPECT_TRUE(oo::EntityStatusFromString("NOT_A_NAME") == oo::EntityStatus::kInactive);  // the fallback
  EXPECT_TRUE(oo::EntityStatusFromString("") == oo::EntityStatus::kInactive);
}

TEST(EnumNameFallbacks, OOCompassModeFromString) {
  EXPECT_TRUE(oo::CompassModeFromString("COMPASS_MODE_INACTIVE") == oo::CompassMode::kInactive);
  EXPECT_TRUE(oo::CompassModeFromString("COMPASS_MODE_BASIC") == oo::CompassMode::kBasic);
  EXPECT_TRUE(oo::CompassModeFromString("COMPASS_MODE_PLANET") == oo::CompassMode::kPlanet);
  EXPECT_TRUE(oo::CompassModeFromString("COMPASS_MODE_STATION") == oo::CompassMode::kStation);
  EXPECT_TRUE(oo::CompassModeFromString("COMPASS_MODE_SUN") == oo::CompassMode::kSun);
  EXPECT_TRUE(oo::CompassModeFromString("COMPASS_MODE_TARGET") == oo::CompassMode::kTarget);
  EXPECT_TRUE(oo::CompassModeFromString("COMPASS_MODE_BEACONS") == oo::CompassMode::kBeacons);
  EXPECT_TRUE(oo::CompassModeFromString("NOT_A_NAME") == oo::CompassMode::kBasic);  // the fallback
  EXPECT_TRUE(oo::CompassModeFromString("") == oo::CompassMode::kBasic);
}

TEST(EnumNameFallbacks, OOGalacticHyperspaceBehaviourFromString) {
  EXPECT_TRUE(oo::GalacticHyperspaceBehaviourFromString("BEHAVIOUR_STANDARD") == oo::GalacticHyperspaceBehaviour::kStandard);
  EXPECT_TRUE(oo::GalacticHyperspaceBehaviourFromString("BEHAVIOUR_ALL_SYSTEMS_REACHABLE") == oo::GalacticHyperspaceBehaviour::kAllSystemsReachable);
  EXPECT_TRUE(oo::GalacticHyperspaceBehaviourFromString("BEHAVIOUR_FIXED_COORDINATES") == oo::GalacticHyperspaceBehaviour::kFixedCoordinates);
  EXPECT_TRUE(oo::GalacticHyperspaceBehaviourFromString("BEHAVIOUR_UNKNOWN") == oo::GalacticHyperspaceBehaviour::kUnknown);
  EXPECT_TRUE(oo::GalacticHyperspaceBehaviourFromString("NOT_A_NAME") == oo::GalacticHyperspaceBehaviour::kUnknown);  // the fallback
  EXPECT_TRUE(oo::GalacticHyperspaceBehaviourFromString("") == oo::GalacticHyperspaceBehaviour::kUnknown);
}

TEST(EnumNameFallbacks, OOGUIScreenIDFromString) {
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_MAIN") == oo::GUIScreenID::kMain);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_INTRO1") == oo::GUIScreenID::kIntro1);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_SHIPLIBRARY") == oo::GUIScreenID::kShipLibrary);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_KEYBOARD") == oo::GUIScreenID::kKeyboard);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_NEWGAME") == oo::GUIScreenID::kNewGame);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_OXZMANAGER") == oo::GUIScreenID::kOXZManager);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_STATUS") == oo::GUIScreenID::kStatus);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_MANIFEST") == oo::GUIScreenID::kManifest);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_EQUIP_SHIP") == oo::GUIScreenID::kEquipShip);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_SHIPYARD") == oo::GUIScreenID::kShipyard);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_LONG_RANGE_CHART") == oo::GUIScreenID::kLongRangeChart);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_SHORT_RANGE_CHART") == oo::GUIScreenID::kShortRangeChart);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_SYSTEM_DATA") == oo::GUIScreenID::kSystemData);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_MARKET") == oo::GUIScreenID::kMarket);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_MARKETINFO") == oo::GUIScreenID::kMarketInfo);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_OPTIONS") == oo::GUIScreenID::kOptions);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_GAMEOPTIONS") == oo::GUIScreenID::kGameOptions);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_LOAD") == oo::GUIScreenID::kLoad);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_SAVE") == oo::GUIScreenID::kSave);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_SAVE_OVERWRITE") == oo::GUIScreenID::kSaveOverwrite);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_STICKMAPPER") == oo::GUIScreenID::kStickMapper);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_STICKPROFILE") == oo::GUIScreenID::kStickProfile);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_MISSION") == oo::GUIScreenID::kMission);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_REPORT") == oo::GUIScreenID::kReport);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_INTERFACES") == oo::GUIScreenID::kInterfaces);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_KEYBOARD_CONFIRMCLEAR") == oo::GUIScreenID::kKeyboardConfirmClear);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_KEYBOARD_CONFIG") == oo::GUIScreenID::kKeyboardConfig);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_KEYBOARD_ENTRY") == oo::GUIScreenID::kKeyboardEntry);
  EXPECT_TRUE(oo::GUIScreenIDFromString("GUI_SCREEN_KEYBOARD_LAYOUT") == oo::GUIScreenID::kKeyboardLayout);
  EXPECT_TRUE(oo::GUIScreenIDFromString("NOT_A_NAME") == oo::GUIScreenID::kMain);  // the fallback
  EXPECT_TRUE(oo::GUIScreenIDFromString("") == oo::GUIScreenID::kMain);
}

TEST(EnumNameFallbacks, OOScanClassFromString) {
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_NOT_SET") == oo::ScanClass::kNotSet);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_NO_DRAW") == oo::ScanClass::kNoDraw);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_NEUTRAL") == oo::ScanClass::kNeutral);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_STATION") == oo::ScanClass::kStation);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_TARGET") == oo::ScanClass::kTarget);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_CARGO") == oo::ScanClass::kCargo);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_MISSILE") == oo::ScanClass::kMissile);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_ROCK") == oo::ScanClass::kRock);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_MINE") == oo::ScanClass::kMine);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_THARGOID") == oo::ScanClass::kThargoid);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_BUOY") == oo::ScanClass::kBuoy);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_VISUAL_EFFECT") == oo::ScanClass::kVisualEffect);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_WORMHOLE") == oo::ScanClass::kWormhole);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_PLAYER") == oo::ScanClass::kPlayer);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_POLICE") == oo::ScanClass::kPolice);
  EXPECT_TRUE(oo::ScanClassFromString("CLASS_MILITARY") == oo::ScanClass::kMilitary);
  EXPECT_TRUE(oo::ScanClassFromString("NOT_A_NAME") == oo::ScanClass::kNotSet);  // the fallback
  EXPECT_TRUE(oo::ScanClassFromString("") == oo::ScanClass::kNotSet);
}

TEST(EnumNameFallbacks, OOLongRangeChartModeFromString) {
  EXPECT_TRUE(oo::LongRangeChartModeFromString("OOLRC_MODE_UNKNOWN") == oo::LongRangeChartMode::kUnknown);
  EXPECT_TRUE(oo::LongRangeChartModeFromString("OOLRC_MODE_SUNCOLOR") == oo::LongRangeChartMode::kSunColor);
  EXPECT_TRUE(oo::LongRangeChartModeFromString("OOLRC_MODE_ECONOMY") == oo::LongRangeChartMode::kEconomy);
  EXPECT_TRUE(oo::LongRangeChartModeFromString("OOLRC_MODE_GOVERNMENT") == oo::LongRangeChartMode::kGovernment);
  EXPECT_TRUE(oo::LongRangeChartModeFromString("OOLRC_MODE_TECHLEVEL") == oo::LongRangeChartMode::kTechLevel);
  EXPECT_TRUE(oo::LongRangeChartModeFromString("NOT_A_NAME") == oo::LongRangeChartMode::kUnknown);  // the fallback
  EXPECT_TRUE(oo::LongRangeChartModeFromString("") == oo::LongRangeChartMode::kUnknown);
}

TEST(EnumNameFallbacks, RouteTypeToString) {
  EXPECT_TRUE(RouteTypeToString(oo::RouteType::kNone) == "OPTIMIZED_BY_NONE");
  EXPECT_TRUE(RouteTypeToString(oo::RouteType::kJumps) == "OPTIMIZED_BY_JUMPS");
  EXPECT_TRUE(RouteTypeToString(oo::RouteType::kTime) == "OPTIMIZED_BY_TIME");
}

TEST(EnumNameFallbacks, CargoTypeToString) {
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kNotCargo) == "CARGO_NOT_CARGO");
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kSlaves) == "CARGO_SLAVES");
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kAlloy) == "CARGO_ALLOY");
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kMinerals) == "CARGO_MINERALS");
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kThargoid) == "CARGO_THARGOID");
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kRandom) == "CARGO_RANDOM");
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kScriptedItem) == "CARGO_SCRIPTED_ITEM");
  EXPECT_TRUE(CargoTypeToString(oo::CargoType::kCharacter) == "CARGO_CHARACTER");
}

TEST(EnumNameFallbacks, StringToCargoType) {
  EXPECT_TRUE(StringToCargoType("CARGO_NOT_CARGO") == oo::CargoType::kNotCargo);
  EXPECT_TRUE(StringToCargoType("CARGO_SLAVES") == oo::CargoType::kSlaves);
  EXPECT_TRUE(StringToCargoType("CARGO_ALLOY") == oo::CargoType::kAlloy);
  EXPECT_TRUE(StringToCargoType("CARGO_MINERALS") == oo::CargoType::kMinerals);
  EXPECT_TRUE(StringToCargoType("CARGO_THARGOID") == oo::CargoType::kThargoid);
  EXPECT_TRUE(StringToCargoType("CARGO_RANDOM") == oo::CargoType::kRandom);
  EXPECT_TRUE(StringToCargoType("CARGO_SCRIPTED_ITEM") == oo::CargoType::kScriptedItem);
  EXPECT_TRUE(StringToCargoType("CARGO_CHARACTER") == oo::CargoType::kCharacter);
  EXPECT_TRUE(StringToCargoType("CARGO_CARRIED") == oo::CargoType::kRandom);
  EXPECT_TRUE(StringToCargoType("NOT_A_NAME") == oo::CargoType::kNotCargo);  // the fallback
  EXPECT_TRUE(StringToCargoType("") == oo::CargoType::kNotCargo);
}

TEST(EnumNameFallbacks, EnergyUnitTypeToString) {
  EXPECT_TRUE(EnergyUnitTypeToString(oo::EnergyUnitType::kNone) == "ENERGY_UNIT_NONE");
  EXPECT_TRUE(EnergyUnitTypeToString(oo::EnergyUnitType::kNormal) == "ENERGY_UNIT_NORMAL");
  EXPECT_TRUE(EnergyUnitTypeToString(oo::EnergyUnitType::kNaval) == "ENERGY_UNIT_NAVAL");
  EXPECT_TRUE(EnergyUnitTypeToString(oo::EnergyUnitType::kNormalDamaged) == "ENERGY_UNIT_NORMAL_DAMAGED");
  EXPECT_TRUE(EnergyUnitTypeToString(oo::EnergyUnitType::kNavalDamaged) == "ENERGY_UNIT_NAVAL_DAMAGED");
  EXPECT_TRUE(EnergyUnitTypeToString(oo::EnergyUnitType::kOldNormal) == "Unsupported energy unit");
  EXPECT_TRUE(EnergyUnitTypeToString(oo::EnergyUnitType::kOldNaval) == "Unsupported energy unit");
}

TEST(EnumNameFallbacks, StringToEnergyUnitType) {
  EXPECT_TRUE(StringToEnergyUnitType("ENERGY_UNIT_NONE") == oo::EnergyUnitType::kNone);
  EXPECT_TRUE(StringToEnergyUnitType("ENERGY_UNIT_NORMAL") == oo::EnergyUnitType::kNormal);
  EXPECT_TRUE(StringToEnergyUnitType("ENERGY_UNIT_NAVAL") == oo::EnergyUnitType::kNaval);
  EXPECT_TRUE(StringToEnergyUnitType("ENERGY_UNIT_NORMAL_DAMAGED") == oo::EnergyUnitType::kNormalDamaged);
  EXPECT_TRUE(StringToEnergyUnitType("ENERGY_UNIT_NAVAL_DAMAGED") == oo::EnergyUnitType::kNavalDamaged);
  EXPECT_TRUE(StringToEnergyUnitType("NOT_A_NAME") == oo::EnergyUnitType::kNone);  // the fallback
  EXPECT_TRUE(StringToEnergyUnitType("") == oo::EnergyUnitType::kNone);
}

TEST(EnumNameFallbacks, OOStringFromShaderSetting) {
  EXPECT_TRUE(oo::StringFromShaderSetting(oo::ShaderSetting::kOff) == "SHADERS_OFF");
  EXPECT_TRUE(oo::StringFromShaderSetting(oo::ShaderSetting::kSimple) == "SHADERS_SIMPLE");
  EXPECT_TRUE(oo::StringFromShaderSetting(oo::ShaderSetting::kFull) == "SHADERS_FULL");
  EXPECT_TRUE(oo::StringFromShaderSetting(oo::ShaderSetting::kNotSupported) == "SHADERS_NOT_SUPPORTED");
}

TEST(EnumNameFallbacks, OOShaderSettingFromString) {
  EXPECT_TRUE(oo::ShaderSettingFromString("SHADERS_OFF") == oo::ShaderSetting::kOff);
  EXPECT_TRUE(oo::ShaderSettingFromString("SHADERS_SIMPLE") == oo::ShaderSetting::kSimple);
  EXPECT_TRUE(oo::ShaderSettingFromString("SHADERS_FULL") == oo::ShaderSetting::kFull);
  EXPECT_TRUE(oo::ShaderSettingFromString("SHADERS_NOT_SUPPORTED") == oo::ShaderSetting::kNotSupported);
  EXPECT_TRUE(oo::ShaderSettingFromString("NOT_A_NAME") == oo::ShaderSetting::kNotSupported);  // the fallback
  EXPECT_TRUE(oo::ShaderSettingFromString("") == oo::ShaderSetting::kNotSupported);
}

TEST(EnumNameFallbacks, StringToRouteType) {
  EXPECT_TRUE(StringToRouteType("OPTIMIZED_BY_NONE") == oo::RouteType::kNone);
  EXPECT_TRUE(StringToRouteType("OPTIMIZED_BY_JUMPS") == oo::RouteType::kJumps);
  EXPECT_TRUE(StringToRouteType("OPTIMIZED_BY_TIME") == oo::RouteType::kTime);
  EXPECT_TRUE(StringToRouteType("NOT_A_NAME") == oo::RouteType::kJumps);  // the fallback
  EXPECT_TRUE(StringToRouteType("") == oo::RouteType::kJumps);
}

TEST(EnumNameFallbacks, DockingClearanceStatusToString) {
  EXPECT_TRUE(DockingClearanceStatusToString(oo::DockingClearanceStatus::kNone) == "DOCKING_CLEARANCE_STATUS_NONE");
  EXPECT_TRUE(DockingClearanceStatusToString(oo::DockingClearanceStatus::kRequested) == "DOCKING_CLEARANCE_STATUS_REQUESTED");
  EXPECT_TRUE(DockingClearanceStatusToString(oo::DockingClearanceStatus::kNotRequired) == "DOCKING_CLEARANCE_STATUS_NOT_REQUIRED");
  EXPECT_TRUE(DockingClearanceStatusToString(oo::DockingClearanceStatus::kGranted) == "DOCKING_CLEARANCE_STATUS_GRANTED");
  EXPECT_TRUE(DockingClearanceStatusToString(oo::DockingClearanceStatus::kTimingOut) == "DOCKING_CLEARANCE_STATUS_TIMING_OUT");
}

TEST(EnumNameFallbacks, OOStringFromGraphicsDetail) {
  EXPECT_TRUE(oo::StringFromGraphicsDetail(oo::GraphicsDetail::kMinimum) == "DETAIL_LEVEL_MINIMUM");
  EXPECT_TRUE(oo::StringFromGraphicsDetail(oo::GraphicsDetail::kNormal) == "DETAIL_LEVEL_NORMAL");
  EXPECT_TRUE(oo::StringFromGraphicsDetail(oo::GraphicsDetail::kShaders) == "DETAIL_LEVEL_SHADERS");
  EXPECT_TRUE(oo::StringFromGraphicsDetail(oo::GraphicsDetail::kExtras) == "DETAIL_LEVEL_EXTRAS");
}

TEST(EnumNameFallbacks, OOGraphicsDetailFromString) {
  EXPECT_TRUE(oo::GraphicsDetailFromString("DETAIL_LEVEL_MINIMUM") == oo::GraphicsDetail::kMinimum);
  EXPECT_TRUE(oo::GraphicsDetailFromString("DETAIL_LEVEL_NORMAL") == oo::GraphicsDetail::kNormal);
  EXPECT_TRUE(oo::GraphicsDetailFromString("DETAIL_LEVEL_SHADERS") == oo::GraphicsDetail::kShaders);
  EXPECT_TRUE(oo::GraphicsDetailFromString("DETAIL_LEVEL_EXTRAS") == oo::GraphicsDetail::kExtras);
  EXPECT_TRUE(oo::GraphicsDetailFromString("NOT_A_NAME") == oo::GraphicsDetail::kMinimum);  // the fallback
  EXPECT_TRUE(oo::GraphicsDetailFromString("") == oo::GraphicsDetail::kMinimum);
}

TEST(EnumNameFallbacks, OOStringFromHDRToneMapper) {
  EXPECT_TRUE(oo::StringFromHDRToneMapper(oo::HDRToneMapper::kNone) == "OOHDR_TONEMAPPER_NONE");
  EXPECT_TRUE(oo::StringFromHDRToneMapper(oo::HDRToneMapper::kAcesApprox) == "OOHDR_TONEMAPPER_ACES_APPROX");
  EXPECT_TRUE(oo::StringFromHDRToneMapper(oo::HDRToneMapper::kDice) == "OOHDR_TONEMAPPER_DICE");
  EXPECT_TRUE(oo::StringFromHDRToneMapper(oo::HDRToneMapper::kUchimura) == "OOHDR_TONEMAPPER_UCHIMURA");
  EXPECT_TRUE(oo::StringFromHDRToneMapper(oo::HDRToneMapper::kReinhard) == "OOHDR_TONEMAPPER_REINHARD");
}

TEST(EnumNameFallbacks, OOHDRToneMapperFromString) {
  EXPECT_TRUE(oo::HDRToneMapperFromString("OOHDR_TONEMAPPER_NONE") == oo::HDRToneMapper::kNone);
  EXPECT_TRUE(oo::HDRToneMapperFromString("OOHDR_TONEMAPPER_ACES_APPROX") == oo::HDRToneMapper::kAcesApprox);
  EXPECT_TRUE(oo::HDRToneMapperFromString("OOHDR_TONEMAPPER_DICE") == oo::HDRToneMapper::kDice);
  EXPECT_TRUE(oo::HDRToneMapperFromString("OOHDR_TONEMAPPER_UCHIMURA") == oo::HDRToneMapper::kUchimura);
  EXPECT_TRUE(oo::HDRToneMapperFromString("OOHDR_TONEMAPPER_REINHARD") == oo::HDRToneMapper::kReinhard);
  EXPECT_TRUE(oo::HDRToneMapperFromString("NOT_A_NAME") == oo::HDRToneMapper::kAcesApprox);  // the fallback
  EXPECT_TRUE(oo::HDRToneMapperFromString("") == oo::HDRToneMapper::kAcesApprox);
}

TEST(EnumNameFallbacks, OOStringFromSDRToneMapper) {
  EXPECT_TRUE(oo::StringFromSDRToneMapper(oo::SDRToneMapper::kNone) == "OOSDR_TONEMAPPER_NONE");
  EXPECT_TRUE(oo::StringFromSDRToneMapper(oo::SDRToneMapper::kAces) == "OOSDR_TONEMAPPER_ACES");
  EXPECT_TRUE(oo::StringFromSDRToneMapper(oo::SDRToneMapper::kAgX) == "OOSDR_TONEMAPPER_AgX");
  EXPECT_TRUE(oo::StringFromSDRToneMapper(oo::SDRToneMapper::kHejlDawson) == "OOSDR_TONEMAPPER_HEJLDAWSON");
  EXPECT_TRUE(oo::StringFromSDRToneMapper(oo::SDRToneMapper::kUC2) == "OOSDR_TONEMAPPER_UC2");
  EXPECT_TRUE(oo::StringFromSDRToneMapper(oo::SDRToneMapper::kUchimura) == "OOSDR_TONEMAPPER_UCHIMURA");
  EXPECT_TRUE(oo::StringFromSDRToneMapper(oo::SDRToneMapper::kReinhard) == "OOSDR_TONEMAPPER_REINHARD");
}

TEST(EnumNameFallbacks, OOSDRToneMapperFromString) {
  EXPECT_TRUE(oo::SDRToneMapperFromString("OOSDR_TONEMAPPER_NONE") == oo::SDRToneMapper::kNone);
  EXPECT_TRUE(oo::SDRToneMapperFromString("OOSDR_TONEMAPPER_ACES") == oo::SDRToneMapper::kAces);
  EXPECT_TRUE(oo::SDRToneMapperFromString("OOSDR_TONEMAPPER_AgX") == oo::SDRToneMapper::kAgX);
  EXPECT_TRUE(oo::SDRToneMapperFromString("OOSDR_TONEMAPPER_HEJLDAWSON") == oo::SDRToneMapper::kHejlDawson);
  EXPECT_TRUE(oo::SDRToneMapperFromString("OOSDR_TONEMAPPER_UC2") == oo::SDRToneMapper::kUC2);
  EXPECT_TRUE(oo::SDRToneMapperFromString("OOSDR_TONEMAPPER_UCHIMURA") == oo::SDRToneMapper::kUchimura);
  EXPECT_TRUE(oo::SDRToneMapperFromString("OOSDR_TONEMAPPER_REINHARD") == oo::SDRToneMapper::kReinhard);
  EXPECT_TRUE(oo::SDRToneMapperFromString("NOT_A_NAME") == oo::SDRToneMapper::kAces);  // the fallback
  EXPECT_TRUE(oo::SDRToneMapperFromString("") == oo::SDRToneMapper::kAces);
}

namespace {

// A bare QuickJS context with the constant tables' atoms. The atoms are interned once per process
// and belong to the runtime, so one runtime serves every case and is kept to the end.
struct JSStrings {
  JSRuntime* runtime = JS_NewRuntime();
  JSContext* context = JS_NewContext(runtime);
  JSStrings() { OOConstToJSStringInit(context); }
  std::string text(JSValue value) const {
    const char* chars = JS_ToCString(context, value);
    std::string result = chars != nullptr ? chars : "(not a string)";
    JS_FreeCString(context, chars);
    JS_FreeValue(context, value);
    return result;
  }
  JSValue string(const char* chars) const { return JS_NewString(context, chars); }
};

JSStrings& Strings() {
  static JSStrings* strings = new JSStrings;
  return *strings;
}

}  // namespace

TEST(EnumNameFallbacks, jsEntityStatus) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kStartGame)) == "STATUS_START_GAME");
  { JSValue v = js.string("STATUS_START_GAME"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kStartGame); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kRestartGame)) == "STATUS_RESTART_GAME");
  { JSValue v = js.string("STATUS_RESTART_GAME"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kRestartGame); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kDead)) == "STATUS_DEAD");
  { JSValue v = js.string("STATUS_DEAD"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kDead); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kInactive)) == "STATUS_INACTIVE");
  { JSValue v = js.string("STATUS_INACTIVE"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kInactive); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kTest)) == "STATUS_TEST");
  { JSValue v = js.string("STATUS_TEST"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kTest); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kCockpitDisplay)) == "STATUS_COCKPIT_DISPLAY");
  { JSValue v = js.string("STATUS_COCKPIT_DISPLAY"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kCockpitDisplay); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kActive)) == "STATUS_ACTIVE");
  { JSValue v = js.string("STATUS_ACTIVE"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kActive); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kEffect)) == "STATUS_EFFECT");
  { JSValue v = js.string("STATUS_EFFECT"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kEffect); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kInFlight)) == "STATUS_IN_FLIGHT");
  { JSValue v = js.string("STATUS_IN_FLIGHT"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kInFlight); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kDocked)) == "STATUS_DOCKED");
  { JSValue v = js.string("STATUS_DOCKED"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kDocked); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kAutopilotEngaged)) == "STATUS_AUTOPILOT_ENGAGED");
  { JSValue v = js.string("STATUS_AUTOPILOT_ENGAGED"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kAutopilotEngaged); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kDocking)) == "STATUS_DOCKING");
  { JSValue v = js.string("STATUS_DOCKING"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kDocking); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kLaunching)) == "STATUS_LAUNCHING");
  { JSValue v = js.string("STATUS_LAUNCHING"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kLaunching); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kWitchspaceCountdown)) == "STATUS_WITCHSPACE_COUNTDOWN");
  { JSValue v = js.string("STATUS_WITCHSPACE_COUNTDOWN"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kWitchspaceCountdown); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kEnteringWitchspace)) == "STATUS_ENTERING_WITCHSPACE");
  { JSValue v = js.string("STATUS_ENTERING_WITCHSPACE"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kEnteringWitchspace); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kExitingWitchspace)) == "STATUS_EXITING_WITCHSPACE");
  { JSValue v = js.string("STATUS_EXITING_WITCHSPACE"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kExitingWitchspace); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kEscapeSequence)) == "STATUS_ESCAPE_SEQUENCE");
  { JSValue v = js.string("STATUS_ESCAPE_SEQUENCE"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kEscapeSequence); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kInHold)) == "STATUS_IN_HOLD");
  { JSValue v = js.string("STATUS_IN_HOLD"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kInHold); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kBeingScooped)) == "STATUS_BEING_SCOOPED");
  { JSValue v = js.string("STATUS_BEING_SCOOPED"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kBeingScooped); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kHandlingError)) == "STATUS_HANDLING_ERROR");
  { JSValue v = js.string("STATUS_HANDLING_ERROR"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kHandlingError); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, v) == (oo::EntityStatus)(oo::EntityStatus::kInactive)); JS_FreeValue(js.context, v); }  // the fallback
  EXPECT_TRUE(OOEntityStatusFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::EntityStatus)(oo::EntityStatus::kInactive));  // not a string
}

TEST(EnumNameFallbacks, jsScanClass) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kNotSet)) == "CLASS_NOT_SET");
  { JSValue v = js.string("CLASS_NOT_SET"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kNotSet); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kNoDraw)) == "CLASS_NO_DRAW");
  { JSValue v = js.string("CLASS_NO_DRAW"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kNoDraw); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kNeutral)) == "CLASS_NEUTRAL");
  { JSValue v = js.string("CLASS_NEUTRAL"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kNeutral); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kStation)) == "CLASS_STATION");
  { JSValue v = js.string("CLASS_STATION"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kStation); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kTarget)) == "CLASS_TARGET");
  { JSValue v = js.string("CLASS_TARGET"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kTarget); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kCargo)) == "CLASS_CARGO");
  { JSValue v = js.string("CLASS_CARGO"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kCargo); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kMissile)) == "CLASS_MISSILE");
  { JSValue v = js.string("CLASS_MISSILE"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kMissile); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kRock)) == "CLASS_ROCK");
  { JSValue v = js.string("CLASS_ROCK"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kRock); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kMine)) == "CLASS_MINE");
  { JSValue v = js.string("CLASS_MINE"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kMine); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kThargoid)) == "CLASS_THARGOID");
  { JSValue v = js.string("CLASS_THARGOID"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kThargoid); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kBuoy)) == "CLASS_BUOY");
  { JSValue v = js.string("CLASS_BUOY"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kBuoy); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kVisualEffect)) == "CLASS_VISUAL_EFFECT");
  { JSValue v = js.string("CLASS_VISUAL_EFFECT"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kVisualEffect); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kWormhole)) == "CLASS_WORMHOLE");
  { JSValue v = js.string("CLASS_WORMHOLE"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kWormhole); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kPlayer)) == "CLASS_PLAYER");
  { JSValue v = js.string("CLASS_PLAYER"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kPlayer); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kPolice)) == "CLASS_POLICE");
  { JSValue v = js.string("CLASS_POLICE"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kPolice); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kMilitary)) == "CLASS_MILITARY");
  { JSValue v = js.string("CLASS_MILITARY"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kMilitary); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); EXPECT_TRUE(OOScanClassFromJSValue(js.context, v) == (oo::ScanClass)(oo::ScanClass::kNotSet)); JS_FreeValue(js.context, v); }  // the fallback
  EXPECT_TRUE(OOScanClassFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::ScanClass)(oo::ScanClass::kNotSet));  // not a string
}

TEST(EnumNameFallbacks, jsCompassMode) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kInactive)) == "COMPASS_MODE_INACTIVE");
  { JSValue v = js.string("COMPASS_MODE_INACTIVE"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kInactive); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kBasic)) == "COMPASS_MODE_BASIC");
  { JSValue v = js.string("COMPASS_MODE_BASIC"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kBasic); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kPlanet)) == "COMPASS_MODE_PLANET");
  { JSValue v = js.string("COMPASS_MODE_PLANET"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kPlanet); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kStation)) == "COMPASS_MODE_STATION");
  { JSValue v = js.string("COMPASS_MODE_STATION"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kStation); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kSun)) == "COMPASS_MODE_SUN");
  { JSValue v = js.string("COMPASS_MODE_SUN"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kSun); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kTarget)) == "COMPASS_MODE_TARGET");
  { JSValue v = js.string("COMPASS_MODE_TARGET"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kTarget); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kBeacons)) == "COMPASS_MODE_BEACONS");
  { JSValue v = js.string("COMPASS_MODE_BEACONS"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kBeacons); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); EXPECT_TRUE(OOCompassModeFromJSValue(js.context, v) == (oo::CompassMode)(oo::CompassMode::kBasic)); JS_FreeValue(js.context, v); }  // the fallback
  EXPECT_TRUE(OOCompassModeFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::CompassMode)(oo::CompassMode::kBasic));  // not a string
}

TEST(EnumNameFallbacks, jsGUIScreenID) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMain)) == "GUI_SCREEN_MAIN");
  { JSValue v = js.string("GUI_SCREEN_MAIN"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMain); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kIntro1)) == "GUI_SCREEN_INTRO1");
  { JSValue v = js.string("GUI_SCREEN_INTRO1"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kIntro1); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kShipLibrary)) == "GUI_SCREEN_SHIPLIBRARY");
  { JSValue v = js.string("GUI_SCREEN_SHIPLIBRARY"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kShipLibrary); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboard)) == "GUI_SCREEN_KEYBOARD");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboard); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kNewGame)) == "GUI_SCREEN_NEWGAME");
  { JSValue v = js.string("GUI_SCREEN_NEWGAME"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kNewGame); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kOXZManager)) == "GUI_SCREEN_OXZMANAGER");
  { JSValue v = js.string("GUI_SCREEN_OXZMANAGER"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kOXZManager); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kStatus)) == "GUI_SCREEN_STATUS");
  { JSValue v = js.string("GUI_SCREEN_STATUS"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kStatus); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kManifest)) == "GUI_SCREEN_MANIFEST");
  { JSValue v = js.string("GUI_SCREEN_MANIFEST"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kManifest); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kEquipShip)) == "GUI_SCREEN_EQUIP_SHIP");
  { JSValue v = js.string("GUI_SCREEN_EQUIP_SHIP"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kEquipShip); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kShipyard)) == "GUI_SCREEN_SHIPYARD");
  { JSValue v = js.string("GUI_SCREEN_SHIPYARD"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kShipyard); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kLongRangeChart)) == "GUI_SCREEN_LONG_RANGE_CHART");
  { JSValue v = js.string("GUI_SCREEN_LONG_RANGE_CHART"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kLongRangeChart); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kShortRangeChart)) == "GUI_SCREEN_SHORT_RANGE_CHART");
  { JSValue v = js.string("GUI_SCREEN_SHORT_RANGE_CHART"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kShortRangeChart); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kSystemData)) == "GUI_SCREEN_SYSTEM_DATA");
  { JSValue v = js.string("GUI_SCREEN_SYSTEM_DATA"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kSystemData); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMarket)) == "GUI_SCREEN_MARKET");
  { JSValue v = js.string("GUI_SCREEN_MARKET"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMarket); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMarketInfo)) == "GUI_SCREEN_MARKETINFO");
  { JSValue v = js.string("GUI_SCREEN_MARKETINFO"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMarketInfo); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kOptions)) == "GUI_SCREEN_OPTIONS");
  { JSValue v = js.string("GUI_SCREEN_OPTIONS"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kOptions); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kGameOptions)) == "GUI_SCREEN_GAMEOPTIONS");
  { JSValue v = js.string("GUI_SCREEN_GAMEOPTIONS"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kGameOptions); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kLoad)) == "GUI_SCREEN_LOAD");
  { JSValue v = js.string("GUI_SCREEN_LOAD"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kLoad); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kSave)) == "GUI_SCREEN_SAVE");
  { JSValue v = js.string("GUI_SCREEN_SAVE"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kSave); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kSaveOverwrite)) == "GUI_SCREEN_SAVE_OVERWRITE");
  { JSValue v = js.string("GUI_SCREEN_SAVE_OVERWRITE"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kSaveOverwrite); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kStickMapper)) == "GUI_SCREEN_STICKMAPPER");
  { JSValue v = js.string("GUI_SCREEN_STICKMAPPER"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kStickMapper); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kStickProfile)) == "GUI_SCREEN_STICKPROFILE");
  { JSValue v = js.string("GUI_SCREEN_STICKPROFILE"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kStickProfile); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMission)) == "GUI_SCREEN_MISSION");
  { JSValue v = js.string("GUI_SCREEN_MISSION"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMission); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kReport)) == "GUI_SCREEN_REPORT");
  { JSValue v = js.string("GUI_SCREEN_REPORT"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kReport); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kInterfaces)) == "GUI_SCREEN_INTERFACES");
  { JSValue v = js.string("GUI_SCREEN_INTERFACES"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kInterfaces); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardConfirmClear)) == "GUI_SCREEN_KEYBOARD_CONFIRMCLEAR");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_CONFIRMCLEAR"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardConfirmClear); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardConfig)) == "GUI_SCREEN_KEYBOARD_CONFIG");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_CONFIG"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardConfig); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardEntry)) == "GUI_SCREEN_KEYBOARD_ENTRY");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_ENTRY"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardEntry); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardLayout)) == "GUI_SCREEN_KEYBOARD_LAYOUT");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_LAYOUT"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardLayout); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, v) == (oo::GUIScreenID)(oo::GUIScreenID::kMain)); JS_FreeValue(js.context, v); }  // the fallback
  EXPECT_TRUE(OOGUIScreenIDFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::GUIScreenID)(oo::GUIScreenID::kMain));  // not a string
}

TEST(EnumNameFallbacks, jsGalacticHyperspaceBehaviour) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kStandard)) == "BEHAVIOUR_STANDARD");
  { JSValue v = js.string("BEHAVIOUR_STANDARD"); EXPECT_TRUE(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kStandard); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kAllSystemsReachable)) == "BEHAVIOUR_ALL_SYSTEMS_REACHABLE");
  { JSValue v = js.string("BEHAVIOUR_ALL_SYSTEMS_REACHABLE"); EXPECT_TRUE(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kAllSystemsReachable); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kFixedCoordinates)) == "BEHAVIOUR_FIXED_COORDINATES");
  { JSValue v = js.string("BEHAVIOUR_FIXED_COORDINATES"); EXPECT_TRUE(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kFixedCoordinates); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kUnknown)) == "BEHAVIOUR_UNKNOWN");
  { JSValue v = js.string("BEHAVIOUR_UNKNOWN"); EXPECT_TRUE(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kUnknown); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); EXPECT_TRUE(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == (oo::GalacticHyperspaceBehaviour)(oo::GalacticHyperspaceBehaviour::kUnknown)); JS_FreeValue(js.context, v); }  // the fallback
  EXPECT_TRUE(OOGalacticHyperspaceBehaviourFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::GalacticHyperspaceBehaviour)(oo::GalacticHyperspaceBehaviour::kUnknown));  // not a string
}

TEST(EnumNameFallbacks, jsViewID) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kForward)) == "VIEW_FORWARD");
  { JSValue v = js.string("VIEW_FORWARD"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kForward); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kAft)) == "VIEW_AFT");
  { JSValue v = js.string("VIEW_AFT"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kAft); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kPort)) == "VIEW_PORT");
  { JSValue v = js.string("VIEW_PORT"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kPort); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kStarboard)) == "VIEW_STARBOARD");
  { JSValue v = js.string("VIEW_STARBOARD"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kStarboard); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kCustom)) == "VIEW_CUSTOM");
  { JSValue v = js.string("VIEW_CUSTOM"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kCustom); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kNone)) == "VIEW_NONE");
  { JSValue v = js.string("VIEW_NONE"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kNone); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kGUIDisplay)) == "VIEW_GUI_DISPLAY");
  { JSValue v = js.string("VIEW_GUI_DISPLAY"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kGUIDisplay); JS_FreeValue(js.context, v); }
  EXPECT_TRUE(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kBreakPattern)) == "VIEW_BREAK_PATTERN");
  { JSValue v = js.string("VIEW_BREAK_PATTERN"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kBreakPattern); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); EXPECT_TRUE(OOViewIDFromJSValue(js.context, v) == (oo::ViewID)(oo::ViewID::kNone)); JS_FreeValue(js.context, v); }  // the fallback
  EXPECT_TRUE(OOViewIDFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::ViewID)(oo::ViewID::kNone));  // not a string
}

TEST(EnumNameFallbacks, jsShipDamageType) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kScrape)) == "scrape damage");
  EXPECT_TRUE(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kHeat)) == "heat damage");
  EXPECT_TRUE(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kEnergy)) == "energy damage");
  EXPECT_TRUE(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kCascadeWeapon)) == "cascade weapon");
  EXPECT_TRUE(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kHitAPlanet)) == "hit a planet");
  EXPECT_TRUE(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kHitASun)) == "hit a sun");
  EXPECT_TRUE(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kRemoved)) == "removed");
}

TEST(EnumNameFallbacks, jsLegalStatusReason) {
  JSStrings& js = Strings();
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kUnknown)) == "unknown");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kSetup)) == "setup actions");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kByScript)) == "scripted");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAttackedPolice)) == "attacked police");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAttackedMainStation)) == "attacked main station");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAttackedInnocent)) == "attacked innocent");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kSeenByPolice)) == "seen by police");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kDistressCall)) == "distress call");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kIllegalExports)) == "illegal exports");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kIllegalImports)) == "illegal imports");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAssistingOffender)) == "assisting offenders");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kNewGalaxy)) == "new galaxy");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kNewSystem)) == "new system");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kPaidFine)) == "paid fine");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kEscapePod)) == "escape pod");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAssistingPolice)) == "assisting police");
  EXPECT_TRUE(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kPoliceAreClean)) == "police are clean");
}
