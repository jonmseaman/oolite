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

#include "OOConstToJSString.h"
#include "OOConstToString.h"
#include "OOOpenGL.h"
#include "OOTypes.h"
#include "PlayerEntity.h"
#include "ShipEntity.h"
#include "oo_test.hpp"

// main.mm defines the debug flags, and the test has its own main.
#ifndef NDEBUG
uint32_t gDebugFlags = 0;
#endif

OO_TEST(OOStringFromEntityStatus) {
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kStartGame) == "STATUS_START_GAME");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kRestartGame) == "STATUS_RESTART_GAME");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kDead) == "STATUS_DEAD");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kInactive) == "STATUS_INACTIVE");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kTest) == "STATUS_TEST");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kCockpitDisplay) == "STATUS_COCKPIT_DISPLAY");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kActive) == "STATUS_ACTIVE");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kEffect) == "STATUS_EFFECT");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kInFlight) == "STATUS_IN_FLIGHT");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kDocked) == "STATUS_DOCKED");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kAutopilotEngaged) == "STATUS_AUTOPILOT_ENGAGED");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kDocking) == "STATUS_DOCKING");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kLaunching) == "STATUS_LAUNCHING");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kWitchspaceCountdown) == "STATUS_WITCHSPACE_COUNTDOWN");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kEnteringWitchspace) == "STATUS_ENTERING_WITCHSPACE");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kExitingWitchspace) == "STATUS_EXITING_WITCHSPACE");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kEscapeSequence) == "STATUS_ESCAPE_SEQUENCE");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kInHold) == "STATUS_IN_HOLD");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kBeingScooped) == "STATUS_BEING_SCOOPED");
  OO_CHECK(OOStringFromEntityStatus(oo::EntityStatus::kHandlingError) == "STATUS_HANDLING_ERROR");
}

OO_TEST(OOStringFromBehaviour) {
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kIdle) == "BEHAVIOUR_IDLE");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kTrackTarget) == "BEHAVIOUR_TRACK_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kTumble) == "BEHAVIOUR_TUMBLE");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kStopStill) == "BEHAVIOUR_STOP_STILL");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kStationKeeping) == "BEHAVIOUR_STATION_KEEPING");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackTarget) == "BEHAVIOUR_ATTACK_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackFlyToTarget) == "BEHAVIOUR_ATTACK_FLY_TO_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackFlyFromTarget) == "BEHAVIOUR_ATTACK_FLY_FROM_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kRunningDefense) == "BEHAVIOUR_RUNNING_DEFENSE");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFleeTarget) == "BEHAVIOUR_FLEE_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackFlyToTargetSix) == "BEHAVIOUR_ATTACK_FLY_TO_TARGET_SIX");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackMiningTarget) == "BEHAVIOUR_ATTACK_MINING_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kEvasiveAction) == "BEHAVIOUR_EVASIVE_ACTION");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFleeEvasiveAction) == "BEHAVIOUR_FLEE_EVASIVE_ACTION");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackBreakOffTarget) == "BEHAVIOUR_ATTACK_BREAK_OFF_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackSlowDogfight) == "BEHAVIOUR_ATTACK_SLOW_DOGFIGHT");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackFlyToTargetTwelve) == "BEHAVIOUR_ATTACK_FLY_TO_TARGET_TWELVE");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackSniper) == "BEHAVIOUR_ATTACK_SNIPER");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackBroadside) == "BEHAVIOUR_ATTACK_BROADSIDE");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackBroadsideLeft) == "BEHAVIOUR_ATTACK_BROADSIDE_LEFT");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAttackBroadsideRight) == "BEHAVIOUR_ATTACK_BROADSIDE_RIGHT");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kCloseToBroadsideRange) == "BEHAVIOUR_CLOSE_TO_BROADSIDE_RANGE");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kCloseWithTarget) == "BEHAVIOUR_CLOSE_WITH_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kAvoidCollision) == "BEHAVIOUR_AVOID_COLLISION");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kTrackAsTurret) == "BEHAVIOUR_TRACK_AS_TURRET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFlyRangeFromDestination) == "BEHAVIOUR_FLY_RANGE_FROM_DESTINATION");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFlyToDestination) == "BEHAVIOUR_FLY_TO_DESTINATION");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFlyFromDestination) == "BEHAVIOUR_FLY_FROM_DESTINATION");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFaceDestination) == "BEHAVIOUR_FACE_DESTINATION");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFlyThruNavpoints) == "BEHAVIOUR_FLY_THRU_NAVPOINTS");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kLandOnPlanet) == "BEHAVIOUR_LAND_ON_PLANET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kCollectTarget) == "BEHAVIOUR_COLLECT_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kInterceptTarget) == "BEHAVIOUR_INTERCEPT_TARGET");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFormationFormUp) == "BEHAVIOUR_FORMATION_FORM_UP");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kFormationBreak) == "BEHAVIOUR_FORMATION_BREAK");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kEnergyBombCountdown) == "BEHAVIOUR_ENERGY_BOMB_COUNTDOWN");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kTractored) == "BEHAVIOUR_TRACTORED");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kScriptedAi) == "BEHAVIOUR_SCRIPTED_AI");
  OO_CHECK(OOStringFromBehaviour(oo::Behaviour::kScriptedAttackAi) == "BEHAVIOUR_SCRIPTED_ATTACK_AI");
}

OO_TEST(OOStringFromCompassMode) {
  OO_CHECK(OOStringFromCompassMode(oo::CompassMode::kInactive) == "COMPASS_MODE_INACTIVE");
  OO_CHECK(OOStringFromCompassMode(oo::CompassMode::kBasic) == "COMPASS_MODE_BASIC");
  OO_CHECK(OOStringFromCompassMode(oo::CompassMode::kPlanet) == "COMPASS_MODE_PLANET");
  OO_CHECK(OOStringFromCompassMode(oo::CompassMode::kStation) == "COMPASS_MODE_STATION");
  OO_CHECK(OOStringFromCompassMode(oo::CompassMode::kSun) == "COMPASS_MODE_SUN");
  OO_CHECK(OOStringFromCompassMode(oo::CompassMode::kTarget) == "COMPASS_MODE_TARGET");
  OO_CHECK(OOStringFromCompassMode(oo::CompassMode::kBeacons) == "COMPASS_MODE_BEACONS");
}

OO_TEST(OOStringFromLongRangeChartMode) {
  OO_CHECK(OOStringFromLongRangeChartMode(oo::LongRangeChartMode::kUnknown) == "OOLRC_MODE_UNKNOWN");
  OO_CHECK(OOStringFromLongRangeChartMode(oo::LongRangeChartMode::kSunColor) == "OOLRC_MODE_SUNCOLOR");
  OO_CHECK(OOStringFromLongRangeChartMode(oo::LongRangeChartMode::kEconomy) == "OOLRC_MODE_ECONOMY");
  OO_CHECK(OOStringFromLongRangeChartMode(oo::LongRangeChartMode::kGovernment) == "OOLRC_MODE_GOVERNMENT");
  OO_CHECK(OOStringFromLongRangeChartMode(oo::LongRangeChartMode::kTechLevel) == "OOLRC_MODE_TECHLEVEL");
}

OO_TEST(OOStringFromGalacticHyperspaceBehaviour) {
  OO_CHECK(OOStringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kStandard) == "BEHAVIOUR_STANDARD");
  OO_CHECK(OOStringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kAllSystemsReachable) == "BEHAVIOUR_ALL_SYSTEMS_REACHABLE");
  OO_CHECK(OOStringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kFixedCoordinates) == "BEHAVIOUR_FIXED_COORDINATES");
  OO_CHECK(OOStringFromGalacticHyperspaceBehaviour(oo::GalacticHyperspaceBehaviour::kUnknown) == "BEHAVIOUR_UNKNOWN");
}

OO_TEST(OOStringFromGUIScreenID) {
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kMain) == "GUI_SCREEN_MAIN");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kIntro1) == "GUI_SCREEN_INTRO1");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kShipLibrary) == "GUI_SCREEN_SHIPLIBRARY");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kKeyboard) == "GUI_SCREEN_KEYBOARD");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kNewGame) == "GUI_SCREEN_NEWGAME");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kOXZManager) == "GUI_SCREEN_OXZMANAGER");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kStatus) == "GUI_SCREEN_STATUS");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kManifest) == "GUI_SCREEN_MANIFEST");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kEquipShip) == "GUI_SCREEN_EQUIP_SHIP");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kShipyard) == "GUI_SCREEN_SHIPYARD");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kLongRangeChart) == "GUI_SCREEN_LONG_RANGE_CHART");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kShortRangeChart) == "GUI_SCREEN_SHORT_RANGE_CHART");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kSystemData) == "GUI_SCREEN_SYSTEM_DATA");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kMarket) == "GUI_SCREEN_MARKET");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kMarketInfo) == "GUI_SCREEN_MARKETINFO");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kOptions) == "GUI_SCREEN_OPTIONS");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kGameOptions) == "GUI_SCREEN_GAMEOPTIONS");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kLoad) == "GUI_SCREEN_LOAD");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kSave) == "GUI_SCREEN_SAVE");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kSaveOverwrite) == "GUI_SCREEN_SAVE_OVERWRITE");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kStickMapper) == "GUI_SCREEN_STICKMAPPER");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kStickProfile) == "GUI_SCREEN_STICKPROFILE");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kMission) == "GUI_SCREEN_MISSION");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kReport) == "GUI_SCREEN_REPORT");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kInterfaces) == "GUI_SCREEN_INTERFACES");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kKeyboardConfirmClear) == "GUI_SCREEN_KEYBOARD_CONFIRMCLEAR");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kKeyboardConfig) == "GUI_SCREEN_KEYBOARD_CONFIG");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kKeyboardEntry) == "GUI_SCREEN_KEYBOARD_ENTRY");
  OO_CHECK(OOStringFromGUIScreenID(oo::GUIScreenID::kKeyboardLayout) == "GUI_SCREEN_KEYBOARD_LAYOUT");
}

OO_TEST(OOStringFromScanClass) {
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kNotSet) == "CLASS_NOT_SET");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kNoDraw) == "CLASS_NO_DRAW");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kNeutral) == "CLASS_NEUTRAL");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kStation) == "CLASS_STATION");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kTarget) == "CLASS_TARGET");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kCargo) == "CLASS_CARGO");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kMissile) == "CLASS_MISSILE");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kRock) == "CLASS_ROCK");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kMine) == "CLASS_MINE");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kThargoid) == "CLASS_THARGOID");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kBuoy) == "CLASS_BUOY");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kVisualEffect) == "CLASS_VISUAL_EFFECT");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kWormhole) == "CLASS_WORMHOLE");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kPlayer) == "CLASS_PLAYER");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kPolice) == "CLASS_POLICE");
  OO_CHECK(OOStringFromScanClass(oo::ScanClass::kMilitary) == "CLASS_MILITARY");
}

OO_TEST(OOStringFromShipDamageType) {
  OO_CHECK(OOStringFromShipDamageType(oo::ShipDamageType::kScrape) == "scrape damage");
  OO_CHECK(OOStringFromShipDamageType(oo::ShipDamageType::kHeat) == "heat damage");
  OO_CHECK(OOStringFromShipDamageType(oo::ShipDamageType::kEnergy) == "energy damage");
  OO_CHECK(OOStringFromShipDamageType(oo::ShipDamageType::kCascadeWeapon) == "cascade weapon");
  OO_CHECK(OOStringFromShipDamageType(oo::ShipDamageType::kHitAPlanet) == "hit a planet");
  OO_CHECK(OOStringFromShipDamageType(oo::ShipDamageType::kHitASun) == "hit a sun");
  OO_CHECK(OOStringFromShipDamageType(oo::ShipDamageType::kRemoved) == "removed");
}

OO_TEST(OOStringFromLegalStatusReason) {
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kUnknown) == "unknown");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kSetup) == "setup actions");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kByScript) == "scripted");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kAttackedPolice) == "attacked police");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kAttackedMainStation) == "attacked main station");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kAttackedInnocent) == "attacked innocent");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kSeenByPolice) == "seen by police");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kDistressCall) == "distress call");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kIllegalExports) == "illegal exports");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kIllegalImports) == "illegal imports");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kAssistingOffender) == "assisting offenders");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kNewGalaxy) == "new galaxy");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kNewSystem) == "new system");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kPaidFine) == "paid fine");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kEscapePod) == "escape pod");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kAssistingPolice) == "assisting police");
  OO_CHECK(OOStringFromLegalStatusReason(oo::LegalStatusReason::kPoliceAreClean) == "police are clean");
}

OO_TEST(OOEntityStatusFromString) {
  OO_CHECK(OOEntityStatusFromString("STATUS_START_GAME") == oo::EntityStatus::kStartGame);
  OO_CHECK(OOEntityStatusFromString("STATUS_RESTART_GAME") == oo::EntityStatus::kRestartGame);
  OO_CHECK(OOEntityStatusFromString("STATUS_DEAD") == oo::EntityStatus::kDead);
  OO_CHECK(OOEntityStatusFromString("STATUS_INACTIVE") == oo::EntityStatus::kInactive);
  OO_CHECK(OOEntityStatusFromString("STATUS_TEST") == oo::EntityStatus::kTest);
  OO_CHECK(OOEntityStatusFromString("STATUS_COCKPIT_DISPLAY") == oo::EntityStatus::kCockpitDisplay);
  OO_CHECK(OOEntityStatusFromString("STATUS_ACTIVE") == oo::EntityStatus::kActive);
  OO_CHECK(OOEntityStatusFromString("STATUS_EFFECT") == oo::EntityStatus::kEffect);
  OO_CHECK(OOEntityStatusFromString("STATUS_IN_FLIGHT") == oo::EntityStatus::kInFlight);
  OO_CHECK(OOEntityStatusFromString("STATUS_DOCKED") == oo::EntityStatus::kDocked);
  OO_CHECK(OOEntityStatusFromString("STATUS_AUTOPILOT_ENGAGED") == oo::EntityStatus::kAutopilotEngaged);
  OO_CHECK(OOEntityStatusFromString("STATUS_DOCKING") == oo::EntityStatus::kDocking);
  OO_CHECK(OOEntityStatusFromString("STATUS_LAUNCHING") == oo::EntityStatus::kLaunching);
  OO_CHECK(OOEntityStatusFromString("STATUS_WITCHSPACE_COUNTDOWN") == oo::EntityStatus::kWitchspaceCountdown);
  OO_CHECK(OOEntityStatusFromString("STATUS_ENTERING_WITCHSPACE") == oo::EntityStatus::kEnteringWitchspace);
  OO_CHECK(OOEntityStatusFromString("STATUS_EXITING_WITCHSPACE") == oo::EntityStatus::kExitingWitchspace);
  OO_CHECK(OOEntityStatusFromString("STATUS_ESCAPE_SEQUENCE") == oo::EntityStatus::kEscapeSequence);
  OO_CHECK(OOEntityStatusFromString("STATUS_IN_HOLD") == oo::EntityStatus::kInHold);
  OO_CHECK(OOEntityStatusFromString("STATUS_BEING_SCOOPED") == oo::EntityStatus::kBeingScooped);
  OO_CHECK(OOEntityStatusFromString("STATUS_HANDLING_ERROR") == oo::EntityStatus::kHandlingError);
  OO_CHECK(OOEntityStatusFromString("NOT_A_NAME") == oo::EntityStatus::kInactive);  // the fallback
  OO_CHECK(OOEntityStatusFromString("") == oo::EntityStatus::kInactive);
}

OO_TEST(OOCompassModeFromString) {
  OO_CHECK(OOCompassModeFromString("COMPASS_MODE_INACTIVE") == oo::CompassMode::kInactive);
  OO_CHECK(OOCompassModeFromString("COMPASS_MODE_BASIC") == oo::CompassMode::kBasic);
  OO_CHECK(OOCompassModeFromString("COMPASS_MODE_PLANET") == oo::CompassMode::kPlanet);
  OO_CHECK(OOCompassModeFromString("COMPASS_MODE_STATION") == oo::CompassMode::kStation);
  OO_CHECK(OOCompassModeFromString("COMPASS_MODE_SUN") == oo::CompassMode::kSun);
  OO_CHECK(OOCompassModeFromString("COMPASS_MODE_TARGET") == oo::CompassMode::kTarget);
  OO_CHECK(OOCompassModeFromString("COMPASS_MODE_BEACONS") == oo::CompassMode::kBeacons);
  OO_CHECK(OOCompassModeFromString("NOT_A_NAME") == oo::CompassMode::kBasic);  // the fallback
  OO_CHECK(OOCompassModeFromString("") == oo::CompassMode::kBasic);
}

OO_TEST(OOGalacticHyperspaceBehaviourFromString) {
  OO_CHECK(OOGalacticHyperspaceBehaviourFromString("BEHAVIOUR_STANDARD") == oo::GalacticHyperspaceBehaviour::kStandard);
  OO_CHECK(OOGalacticHyperspaceBehaviourFromString("BEHAVIOUR_ALL_SYSTEMS_REACHABLE") == oo::GalacticHyperspaceBehaviour::kAllSystemsReachable);
  OO_CHECK(OOGalacticHyperspaceBehaviourFromString("BEHAVIOUR_FIXED_COORDINATES") == oo::GalacticHyperspaceBehaviour::kFixedCoordinates);
  OO_CHECK(OOGalacticHyperspaceBehaviourFromString("BEHAVIOUR_UNKNOWN") == oo::GalacticHyperspaceBehaviour::kUnknown);
  OO_CHECK(OOGalacticHyperspaceBehaviourFromString("NOT_A_NAME") == oo::GalacticHyperspaceBehaviour::kUnknown);  // the fallback
  OO_CHECK(OOGalacticHyperspaceBehaviourFromString("") == oo::GalacticHyperspaceBehaviour::kUnknown);
}

OO_TEST(OOGUIScreenIDFromString) {
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_MAIN") == oo::GUIScreenID::kMain);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_INTRO1") == oo::GUIScreenID::kIntro1);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_SHIPLIBRARY") == oo::GUIScreenID::kShipLibrary);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_KEYBOARD") == oo::GUIScreenID::kKeyboard);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_NEWGAME") == oo::GUIScreenID::kNewGame);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_OXZMANAGER") == oo::GUIScreenID::kOXZManager);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_STATUS") == oo::GUIScreenID::kStatus);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_MANIFEST") == oo::GUIScreenID::kManifest);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_EQUIP_SHIP") == oo::GUIScreenID::kEquipShip);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_SHIPYARD") == oo::GUIScreenID::kShipyard);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_LONG_RANGE_CHART") == oo::GUIScreenID::kLongRangeChart);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_SHORT_RANGE_CHART") == oo::GUIScreenID::kShortRangeChart);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_SYSTEM_DATA") == oo::GUIScreenID::kSystemData);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_MARKET") == oo::GUIScreenID::kMarket);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_MARKETINFO") == oo::GUIScreenID::kMarketInfo);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_OPTIONS") == oo::GUIScreenID::kOptions);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_GAMEOPTIONS") == oo::GUIScreenID::kGameOptions);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_LOAD") == oo::GUIScreenID::kLoad);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_SAVE") == oo::GUIScreenID::kSave);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_SAVE_OVERWRITE") == oo::GUIScreenID::kSaveOverwrite);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_STICKMAPPER") == oo::GUIScreenID::kStickMapper);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_STICKPROFILE") == oo::GUIScreenID::kStickProfile);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_MISSION") == oo::GUIScreenID::kMission);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_REPORT") == oo::GUIScreenID::kReport);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_INTERFACES") == oo::GUIScreenID::kInterfaces);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_KEYBOARD_CONFIRMCLEAR") == oo::GUIScreenID::kKeyboardConfirmClear);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_KEYBOARD_CONFIG") == oo::GUIScreenID::kKeyboardConfig);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_KEYBOARD_ENTRY") == oo::GUIScreenID::kKeyboardEntry);
  OO_CHECK(OOGUIScreenIDFromString("GUI_SCREEN_KEYBOARD_LAYOUT") == oo::GUIScreenID::kKeyboardLayout);
  OO_CHECK(OOGUIScreenIDFromString("NOT_A_NAME") == oo::GUIScreenID::kMain);  // the fallback
  OO_CHECK(OOGUIScreenIDFromString("") == oo::GUIScreenID::kMain);
}

OO_TEST(OOScanClassFromString) {
  OO_CHECK(OOScanClassFromString("CLASS_NOT_SET") == oo::ScanClass::kNotSet);
  OO_CHECK(OOScanClassFromString("CLASS_NO_DRAW") == oo::ScanClass::kNoDraw);
  OO_CHECK(OOScanClassFromString("CLASS_NEUTRAL") == oo::ScanClass::kNeutral);
  OO_CHECK(OOScanClassFromString("CLASS_STATION") == oo::ScanClass::kStation);
  OO_CHECK(OOScanClassFromString("CLASS_TARGET") == oo::ScanClass::kTarget);
  OO_CHECK(OOScanClassFromString("CLASS_CARGO") == oo::ScanClass::kCargo);
  OO_CHECK(OOScanClassFromString("CLASS_MISSILE") == oo::ScanClass::kMissile);
  OO_CHECK(OOScanClassFromString("CLASS_ROCK") == oo::ScanClass::kRock);
  OO_CHECK(OOScanClassFromString("CLASS_MINE") == oo::ScanClass::kMine);
  OO_CHECK(OOScanClassFromString("CLASS_THARGOID") == oo::ScanClass::kThargoid);
  OO_CHECK(OOScanClassFromString("CLASS_BUOY") == oo::ScanClass::kBuoy);
  OO_CHECK(OOScanClassFromString("CLASS_VISUAL_EFFECT") == oo::ScanClass::kVisualEffect);
  OO_CHECK(OOScanClassFromString("CLASS_WORMHOLE") == oo::ScanClass::kWormhole);
  OO_CHECK(OOScanClassFromString("CLASS_PLAYER") == oo::ScanClass::kPlayer);
  OO_CHECK(OOScanClassFromString("CLASS_POLICE") == oo::ScanClass::kPolice);
  OO_CHECK(OOScanClassFromString("CLASS_MILITARY") == oo::ScanClass::kMilitary);
  OO_CHECK(OOScanClassFromString("NOT_A_NAME") == oo::ScanClass::kNotSet);  // the fallback
  OO_CHECK(OOScanClassFromString("") == oo::ScanClass::kNotSet);
}

OO_TEST(OOLongRangeChartModeFromString) {
  OO_CHECK(OOLongRangeChartModeFromString("OOLRC_MODE_UNKNOWN") == oo::LongRangeChartMode::kUnknown);
  OO_CHECK(OOLongRangeChartModeFromString("OOLRC_MODE_SUNCOLOR") == oo::LongRangeChartMode::kSunColor);
  OO_CHECK(OOLongRangeChartModeFromString("OOLRC_MODE_ECONOMY") == oo::LongRangeChartMode::kEconomy);
  OO_CHECK(OOLongRangeChartModeFromString("OOLRC_MODE_GOVERNMENT") == oo::LongRangeChartMode::kGovernment);
  OO_CHECK(OOLongRangeChartModeFromString("OOLRC_MODE_TECHLEVEL") == oo::LongRangeChartMode::kTechLevel);
  OO_CHECK(OOLongRangeChartModeFromString("NOT_A_NAME") == oo::LongRangeChartMode::kUnknown);  // the fallback
  OO_CHECK(OOLongRangeChartModeFromString("") == oo::LongRangeChartMode::kUnknown);
}

OO_TEST(RouteTypeToString) {
  OO_CHECK(RouteTypeToString(OORouteType::kNone) == "OPTIMIZED_BY_NONE");
  OO_CHECK(RouteTypeToString(OORouteType::kJumps) == "OPTIMIZED_BY_JUMPS");
  OO_CHECK(RouteTypeToString(OORouteType::kTime) == "OPTIMIZED_BY_TIME");
}

OO_TEST(CargoTypeToString) {
  OO_CHECK(CargoTypeToString(OOCargoType::kNotCargo) == "CARGO_NOT_CARGO");
  OO_CHECK(CargoTypeToString(OOCargoType::kSlaves) == "CARGO_SLAVES");
  OO_CHECK(CargoTypeToString(OOCargoType::kAlloy) == "CARGO_ALLOY");
  OO_CHECK(CargoTypeToString(OOCargoType::kMinerals) == "CARGO_MINERALS");
  OO_CHECK(CargoTypeToString(OOCargoType::kThargoid) == "CARGO_THARGOID");
  OO_CHECK(CargoTypeToString(OOCargoType::kRandom) == "CARGO_RANDOM");
  OO_CHECK(CargoTypeToString(OOCargoType::kScriptedItem) == "CARGO_SCRIPTED_ITEM");
  OO_CHECK(CargoTypeToString(OOCargoType::kCharacter) == "CARGO_CHARACTER");
}

OO_TEST(StringToCargoType) {
  OO_CHECK(StringToCargoType("CARGO_NOT_CARGO") == OOCargoType::kNotCargo);
  OO_CHECK(StringToCargoType("CARGO_SLAVES") == OOCargoType::kSlaves);
  OO_CHECK(StringToCargoType("CARGO_ALLOY") == OOCargoType::kAlloy);
  OO_CHECK(StringToCargoType("CARGO_MINERALS") == OOCargoType::kMinerals);
  OO_CHECK(StringToCargoType("CARGO_THARGOID") == OOCargoType::kThargoid);
  OO_CHECK(StringToCargoType("CARGO_RANDOM") == OOCargoType::kRandom);
  OO_CHECK(StringToCargoType("CARGO_SCRIPTED_ITEM") == OOCargoType::kScriptedItem);
  OO_CHECK(StringToCargoType("CARGO_CHARACTER") == OOCargoType::kCharacter);
  OO_CHECK(StringToCargoType("CARGO_CARRIED") == OOCargoType::kRandom);
  OO_CHECK(StringToCargoType("NOT_A_NAME") == OOCargoType::kNotCargo);  // the fallback
  OO_CHECK(StringToCargoType("") == OOCargoType::kNotCargo);
}

OO_TEST(EnergyUnitTypeToString) {
  OO_CHECK(EnergyUnitTypeToString(OOEnergyUnitType::kNone) == "ENERGY_UNIT_NONE");
  OO_CHECK(EnergyUnitTypeToString(OOEnergyUnitType::kNormal) == "ENERGY_UNIT_NORMAL");
  OO_CHECK(EnergyUnitTypeToString(OOEnergyUnitType::kNaval) == "ENERGY_UNIT_NAVAL");
  OO_CHECK(EnergyUnitTypeToString(OOEnergyUnitType::kNormalDamaged) == "ENERGY_UNIT_NORMAL_DAMAGED");
  OO_CHECK(EnergyUnitTypeToString(OOEnergyUnitType::kNavalDamaged) == "ENERGY_UNIT_NAVAL_DAMAGED");
  OO_CHECK(EnergyUnitTypeToString(OOEnergyUnitType::kOldNormal) == "Unsupported energy unit");
  OO_CHECK(EnergyUnitTypeToString(OOEnergyUnitType::kOldNaval) == "Unsupported energy unit");
}

OO_TEST(StringToEnergyUnitType) {
  OO_CHECK(StringToEnergyUnitType("ENERGY_UNIT_NONE") == OOEnergyUnitType::kNone);
  OO_CHECK(StringToEnergyUnitType("ENERGY_UNIT_NORMAL") == OOEnergyUnitType::kNormal);
  OO_CHECK(StringToEnergyUnitType("ENERGY_UNIT_NAVAL") == OOEnergyUnitType::kNaval);
  OO_CHECK(StringToEnergyUnitType("ENERGY_UNIT_NORMAL_DAMAGED") == OOEnergyUnitType::kNormalDamaged);
  OO_CHECK(StringToEnergyUnitType("ENERGY_UNIT_NAVAL_DAMAGED") == OOEnergyUnitType::kNavalDamaged);
  OO_CHECK(StringToEnergyUnitType("NOT_A_NAME") == OOEnergyUnitType::kNone);  // the fallback
  OO_CHECK(StringToEnergyUnitType("") == OOEnergyUnitType::kNone);
}

OO_TEST(OOStringFromShaderSetting) {
  OO_CHECK(OOStringFromShaderSetting(OOShaderSetting::kOff) == "SHADERS_OFF");
  OO_CHECK(OOStringFromShaderSetting(OOShaderSetting::kSimple) == "SHADERS_SIMPLE");
  OO_CHECK(OOStringFromShaderSetting(OOShaderSetting::kFull) == "SHADERS_FULL");
  OO_CHECK(OOStringFromShaderSetting(OOShaderSetting::kNotSupported) == "SHADERS_NOT_SUPPORTED");
}

OO_TEST(OOShaderSettingFromString) {
  OO_CHECK(OOShaderSettingFromString("SHADERS_OFF") == OOShaderSetting::kOff);
  OO_CHECK(OOShaderSettingFromString("SHADERS_SIMPLE") == OOShaderSetting::kSimple);
  OO_CHECK(OOShaderSettingFromString("SHADERS_FULL") == OOShaderSetting::kFull);
  OO_CHECK(OOShaderSettingFromString("SHADERS_NOT_SUPPORTED") == OOShaderSetting::kNotSupported);
  OO_CHECK(OOShaderSettingFromString("NOT_A_NAME") == OOShaderSetting::kNotSupported);  // the fallback
  OO_CHECK(OOShaderSettingFromString("") == OOShaderSetting::kNotSupported);
}

OO_TEST(StringToRouteType) {
  OO_CHECK(StringToRouteType("OPTIMIZED_BY_NONE") == OORouteType::kNone);
  OO_CHECK(StringToRouteType("OPTIMIZED_BY_JUMPS") == OORouteType::kJumps);
  OO_CHECK(StringToRouteType("OPTIMIZED_BY_TIME") == OORouteType::kTime);
  OO_CHECK(StringToRouteType("NOT_A_NAME") == OORouteType::kJumps);  // the fallback
  OO_CHECK(StringToRouteType("") == OORouteType::kJumps);
}

OO_TEST(DockingClearanceStatusToString) {
  OO_CHECK(DockingClearanceStatusToString(OODockingClearanceStatus::kNone) == "DOCKING_CLEARANCE_STATUS_NONE");
  OO_CHECK(DockingClearanceStatusToString(OODockingClearanceStatus::kRequested) == "DOCKING_CLEARANCE_STATUS_REQUESTED");
  OO_CHECK(DockingClearanceStatusToString(OODockingClearanceStatus::kNotRequired) == "DOCKING_CLEARANCE_STATUS_NOT_REQUIRED");
  OO_CHECK(DockingClearanceStatusToString(OODockingClearanceStatus::kGranted) == "DOCKING_CLEARANCE_STATUS_GRANTED");
  OO_CHECK(DockingClearanceStatusToString(OODockingClearanceStatus::kTimingOut) == "DOCKING_CLEARANCE_STATUS_TIMING_OUT");
}

OO_TEST(OOStringFromGraphicsDetail) {
  OO_CHECK(OOStringFromGraphicsDetail(OOGraphicsDetail::kMinimum) == "DETAIL_LEVEL_MINIMUM");
  OO_CHECK(OOStringFromGraphicsDetail(OOGraphicsDetail::kNormal) == "DETAIL_LEVEL_NORMAL");
  OO_CHECK(OOStringFromGraphicsDetail(OOGraphicsDetail::kShaders) == "DETAIL_LEVEL_SHADERS");
  OO_CHECK(OOStringFromGraphicsDetail(OOGraphicsDetail::kExtras) == "DETAIL_LEVEL_EXTRAS");
}

OO_TEST(OOGraphicsDetailFromString) {
  OO_CHECK(OOGraphicsDetailFromString("DETAIL_LEVEL_MINIMUM") == OOGraphicsDetail::kMinimum);
  OO_CHECK(OOGraphicsDetailFromString("DETAIL_LEVEL_NORMAL") == OOGraphicsDetail::kNormal);
  OO_CHECK(OOGraphicsDetailFromString("DETAIL_LEVEL_SHADERS") == OOGraphicsDetail::kShaders);
  OO_CHECK(OOGraphicsDetailFromString("DETAIL_LEVEL_EXTRAS") == OOGraphicsDetail::kExtras);
  OO_CHECK(OOGraphicsDetailFromString("NOT_A_NAME") == OOGraphicsDetail::kMinimum);  // the fallback
  OO_CHECK(OOGraphicsDetailFromString("") == OOGraphicsDetail::kMinimum);
}

OO_TEST(OOStringFromHDRToneMapper) {
  OO_CHECK(OOStringFromHDRToneMapper(OOHDRToneMapper::kNone) == "OOHDR_TONEMAPPER_NONE");
  OO_CHECK(OOStringFromHDRToneMapper(OOHDRToneMapper::kAcesApprox) == "OOHDR_TONEMAPPER_ACES_APPROX");
  OO_CHECK(OOStringFromHDRToneMapper(OOHDRToneMapper::kDice) == "OOHDR_TONEMAPPER_DICE");
  OO_CHECK(OOStringFromHDRToneMapper(OOHDRToneMapper::kUchimura) == "OOHDR_TONEMAPPER_UCHIMURA");
  OO_CHECK(OOStringFromHDRToneMapper(OOHDRToneMapper::kReinhard) == "OOHDR_TONEMAPPER_REINHARD");
}

OO_TEST(OOHDRToneMapperFromString) {
  OO_CHECK(OOHDRToneMapperFromString("OOHDR_TONEMAPPER_NONE") == OOHDRToneMapper::kNone);
  OO_CHECK(OOHDRToneMapperFromString("OOHDR_TONEMAPPER_ACES_APPROX") == OOHDRToneMapper::kAcesApprox);
  OO_CHECK(OOHDRToneMapperFromString("OOHDR_TONEMAPPER_DICE") == OOHDRToneMapper::kDice);
  OO_CHECK(OOHDRToneMapperFromString("OOHDR_TONEMAPPER_UCHIMURA") == OOHDRToneMapper::kUchimura);
  OO_CHECK(OOHDRToneMapperFromString("OOHDR_TONEMAPPER_REINHARD") == OOHDRToneMapper::kReinhard);
  OO_CHECK(OOHDRToneMapperFromString("NOT_A_NAME") == OOHDRToneMapper::kAcesApprox);  // the fallback
  OO_CHECK(OOHDRToneMapperFromString("") == OOHDRToneMapper::kAcesApprox);
}

OO_TEST(OOStringFromSDRToneMapper) {
  OO_CHECK(OOStringFromSDRToneMapper(OOSDRToneMapper::kNone) == "OOSDR_TONEMAPPER_NONE");
  OO_CHECK(OOStringFromSDRToneMapper(OOSDRToneMapper::kAces) == "OOSDR_TONEMAPPER_ACES");
  OO_CHECK(OOStringFromSDRToneMapper(OOSDRToneMapper::kAgX) == "OOSDR_TONEMAPPER_AgX");
  OO_CHECK(OOStringFromSDRToneMapper(OOSDRToneMapper::kHejlDawson) == "OOSDR_TONEMAPPER_HEJLDAWSON");
  OO_CHECK(OOStringFromSDRToneMapper(OOSDRToneMapper::kUC2) == "OOSDR_TONEMAPPER_UC2");
  OO_CHECK(OOStringFromSDRToneMapper(OOSDRToneMapper::kUchimura) == "OOSDR_TONEMAPPER_UCHIMURA");
  OO_CHECK(OOStringFromSDRToneMapper(OOSDRToneMapper::kReinhard) == "OOSDR_TONEMAPPER_REINHARD");
}

OO_TEST(OOSDRToneMapperFromString) {
  OO_CHECK(OOSDRToneMapperFromString("OOSDR_TONEMAPPER_NONE") == OOSDRToneMapper::kNone);
  OO_CHECK(OOSDRToneMapperFromString("OOSDR_TONEMAPPER_ACES") == OOSDRToneMapper::kAces);
  OO_CHECK(OOSDRToneMapperFromString("OOSDR_TONEMAPPER_AgX") == OOSDRToneMapper::kAgX);
  OO_CHECK(OOSDRToneMapperFromString("OOSDR_TONEMAPPER_HEJLDAWSON") == OOSDRToneMapper::kHejlDawson);
  OO_CHECK(OOSDRToneMapperFromString("OOSDR_TONEMAPPER_UC2") == OOSDRToneMapper::kUC2);
  OO_CHECK(OOSDRToneMapperFromString("OOSDR_TONEMAPPER_UCHIMURA") == OOSDRToneMapper::kUchimura);
  OO_CHECK(OOSDRToneMapperFromString("OOSDR_TONEMAPPER_REINHARD") == OOSDRToneMapper::kReinhard);
  OO_CHECK(OOSDRToneMapperFromString("NOT_A_NAME") == OOSDRToneMapper::kAces);  // the fallback
  OO_CHECK(OOSDRToneMapperFromString("") == OOSDRToneMapper::kAces);
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

OO_TEST(jsEntityStatus) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kStartGame)) == "STATUS_START_GAME");
  { JSValue v = js.string("STATUS_START_GAME"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kStartGame); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kRestartGame)) == "STATUS_RESTART_GAME");
  { JSValue v = js.string("STATUS_RESTART_GAME"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kRestartGame); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kDead)) == "STATUS_DEAD");
  { JSValue v = js.string("STATUS_DEAD"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kDead); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kInactive)) == "STATUS_INACTIVE");
  { JSValue v = js.string("STATUS_INACTIVE"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kInactive); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kTest)) == "STATUS_TEST");
  { JSValue v = js.string("STATUS_TEST"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kTest); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kCockpitDisplay)) == "STATUS_COCKPIT_DISPLAY");
  { JSValue v = js.string("STATUS_COCKPIT_DISPLAY"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kCockpitDisplay); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kActive)) == "STATUS_ACTIVE");
  { JSValue v = js.string("STATUS_ACTIVE"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kActive); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kEffect)) == "STATUS_EFFECT");
  { JSValue v = js.string("STATUS_EFFECT"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kEffect); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kInFlight)) == "STATUS_IN_FLIGHT");
  { JSValue v = js.string("STATUS_IN_FLIGHT"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kInFlight); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kDocked)) == "STATUS_DOCKED");
  { JSValue v = js.string("STATUS_DOCKED"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kDocked); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kAutopilotEngaged)) == "STATUS_AUTOPILOT_ENGAGED");
  { JSValue v = js.string("STATUS_AUTOPILOT_ENGAGED"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kAutopilotEngaged); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kDocking)) == "STATUS_DOCKING");
  { JSValue v = js.string("STATUS_DOCKING"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kDocking); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kLaunching)) == "STATUS_LAUNCHING");
  { JSValue v = js.string("STATUS_LAUNCHING"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kLaunching); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kWitchspaceCountdown)) == "STATUS_WITCHSPACE_COUNTDOWN");
  { JSValue v = js.string("STATUS_WITCHSPACE_COUNTDOWN"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kWitchspaceCountdown); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kEnteringWitchspace)) == "STATUS_ENTERING_WITCHSPACE");
  { JSValue v = js.string("STATUS_ENTERING_WITCHSPACE"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kEnteringWitchspace); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kExitingWitchspace)) == "STATUS_EXITING_WITCHSPACE");
  { JSValue v = js.string("STATUS_EXITING_WITCHSPACE"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kExitingWitchspace); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kEscapeSequence)) == "STATUS_ESCAPE_SEQUENCE");
  { JSValue v = js.string("STATUS_ESCAPE_SEQUENCE"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kEscapeSequence); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kInHold)) == "STATUS_IN_HOLD");
  { JSValue v = js.string("STATUS_IN_HOLD"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kInHold); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kBeingScooped)) == "STATUS_BEING_SCOOPED");
  { JSValue v = js.string("STATUS_BEING_SCOOPED"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kBeingScooped); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromEntityStatus(js.context, oo::EntityStatus::kHandlingError)) == "STATUS_HANDLING_ERROR");
  { JSValue v = js.string("STATUS_HANDLING_ERROR"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == oo::EntityStatus::kHandlingError); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); OO_CHECK(OOEntityStatusFromJSValue(js.context, v) == (oo::EntityStatus)(oo::EntityStatus::kInactive)); JS_FreeValue(js.context, v); }  // the fallback
  OO_CHECK(OOEntityStatusFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::EntityStatus)(oo::EntityStatus::kInactive));  // not a string
}

OO_TEST(jsScanClass) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kNotSet)) == "CLASS_NOT_SET");
  { JSValue v = js.string("CLASS_NOT_SET"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kNotSet); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kNoDraw)) == "CLASS_NO_DRAW");
  { JSValue v = js.string("CLASS_NO_DRAW"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kNoDraw); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kNeutral)) == "CLASS_NEUTRAL");
  { JSValue v = js.string("CLASS_NEUTRAL"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kNeutral); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kStation)) == "CLASS_STATION");
  { JSValue v = js.string("CLASS_STATION"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kStation); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kTarget)) == "CLASS_TARGET");
  { JSValue v = js.string("CLASS_TARGET"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kTarget); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kCargo)) == "CLASS_CARGO");
  { JSValue v = js.string("CLASS_CARGO"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kCargo); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kMissile)) == "CLASS_MISSILE");
  { JSValue v = js.string("CLASS_MISSILE"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kMissile); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kRock)) == "CLASS_ROCK");
  { JSValue v = js.string("CLASS_ROCK"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kRock); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kMine)) == "CLASS_MINE");
  { JSValue v = js.string("CLASS_MINE"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kMine); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kThargoid)) == "CLASS_THARGOID");
  { JSValue v = js.string("CLASS_THARGOID"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kThargoid); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kBuoy)) == "CLASS_BUOY");
  { JSValue v = js.string("CLASS_BUOY"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kBuoy); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kVisualEffect)) == "CLASS_VISUAL_EFFECT");
  { JSValue v = js.string("CLASS_VISUAL_EFFECT"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kVisualEffect); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kWormhole)) == "CLASS_WORMHOLE");
  { JSValue v = js.string("CLASS_WORMHOLE"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kWormhole); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kPlayer)) == "CLASS_PLAYER");
  { JSValue v = js.string("CLASS_PLAYER"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kPlayer); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kPolice)) == "CLASS_POLICE");
  { JSValue v = js.string("CLASS_POLICE"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kPolice); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromScanClass(js.context, oo::ScanClass::kMilitary)) == "CLASS_MILITARY");
  { JSValue v = js.string("CLASS_MILITARY"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == oo::ScanClass::kMilitary); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); OO_CHECK(OOScanClassFromJSValue(js.context, v) == (oo::ScanClass)(oo::ScanClass::kNotSet)); JS_FreeValue(js.context, v); }  // the fallback
  OO_CHECK(OOScanClassFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::ScanClass)(oo::ScanClass::kNotSet));  // not a string
}

OO_TEST(jsCompassMode) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kInactive)) == "COMPASS_MODE_INACTIVE");
  { JSValue v = js.string("COMPASS_MODE_INACTIVE"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kInactive); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kBasic)) == "COMPASS_MODE_BASIC");
  { JSValue v = js.string("COMPASS_MODE_BASIC"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kBasic); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kPlanet)) == "COMPASS_MODE_PLANET");
  { JSValue v = js.string("COMPASS_MODE_PLANET"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kPlanet); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kStation)) == "COMPASS_MODE_STATION");
  { JSValue v = js.string("COMPASS_MODE_STATION"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kStation); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kSun)) == "COMPASS_MODE_SUN");
  { JSValue v = js.string("COMPASS_MODE_SUN"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kSun); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kTarget)) == "COMPASS_MODE_TARGET");
  { JSValue v = js.string("COMPASS_MODE_TARGET"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kTarget); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromCompassMode(js.context, oo::CompassMode::kBeacons)) == "COMPASS_MODE_BEACONS");
  { JSValue v = js.string("COMPASS_MODE_BEACONS"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == oo::CompassMode::kBeacons); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); OO_CHECK(OOCompassModeFromJSValue(js.context, v) == (oo::CompassMode)(oo::CompassMode::kBasic)); JS_FreeValue(js.context, v); }  // the fallback
  OO_CHECK(OOCompassModeFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::CompassMode)(oo::CompassMode::kBasic));  // not a string
}

OO_TEST(jsGUIScreenID) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMain)) == "GUI_SCREEN_MAIN");
  { JSValue v = js.string("GUI_SCREEN_MAIN"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMain); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kIntro1)) == "GUI_SCREEN_INTRO1");
  { JSValue v = js.string("GUI_SCREEN_INTRO1"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kIntro1); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kShipLibrary)) == "GUI_SCREEN_SHIPLIBRARY");
  { JSValue v = js.string("GUI_SCREEN_SHIPLIBRARY"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kShipLibrary); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboard)) == "GUI_SCREEN_KEYBOARD");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboard); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kNewGame)) == "GUI_SCREEN_NEWGAME");
  { JSValue v = js.string("GUI_SCREEN_NEWGAME"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kNewGame); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kOXZManager)) == "GUI_SCREEN_OXZMANAGER");
  { JSValue v = js.string("GUI_SCREEN_OXZMANAGER"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kOXZManager); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kStatus)) == "GUI_SCREEN_STATUS");
  { JSValue v = js.string("GUI_SCREEN_STATUS"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kStatus); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kManifest)) == "GUI_SCREEN_MANIFEST");
  { JSValue v = js.string("GUI_SCREEN_MANIFEST"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kManifest); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kEquipShip)) == "GUI_SCREEN_EQUIP_SHIP");
  { JSValue v = js.string("GUI_SCREEN_EQUIP_SHIP"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kEquipShip); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kShipyard)) == "GUI_SCREEN_SHIPYARD");
  { JSValue v = js.string("GUI_SCREEN_SHIPYARD"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kShipyard); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kLongRangeChart)) == "GUI_SCREEN_LONG_RANGE_CHART");
  { JSValue v = js.string("GUI_SCREEN_LONG_RANGE_CHART"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kLongRangeChart); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kShortRangeChart)) == "GUI_SCREEN_SHORT_RANGE_CHART");
  { JSValue v = js.string("GUI_SCREEN_SHORT_RANGE_CHART"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kShortRangeChart); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kSystemData)) == "GUI_SCREEN_SYSTEM_DATA");
  { JSValue v = js.string("GUI_SCREEN_SYSTEM_DATA"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kSystemData); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMarket)) == "GUI_SCREEN_MARKET");
  { JSValue v = js.string("GUI_SCREEN_MARKET"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMarket); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMarketInfo)) == "GUI_SCREEN_MARKETINFO");
  { JSValue v = js.string("GUI_SCREEN_MARKETINFO"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMarketInfo); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kOptions)) == "GUI_SCREEN_OPTIONS");
  { JSValue v = js.string("GUI_SCREEN_OPTIONS"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kOptions); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kGameOptions)) == "GUI_SCREEN_GAMEOPTIONS");
  { JSValue v = js.string("GUI_SCREEN_GAMEOPTIONS"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kGameOptions); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kLoad)) == "GUI_SCREEN_LOAD");
  { JSValue v = js.string("GUI_SCREEN_LOAD"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kLoad); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kSave)) == "GUI_SCREEN_SAVE");
  { JSValue v = js.string("GUI_SCREEN_SAVE"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kSave); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kSaveOverwrite)) == "GUI_SCREEN_SAVE_OVERWRITE");
  { JSValue v = js.string("GUI_SCREEN_SAVE_OVERWRITE"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kSaveOverwrite); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kStickMapper)) == "GUI_SCREEN_STICKMAPPER");
  { JSValue v = js.string("GUI_SCREEN_STICKMAPPER"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kStickMapper); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kStickProfile)) == "GUI_SCREEN_STICKPROFILE");
  { JSValue v = js.string("GUI_SCREEN_STICKPROFILE"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kStickProfile); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kMission)) == "GUI_SCREEN_MISSION");
  { JSValue v = js.string("GUI_SCREEN_MISSION"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kMission); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kReport)) == "GUI_SCREEN_REPORT");
  { JSValue v = js.string("GUI_SCREEN_REPORT"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kReport); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kInterfaces)) == "GUI_SCREEN_INTERFACES");
  { JSValue v = js.string("GUI_SCREEN_INTERFACES"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kInterfaces); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardConfirmClear)) == "GUI_SCREEN_KEYBOARD_CONFIRMCLEAR");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_CONFIRMCLEAR"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardConfirmClear); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardConfig)) == "GUI_SCREEN_KEYBOARD_CONFIG");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_CONFIG"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardConfig); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardEntry)) == "GUI_SCREEN_KEYBOARD_ENTRY");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_ENTRY"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardEntry); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGUIScreenID(js.context, oo::GUIScreenID::kKeyboardLayout)) == "GUI_SCREEN_KEYBOARD_LAYOUT");
  { JSValue v = js.string("GUI_SCREEN_KEYBOARD_LAYOUT"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == oo::GUIScreenID::kKeyboardLayout); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); OO_CHECK(OOGUIScreenIDFromJSValue(js.context, v) == (oo::GUIScreenID)(oo::GUIScreenID::kMain)); JS_FreeValue(js.context, v); }  // the fallback
  OO_CHECK(OOGUIScreenIDFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::GUIScreenID)(oo::GUIScreenID::kMain));  // not a string
}

OO_TEST(jsGalacticHyperspaceBehaviour) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kStandard)) == "BEHAVIOUR_STANDARD");
  { JSValue v = js.string("BEHAVIOUR_STANDARD"); OO_CHECK(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kStandard); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kAllSystemsReachable)) == "BEHAVIOUR_ALL_SYSTEMS_REACHABLE");
  { JSValue v = js.string("BEHAVIOUR_ALL_SYSTEMS_REACHABLE"); OO_CHECK(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kAllSystemsReachable); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kFixedCoordinates)) == "BEHAVIOUR_FIXED_COORDINATES");
  { JSValue v = js.string("BEHAVIOUR_FIXED_COORDINATES"); OO_CHECK(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kFixedCoordinates); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromGalacticHyperspaceBehaviour(js.context, oo::GalacticHyperspaceBehaviour::kUnknown)) == "BEHAVIOUR_UNKNOWN");
  { JSValue v = js.string("BEHAVIOUR_UNKNOWN"); OO_CHECK(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == oo::GalacticHyperspaceBehaviour::kUnknown); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); OO_CHECK(OOGalacticHyperspaceBehaviourFromJSValue(js.context, v) == (oo::GalacticHyperspaceBehaviour)(oo::GalacticHyperspaceBehaviour::kUnknown)); JS_FreeValue(js.context, v); }  // the fallback
  OO_CHECK(OOGalacticHyperspaceBehaviourFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::GalacticHyperspaceBehaviour)(oo::GalacticHyperspaceBehaviour::kUnknown));  // not a string
}

OO_TEST(jsViewID) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kForward)) == "VIEW_FORWARD");
  { JSValue v = js.string("VIEW_FORWARD"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kForward); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kAft)) == "VIEW_AFT");
  { JSValue v = js.string("VIEW_AFT"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kAft); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kPort)) == "VIEW_PORT");
  { JSValue v = js.string("VIEW_PORT"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kPort); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kStarboard)) == "VIEW_STARBOARD");
  { JSValue v = js.string("VIEW_STARBOARD"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kStarboard); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kCustom)) == "VIEW_CUSTOM");
  { JSValue v = js.string("VIEW_CUSTOM"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kCustom); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kNone)) == "VIEW_NONE");
  { JSValue v = js.string("VIEW_NONE"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kNone); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kGUIDisplay)) == "VIEW_GUI_DISPLAY");
  { JSValue v = js.string("VIEW_GUI_DISPLAY"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kGUIDisplay); JS_FreeValue(js.context, v); }
  OO_CHECK(js.text(OOJSValueFromViewID(js.context, oo::ViewID::kBreakPattern)) == "VIEW_BREAK_PATTERN");
  { JSValue v = js.string("VIEW_BREAK_PATTERN"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == oo::ViewID::kBreakPattern); JS_FreeValue(js.context, v); }
  { JSValue v = js.string("NOT_A_NAME"); OO_CHECK(OOViewIDFromJSValue(js.context, v) == (oo::ViewID)(oo::ViewID::kNone)); JS_FreeValue(js.context, v); }  // the fallback
  OO_CHECK(OOViewIDFromJSValue(js.context, JS_NewInt32(js.context, 1)) == (oo::ViewID)(oo::ViewID::kNone));  // not a string
}

OO_TEST(jsShipDamageType) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kScrape)) == "scrape damage");
  OO_CHECK(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kHeat)) == "heat damage");
  OO_CHECK(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kEnergy)) == "energy damage");
  OO_CHECK(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kCascadeWeapon)) == "cascade weapon");
  OO_CHECK(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kHitAPlanet)) == "hit a planet");
  OO_CHECK(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kHitASun)) == "hit a sun");
  OO_CHECK(js.text(OOJSValueFromShipDamageType(js.context, oo::ShipDamageType::kRemoved)) == "removed");
}

OO_TEST(jsLegalStatusReason) {
  JSStrings& js = Strings();
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kUnknown)) == "unknown");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kSetup)) == "setup actions");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kByScript)) == "scripted");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAttackedPolice)) == "attacked police");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAttackedMainStation)) == "attacked main station");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAttackedInnocent)) == "attacked innocent");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kSeenByPolice)) == "seen by police");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kDistressCall)) == "distress call");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kIllegalExports)) == "illegal exports");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kIllegalImports)) == "illegal imports");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAssistingOffender)) == "assisting offenders");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kNewGalaxy)) == "new galaxy");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kNewSystem)) == "new system");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kPaidFine)) == "paid fine");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kEscapePod)) == "escape pod");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kAssistingPolice)) == "assisting police");
  OO_CHECK(js.text(OOJSValueFromLegalStatusReason(js.context, oo::LegalStatusReason::kPoliceAreClean)) == "police are clean");
}

OO_TEST_MAIN()
