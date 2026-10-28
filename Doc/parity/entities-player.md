# Parity findings: Player entity and its categories

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## PlayerEntity

<a id="playerentity"></a>

### PlayerEntity (part 1: lines 1-5981)

Upstream `src/Core/Entities/PlayerEntity.m` lines 1–5981 (up to but excluding `-setBounty:`) ↔ `Core/Entities/PlayerEntity.cpp`

**Status:** done — 258 rows: 248 ✅ 5 ⚠️ 2 ❌ 3 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `PLAYER_DEFAULT_NAME` (89) | `PLAYER_DEFAULT_NAME` `Core/Entities/PlayerEntity.cpp:103` | ✅ |
| `enum { kCommLogTrimThreshold, kCommLogTrimSize }` (91) | enum `Core/Entities/PlayerEntity.cpp:105` | ✅ |
| `kOOLogBuyMountedOK` / `kOOLogBuyMountedFailed` (97) | inlined literals `"equip.buy.mounted"` / `"equip.buy.mounted.failed"` `Core/Entities/PlayerEntity.cpp:9847` | ➖ |
| `kDeadResetTime` (99) | `kDeadResetTime` `Core/Entities/PlayerEntity.cpp:305` | ✅ |
| `gOOPlayer` (101) | `gOOPlayer` `Core/Entities/PlayerEntity.cpp:400` | ✅ |
| `sBaseMass` (102) | `sBaseMass` `Core/Entities/PlayerEntity.cpp:307` | ✅ |
| `STAGE_TRACKING_BEGIN` / `STAGE_TRACKING_END` / `UPDATE_STAGE` macros (2355) | same macros `Core/Entities/PlayerEntity.cpp:334` | ⚠️ |
| `VELOCITY_CLEANUP_MIN/FULL/RATE` (3346) | same macros `Core/Entities/PlayerEntity.cpp:363` | ✅ |
| `+[PlayerEntity sharedPlayer]` (168) | `sharedPlayer` `Core/Entities/PlayerEntity.cpp:422` | ✅ |
| `-[PlayerEntity setName:]` (176) | `setName` `Core/Entities/PlayerEntity.cpp:525` | ✅ |
| `-[PlayerEntity baseMass]` (181) | `baseMass` `Core/Entities/PlayerEntity.cpp:530` | ✅ |
| `-[PlayerEntity unloadAllCargoPodsForType:toManifest:]` (199) | `unloadAllCargoPodsForType` `Core/Entities/PlayerEntity.cpp:548` | ✅ |
| `-[PlayerEntity unloadCargoPodsForType:amount:]` (223) | `unloadCargoPodsForType` `Core/Entities/PlayerEntity.cpp:572` | ✅ |
| `-[PlayerEntity unloadCargoPods]` (263) | `unloadCargoPods` `Core/Entities/PlayerEntity.cpp:614` | ✅ |
| `-[PlayerEntity createCargoPodWithType:andAmount:]` (282) | `createCargoPodWithType` `Core/Entities/PlayerEntity.cpp:632` | ✅ |
| `-[PlayerEntity loadCargoPodsForType:fromManifest:]` (299) | `loadCargoPodsForType` `Core/Entities/PlayerEntity.cpp:652` | ✅ |
| `-[PlayerEntity loadCargoPodsForType:amount:]` (359) | `loadCargoPodsForType` `Core/Entities/PlayerEntity.cpp:712` | ✅ |
| `-[PlayerEntity loadCargoPods]` (410) | `loadCargoPods` `Core/Entities/PlayerEntity.cpp:772` | ✅ |
| `-[PlayerEntity shipCommodityData]` (421) | `getShipCommodityData` `Core/Entities/PlayerEntity.cpp:782` | ✅ |
| `-[PlayerEntity deciCredits]` (426) | `deciCredits` `Core/Entities/PlayerEntity.cpp:787` | ✅ |
| `-[PlayerEntity random_factor]` (431) | `random_factor` `Core/Entities/PlayerEntity.cpp:792` | ✅ |
| `-[PlayerEntity setRandom_factor:]` (436) | `setRandom_factor` `Core/Entities/PlayerEntity.cpp:797` | ✅ |
| `-[PlayerEntity galaxyNumber]` (441) | `galaxyNumber` `Core/Entities/PlayerEntity.cpp:802` | ✅ |
| `-[PlayerEntity galaxy_coordinates]` (446) | `getGalaxy_coordinates` `Core/Entities/PlayerEntity.cpp:807` | ✅ |
| `-[PlayerEntity setGalaxyCoordinates:]` (451) | `setGalaxyCoordinates` `Core/Entities/PlayerEntity.cpp:812` | ✅ |
| `-[PlayerEntity cursor_coordinates]` (457) | `getCursor_coordinates` `Core/Entities/PlayerEntity.cpp:818` | ✅ |
| `-[PlayerEntity chart_centre_coordinates]` (462) | `getChart_centre_coordinates` `Core/Entities/PlayerEntity.cpp:823` | ✅ |
| `-[PlayerEntity chart_zoom]` (467) | `getChart_zoom` `Core/Entities/PlayerEntity.cpp:828` | ✅ |
| `-[PlayerEntity custom_chart_zoom]` (479) | `getCustom_chart_zoom` `Core/Entities/PlayerEntity.cpp:840` | ✅ |
| `-[PlayerEntity setCustomChartZoom:]` (484) | `setCustomChartZoom` `Core/Entities/PlayerEntity.cpp:845` | ✅ |
| `-[PlayerEntity custom_chart_centre_coordinates]` (489) | `getCustom_chart_centre_coordinates` `Core/Entities/PlayerEntity.cpp:850` | ✅ |
| `-[PlayerEntity setCustomChartCentre:]` (494) | `setCustomChartCentre` `Core/Entities/PlayerEntity.cpp:855` | ✅ |
| `-[PlayerEntity adjusted_chart_centre]` (500) | `adjusted_chart_centre` `Core/Entities/PlayerEntity.cpp:861` | ✅ |
| `-[PlayerEntity ANAMode]` (558) | `ANAMode` `Core/Entities/PlayerEntity.cpp:919` | ✅ |
| `-[PlayerEntity systemID]` (563) | `systemID` `Core/Entities/PlayerEntity.cpp:924` | ✅ |
| `-[PlayerEntity setSystemID:]` (568) | `setSystemID` `Core/Entities/PlayerEntity.cpp:929` | ✅ |
| `-[PlayerEntity previousSystemID]` (576) | `previousSystemID` `Core/Entities/PlayerEntity.cpp:937` | ✅ |
| `-[PlayerEntity setPreviousSystemID:]` (581) | `setPreviousSystemID` `Core/Entities/PlayerEntity.cpp:942` | ✅ |
| `-[PlayerEntity targetSystemID]` (586) | `targetSystemID` `Core/Entities/PlayerEntity.cpp:950` | ✅ |
| `-[PlayerEntity setTargetSystemID:]` (591) | `setTargetSystemID` `Core/Entities/PlayerEntity.cpp:955` | ✅ |
| `-[PlayerEntity nextHopTargetSystemID]` (598) | `nextHopTargetSystemID` `Core/Entities/PlayerEntity.cpp:962` | ✅ |
| `-[PlayerEntity infoSystemID]` (621) | `infoSystemID` `Core/Entities/PlayerEntity.cpp:985` | ✅ |
| `-[PlayerEntity setInfoSystemID:moveChart:]` (626) | `setInfoSystemID` `Core/Entities/PlayerEntity.cpp:990` | ✅ |
| `-[PlayerEntity nextInfoSystem]` (653) | `nextInfoSystem` `Core/Entities/PlayerEntity.cpp:1017` | ✅ |
| `-[PlayerEntity previousInfoSystem]` (680) | `previousInfoSystem` `Core/Entities/PlayerEntity.cpp:1043` | ✅ |
| `-[PlayerEntity homeInfoSystem]` (707) | `homeInfoSystem` `Core/Entities/PlayerEntity.cpp:1069` | ✅ |
| `-[PlayerEntity targetInfoSystem]` (713) | `targetInfoSystem` `Core/Entities/PlayerEntity.cpp:1075` | ✅ |
| `-[PlayerEntity infoSystemOnRoute]` (719) | `infoSystemOnRoute` `Core/Entities/PlayerEntity.cpp:1081` | ✅ |
| `-[PlayerEntity wormhole]` (734) | `getWormhole` `Core/Entities/PlayerEntity.cpp:1097` | ✅ |
| `-[PlayerEntity setWormhole:]` (739) | `setWormhole` `Core/Entities/PlayerEntity.cpp:1102` | ✅ |
| `-[PlayerEntity commanderDataDictionary]` (749) | `commanderDataDictionary` `Core/Entities/PlayerEntity.cpp:1114` | ⚠️ |
| `-[PlayerEntity setCommanderDataFromDictionary:]` (1012) | `setCommanderDataFromDictionary` `Core/Entities/PlayerEntity.cpp:1413` | ❌ |
| `-[PlayerEntity init]` (1595) | `PlayerEntity` `Core/Entities/PlayerEntity.cpp:419` + `newPlayerObject` `Core/Entities/PlayerEntity.cpp:432` | ➖ |
| `-[PlayerEntity deferredInit]` (1601) | `deferredInit` `Core/Entities/PlayerEntity.cpp:443` | ✅ |
| `-[PlayerEntity setUpAndConfirmOK:]` (1639) | `setUpAndConfirmOK` `Core/Entities/PlayerEntity.cpp:1996` | ✅ |
| `-[PlayerEntity setUpAndConfirmOK:saveGame:]` (1644) | `setUpAndConfirmOK` `Core/Entities/PlayerEntity.cpp:2001` | ✅ |
| `-[PlayerEntity completeSetUp]` (2022) | `completeSetUp` `Core/Entities/PlayerEntity.cpp:2364` | ✅ |
| `-[PlayerEntity completeSetUpAndSetTarget:]` (2027) | `completeSetUpAndSetTarget` `Core/Entities/PlayerEntity.cpp:2369` | ✅ |
| `-[PlayerEntity startUpComplete]` (2043) | `startUpComplete` `Core/Entities/PlayerEntity.cpp:2388` | ✅ |
| `-[PlayerEntity setUpShipFromDictionary:]` (2050) | `setUpShipFromDictionary` `Core/Entities/PlayerEntity.cpp:2395` | ✅ |
| `-[PlayerEntity dealloc]` (2145) | `willDealloc` `Core/Entities/PlayerEntity.cpp:483` + `~PlayerEntity` `Core/Entities/PlayerEntity.cpp:420` | ➖ |
| `-[PlayerEntity sessionID]` (2231) | `sessionID` `Core/Entities/PlayerEntity.cpp:2487` | ✅ |
| `-[PlayerEntity warnAboutHostiles]` (2237) | `warnAboutHostiles` `Core/Entities/PlayerEntity.cpp:2493` | ✅ |
| `-[PlayerEntity canCollide]` (2242) | `canCollide` `Core/Entities/PlayerEntity.cpp:2498` | ✅ |
| `-[PlayerEntity compareZeroDistance:]` (2257) | `compareZeroDistance` `Core/Entities/PlayerEntity.cpp:2516` | ✅ |
| `-[PlayerEntity validForAddToUniverse]` (2262) | `validForAddToUniverse` `Core/Entities/PlayerEntity.cpp:2521` | ✅ |
| `-[PlayerEntity lookingAtSunWithThresholdAngleCos:]` (2267) | `lookingAtSunWithThresholdAngleCos` `Core/Entities/PlayerEntity.cpp:2526` | ✅ |
| `-[PlayerEntity insideAtmosphereFraction]` (2342) | `insideAtmosphereFraction` `Core/Entities/PlayerEntity.cpp:2601` | ✅ |
| `-[PlayerEntity update:]` (2380) | `update` `Core/Entities/PlayerEntity.cpp:2613` (calls `getPollControls`) | ✅ |
| `-[PlayerEntity doBookkeeping:]` (2461) | `doBookkeeping` `Core/Entities/PlayerEntity.cpp:2697` | ✅ |
| `-[PlayerEntity updateMovementFlags]` (2885) | `updateMovementFlags` `Core/Entities/PlayerEntity.cpp:3147` | ✅ |
| `-[PlayerEntity updateAlertConditionForNearbyEntities]` (2893) | `updateAlertConditionForNearbyEntities` `Core/Entities/PlayerEntity.cpp:3158` | ✅ |
| `-[PlayerEntity setMaxFlightPitch:]` (2996) | `setMaxFlightPitch` `Core/Entities/PlayerEntity.cpp:3260` | ✅ |
| `-[PlayerEntity setMaxFlightRoll:]` (3002) | `setMaxFlightRoll` `Core/Entities/PlayerEntity.cpp:3266` | ✅ |
| `-[PlayerEntity setMaxFlightYaw:]` (3008) | `setMaxFlightYaw` `Core/Entities/PlayerEntity.cpp:3272` | ✅ |
| `-[PlayerEntity checkEntityForMassLock:withScanClass:]` (3014) | `checkEntityForMassLock` `Core/Entities/PlayerEntity.cpp:3278` | ✅ |
| `-[PlayerEntity updateAlertCondition]` (3068) | `updateAlertCondition` `Core/Entities/PlayerEntity.cpp:3332` | ✅ |
| `-[PlayerEntity updateFuelScoops:]` (3099) | `updateFuelScoops` `Core/Entities/PlayerEntity.cpp:3363` | ✅ |
| `-[PlayerEntity updateClocks:]` (3110) | `updateClocks` `Core/Entities/PlayerEntity.cpp:3374` | ✅ |
| `-[PlayerEntity checkScriptsIfAppropriate]` (3160) | `checkScriptsIfAppropriate` `Core/Entities/PlayerEntity.cpp:3425` | ✅ |
| `-[PlayerEntity updateTrumbles:]` (3211) | `updateTrumbles` `Core/Entities/PlayerEntity.cpp:3476` | ✅ |
| `-[PlayerEntity performAutopilotUpdates:]` (3222) | `performAutopilotUpdates` `Core/Entities/PlayerEntity.cpp:3487` | ✅ |
| `-[PlayerEntity performDockingRequest:]` (3229) | `performDockingRequest` `Core/Entities/PlayerEntity.cpp:3494` | ✅ |
| `-[PlayerEntity requestDockingClearance:]` (3251) | `requestDockingClearance` `Core/Entities/PlayerEntity.cpp:3516` | ✅ |
| `-[PlayerEntity cancelDockingRequest:]` (3258) | `cancelDockingRequest` `Core/Entities/PlayerEntity.cpp:3523` | ✅ |
| `-[PlayerEntity engageAutopilotToStation:]` (3279) | `engageAutopilotToStation` `Core/Entities/PlayerEntity.cpp:3544` | ✅ |
| `-[PlayerEntity disengageAutopilot]` (3314) | `disengageAutopilot` `Core/Entities/PlayerEntity.cpp:3580` | ✅ |
| `-[PlayerEntity resetAutopilotAI]` (3333) | `resetAutopilotAI` `Core/Entities/PlayerEntity.cpp:3602` | ✅ |
| `-[PlayerEntity hyperspeedFactor]` (3351) | `getHyperspeedFactor` `Core/Entities/PlayerEntity.cpp:3621` | ✅ |
| `-[PlayerEntity injectorsEngaged]` (3357) | `injectorsEngaged` `Core/Entities/PlayerEntity.cpp:3627` | ✅ |
| `-[PlayerEntity hyperspeedEngaged]` (3362) | `hyperspeedEngaged` `Core/Entities/PlayerEntity.cpp:3632` | ✅ |
| `-[PlayerEntity performInFlightUpdates:]` (3367) | `performInFlightUpdates` `Core/Entities/PlayerEntity.cpp:3637` | ✅ |
| `-[PlayerEntity performWitchspaceCountdownUpdates:]` (3422) | `performWitchspaceCountdownUpdates` `Core/Entities/PlayerEntity.cpp:3692` | ✅ |
| `-[PlayerEntity performWitchspaceExitUpdates:]` (3479) | `performWitchspaceExitUpdates` `Core/Entities/PlayerEntity.cpp:3752` | ✅ |
| `-[PlayerEntity performLaunchingUpdates:]` (3519) | `performLaunchingUpdates` `Core/Entities/PlayerEntity.cpp:3796` | ✅ |
| `-[PlayerEntity performDockingUpdates:]` (3539) | `performDockingUpdates` `Core/Entities/PlayerEntity.cpp:3816` | ✅ |
| `-[PlayerEntity performDeadUpdates:]` (3551) | `performDeadUpdates` `Core/Entities/PlayerEntity.cpp:3831` | ✅ |
| `-[PlayerEntity gameOverFadeToBW]` (3566) | `gameOverFadeToBW` `Core/Entities/PlayerEntity.cpp:3850` | ✅ |
| `-[PlayerEntity isValidTarget:]` (3595) | `isValidTarget` `Core/Entities/PlayerEntity.cpp:3882` | ✅ |
| `-[PlayerEntity showGameOver]` (3630) | `showGameOver` `Core/Entities/PlayerEntity.cpp:3916` | ✅ |
| `-[PlayerEntity showShipModelWithKey:shipData:personality:factorX:factorY:factorZ:inContext:]` (3646) | `showShipModelWithKey` `Core/Entities/PlayerEntity.cpp:3940` | ✅ |
| `-[PlayerEntity updateTargeting]` (3699) | `updateTargeting` `Core/Entities/PlayerEntity.cpp:4007` | ✅ |
| `-[PlayerEntity orientationChanged]` (3803) | `orientationChanged` `Core/Entities/PlayerEntity.cpp:4123` | ✅ |
| `-[PlayerEntity applyAttitudeChanges:]` (3814) | `applyAttitudeChanges` `Core/Entities/PlayerEntity.cpp:4134` | ✅ |
| `-[PlayerEntity applyRoll:andClimb:]` (3820) | `applyRoll` `Core/Entities/PlayerEntity.cpp:4140` | ✅ |
| `-[PlayerEntity applyYaw:]` (3849) | `applyYaw` `Core/Entities/PlayerEntity.cpp:4169` | ✅ |
| `-[PlayerEntity drawRotationMatrix]` (3856) | `drawRotationMatrix` `Core/Entities/PlayerEntity.cpp:4176` | ✅ |
| `-[PlayerEntity drawTransformationMatrix]` (3861) | `drawTransformationMatrix` `Core/Entities/PlayerEntity.cpp:4181` | ✅ |
| `-[PlayerEntity normalOrientation]` (3868) | `normalOrientation` `Core/Entities/PlayerEntity.cpp:4188` | ✅ |
| `-[PlayerEntity setNormalOrientation:]` (3873) | `setNormalOrientation` `Core/Entities/PlayerEntity.cpp:4193` | ✅ |
| `-[PlayerEntity moveForward:]` (3878) | `moveForward` `Core/Entities/PlayerEntity.cpp:4198` | ✅ |
| `-[PlayerEntity breakPatternPosition]` (3884) | `breakPatternPosition` `Core/Entities/PlayerEntity.cpp:4204` | ✅ |
| `-[PlayerEntity viewpointOffset]` (3889) | `viewpointOffset` `Core/Entities/PlayerEntity.cpp:4209` | ✅ |
| `-[PlayerEntity viewpointOffsetAft]` (3916) | `viewpointOffsetAft` `Core/Entities/PlayerEntity.cpp:4236` | ✅ |
| `-[PlayerEntity viewpointOffsetForward]` (3921) | `viewpointOffsetForward` `Core/Entities/PlayerEntity.cpp:4241` | ✅ |
| `-[PlayerEntity viewpointOffsetPort]` (3926) | `viewpointOffsetPort` `Core/Entities/PlayerEntity.cpp:4246` | ✅ |
| `-[PlayerEntity viewpointOffsetStarboard]` (3931) | `viewpointOffsetStarboard` `Core/Entities/PlayerEntity.cpp:4251` | ✅ |
| `-[PlayerEntity viewpointPosition]` (3938) | `viewpointPosition` `Core/Entities/PlayerEntity.cpp:4258` | ✅ |
| `-[PlayerEntity drawImmediate:translucent:]` (3962) | `drawImmediate` `Core/Entities/PlayerEntity.cpp:4282` | ✅ |
| `-[PlayerEntity setMassLockable:]` (3979) | `setMassLockable` `Core/Entities/PlayerEntity.cpp:4299` | ✅ |
| `-[PlayerEntity massLockable]` (3985) | `getMassLockable` `Core/Entities/PlayerEntity.cpp:4305` | ✅ |
| `-[PlayerEntity massLocked]` (3990) | `massLocked` `Core/Entities/PlayerEntity.cpp:4310` | ✅ |
| `-[PlayerEntity atHyperspeed]` (3995) | `atHyperspeed` `Core/Entities/PlayerEntity.cpp:4315` | ✅ |
| `-[PlayerEntity occlusionLevel]` (4000) | `occlusionLevel` `Core/Entities/PlayerEntity.cpp:4320` | ✅ |
| `-[PlayerEntity setOcclusionLevel:]` (4005) | `setOcclusionLevel` `Core/Entities/PlayerEntity.cpp:4325` | ✅ |
| `-[PlayerEntity setDockedAtMainStation]` (4010) | `setDockedAtMainStation` `Core/Entities/PlayerEntity.cpp:4330` | ✅ |
| `-[PlayerEntity dockedStation]` (4017) | `dockedStation` `Core/Entities/PlayerEntity.cpp:4337` | ✅ |
| `-[PlayerEntity setDockedStation:]` (4022) | `setDockedStation` `Core/Entities/PlayerEntity.cpp:4342` | ✅ |
| `-[PlayerEntity setTargetDockStationTo:]` (4028) | `setTargetDockStationTo` `Core/Entities/PlayerEntity.cpp:4347` | ✅ |
| `-[PlayerEntity getTargetDockStation]` (4033) | `getTargetDockStation` `Core/Entities/PlayerEntity.cpp:4352` | ✅ |
| `-[PlayerEntity hud]` (4038) | `getHud` `Core/Entities/PlayerEntity.cpp:4357` | ✅ |
| `-[PlayerEntity resetHud]` (4043) | `resetHud` `Core/Entities/PlayerEntity.cpp:4362` | ✅ |
| `-[PlayerEntity switchHudTo:]` (4052) | `switchHudTo` `Core/Entities/PlayerEntity.cpp:4371` | ✅ |
| `-[PlayerEntity dialCustomFloat:]` (4112) | `dialCustomFloat` `Core/Entities/PlayerEntity.cpp:4430` | ✅ |
| `-[PlayerEntity dialCustomString:]` (4117) | `dialCustomString` `Core/Entities/PlayerEntity.cpp:4436` | ✅ |
| `-[PlayerEntity dialCustomColor:]` (4122) | `dialCustomColor` `Core/Entities/PlayerEntity.cpp:4442` | ✅ |
| `-[PlayerEntity setDialCustom:forKey:]` (4127) | `setDialCustom` `Core/Entities/PlayerEntity.cpp:4448` | ⚠️ |
| `-[PlayerEntity setShowDemoShips:]` (4132) | `setShowDemoShips` `Core/Entities/PlayerEntity.cpp:4453` | ✅ |
| `-[PlayerEntity showDemoShips]` (4137) | `getShowDemoShips` `Core/Entities/PlayerEntity.cpp:4458` | ✅ |
| `-[PlayerEntity maxForwardShieldLevel]` (4142) | `maxForwardShieldLevel` `Core/Entities/PlayerEntity.cpp:4463` | ✅ |
| `-[PlayerEntity maxAftShieldLevel]` (4147) | `maxAftShieldLevel` `Core/Entities/PlayerEntity.cpp:4468` | ✅ |
| `-[PlayerEntity forwardShieldRechargeRate]` (4152) | `forwardShieldRechargeRate` `Core/Entities/PlayerEntity.cpp:4473` | ✅ |
| `-[PlayerEntity aftShieldRechargeRate]` (4157) | `aftShieldRechargeRate` `Core/Entities/PlayerEntity.cpp:4478` | ✅ |
| `-[PlayerEntity setMaxForwardShieldLevel:]` (4162) | `setMaxForwardShieldLevel` `Core/Entities/PlayerEntity.cpp:4483` | ✅ |
| `-[PlayerEntity setMaxAftShieldLevel:]` (4167) | `setMaxAftShieldLevel` `Core/Entities/PlayerEntity.cpp:4488` | ✅ |
| `-[PlayerEntity setForwardShieldRechargeRate:]` (4172) | `setForwardShieldRechargeRate` `Core/Entities/PlayerEntity.cpp:4493` | ✅ |
| `-[PlayerEntity setAftShieldRechargeRate:]` (4177) | `setAftShieldRechargeRate` `Core/Entities/PlayerEntity.cpp:4498` | ✅ |
| `-[PlayerEntity forwardShieldLevel]` (4182) | `forwardShieldLevel` `Core/Entities/PlayerEntity.cpp:4503` | ✅ |
| `-[PlayerEntity aftShieldLevel]` (4187) | `aftShieldLevel` `Core/Entities/PlayerEntity.cpp:4508` | ✅ |
| `-[PlayerEntity setForwardShieldLevel:]` (4192) | `setForwardShieldLevel` `Core/Entities/PlayerEntity.cpp:4513` | ✅ |
| `-[PlayerEntity setAftShieldLevel:]` (4197) | `setAftShieldLevel` `Core/Entities/PlayerEntity.cpp:4518` | ✅ |
| `-[PlayerEntity keyConfig]` (4202) | `keyConfig` `Core/Entities/PlayerEntity.cpp:4523` | ✅ |
| `-[PlayerEntity isMouseControlOn]` (4208) | `isMouseControlOn` `Core/Entities/PlayerEntity.cpp:4529` | ✅ |
| `-[PlayerEntity dialRoll]` (4213) | `dialRoll` `Core/Entities/PlayerEntity.cpp:4534` | ✅ |
| `-[PlayerEntity dialPitch]` (4223) | `dialPitch` `Core/Entities/PlayerEntity.cpp:4544` | ✅ |
| `-[PlayerEntity dialYaw]` (4233) | `dialYaw` `Core/Entities/PlayerEntity.cpp:4554` | ✅ |
| `-[PlayerEntity dialSpeed]` (4243) | `dialSpeed` `Core/Entities/PlayerEntity.cpp:4564` | ✅ |
| `-[PlayerEntity dialHyperSpeed]` (4249) | `dialHyperSpeed` `Core/Entities/PlayerEntity.cpp:4570` | ✅ |
| `-[PlayerEntity dialForwardShield]` (4254) | `dialForwardShield` `Core/Entities/PlayerEntity.cpp:4575` | ✅ |
| `-[PlayerEntity dialAftShield]` (4263) | `dialAftShield` `Core/Entities/PlayerEntity.cpp:4587` | ✅ |
| `-[PlayerEntity dialEnergy]` (4272) | `dialEnergy` `Core/Entities/PlayerEntity.cpp:4596` | ✅ |
| `-[PlayerEntity dialMaxEnergy]` (4278) | `dialMaxEnergy` `Core/Entities/PlayerEntity.cpp:4602` | ✅ |
| `-[PlayerEntity dialFuel]` (4283) | `dialFuel` `Core/Entities/PlayerEntity.cpp:4607` | ✅ |
| `-[PlayerEntity dialHyperRange]` (4292) | `dialHyperRange` `Core/Entities/PlayerEntity.cpp:4616` | ✅ |
| `-[PlayerEntity laserHeatLevel]` (4299) | `laserHeatLevel` `Core/Entities/PlayerEntity.cpp:4623` | ✅ |
| `-[PlayerEntity laserHeatLevelAft]` (4305) | `laserHeatLevelAft` `Core/Entities/PlayerEntity.cpp:4629` | ✅ |
| `-[PlayerEntity laserHeatLevelForward]` (4311) | `laserHeatLevelForward` `Core/Entities/PlayerEntity.cpp:4635` | ✅ |
| `-[PlayerEntity laserHeatLevelPort]` (4318) | `laserHeatLevelPort` `Core/Entities/PlayerEntity.cpp:4642` | ✅ |
| `-[PlayerEntity laserHeatLevelStarboard]` (4324) | `laserHeatLevelStarboard` `Core/Entities/PlayerEntity.cpp:4648` | ✅ |
| `-[PlayerEntity dialAltitude]` (4330) | `dialAltitude` `Core/Entities/PlayerEntity.cpp:4654` | ✅ |
| `-[PlayerEntity clockTime]` (4349) | `clockTime` `Core/Entities/PlayerEntity.cpp:4673` | ✅ |
| `-[PlayerEntity clockTimeAdjusted]` (4354) | `clockTimeAdjusted` `Core/Entities/PlayerEntity.cpp:4678` | ✅ |
| `-[PlayerEntity clockAdjusting]` (4359) | `clockAdjusting` `Core/Entities/PlayerEntity.cpp:4683` | ✅ |
| `-[PlayerEntity addToAdjustTime:]` (4364) | `addToAdjustTime` `Core/Entities/PlayerEntity.cpp:4688` | ✅ |
| `-[PlayerEntity escapePodRescueTime]` (4369) | `escapePodRescueTime` `Core/Entities/PlayerEntity.cpp:4693` | ✅ |
| `-[PlayerEntity setEscapePodRescueTime:]` (4374) | `setEscapePodRescueTime` `Core/Entities/PlayerEntity.cpp:4698` | ✅ |
| `-[PlayerEntity dial_clock]` (4379) | `dial_clock` `Core/Entities/PlayerEntity.cpp:4703` | ✅ |
| `-[PlayerEntity dial_clock_adjusted]` (4384) | `dial_clock_adjusted` `Core/Entities/PlayerEntity.cpp:4708` | ✅ |
| `-[PlayerEntity dial_fpsinfo]` (4389) | `dial_fpsinfo` `Core/Entities/PlayerEntity.cpp:4713` | ✅ |
| `-[PlayerEntity dial_objinfo]` (4395) | `dial_objinfo` `Core/Entities/PlayerEntity.cpp:4719` | ✅ |
| `-[PlayerEntity countMissiles]` (4405) | `countMissiles` `Core/Entities/PlayerEntity.cpp:4729` | ✅ |
| `-[PlayerEntity dialMissileStatus]` (4416) | `dialMissileStatus` `Core/Entities/PlayerEntity.cpp:4740` | ✅ |
| `-[PlayerEntity canScoop:]` (4430) | `canScoop` `Core/Entities/PlayerEntity.cpp:4754` | ✅ |
| `-[PlayerEntity dialFuelScoopStatus]` (4437) | `dialFuelScoopStatus` `Core/Entities/PlayerEntity.cpp:4761` | ✅ |
| `-[PlayerEntity fuelLeakRate]` (4452) | `fuelLeakRate` `Core/Entities/PlayerEntity.cpp:4776` | ✅ |
| `-[PlayerEntity setFuelLeakRate:]` (4457) | `setFuelLeakRate` `Core/Entities/PlayerEntity.cpp:4781` | ✅ |
| `-[PlayerEntity commLog]` (4462) | `getCommLog` `Core/Entities/PlayerEntity.cpp:4786` | ✅ |
| `-[PlayerEntity roleWeights]` (4478) | `getRoleWeights` `Core/Entities/PlayerEntity.cpp:4798` | ✅ |
| `-[PlayerEntity addRoleForAggression:]` (4483) | `addRoleForAggression` `Core/Entities/PlayerEntity.cpp:4803` | ✅ |
| `-[PlayerEntity addRoleForMining]` (4510) | `addRoleForMining` `Core/Entities/PlayerEntity.cpp:4830` | ✅ |
| `-[PlayerEntity addRoleToPlayer:]` (4522) | `addRoleToPlayer` `Core/Entities/PlayerEntity.cpp:4842` | ✅ |
| `-[PlayerEntity addRoleToPlayer:inSlot:]` (4528) | `addRoleToPlayer` `Core/Entities/PlayerEntity.cpp:4848` | ✅ |
| `-[PlayerEntity clearRoleFromPlayer:]` (4540) | `clearRoleFromPlayer` `Core/Entities/PlayerEntity.cpp:4860` | ✅ |
| `-[PlayerEntity clearRolesFromPlayer:]` (4553) | `clearRolesFromPlayer` `Core/Entities/PlayerEntity.cpp:4873` | ✅ |
| `-[PlayerEntity maxPlayerRoles]` (4563) | `maxPlayerRoles` `Core/Entities/PlayerEntity.cpp:4883` | ✅ |
| `-[PlayerEntity updateSystemMemory]` (4574) | `updateSystemMemory` `Core/Entities/PlayerEntity.cpp:4894` | ✅ |
| `-[PlayerEntity compassTarget]` (4594) | `getCompassTargetEntity` `Core/Entities/PlayerEntity.cpp:4916` | ✅ |
| `-[PlayerEntity setCompassTarget:]` (4604) | `setCompassTargetEntity` `Core/Entities/PlayerEntity.cpp:4926` | ✅ |
| `-[PlayerEntity validateCompassTarget]` (4610) | `validateCompassTarget` `Core/Entities/PlayerEntity.cpp:4931` | ✅ |
| `-[PlayerEntity compassTargetLabel]` (4668) | `compassTargetLabel` `Core/Entities/PlayerEntity.cpp:4994` | ✅ |
| `-[PlayerEntity compassMode]` (4694) | `getCompassMode` `Core/Entities/PlayerEntity.cpp:5023` | ✅ |
| `-[PlayerEntity setCompassMode:]` (4699) | `setCompassMode` `Core/Entities/PlayerEntity.cpp:5028` | ✅ |
| `-[PlayerEntity setPrevCompassMode]` (4704) | `setPrevCompassMode` `Core/Entities/PlayerEntity.cpp:5033` | ✅ |
| `-[PlayerEntity setNextCompassMode]` (4761) | `setNextCompassMode` `Core/Entities/PlayerEntity.cpp:5090` | ✅ |
| `-[PlayerEntity activeMissile]` (4816) | `getActiveMissile` `Core/Entities/PlayerEntity.cpp:5145` | ✅ |
| `-[PlayerEntity setActiveMissile:]` (4821) | `setActiveMissile` `Core/Entities/PlayerEntity.cpp:5150` | ✅ |
| `-[PlayerEntity dialMaxMissiles]` (4826) | `dialMaxMissiles` `Core/Entities/PlayerEntity.cpp:5155` | ✅ |
| `-[PlayerEntity dialIdentEngaged]` (4831) | `dialIdentEngaged` `Core/Entities/PlayerEntity.cpp:5160` | ✅ |
| `-[PlayerEntity setDialIdentEngaged:]` (4836) | `setDialIdentEngaged` `Core/Entities/PlayerEntity.cpp:5165` | ✅ |
| `-[PlayerEntity specialCargo]` (4841) | `getSpecialCargo` `Core/Entities/PlayerEntity.cpp:5170` | ✅ |
| `-[PlayerEntity dialTargetName]` (4846) | `dialTargetName` `Core/Entities/PlayerEntity.cpp:5175` | ✅ |
| `-[PlayerEntity multiFunctionDisplayList]` (4865) | `multiFunctionDisplayList` `Core/Entities/PlayerEntity.cpp:5199` | ✅ |
| `-[PlayerEntity multiFunctionText:]` (4870) | `multiFunctionText` `Core/Entities/PlayerEntity.cpp:5204` | ✅ |
| `-[PlayerEntity setMultiFunctionText:forKey:]` (4880) | `setMultiFunctionText` `Core/Entities/PlayerEntity.cpp:5215` | ✅ |
| `-[PlayerEntity setMultiFunctionDisplay:toKey:]` (4894) | `setMultiFunctionDisplay` `Core/Entities/PlayerEntity.cpp:5227` | ✅ |
| `-[PlayerEntity cycleNextMultiFunctionDisplay:]` (4916) | `cycleNextMultiFunctionDisplay` `Core/Entities/PlayerEntity.cpp:5247` | ⚠️ |
| `-[PlayerEntity cyclePreviousMultiFunctionDisplay:]` (4946) | `cyclePreviousMultiFunctionDisplay` `Core/Entities/PlayerEntity.cpp:5281` | ⚠️ |
| `-[PlayerEntity selectNextMultiFunctionDisplay]` (4976) | `selectNextMultiFunctionDisplay` `Core/Entities/PlayerEntity.cpp:5315` | ✅ |
| `-[PlayerEntity selectPreviousMultiFunctionDisplay]` (4988) | `selectPreviousMultiFunctionDisplay` `Core/Entities/PlayerEntity.cpp:5328` | ✅ |
| `-[PlayerEntity activeMFD]` (5004) | `getActiveMFD` `Core/Entities/PlayerEntity.cpp:5345` | ✅ |
| `-[PlayerEntity missileForPylon:]` (5009) | `missileForPylon` `Core/Entities/PlayerEntity.cpp:5350` | ✅ |
| `-[PlayerEntity safeAllMissiles]` (5016) | `safeAllMissiles` `Core/Entities/PlayerEntity.cpp:5357` | ✅ |
| `-[PlayerEntity tidyMissilePylons]` (5028) | `tidyMissilePylons` `Core/Entities/PlayerEntity.cpp:5370` | ✅ |
| `-[PlayerEntity selectNextMissile]` (5050) | `selectNextMissile` `Core/Entities/PlayerEntity.cpp:5393` | ✅ |
| `-[PlayerEntity clearAlertFlags]` (5096) | `clearAlertFlags` `Core/Entities/PlayerEntity.cpp:5441` | ✅ |
| `-[PlayerEntity alertFlags]` (5101) | `getAlertFlags` `Core/Entities/PlayerEntity.cpp:5446` | ✅ |
| `-[PlayerEntity setAlertFlag:to:]` (5106) | `setAlertFlag` `Core/Entities/PlayerEntity.cpp:5451` | ✅ |
| `-[PlayerEntity realAlertCondition]` (5117) | `realAlertCondition` `Core/Entities/PlayerEntity.cpp:5462` | ✅ |
| `-[PlayerEntity alertCondition]` (5122) | `getAlertCondition` `Core/Entities/PlayerEntity.cpp:5470` | ✅ |
| `-[PlayerEntity fleeingStatus]` (5146) | `fleeingStatus` `Core/Entities/PlayerEntity.cpp:5494` | ✅ |
| `-[PlayerEntity interpretAIMessage:]` (5153) | `interpretAIMessage` `Core/Entities/PlayerEntity.cpp:5499` | ✅ |
| `-[PlayerEntity mountMissile:]` (5203) | `mountMissile` `Core/Entities/PlayerEntity.cpp:5554` | ✅ |
| `-[PlayerEntity mountMissileWithRole:]` (5223) | `mountMissileWithRole` `Core/Entities/PlayerEntity.cpp:5575` | ✅ |
| `-[PlayerEntity fireMissile]` (5230) | `fireMissile` `Core/Entities/PlayerEntity.cpp:5582` | ✅ |
| `-[PlayerEntity launchMine:]` (5282) | `launchMine` `Core/Entities/PlayerEntity.cpp:5634` | ✅ |
| `-[PlayerEntity assignToActivePylon:]` (5302) | `assignToActivePylon` `Core/Entities/PlayerEntity.cpp:5658` | ✅ |
| `-[PlayerEntity activateCloakingDevice]` (5342) | `activateCloakingDevice` `Core/Entities/PlayerEntity.cpp:5697` | ✅ |
| `-[PlayerEntity deactivateCloakingDevice]` (5359) | `deactivateCloakingDevice` `Core/Entities/PlayerEntity.cpp:5717` | ✅ |
| `-[PlayerEntity scannerFuzziness]` (5372) | `scannerFuzziness` `Core/Entities/PlayerEntity.cpp:5732` | ✅ |
| `-[PlayerEntity noticeECM]` (5388) | `noticeECM` `Core/Entities/PlayerEntity.cpp:5748` | ✅ |
| `-[PlayerEntity fireECM]` (5393) | `fireECM` `Core/Entities/PlayerEntity.cpp:5753` | ✅ |
| `-[PlayerEntity installedEnergyUnitType]` (5404) | `installedEnergyUnitType` `Core/Entities/PlayerEntity.cpp:5764` | ✅ |
| `-[PlayerEntity energyUnitType]` (5413) | `energyUnitType` `Core/Entities/PlayerEntity.cpp:5773` | ✅ |
| `-[PlayerEntity currentWeaponStats]` (5426) | `currentWeaponStats` `Core/Entities/PlayerEntity.cpp:5786` | ✅ |
| `-[PlayerEntity weaponsOnline]` (5435) | `weaponsOnline` `Core/Entities/PlayerEntity.cpp:5795` | ✅ |
| `-[PlayerEntity setWeaponsOnline:]` (5440) | `setWeaponsOnline` `Core/Entities/PlayerEntity.cpp:5800` | ✅ |
| `-[PlayerEntity currentLaserOffset]` (5447) | `currentLaserOffset` `Core/Entities/PlayerEntity.cpp:5807` | ✅ |
| `-[PlayerEntity fireMainWeapon]` (5452) | `fireMainWeapon` `Core/Entities/PlayerEntity.cpp:5812` | ❌ |
| `-[PlayerEntity weaponForFacing:]` (5529) | `weaponForFacing` `Core/Entities/PlayerEntity.cpp:5891` | ✅ |
| `-[PlayerEntity currentWeapon]` (5550) | `currentWeapon` `Core/Entities/PlayerEntity.cpp:5912` | ✅ |
| `-[PlayerEntity doesHitLine:v1:hitEntity:]` (5558) | `doesHitLine` `Core/Entities/PlayerEntity.cpp:5923` | ✅ |
| `-[PlayerEntity takeEnergyDamage:from:becauseOf:weaponIdentifier:]` (5599) | `takeEnergyDamage` `Core/Entities/PlayerEntity.cpp:5966` | ✅ |
| `-[PlayerEntity takeScrapeDamage:from:]` (5701) | `takeScrapeDamage` `Core/Entities/PlayerEntity.cpp:6073` | ✅ |
| `-[PlayerEntity takeHeatDamage:]` (5755) | `takeHeatDamage` `Core/Entities/PlayerEntity.cpp:6127` | ✅ |
| `-[PlayerEntity createDoppelganger]` (5785) | `createDoppelganger` `Core/Entities/PlayerEntity.cpp:6157` | ✅ |
| `-[PlayerEntity launchEscapeCapsule]` (5806) | `launchEscapeCapsule` `Core/Entities/PlayerEntity.cpp:6191` | ✅ |
| `-[PlayerEntity dumpCargo]` (5928) | `dumpCargo` `Core/Entities/PlayerEntity.cpp:6324` | ✅ |
| `-[PlayerEntity rotateCargo]` (5944) | `rotateCargo` `Core/Entities/PlayerEntity.cpp:6343` | ✅ |

**Differences:**

- ❌ **low** — Loading `role_weights` keeps only string elements (non-strings are dropped, shifting slots and shortening the list); upstream `mutableCopy` kept every element. Likewise a non-string `comm_log` entry is passed as nil rather than the object, and an int contract `type` with no legacy mapping becomes `""` where upstream raised on `setObject:nil`. Only hand-edited or corrupt saves are affected — upstream `Core/Entities/PlayerEntity.m:1425`, `:1473`, `:1266` ↔ port `Core/Entities/PlayerEntity.cpp:1836`, `:1880`, `:1675`
- ❌ **low** — `fireMainWeapon` overheat path uses `currentLaserOffset().at(0)`: with an empty weapon-offset list this throws `std::out_of_range`, where upstream `oo_vectorAtIndex:0` returned `kZeroVector` and carried on (overheat sound + message) — upstream `Core/Entities/PlayerEntity.m:5461` ↔ port `Core/Entities/PlayerEntity.cpp:5821`
- ⚠️ **low** — `cycleNext/PreviousMultiFunctionDisplay` cycle MFD keys in `std::map` byte order instead of `-[NSDictionary allKeys]` hash order, so the order of MFDs selected by the cycle key differs (the code comment says so) — upstream `Core/Entities/PlayerEntity.m:4920`, `:4950` ↔ port `Core/Entities/PlayerEntity.cpp:5251`, `:5285`
- ⚠️ **low** — `commanderDataDictionary` omits a key whose value is nil (`written_by_version`, `current_system_name`, `player_name`, `subentities_status`, `primed_equipment_a/b`, `trumbles`, …) where upstream `setObject:nil` raised mid-save (the code comment says so); a nil missile `primaryRole` is saved as `""`. Key spellings, number kinds (`chart_zoom` single, `credits` double, `fuel`/`entity_personality` unsigned, ints signed) and defaults all match. The checksum hashes the length of `oo::DescriptionOf(mission_variables/equipment)` for `NSDictionary -description`; nothing reads `checksum` back — upstream `Core/Entities/PlayerEntity.m:755`, `:834`, `:998` ↔ port `Core/Entities/PlayerEntity.cpp:1122`, `:1216`, `:1397`
- ⚠️ **low** — `setDialCustom` stores a null value as a null entry, where upstream `setObject:nil forKey:` raised (the code comment says so) — upstream `Core/Entities/PlayerEntity.m:4129` ↔ port `Core/Entities/PlayerEntity.cpp:4450`
- ⚠️ **low** — Debug `STAGE_TRACKING_END` logs `"***** Exception during [...]"` only for `OOException`; other C++ exceptions are rethrown without the log line, where upstream `@catch (NSException *)` logged every NSException (the code comment says so) — upstream `Core/Entities/PlayerEntity.m:2361` ↔ port `Core/Entities/PlayerEntity.cpp:345`
- ➖ — `-init` is `PlayerEntity()` + `newPlayerObject()`; `-dealloc` is `willDealloc()` (run first by the ship's dealloc) + member destructors; `kOOLogBuyMounted*` constants are inlined at their use sites (beyond line 5981).
- Subtle (✅): setting up with `customDialSettings`, `multiFunctionDisplay*`, `reputation` (all 9 keys at 0/0/`MAX_CONTRACT_REP`), `roleWeights` (8 × `player-unknown`), credits 1000, `PLAYER_MAX_FUEL`, galaxy coords (0x14,0xAD), `ship_trade_in_factor` 95, energy 256, temperature 60, shields `BASELINE_SHIELD_LEVEL` / recharge 2.0 all match. The ship clock start now comes from `oo::date::localUTCOffsetMinutes` (h/m/s of local time) rather than `NSCalendarDate` — upstream `Core/Entities/PlayerEntity.m:1850` ↔ port `Core/Entities/PlayerEntity.cpp:2198`
- Subtle (✅): `setCommanderDataFromDictionary` guards `% [_customViews count]` against an empty `custom_views` array (upstream divided by zero) and skips `removeObjectForKey:nil` for a passenger without a name (upstream raised) — upstream `Core/Entities/PlayerEntity.m:1569`, `:1333` ↔ port `Core/Entities/PlayerEntity.cpp:1978`, `:1734`
- Subtle (✅): `addRoleForAggression` / `addRoleForMining` store `role_weight_flags` counts as signed integers (upstream `numberWithUnsignedInteger:`). This makes no difference to the XML plist `<integer>` — upstream `Core/Entities/PlayerEntity.m:4503` ↔ port `Core/Entities/PlayerEntity.cpp:4823`
- Subtle (✅): `dumpCargo` returns `void` (upstream returned the commodity). No caller used the value; the hold-locked check, message and sound are unchanged — upstream `Core/Entities/PlayerEntity.m:5928` ↔ port `Core/Entities/PlayerEntity.cpp:6324`
- Subtle (✅): doBookkeeping (weapon cooling, cloak/jammer/ECM drain, shield/energy recharge with `shield_charge_energybank_threshold` 0.25, sun-skim fuel scoop `0.010/fuelChargeRate`, cabin heat, hyperspeed/injector fuel burn, fuel leak), alert flags/mass lock and the fleeing heuristic, witchspace countdown (`fdim`, gal-drive abort, `witch-to-x-in-y-seconds` with `seconds`/`destination`), launching/docking/dead updates and the escape-capsule sequence are line-for-line equal.

### PlayerEntity (part 2: lines 5982-end)

Upstream `src/Core/Entities/PlayerEntity.m` lines 5982–11911 ↔ `Core/Entities/PlayerEntity.cpp`

**Status:** done — 234 rows: 220 ✅ 9 ⚠️ 5 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity setBounty:]` (5982) | `setBounty(OOCreditsQuantity)` Core/Entities/PlayerEntity.cpp:6388 | ✅ |
| `-[PlayerEntity setBounty:withReason:]` (5987) | `setBounty(amount, OOLegalStatusReason)` Core/Entities/PlayerEntity.cpp:6393 | ✅ |
| `-[PlayerEntity setBounty:withReasonAsString:]` (5993) | `setBounty(amount, const std::string&)` Core/Entities/PlayerEntity.cpp:6398 | ✅ |
| `-[PlayerEntity bounty]` (6010) | `getBounty` Core/Entities/PlayerEntity.cpp:6418 | ✅ |
| `-[PlayerEntity legalStatus]` (6015) | `getLegalStatus` Core/Entities/PlayerEntity.cpp:6423 (virtual `legalStatus()` forwards, :516) | ✅ |
| `-[PlayerEntity markAsOffender:]` (6020) | `markAsOffender(int)` Core/Entities/PlayerEntity.cpp:6428 | ✅ |
| `-[PlayerEntity markAsOffender:withReason:]` (6025) | `markAsOffender(int, OOLegalStatusReason)` Core/Entities/PlayerEntity.cpp:6433 | ✅ |
| `-[PlayerEntity collectBountyFor:]` (6044) | `collectBountyFor` Core/Entities/PlayerEntity.cpp:6452 | ✅ |
| `-[PlayerEntity takeInternalDamage]` (6099) | `takeInternalDamage` Core/Entities/PlayerEntity.cpp:6508 | ✅ |
| `-[PlayerEntity getDestroyedBy:damageType:]` (6189) | `getDestroyedBy` Core/Entities/PlayerEntity.cpp:6598 | ✅ |
| `-[PlayerEntity loseTargetStatus]` (6227) | `loseTargetStatus` Core/Entities/PlayerEntity.cpp:6640 | ✅ |
| `-[PlayerEntity endScenario:]` (6251) | `endScenario` Core/Entities/PlayerEntity.cpp:6665 | ✅ |
| `-[PlayerEntity enterDock:]` (6260) | `enterDock` Core/Entities/PlayerEntity.cpp:6674 | ✅ |
| `-[PlayerEntity docked]` (6299) | `docked` Core/Entities/PlayerEntity.cpp:6719 | ✅ |
| `-[PlayerEntity leaveDock:]` (6394) | `leaveDock` Core/Entities/PlayerEntity.cpp:6818 | ✅ |
| `-[PlayerEntity witchStart]` (6490) | `witchStart` Core/Entities/PlayerEntity.cpp:6928 | ✅ |
| `-[PlayerEntity witchEnd]` (6533) | `witchEnd` Core/Entities/PlayerEntity.cpp:6975 | ✅ |
| `-[PlayerEntity witchJumpChecklist:]` (6546) | `witchJumpChecklist` Core/Entities/PlayerEntity.cpp:6992 | ✅ |
| `-[PlayerEntity setJumpType:]` (6617) | `setJumpType` Core/Entities/PlayerEntity.cpp:7071 | ✅ |
| `-[PlayerEntity hyperspaceJumpDistance]` (6626) | `hyperspaceJumpDistance` Core/Entities/PlayerEntity.cpp:7080 | ✅ |
| `-[PlayerEntity fuelRequiredForJump]` (6632) | `fuelRequiredForJump` Core/Entities/PlayerEntity.cpp:7086 | ✅ |
| `-[PlayerEntity hasSufficientFuelForJump]` (6637) | `hasSufficientFuelForJump` Core/Entities/PlayerEntity.cpp:7091 | ✅ |
| `-[PlayerEntity noteCompassLostTarget]` (6642) | `noteCompassLostTarget` Core/Entities/PlayerEntity.cpp:7096 | ✅ |
| `-[PlayerEntity enterGalacticWitchspace]` (6655) | `enterGalacticWitchspace` Core/Entities/PlayerEntity.cpp:7109 | ✅ |
| `-[PlayerEntity enterWormhole:]` (6747) | `enterWormhole` Core/Entities/PlayerEntity.cpp:7200 | ✅ |
| `-[PlayerEntity enterWitchspace]` (6774) | `enterWitchspace` Core/Entities/PlayerEntity.cpp:7230 | ✅ |
| `-[PlayerEntity witchJumpTo:misjump:]` (6863) | `witchJumpTo` Core/Entities/PlayerEntity.cpp:7328 | ✅ |
| `-[PlayerEntity leaveWitchspace]` (6915) | `leaveWitchspace` Core/Entities/PlayerEntity.cpp:7387 | ✅ |
| `-[PlayerEntity setGuiToStatusScreen]` (7006) | `setGuiToStatusScreen` Core/Entities/PlayerEntity.cpp:7485 | ✅ |
| `-[PlayerEntity equipmentList]` (7146) | `equipmentList` Core/Entities/PlayerEntity.cpp:7624 | ✅ |
| `-[PlayerEntity primedEquipmentCount]` (7245) | `primedEquipmentCount` Core/Entities/PlayerEntity.cpp:7726 | ✅ |
| `-[PlayerEntity primedEquipmentName:]` (7250) | `primedEquipmentName` Core/Entities/PlayerEntity.cpp:7731 | ✅ |
| `-[PlayerEntity currentPrimedEquipment]` (7261) | `currentPrimedEquipment` Core/Entities/PlayerEntity.cpp:7742 | ✅ |
| `-[PlayerEntity setPrimedEquipment:showMessage:]` (7271) | `setPrimedEquipment` Core/Entities/PlayerEntity.cpp:7752 | ✅ |
| `-[PlayerEntity activatePrimableEquipment:withMode:]` (7291) | `activatePrimableEquipment` Core/Entities/PlayerEntity.cpp:7780 | ✅ |
| `-[PlayerEntity fastEquipmentA]` (7311) | `fastEquipmentA` Core/Entities/PlayerEntity.cpp:7802 | ✅ |
| `-[PlayerEntity fastEquipmentB]` (7316) | `fastEquipmentB` Core/Entities/PlayerEntity.cpp:7807 | ✅ |
| `-[PlayerEntity setFastEquipmentA:]` (7321) | `setFastEquipmentA` Core/Entities/PlayerEntity.cpp:7812 | ✅ |
| `-[PlayerEntity setFastEquipmentB:]` (7327) | `setFastEquipmentB` Core/Entities/PlayerEntity.cpp:7817 | ✅ |
| `-[PlayerEntity weaponTypeForFacing:strict:]` (7333) | `weaponTypeForFacing` Core/Entities/PlayerEntity.cpp:7822 | ✅ |
| `-[PlayerEntity missilesList]` (7361) | `missilesList` Core/Entities/PlayerEntity.cpp:7853 | ✅ |
| `-[PlayerEntity cargoList]` (7367) | `cargoList` Core/Entities/PlayerEntity.cpp:7859 | ✅ |
| `-[PlayerEntity cargoListForScripting]` (7393) | `cargoListForScripting` Core/Entities/PlayerEntity.cpp:7890 | ⚠️ |
| `-[PlayerEntity legalStatusOfCargoList]` (7432) | `legalStatusOfCargoList` Core/Entities/PlayerEntity.cpp:7934 | ✅ |
| `-[PlayerEntity contractsListForScriptingFromArray:forCargo:]` (7445) | `contractsListForScriptingFromArray` Core/Entities/PlayerEntity.cpp:7946 | ✅ |
| `-[PlayerEntity passengerListForScripting]` (7483) | `passengerListForScripting` Core/Entities/PlayerEntity.cpp:7990 | ✅ |
| `-[PlayerEntity parcelListForScripting]` (7488) | `parcelListForScripting` Core/Entities/PlayerEntity.cpp:7995 | ✅ |
| `-[PlayerEntity contractListForScripting]` (7493) | `contractListForScripting` Core/Entities/PlayerEntity.cpp:8000 | ✅ |
| `-[PlayerEntity setGuiToSystemDataScreen]` (7498) | `setGuiToSystemDataScreen` Core/Entities/PlayerEntity.cpp:8005 | ✅ |
| `-[PlayerEntity setGuiToSystemDataScreenRefreshBackground:]` (7503) | `setGuiToSystemDataScreenRefreshBackground` Core/Entities/PlayerEntity.cpp:8010 | ✅ |
| `-[PlayerEntity prepareMarkedDestination:marker:]` (7699) | `PrepareMarkedDestination` (anonymous-namespace free function) Core/Entities/PlayerEntity.cpp:375 | ✅ |
| `-[PlayerEntity markedDestinations]` (7711) | `markedDestinations` Core/Entities/PlayerEntity.cpp:8186 | ⚠️ |
| `-[PlayerEntity setGuiToLongRangeChartScreen]` (7746) | `setGuiToLongRangeChartScreen` Core/Entities/PlayerEntity.cpp:8215 | ✅ |
| `-[PlayerEntity setGuiToShortRangeChartScreen]` (7758) | `setGuiToShortRangeChartScreen` Core/Entities/PlayerEntity.cpp:8227 | ✅ |
| `-[PlayerEntity setGuiToChartScreenFrom:]` (7769) | `setGuiToChartScreenFrom` Core/Entities/PlayerEntity.cpp:8238 | ✅ |
| `SliderString()` (7823) | `SliderString` Core/Entities/PlayerEntity.cpp:385 | ✅ |
| `-[PlayerEntity setGuiToGameOptionsScreen]` (7830) | `setGuiToGameOptionsScreen` Core/Entities/PlayerEntity.cpp:8305 | ✅ |
| `-[PlayerEntity setGuiToLoadSaveScreen]` (8061) | `setGuiToLoadSaveScreen` Core/Entities/PlayerEntity.cpp:8545 | ✅ |
| `static last_outfitting_key` (8155) | `last_outfitting_key` Core/Entities/PlayerEntity.cpp:396 | ✅ |
| `-[PlayerEntity highlightEquipShipScreenKey:]` (8157) | `highlightEquipShipScreenKey` Core/Entities/PlayerEntity.cpp:8643 | ✅ |
| `-[PlayerEntity availableFacings]` (8197) | `availableFacings` Core/Entities/PlayerEntity.cpp:8683 | ✅ |
| `-[PlayerEntity setGuiToEquipShipScreen:selectingFacingFor:]` (8206) | `setGuiToEquipShipScreen(int, const std::optional<std::string>&)` Core/Entities/PlayerEntity.cpp:8695 | ✅ |
| `-[PlayerEntity setGuiToEquipShipScreen:]` (8547) | `setGuiToEquipShipScreen(int)` Core/Entities/PlayerEntity.cpp:9046 | ✅ |
| `-[PlayerEntity showInformationForSelectedUpgrade]` (8552) | `showInformationForSelectedUpgrade` Core/Entities/PlayerEntity.cpp:9051 | ✅ |
| `-[PlayerEntity showInformationForSelectedUpgradeWithFormatString:]` (8557) | `showInformationForSelectedUpgradeWithFormatString` Core/Entities/PlayerEntity.cpp:9056 | ✅ |
| `-[PlayerEntity setGuiToInterfacesScreen:]` (8588) | `setGuiToInterfacesScreen` Core/Entities/PlayerEntity.cpp:9090 | ⚠️ |
| `-[PlayerEntity showInformationForSelectedInterface]` (8698) | `showInformationForSelectedInterface` Core/Entities/PlayerEntity.cpp:9219 | ✅ |
| `-[PlayerEntity activateSelectedInterface]` (8719) | `activateSelectedInterface` Core/Entities/PlayerEntity.cpp:9239 | ✅ |
| `-[PlayerEntity setupStartScreenGui]` (8747) | `setupStartScreenGui` Core/Entities/PlayerEntity.cpp:9267 | ✅ |
| `-[PlayerEntity setGuiToIntroFirstGo:]` (8819) | `setGuiToIntroFirstGo` Core/Entities/PlayerEntity.cpp:9340 | ✅ |
| `-[PlayerEntity setGuiToOXZManager]` (8922) | `setGuiToOXZManager` Core/Entities/PlayerEntity.cpp:9454 | ✅ |
| `-[PlayerEntity noteGUIWillChangeTo:]` (8940) | `noteGUIWillChangeTo` Core/Entities/PlayerEntity.cpp:9476 | ✅ |
| `-[PlayerEntity noteGUIDidChangeFrom:to:]` (8947) | `noteGUIDidChangeFrom(from, to)` Core/Entities/PlayerEntity.cpp:9483 | ✅ |
| `-[PlayerEntity noteGUIDidChangeFrom:to:refresh:]` (8952) | `noteGUIDidChangeFrom(from, to, refresh)` Core/Entities/PlayerEntity.cpp:9488 | ✅ |
| `-[PlayerEntity noteViewDidChangeFrom:toView:]` (8985) | `noteViewDidChangeFrom` Core/Entities/PlayerEntity.cpp:9522 | ✅ |
| `-[PlayerEntity buySelectedItem]` (8990) | `buySelectedItem` Core/Entities/PlayerEntity.cpp:9530 | ✅ |
| `-[PlayerEntity adjustPriceByScriptForEqKey:withCurrent:]` (9070) | `adjustPriceByScriptForEqKey` Core/Entities/PlayerEntity.cpp:9620 | ✅ |
| `-[PlayerEntity tryBuyingItem:]` (9104) | `tryBuyingItem` Core/Entities/PlayerEntity.cpp:9650 | ✅ |
| `-[PlayerEntity setWeaponMount:toWeapon:]` (9345) | `setWeaponMount(facing, eqKey)` Core/Entities/PlayerEntity.cpp:9892 | ✅ |
| `-[PlayerEntity setWeaponMount:toWeapon:inContext:]` (9350) | `setWeaponMount(facing, eqKey, context)` Core/Entities/PlayerEntity.cpp:9897 | ✅ |
| `-[PlayerEntity changePassengerBerths:]` (9395) | `changePassengerBerths` Core/Entities/PlayerEntity.cpp:9943 | ✅ |
| `-[PlayerEntity removeMissiles]` (9410) | `removeMissiles` Core/Entities/PlayerEntity.cpp:9958 | ✅ |
| `-[PlayerEntity doTradeIn:forPriceFactor:]` (9431) | `doTradeIn` Core/Entities/PlayerEntity.cpp:9978 | ✅ |
| `-[PlayerEntity cargoQuantityForType:]` (9440) | `cargoQuantityForType` Core/Entities/PlayerEntity.cpp:9990 | ✅ |
| `-[PlayerEntity setCargoQuantityForType:amount:]` (9461) | `setCargoQuantityForType` Core/Entities/PlayerEntity.cpp:10010 | ✅ |
| `-[PlayerEntity calculateCurrentCargo]` (9503) | `calculateCurrentCargo` Core/Entities/PlayerEntity.cpp:10053 | ✅ |
| `-[PlayerEntity cargoQuantityOnBoard]` (9508) | `cargoQuantityOnBoard` Core/Entities/PlayerEntity.cpp:10058 | ✅ |
| `-[PlayerEntity localMarket]` (9547) | `localMarket` Core/Entities/PlayerEntity.cpp:10096 | ✅ |
| `-[PlayerEntity applyMarketFilter:onMarket:]` (9569) | `applyMarketFilter` Core/Entities/PlayerEntity.cpp:10118 | ✅ |
| `-[PlayerEntity applyMarketSorter:onMarket:]` (9612) | `applyMarketSorter` Core/Entities/PlayerEntity.cpp:10161 | ✅ |
| `-[PlayerEntity showMarketScreenHeaders]` (9632) | `showMarketScreenHeaders` Core/Entities/PlayerEntity.cpp:10187 | ✅ |
| `-[PlayerEntity showMarketScreenDataLine:forGood:inMarket:holdQuantity:]` (9654) | `showMarketScreenDataLine` Core/Entities/PlayerEntity.cpp:10205 | ✅ |
| `-[PlayerEntity marketScreenTitle]` (9700) | `marketScreenTitle` Core/Entities/PlayerEntity.cpp:10254 | ✅ |
| `-[PlayerEntity setGuiToMarketScreen]` (9728) | `setGuiToMarketScreen` Core/Entities/PlayerEntity.cpp:10285 | ✅ |
| `-[PlayerEntity setGuiToMarketInfoScreen]` (9895) | `setGuiToMarketInfoScreen` Core/Entities/PlayerEntity.cpp:10455 | ❌ |
| `-[PlayerEntity showMarketCashAndLoadLine]` (9979) | `showMarketCashAndLoadLine` Core/Entities/PlayerEntity.cpp:10547 | ✅ |
| `-[PlayerEntity guiScreen]` (9988) | `guiScreen` Core/Entities/PlayerEntity.cpp:10556 | ✅ |
| `-[PlayerEntity tryBuyingCommodity:all:]` (9993) | `tryBuyingCommodity` Core/Entities/PlayerEntity.cpp:10561 | ✅ |
| `-[PlayerEntity trySellingCommodity:all:]` (10074) | `trySellingCommodity` Core/Entities/PlayerEntity.cpp:10646 | ✅ |
| `-[PlayerEntity isMining]` (10126) | `isMining` Core/Entities/PlayerEntity.cpp:10702 | ✅ |
| `-[PlayerEntity isSpeechOn]` (10131) | `getIsSpeechOn` Core/Entities/PlayerEntity.cpp:10707 | ✅ |
| `-[PlayerEntity canAddEquipment:inContext:]` (10136) | `canAddEquipment` Core/Entities/PlayerEntity.cpp:10712 | ✅ |
| `-[PlayerEntity addEquipmentItem:inContext:]` (10150) | `addEquipmentItem` Core/Entities/PlayerEntity.cpp:10727 | ✅ |
| `-[PlayerEntity addEquipmentItem:withValidation:inContext:]` (10155) | `addEquipmentItem` Core/Entities/PlayerEntity.cpp:10735 | ✅ |
| `-[PlayerEntity customEquipmentActivation]` (10186) | `customEquipmentActivation` Core/Entities/PlayerEntity.cpp:10767 | ✅ |
| `-[PlayerEntity addEquipmentWithScriptToCustomKeyArray:]` (10191) | `addEquipmentWithScriptToCustomKeyArray` Core/Entities/PlayerEntity.cpp:10772 | ✅ |
| `-[PlayerEntity validateCustomEquipActivationArray]` (10233) | `validateCustomEquipActivationArray` Core/Entities/PlayerEntity.cpp:10814 | ✅ |
| `-[PlayerEntity removeEquipmentItem:]` (10256) | `removeEquipmentItem` Core/Entities/PlayerEntity.cpp:10836 | ✅ |
| `-[PlayerEntity addEquipmentFromCollection:]` (10268) | `addEquipmentFromCollection` Core/Entities/PlayerEntity.cpp:10849 | ⚠️ |
| `-[PlayerEntity hasOneEquipmentItem:includeMissiles:]` (10335) | `hasOneEquipmentItem` Core/Entities/PlayerEntity.cpp:10913 | ✅ |
| `-[PlayerEntity hasPrimaryWeapon:]` (10357) | `hasPrimaryWeapon` Core/Entities/PlayerEntity.cpp:10935 | ✅ |
| `-[PlayerEntity removeExternalStore:]` (10369) | `removeExternalStore` Core/Entities/PlayerEntity.cpp:10946 | ✅ |
| `-[PlayerEntity removeFromPylon:]` (10386) | `removeFromPylon` Core/Entities/PlayerEntity.cpp:10963 | ✅ |
| `-[PlayerEntity parcelCount]` (10419) | `parcelCount` Core/Entities/PlayerEntity.cpp:10995 | ✅ |
| `-[PlayerEntity passengerCount]` (10424) | `passengerCount` Core/Entities/PlayerEntity.cpp:11000 | ✅ |
| `-[PlayerEntity passengerCapacity]` (10429) | `passengerCapacity` Core/Entities/PlayerEntity.cpp:11005 | ✅ |
| `-[PlayerEntity hasHostileTarget]` (10434) | `hasHostileTarget` Core/Entities/PlayerEntity.cpp:11010 | ✅ |
| `-[PlayerEntity receiveCommsMessage:from:]` (10440) | `receiveCommsMessage` Core/Entities/PlayerEntity.cpp:11016 | ✅ |
| `-[PlayerEntity getFined]` (10450) | `getFined` Core/Entities/PlayerEntity.cpp:11027 | ✅ |
| `-[PlayerEntity adjustTradeInFactorBy:]` (10476) | `adjustTradeInFactorBy` Core/Entities/PlayerEntity.cpp:11054 | ✅ |
| `-[PlayerEntity tradeInFactor]` (10485) | `tradeInFactor` Core/Entities/PlayerEntity.cpp:11063 | ✅ |
| `-[PlayerEntity renovationCosts]` (10490) | `renovationCosts` Core/Entities/PlayerEntity.cpp:11068 | ✅ |
| `-[PlayerEntity renovationFactor]` (10501) | `renovationFactor` Core/Entities/PlayerEntity.cpp:11079 | ✅ |
| `-[PlayerEntity setDefaultViewOffsets]` (10508) | `setDefaultViewOffsets` Core/Entities/PlayerEntity.cpp:11086 | ✅ |
| `-[PlayerEntity setDefaultCustomViews]` (10520) | `setDefaultCustomViews` Core/Entities/PlayerEntity.cpp:11098 | ✅ |
| `-[PlayerEntity weaponViewOffset]` (10532) | `weaponViewOffset` Core/Entities/PlayerEntity.cpp:11110 | ✅ |
| `-[PlayerEntity setUpTrumbles]` (10551) | `setUpTrumbles` Core/Entities/PlayerEntity.cpp:11129 | ✅ |
| `-[PlayerEntity addTrumble:]` (10580) | `addTrumble` Core/Entities/PlayerEntity.cpp:11157 | ✅ |
| `-[PlayerEntity removeTrumble:]` (10590) | `removeTrumble` Core/Entities/PlayerEntity.cpp:11166 | ❌ |
| `-[PlayerEntity trumbleArray]` (10611) | `trumbleArray` Core/Entities/PlayerEntity.cpp:11186 | ✅ |
| `-[PlayerEntity trumbleCount]` (10616) | `getTrumbleCount` Core/Entities/PlayerEntity.cpp:11191 | ✅ |
| `-[PlayerEntity trumbleValue]` (10621) | `trumbleValue` Core/Entities/PlayerEntity.cpp:11199 | ✅ |
| `-[PlayerEntity setTrumbleValueFrom:]` (10643) | `setTrumbleValueFrom` Core/Entities/PlayerEntity.cpp:11223 | ✅ |
| `-[PlayerEntity trumbleAppetiteAccumulator]` (10741) | `trumbleAppetiteAccumulator` Core/Entities/PlayerEntity.cpp:11321 | ✅ |
| `-[PlayerEntity setTrumbleAppetiteAccumulator:]` (10746) | `setTrumbleAppetiteAccumulator` Core/Entities/PlayerEntity.cpp:11326 | ✅ |
| `-[PlayerEntity mungChecksumWithNSString:]` (10751) | `mungChecksumWithString` Core/Entities/PlayerEntity.cpp:11331 | ✅ |
| `-[PlayerEntity screenModeStringForWidth:height:refreshRate:]` (10762) | `screenModeStringForWidth` Core/Entities/PlayerEntity.cpp:11342 | ✅ |
| `-[PlayerEntity suppressTargetLost]` (10771) | `getSuppressTargetLost` Core/Entities/PlayerEntity.cpp:11351 | ✅ |
| `-[PlayerEntity setScoopsActive]` (10776) | `setScoopsActive` Core/Entities/PlayerEntity.cpp:11356 | ✅ |
| `-[PlayerEntity setFoundTarget:]` (10782) | `setFoundTarget` Core/Entities/PlayerEntity.cpp:11362 | ✅ |
| `-[PlayerEntity addTarget:]` (10794) | `addTarget` Core/Entities/PlayerEntity.cpp:11373 | ✅ |
| `-[PlayerEntity clearTargetMemory]` (10847) | `clearTargetMemory` Core/Entities/PlayerEntity.cpp:11432 | ✅ |
| `-[PlayerEntity targetMemory]` (10860) | `targetMemory` Core/Entities/PlayerEntity.cpp:11445 | ✅ |
| `-[PlayerEntity moveTargetMemoryBy:]` (10865) | `moveTargetMemoryBy` Core/Entities/PlayerEntity.cpp:11450 | ✅ |
| `-[PlayerEntity printIdentLockedOnForMissile:]` (10911) | `printIdentLockedOnForMissile` Core/Entities/PlayerEntity.cpp:11498 | ❌ |
| `-[PlayerEntity customViewQuaternion]` (10921) | `getCustomViewQuaternion` Core/Entities/PlayerEntity.cpp:11509 | ✅ |
| `-[PlayerEntity setCustomViewQuaternion:]` (10926) | `setCustomViewQuaternion` Core/Entities/PlayerEntity.cpp:11514 | ✅ |
| `-[PlayerEntity customViewMatrix]` (10932) | `getCustomViewMatrix` Core/Entities/PlayerEntity.cpp:11520 | ✅ |
| `-[PlayerEntity customViewOffset]` (10937) | `getCustomViewOffset` Core/Entities/PlayerEntity.cpp:11525 | ✅ |
| `-[PlayerEntity setCustomViewOffset:]` (10942) | `setCustomViewOffset` Core/Entities/PlayerEntity.cpp:11530 | ✅ |
| `-[PlayerEntity customViewRotationCenter]` (10947) | `getCustomViewRotationCenter` Core/Entities/PlayerEntity.cpp:11535 | ✅ |
| `-[PlayerEntity setCustomViewRotationCenter:]` (10952) | `setCustomViewRotationCenter` Core/Entities/PlayerEntity.cpp:11540 | ✅ |
| `-[PlayerEntity customViewZoomIn:]` (10957) | `customViewZoomIn` Core/Entities/PlayerEntity.cpp:11545 | ✅ |
| `-[PlayerEntity customViewZoomOut:]` (10968) | `customViewZoomOut` Core/Entities/PlayerEntity.cpp:11556 | ✅ |
| `-[PlayerEntity customViewRotateLeft:]` (10979) | `customViewRotateLeft` Core/Entities/PlayerEntity.cpp:11567 | ✅ |
| `-[PlayerEntity customViewRotateRight:]` (10990) | `customViewRotateRight` Core/Entities/PlayerEntity.cpp:11578 | ✅ |
| `-[PlayerEntity customViewRotateUp:]` (11001) | `customViewRotateUp` Core/Entities/PlayerEntity.cpp:11589 | ✅ |
| `-[PlayerEntity customViewRotateDown:]` (11012) | `customViewRotateDown` Core/Entities/PlayerEntity.cpp:11600 | ✅ |
| `-[PlayerEntity customViewRollRight:]` (11023) | `customViewRollRight` Core/Entities/PlayerEntity.cpp:11611 | ✅ |
| `-[PlayerEntity customViewRollLeft:]` (11034) | `customViewRollLeft` Core/Entities/PlayerEntity.cpp:11622 | ✅ |
| `-[PlayerEntity customViewPanUp:]` (11045) | `customViewPanUp` Core/Entities/PlayerEntity.cpp:11633 | ✅ |
| `-[PlayerEntity customViewPanDown:]` (11052) | `customViewPanDown` Core/Entities/PlayerEntity.cpp:11640 | ✅ |
| `-[PlayerEntity customViewPanLeft:]` (11059) | `customViewPanLeft` Core/Entities/PlayerEntity.cpp:11650 | ✅ |
| `-[PlayerEntity customViewPanRight:]` (11066) | `customViewPanRight` Core/Entities/PlayerEntity.cpp:11657 | ✅ |
| `-[PlayerEntity customViewForwardVector]` (11073) | `getCustomViewForwardVector` Core/Entities/PlayerEntity.cpp:11664 | ✅ |
| `-[PlayerEntity customViewUpVector]` (11078) | `getCustomViewUpVector` Core/Entities/PlayerEntity.cpp:11669 | ✅ |
| `-[PlayerEntity customViewRightVector]` (11083) | `getCustomViewRightVector` Core/Entities/PlayerEntity.cpp:11674 | ✅ |
| `-[PlayerEntity customViewDescription]` (11088) | `getCustomViewDescription` Core/Entities/PlayerEntity.cpp:11679 | ✅ |
| `-[PlayerEntity resetCustomView]` (11093) | `resetCustomView` Core/Entities/PlayerEntity.cpp:11684 | ✅ |
| `-[PlayerEntity setCustomViewData]` (11098) | `setCustomViewData` Core/Entities/PlayerEntity.cpp:11690 | ✅ |
| `-[PlayerEntity setCustomViewDataFromDictionary:withScaling:]` (11109) | `setCustomViewDataFromDictionary` Core/Entities/PlayerEntity.cpp:11701 | ✅ |
| `-[PlayerEntity showInfoFlag]` (11143) | `showInfoFlag` Core/Entities/PlayerEntity.cpp:11736 | ✅ |
| `-[PlayerEntity missionOverlayDescriptor]` (11148) | `missionOverlayDescriptor` Core/Entities/PlayerEntity.cpp:11741 | ✅ |
| `-[PlayerEntity missionOverlayDescriptorOrDefault]` (11153) | `missionOverlayDescriptorOrDefault` Core/Entities/PlayerEntity.cpp:11746 | ✅ |
| `-[PlayerEntity setMissionOverlayDescriptor:]` (11167) | `setMissionOverlayDescriptor` Core/Entities/PlayerEntity.cpp:11760 | ✅ |
| `-[PlayerEntity missionBackgroundDescriptor]` (11175) | `missionBackgroundDescriptor` Core/Entities/PlayerEntity.cpp:11765 | ✅ |
| `-[PlayerEntity missionBackgroundDescriptorOrDefault]` (11180) | `missionBackgroundDescriptorOrDefault` Core/Entities/PlayerEntity.cpp:11770 | ✅ |
| `-[PlayerEntity setMissionBackgroundDescriptor:]` (11190) | `setMissionBackgroundDescriptor` Core/Entities/PlayerEntity.cpp:11780 | ✅ |
| `-[PlayerEntity missionBackgroundSpecial]` (11198) | `missionBackgroundSpecial` Core/Entities/PlayerEntity.cpp:11785 | ✅ |
| `-[PlayerEntity setMissionBackgroundSpecial:]` (11203) | `setMissionBackgroundSpecial` Core/Entities/PlayerEntity.cpp:11790 | ✅ |
| `-[PlayerEntity setMissionExitScreen:]` (11254) | `setMissionExitScreen` Core/Entities/PlayerEntity.cpp:11841 | ✅ |
| `-[PlayerEntity missionExitScreen]` (11259) | `missionExitScreen` Core/Entities/PlayerEntity.cpp:11846 | ✅ |
| `-[PlayerEntity equipScreenBackgroundDescriptor]` (11264) | `equipScreenBackgroundDescriptor` Core/Entities/PlayerEntity.cpp:11851 | ✅ |
| `-[PlayerEntity setEquipScreenBackgroundDescriptor:]` (11269) | `setEquipScreenBackgroundDescriptor` Core/Entities/PlayerEntity.cpp:11856 | ✅ |
| `-[PlayerEntity scriptsLoaded]` (11277) | `scriptsLoaded` Core/Entities/PlayerEntity.cpp:11861 | ✅ |
| `-[PlayerEntity worldScriptNames]` (11282) | `worldScriptNames` Core/Entities/PlayerEntity.cpp:11866 | ⚠️ |
| `-[PlayerEntity worldScriptsByName]` (11287) | `worldScriptsByName` Core/Entities/PlayerEntity.cpp:11875 | ⚠️ |
| `-[PlayerEntity commodityScriptNamed:]` (11292) | `commodityScriptNamed` Core/Entities/PlayerEntity.cpp:11880 | ✅ |
| `-[PlayerEntity doScriptEvent:inContext:withArguments:count:]` (11311) | `doScriptEvent` Core/Entities/PlayerEntity.cpp:11904 | ✅ |
| `-[PlayerEntity doWorldEventUntilMissionScreen:]` (11317) | `doWorldEventUntilMissionScreen` Core/Entities/PlayerEntity.cpp:11911 | ⚠️ |
| `-[PlayerEntity doWorldScriptEvent:inContext:withArguments:count:timeLimit:]` (11344) | `doWorldScriptEvent` Core/Entities/PlayerEntity.cpp:11940 | ⚠️ |
| `-[PlayerEntity setGalacticHyperspaceBehaviour:]` (11357) | `setGalacticHyperspaceBehaviour` Core/Entities/PlayerEntity.cpp:11954 | ✅ |
| `-[PlayerEntity galacticHyperspaceBehaviour]` (11364) | `getGalacticHyperspaceBehaviour` Core/Entities/PlayerEntity.cpp:11961 | ✅ |
| `-[PlayerEntity setGalacticHyperspaceFixedCoords:]` (11369) | `setGalacticHyperspaceFixedCoords` Core/Entities/PlayerEntity.cpp:11966 | ✅ |
| `-[PlayerEntity setGalacticHyperspaceFixedCoordsX:y:]` (11374) | `setGalacticHyperspaceFixedCoordsX` Core/Entities/PlayerEntity.cpp:11971 | ✅ |
| `-[PlayerEntity galacticHyperspaceFixedCoords]` (11380) | `getGalacticHyperspaceFixedCoords` Core/Entities/PlayerEntity.cpp:11977 | ✅ |
| `-[PlayerEntity setWitchspaceCountdown:]` (11385) | `setWitchspaceCountdown` Core/Entities/PlayerEntity.cpp:11982 | ✅ |
| `-[PlayerEntity longRangeChartMode]` (11390) | `getLongRangeChartMode` Core/Entities/PlayerEntity.cpp:11987 | ✅ |
| `-[PlayerEntity setLongRangeChartMode:]` (11395) | `setLongRangeChartMode` Core/Entities/PlayerEntity.cpp:11992 | ✅ |
| `-[PlayerEntity scoopOverride]` (11400) | `getScoopOverride` Core/Entities/PlayerEntity.cpp:11997 | ✅ |
| `-[PlayerEntity setScoopOverride:]` (11405) | `setScoopOverride` Core/Entities/PlayerEntity.cpp:12002 | ✅ |
| `-[PlayerEntity fuelChargeRate]` (11413) | `fuelChargeRate` Core/Entities/PlayerEntity.cpp:12009 | ✅ |
| `-[PlayerEntity setDockTarget:]` (11429) | `setDockTarget` Core/Entities/PlayerEntity.cpp:12029 | ✅ |
| `-[PlayerEntity jumpCause]` (11438) | `jumpCause` Core/Entities/PlayerEntity.cpp:12038 | ✅ |
| `-[PlayerEntity setJumpCause:]` (11443) | `setJumpCause` Core/Entities/PlayerEntity.cpp:12043 | ✅ |
| `-[PlayerEntity commanderName]` (11450) | `commanderName` Core/Entities/PlayerEntity.cpp:12049 | ✅ |
| `-[PlayerEntity lastsaveName]` (11455) | `lastsaveName` Core/Entities/PlayerEntity.cpp:12054 | ✅ |
| `-[PlayerEntity setCommanderName:]` (11460) | `setCommanderName` Core/Entities/PlayerEntity.cpp:12059 | ✅ |
| `-[PlayerEntity setLastsaveName:]` (11467) | `setLastsaveName` Core/Entities/PlayerEntity.cpp:12065 | ✅ |
| `-[PlayerEntity isDocked]` (11475) | `isDocked` Core/Entities/PlayerEntity.cpp:12075 | ✅ |
| `-[PlayerEntity clearedToDock]` (11529) | `clearedToDock` Core/Entities/PlayerEntity.cpp:12129 | ✅ |
| `-[PlayerEntity setDockingClearanceStatus:]` (11534) | `setDockingClearanceStatus` Core/Entities/PlayerEntity.cpp:12134 | ✅ |
| `-[PlayerEntity getDockingClearanceStatus]` (11550) | `getDockingClearanceStatus` Core/Entities/PlayerEntity.cpp:12150 | ✅ |
| `-[PlayerEntity penaltyForUnauthorizedDocking]` (11555) | `penaltyForUnauthorizedDocking` Core/Entities/PlayerEntity.cpp:12155 | ✅ |
| `-[PlayerEntity addScannedWormhole:]` (11572) | `addScannedWormhole` Core/Entities/PlayerEntity.cpp:12172 | ✅ |
| `-[PlayerEntity updateWormholes]` (11590) | `updateWormholes` Core/Entities/PlayerEntity.cpp:12189 | ✅ |
| `-[PlayerEntity scannedWormholes]` (11620) | `getScannedWormholes` Core/Entities/PlayerEntity.cpp:12218 | ✅ |
| `-[PlayerEntity initialiseMissionDestinations:andLegacy:]` (11625) | `initialiseMissionDestinations` Core/Entities/PlayerEntity.cpp:12223 | ✅ |
| `-[PlayerEntity markerKey:]` (11657) | `markerKey` Core/Entities/PlayerEntity.cpp:12245 | ✅ |
| `-[PlayerEntity addMissionDestinationMarker:]` (11662) | `addMissionDestinationMarker` Core/Entities/PlayerEntity.cpp:12253 | ✅ |
| `-[PlayerEntity removeMissionDestinationMarker:]` (11672) | `removeMissionDestinationMarker` Core/Entities/PlayerEntity.cpp:12263 | ✅ |
| `-[PlayerEntity getMissionDestinations]` (11686) | `getMissionDestinations` Core/Entities/PlayerEntity.cpp:12273 | ❌ |
| `-[PlayerEntity shipyardRecord]` (11691) | `shipyardRecord` Core/Entities/PlayerEntity.cpp:12278 | ✅ |
| `-[PlayerEntity setLastShot:]` (11696) | `setLastShot` Core/Entities/PlayerEntity.cpp:12283 | ✅ |
| `-[PlayerEntity clearExtraMissionKeys]` (11701) | `clearExtraMissionKeys` Core/Entities/PlayerEntity.cpp:12288 | ✅ |
| `-[PlayerEntity setExtraMissionKeys:]` (11707) | `setExtraMissionKeys` Core/Entities/PlayerEntity.cpp:12293 | ✅ |
| `-[PlayerEntity clearExtraGuiScreenKeys:key:]` (11718) | `clearExtraGuiScreenKeys` Core/Entities/PlayerEntity.cpp:12304 | ❌ |
| `-[PlayerEntity setExtraGuiScreenKeys:definition:]` (11733) | `setExtraGuiScreenKeys` Core/Entities/PlayerEntity.cpp:12325 | ⚠️ |
| `-[PlayerEntity dumpSelfState]` (11794) | `dumpSelfState` Core/Entities/PlayerEntity.cpp:12381 | ✅ |
| `-[PlayerEntity suppressClangStuff]` (11856) | `suppressClangStuff` Core/Entities/PlayerEntity.cpp:12458 | ✅ |
| `marketSorterByName()` (11868) | `marketSorterByName` Core/Entities/PlayerEntity.cpp:209 | ✅ |
| `marketSorterByPrice()` (11874) | `marketSorterByPrice` Core/Entities/PlayerEntity.cpp:217 | ✅ |
| `marketSorterByQuantity()` (11887) | `marketSorterByQuantity` Core/Entities/PlayerEntity.cpp:223 | ✅ |
| `marketSorterByMassUnit()` (11900) | `marketSorterByMassUnit` Core/Entities/PlayerEntity.cpp:229 | ✅ |

**Differences:**

- ❌ **med** — `printIdentLockedOnForMissile` resolves the target name via `oo::ToShip(primaryTarget())`; upstream sent `-identFromShip:` to any target. `WormholeEntity` is not a `ShipEntity` but has its own `identFromShip` (`Core/Entities/WormholeEntity.cpp:592`), so with ident engaged on a wormhole the "ident-locked-onto-target" message shows an empty name instead of the wormhole description. — upstream `Core/Entities/PlayerEntity.m:10917` ↔ port `Core/Entities/PlayerEntity.cpp:11504`
- ❌ **low** — `setGuiToMarketInfoScreen` adds pod contents at `quantityInHold[j]` with no `NSNotFound` guard (the market screen has one). With a filter hiding a carried good, `j` is out of range in both trees; upstream scribbled below a stack VLA, the port writes before a `std::vector` heap buffer (heap corruption/crash risk). — upstream `Core/Entities/PlayerEntity.m:9922` ↔ port `Core/Entities/PlayerEntity.cpp:10485`
- ❌ **low** — `getMissionDestinations` now returns a `std::map` ordered by the `"%d-name"` key, not NSMutableDictionary order, so `mission.markedSystems` lists markers in a different order; no comment/ADR marks it intentional. — upstream `Core/Entities/PlayerEntity.m:11686` (`Core/Scripting/OOJSMission.m:191`) ↔ port `Core/Entities/PlayerEntity.cpp:12273` (`Core/Entities/PlayerEntity.h:1463`)
- ❌ **low** — `clearExtraGuiScreenKeys`: upstream sent `-oo_stringForKey:` to an `OOJSGuiScreenKeyDefinition` (not a dictionary), raising an unrecognised-selector exception reported as a JS error whenever definitions exist; the port silently never matches. Nothing is removed in either tree; only the upstream error report disappears. — upstream `Core/Entities/PlayerEntity.m:11725` ↔ port `Core/Entities/PlayerEntity.cpp:12316`
- ❌ **low** — `removeTrumble` "trumble.zombie" log prints a bare pointer where upstream's `%@` printed `<OOTrumble: 0x…>`; log class unchanged, text differs. — upstream `Core/Entities/PlayerEntity.m:10603` ↔ port `Core/Entities/PlayerEntity.cpp:11179`
- ⚠️ **low** — `cargoListForScripting` skips pods whose commodity is nil/unknown (upstream indexed with `NSNotFound`, undefined behaviour) and omits `displayName` where upstream raised on nil; port comment marks it deliberate. — upstream `Core/Entities/PlayerEntity.m:7409` ↔ port `Core/Entities/PlayerEntity.cpp:7906`
- ⚠️ **low** — `markedDestinations` adds a system's markers in `std::map` key byte order, not NSDictionary order (comment "ORDER-SENSITIVE"); draw order of several markers on one system can change. — upstream `Core/Entities/PlayerEntity.m:7737` ↔ port `Core/Entities/PlayerEntity.cpp:8206`
- ⚠️ **low** — `setGuiToInterfacesScreen` uses `std::stable_sort` over `interfaceCompare`; ties (same category and title) keep map byte order instead of NSDictionary key order (comment "ORDER-SENSITIVE"). — upstream `Core/Entities/PlayerEntity.m:8597` ↔ port `Core/Entities/PlayerEntity.cpp:9104`
- ⚠️ **low** — `addEquipmentFromCollection` walks a dictionary in key byte order (comment "key byte order (was NSDictionary order)"), which fixes `eqScripts`/`customEquipActivation` order and so priming order after load; NSSet input gone, non-string array elements skipped. — upstream `Core/Entities/PlayerEntity.m:10278` ↔ port `Core/Entities/PlayerEntity.cpp:10859`
- ⚠️ **low** — `worldScriptNames`/`worldScriptsByName` return world scripts in load order rather than `-allKeys` order ("ORDER-SENSITIVE (decision D)"). — upstream `Core/Entities/PlayerEntity.m:11284,11289` ↔ port `Core/Entities/PlayerEntity.cpp:11866,11875`
- ⚠️ **low** — `doWorldEventUntilMissionScreen`/`doWorldScriptEvent` call world scripts in load order over a snapshot ("ORDER-SENSITIVE (decision D)"); which script runs first, and so which one raises a mission screen first, can differ. — upstream `Core/Entities/PlayerEntity.m:11330,11350` ↔ port `Core/Entities/PlayerEntity.cpp:11910,11944`
- ⚠️ **low** — `setExtraGuiScreenKeys` checks existing definitions in byte order; only the order of "key already in use" log lines can change (comment). — upstream `Core/Entities/PlayerEntity.m:11773` ↔ port `Core/Entities/PlayerEntity.cpp:12359`
- ❌ **low** — (helper, no row) `adjustPriceByScriptForEqKey` relies on `ooscript::valueToInt32`, whose range check is `d > 2147483647 || d < -2147483648`; SpiderMonkey 1.8.5 `JS_ValueToInt32` accepted up to `< 2147483648`. A script price in (2147483647, 2147483648) now errors and is ignored instead of rounding. — upstream `Core/Entities/PlayerEntity.m:9093` ↔ port `Core/Scripting/ooscript/JSEngine_quickjs.cpp:2445`
- ➖ **low** — `setGuiToGameOptionsScreen` drops the `#if !NEW_PLANETS` "procedurally textured planets" row and its enum entry; upstream compiles with `NEW_PLANETS 1` (`OOStellarBody.h:35`), so the code was dead and row numbers match. — upstream `Core/Entities/PlayerEntity.m:8019` ↔ port `Core/Entities/PlayerEntity.cpp:8508`
- ✅ **low** — Nil arguments to `OOExpandKey` (which raised upstream) become `""` in the port, with comments (`witch-blocked`, `status-hyperspace-system-multi`, `equipment-plural*`, `equipment-primed`, `manifest-cargo-quantity*`); an exception path becomes text with an empty field. — upstream `Core/Entities/PlayerEntity.m:6556,7043,7177,7193,7285,7384` ↔ port `Core/Entities/PlayerEntity.cpp:7002,7524,7655,7672,7771,7877`
- ✅ **low** — `contractsListForScriptingFromArray` omits a key whose value is nil where upstream threw in `setObject:forKey:` (port comment :7951). — upstream `Core/Entities/PlayerEntity.m:7455-7474` ↔ port `Core/Entities/PlayerEntity.cpp:7952-7981`
- ✅ **low** — Status screen and `equipmentList` dereference `gui` unguarded where upstream nil-messaging made a no-op; unreachable (gui always exists). `currentPrimedEquipment` returns `""` rather than nil when the index exceeds the count (extra guard). — upstream `Core/Entities/PlayerEntity.m:7058,7156,7265` ↔ port `Core/Entities/PlayerEntity.cpp:7540,7632,7746`
- ✅ **low** — Status screen verified line-for-line: rows 0–7 and 9 with the same localisation keys, tab stops 20/160/290, colours, `"%.1f %s"` fuel, "(null)" fallbacks matching `%@` of nil, two-column rows truncated at the first nil (`RowOf`, :270), equipment order = reverse `sEquipmentTypes`, damaged first when `prioritiseDamaged`, then berths, then fwd/aft/port/stb weapons. — upstream `Core/Entities/PlayerEntity.m:7006` ↔ port `Core/Entities/PlayerEntity.cpp:7485`
- ✅ **low** — System data screen verified: tabs 0/96/144 (`kGuiSystemdataTabs`), 16 `sysdata-line-N` lines with the same 8 args split on `\t` (empty third column dropped), `%.1f ly` with 0.1 floor, ANA route string with the 0.95/1.05 plural rule, long text at row 17 (15 without data), NONAME/NODATA concealment, nova zeroing with `nova-system-*` keys, RNG save/restore around the background. — upstream `Core/Entities/PlayerEntity.m:7503` ↔ port `Core/Entities/PlayerEntity.cpp:8010`
- ✅ **low** — Bounty/kills/witchspace maths verified: `markAsOffender` OR, `shipBountyChanged` args, score ×10 (÷10 for cargo/buoys/rocks except tharglets), `bounty-awarded` when score > 9, `right-on-commander` every 256 kills, fuel `10*max(0.1,d)`, malfunction 253/256 at trade-in ≥ 100, misjump when `randf() > 0.75`, galactic-jump role clearing at 6400/2560 kills. — upstream `Core/Entities/PlayerEntity.m:6020-6098,6546-6747` ↔ port `Core/Entities/PlayerEntity.cpp:6428-6507,6992-7200`
- ✅ **low** — Equipment purchase maths verified: price = fuel/renovation/unit price → script adjustment (truncated) → ×priceFactor → ÷2 for repair; install time `600+price`; laser trade-in × weapon-offset multiplier with `doTradeIn` factor only below 1.0; energy-unit refunds /2, /4, ×3/4, ×3/8 in integer maths; renovation adds `5+techLevel` capped at 100; berths ±1 at 5 t; missile removal credit `(int)` price; `playerBoughtEquipment` args and unsigned wrap identical. — upstream `Core/Entities/PlayerEntity.m:8990-9440` ↔ port `Core/Entities/PlayerEntity.cpp:9530-9989`
- ✅ **low** — `noteGUIDidChangeFrom` verified: demo ship released for SHIPYARD/LOAD/SAVE, sun colour from `info_system_id` on SYSTEM_DATA else `system_id`, `guiScreenChanged(to, from)` skipped when paused. — upstream `Core/Entities/PlayerEntity.m:8952` ↔ port `Core/Entities/PlayerEntity.cpp:9488`
- ✅ **low** — (risk) `marketSorterByName` compares UTF-16 units via `oo::str::compare`, not modelling GNUstep composed/decomposed equivalence (`oofnd/String.hpp:1260`); localised names with combining marks could sort differently. Ties stable. — upstream `Core/Entities/PlayerEntity.m:11871` ↔ port `Core/Entities/PlayerEntity.cpp:214`
- ✅ **low** — `renovationFactor` returns 0.0 (not 1.0) with no shipyard entry in both trees (nil messaging upstream, null `PList::get` in the port). `getFined`, `penaltyForUnauthorizedDocking` (`credits*0.05`, cap 50000), buy/sell limits, `removeFromPylon` and `fuelChargeRate` integer division all match. — upstream `Core/Entities/PlayerEntity.m:10450-10505,11555` ↔ port `Core/Entities/PlayerEntity.cpp:11027-11083,12155`

## PlayerEntityContracts

<a id="playerentitycontracts"></a>
Upstream `src/Core/Entities/PlayerEntityContracts.m` ↔ `Core/Entities/PlayerEntityContracts.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(Contracts) processEscapePods]` (61) | | |
| `-[PlayerEntity(Contracts) checkPassengerContracts]` (147) | | |
| `-[PlayerEntity(Contracts) contractedVolumeForGood:]` (422) | | |
| `-[PlayerEntity(Contracts) addMessageToReport:]` (435) | | |
| `-[PlayerEntity(Contracts) reputation]` (445) | | |
| `-[PlayerEntity(Contracts) passengerReputation]` (450) | | |
| `-[PlayerEntity(Contracts) increasePassengerReputation:]` (464) | | |
| `-[PlayerEntity(Contracts) decreasePassengerReputation:]` (489) | | |
| `-[PlayerEntity(Contracts) parcelReputation]` (514) | | |
| `-[PlayerEntity(Contracts) increaseParcelReputation:]` (528) | | |
| `-[PlayerEntity(Contracts) decreaseParcelReputation:]` (553) | | |
| `-[PlayerEntity(Contracts) contractReputation]` (578) | | |
| `-[PlayerEntity(Contracts) increaseContractReputation:]` (592) | | |
| `-[PlayerEntity(Contracts) decreaseContractReputation:]` (617) | | |
| `-[PlayerEntity(Contracts) erodeReputation]` (642) | | |
| `-[PlayerEntity(Contracts) normaliseReputation]` (696) | | |
| `-[PlayerEntity(Contracts) addPassenger:start:destination:eta:fee:advance:risk:]` (746) | | |
| `-[PlayerEntity(Contracts) removePassenger:]` (776) | | |
| `-[PlayerEntity(Contracts) addParcel:start:destination:eta:fee:premium:risk:]` (797) | | |
| `-[PlayerEntity(Contracts) removeParcel:]` (828) | | |
| `-[PlayerEntity(Contracts) awardContract:commodity:start:]` (849) | | |
| `-[PlayerEntity(Contracts) removeContract:destination:]` (920) | | |
| `-[PlayerEntity(Contracts) passengerList]` (945) | | |
| `-[PlayerEntity(Contracts) parcelList]` (950) | | |
| `-[PlayerEntity(Contracts) contractList]` (955) | | |
| `-[PlayerEntity(Contracts) contractsListFromArray:forCargo:forParcels:]` (960) | | |
| `-[PlayerEntity(Contracts) setGuiToManifestScreen]` (986) | | |
| `-[PlayerEntity(Contracts) setManifestScreenRow:inColor:forRow:ofRows:andOffset:inMultipage:]` (1167) | | |
| `-[PlayerEntity(Contracts) setGuiToDockingReportScreen]` (1183) | | |
| `-[PlayerEntity(Contracts) priceForShipKey:]` (1255) | | |
| `-[PlayerEntity(Contracts) setGuiToShipyardScreen:]` (1261) | | |
| `-[PlayerEntity(Contracts) showShipyardInfoForSelection]` (1393) | | |
| `-[PlayerEntity(Contracts) showTradeInInformationFooter]` (1463) | | |
| `-[PlayerEntity(Contracts) showShipyardModel:shipData:personality:]` (1476) | | |
| `-[PlayerEntity(Contracts) missingSubEntitiesAdjustment]` (1483) | | |
| `-[PlayerEntity(Contracts) tradeInValue]` (1490) | | |
| `-[PlayerEntity(Contracts) buySelectedShip]` (1510) | | |
| `-[PlayerEntity(Contracts) replaceShipWithNamedShip:]` (1620) | | |
| `-[PlayerEntity(Contracts) newShipCommonSetup:yardInfo:baseInfo:]` (1658) | | |
| `RepForRisk()` (1764) | | |

**Differences:**

- *pending*


## PlayerEntityControls

<a id="playerentitycontrols"></a>
Upstream `src/Core/Entities/PlayerEntityControls.m` ↔ `Core/Entities/PlayerEntityControls.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(Controls) initControls]` (203) | | |
| `-[PlayerEntity(Controls) initKeyConfigSettings]` (293) | | |
| `-[PlayerEntity(Controls) processKeyCode:]` (512) | | |
| `-[PlayerEntity(Controls) checkNavKeyPress:]` (591) | | |
| `-[PlayerEntity(Controls) checkKeyPress:]` (603) | | |
| `-[PlayerEntity(Controls) checkKeyPress:fKey_only:]` (608) | | |
| `-[PlayerEntity(Controls) checkKeyPress:ignore_ctrl:]` (613) | | |
| `-[PlayerEntity(Controls) checkKeyPress:fKey_only:ignore_ctrl:]` (618) | | |
| `-[PlayerEntity(Controls) getFirstKeyCode:]` (642) | | |
| `-[PlayerEntity(Controls) pollControls:]` (648) | | |
| `-[PlayerEntity(Controls) handleGUIUpDownArrowKeys]` (697) | | |
| `-[PlayerEntity(Controls) targetNewSystem:whileTyping:]` (772) | | |
| `-[PlayerEntity(Controls) clearPlanetSearchString]` (785) | | |
| `-[PlayerEntity(Controls) targetNewSystem:]` (793) | | |
| `-[PlayerEntity(Controls) switchToMainView]` (798) | | |
| `-[PlayerEntity(Controls) noteSwitchToView:fromView:]` (813) | | |
| `-[PlayerEntity(Controls) beginWitchspaceCountdown:]` (818) | | |
| `-[PlayerEntity(Controls) beginWitchspaceCountdown]` (846) | | |
| `-[PlayerEntity(Controls) cancelWitchspaceCountdown]` (853) | | |
| `-[PlayerEntity(OOControlsPrivate) pollApplicationControls]` (868) | | |
| `-[PlayerEntity(OOControlsPrivate) pollFlightControls:]` (1043) | | |
| `-[PlayerEntity(OOControlsPrivate) pollGuiArrowKeyControls:]` (1795) | | |
| `-[PlayerEntity(OOControlsPrivate) pollMarketScreenControls]` (2706) | | |
| `-[PlayerEntity(OOControlsPrivate) handleGameOptionsScreenKeys]` (2899) | | |
| `-[PlayerEntity(OOControlsPrivate) handleKeyMapperScreenKeys]` (3332) | | |
| `-[PlayerEntity(OOControlsPrivate) handleKeyboardLayoutKeys]` (3365) | | |
| `-[PlayerEntity(OOControlsPrivate) handleStickMapperScreenKeys]` (3398) | | |
| `-[PlayerEntity(OOControlsPrivate) pollCustomViewControls]` (3433) | | |
| `-[PlayerEntity(OOControlsPrivate) pollViewControls]` (3554) | | |
| `-[PlayerEntity(OOControlsPrivate) pollFlightArrowKeyControls:]` (3679) | | |
| `-[PlayerEntity(OOControlsPrivate) pollGuiScreenControls]` (3889) | | |
| `-[PlayerEntity(OOControlsPrivate) pollGuiScreenControlsWithFKeyAlias:]` (3894) | | |
| `-[PlayerEntity(OOControlsPrivate) pollGameOverControls:]` (4016) | | |
| `-[PlayerEntity(OOControlsPrivate) pollAutopilotControls:]` (4034) | | |
| `-[PlayerEntity(OOControlsPrivate) pollDockedControls:]` (4108) | | |
| `-[PlayerEntity(OOControlsPrivate) handleUndockControl]` (4174) | | |
| `-[PlayerEntity(OOControlsPrivate) pollDemoControls:]` (4188) | | |
| `-[PlayerEntity(OOControlsPrivate) pollMissionInterruptControls]` (4497) | | |
| `-[PlayerEntity(OOControlsPrivate) handleMissionCallback]` (4514) | | |
| `-[PlayerEntity(OOControlsPrivate) setGuiToMissionEndScreen]` (4542) | | |
| `-[PlayerEntity(OOControlsPrivate) switchToThisView:]` (4598) | | |
| `-[PlayerEntity(OOControlsPrivate) switchToThisView:andProcessWeaponFacing:]` (4603) | | |
| `-[PlayerEntity(OOControlsPrivate) switchToThisView:fromView:andProcessWeaponFacing:justNotify:]` (4608) | | |
| `-[PlayerEntity(OOControlsPrivate) handleAutopilotOn:]` (4653) | | |
| `-[PlayerEntity(OOControlsPrivate) handleButtonIdent]` (4758) | | |
| `-[PlayerEntity(OOControlsPrivate) handleButtonTargetMissile]` (4775) | | |

**Differences:**

- *pending*


## PlayerEntityKeyMapper

<a id="playerentitykeymapper"></a>
Upstream `src/Core/Entities/PlayerEntityKeyMapper.m` ↔ `Core/Entities/PlayerEntityKeyMapper.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(KeyMapper) initCheckingDictionary]` (77) | | |
| `-[PlayerEntity(KeyMapper) resetKeyFunctions]` (112) | | |
| `-[PlayerEntity(KeyMapper) setGuiToKeyMapperScreen:]` (118) | | |
| `-[PlayerEntity(KeyMapper) setGuiToKeyMapperScreen:resetCurrentRow:]` (123) | | |
| `-[PlayerEntity(KeyMapper) keyMapperInputHandler:view:]` (188) | | |
| `-[PlayerEntity(KeyMapper) entryIsIndexCustomEquip:]` (296) | | |
| `-[PlayerEntity(KeyMapper) entryIsDictCustomEquip:]` (301) | | |
| `-[PlayerEntity(KeyMapper) entryIsCustomEquip:]` (306) | | |
| `-[PlayerEntity(KeyMapper) getCustomEquipArray:]` (314) | | |
| `-[PlayerEntity(KeyMapper) getCustomEquipIndex:]` (337) | | |
| `-[PlayerEntity(KeyMapper) getCustomEquipKeyDefType:]` (357) | | |
| `-[PlayerEntity(KeyMapper) setGuiToKeyConfigScreen]` (368) | | |
| `-[PlayerEntity(KeyMapper) setGuiToKeyConfigScreen:]` (373) | | |
| `-[PlayerEntity(KeyMapper) outputKeyDefinition:shift:mod1:mod2:skiprows:]` (469) | | |
| `-[PlayerEntity(KeyMapper) handleKeyConfigKeys:view:]` (512) | | |
| `-[PlayerEntity(KeyMapper) setGuiToKeyConfigEntryScreen]` (548) | | |
| `-[PlayerEntity(KeyMapper) handleKeyConfigEntryKeys:view:]` (598) | | |
| `-[PlayerEntity(KeyMapper) updateKeyDefinition:index:]` (630) | | |
| `-[PlayerEntity(KeyMapper) updateShiftKeyDefinition:index:]` (656) | | |
| `-[PlayerEntity(KeyMapper) setGuiToConfirmClearScreen]` (698) | | |
| `-[PlayerEntity(KeyMapper) handleKeyMapperConfirmClearKeys:view:]` (731) | | |
| `-[PlayerEntity(KeyMapper) displayKeyFunctionList:skip:]` (760) | | |
| `-[PlayerEntity(KeyMapper) keyFunctionList]` (875) | | |
| `-[PlayerEntity(KeyMapper) makeKeyGuiDict:keyDef:]` (1042) | | |
| `-[PlayerEntity(KeyMapper) makeKeyGuiDictHeader:]` (1052) | | |
| `-[PlayerEntity(KeyMapper) setGuiToKeyboardLayoutScreen:]` (1061) | | |
| `-[PlayerEntity(KeyMapper) setGuiToKeyboardLayoutScreen:resetCurrentRow:]` (1066) | | |
| `-[PlayerEntity(KeyMapper) handleKeyboardLayoutEntryKeys:view:]` (1101) | | |
| `-[PlayerEntity(KeyMapper) keyboardDescription:]` (1142) | | |
| `-[PlayerEntity(KeyMapper) keyboardLayoutList]` (1159) | | |
| `-[PlayerEntity(KeyMapper) displayKeyboardLayoutList:skip:]` (1202) | | |
| `-[PlayerEntity(KeyMapper) validateAllKeys]` (1265) | | |
| `-[PlayerEntity(KeyMapper) validateKey:checkKeys:]` (1282) | | |
| `-[PlayerEntity(KeyMapper) searchArrayForMatch:key:checkKeys:]` (1416) | | |
| `-[PlayerEntity(KeyMapper) entryIsEqualToDefault:]` (1446) | | |
| `-[PlayerEntity(KeyMapper) compareKeyEntries:second:]` (1463) | | |
| `-[PlayerEntity(KeyMapper) saveKeySetting:]` (1475) | | |
| `-[PlayerEntity(KeyMapper) unsetKeySetting:]` (1526) | | |
| `-[PlayerEntity(KeyMapper) deleteKeySetting:]` (1548) | | |
| `-[PlayerEntity(KeyMapper) deleteAllKeySettings]` (1565) | | |
| `-[PlayerEntity(KeyMapper) loadKeySettings]` (1591) | | |
| `-[PlayerEntity(KeyMapper) reloadPage]` (1598) | | |

**Differences:**

- *pending*


## PlayerEntityLoadSave

<a id="playerentityloadsave"></a>
Upstream `src/Core/Entities/PlayerEntityLoadSave.m` ↔ `Core/Entities/PlayerEntityLoadSave.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(LoadSave) loadPlayer]` (79) | | |
| `-[PlayerEntity(LoadSave) savePlayer]` (87) | | |
| `-[PlayerEntity(LoadSave) autosavePlayer]` (92) | | |
| `-[PlayerEntity(LoadSave) quicksavePlayer]` (127) | | |
| `-[PlayerEntity(LoadSave) setGuiToScenarioScreen:]` (148) | | |
| `-[PlayerEntity(LoadSave) addScenarioModel:]` (214) | | |
| `-[PlayerEntity(LoadSave) showScenarioDetails]` (219) | | |
| `-[PlayerEntity(LoadSave) startScenario]` (243) | | |
| `-[PlayerEntity(LoadSave) commanderSelector]` (281) | | |
| `-[PlayerEntity(LoadSave) saveCommanderInputHandler]` (364) | | |
| `-[PlayerEntity(LoadSave) overwriteCommanderInputHandler]` (450) | | |
| `-[PlayerEntity(LoadSave) loadPlayerFromFile:asNew:]` (481) | | |
| `-[PlayerEntity(OOLoadSavePrivate) writePlayerToPath:]` (679) | | |
| `-[PlayerEntity(OOLoadSavePrivate) nativeSavePlayer:]` (714) | | |
| `-[PlayerEntity(OOLoadSavePrivate) setGuiToLoadCommanderScreen]` (726) | | |
| `-[PlayerEntity(OOLoadSavePrivate) setGuiToSaveCommanderScreen:]` (748) | | |
| `-[PlayerEntity(OOLoadSavePrivate) setGuiToOverwriteScreen:]` (778) | | |
| `sortCommanders()` (814) | | |
| `-[PlayerEntity(OOLoadSavePrivate) lsCommanders:]` (819) | | |
| `-[PlayerEntity(OOLoadSavePrivate) existingNativeSave:]` (983) | | |
| `-[PlayerEntity(OOLoadSavePrivate) showCommanderShip:]` (992) | | |
| `-[PlayerEntity(OOLoadSavePrivate) findIndexOfCommander:]` (1110) | | |
| `-[MyOpenGLView(OOLoadSaveExtensions) isCommandModifierKeyDown]` (1128) | | |
| `PersonalityForCommanderDict()` (1135) | | |
| `OODeciCreditsFromDouble()` (1147) | | |
| `OODeciCreditsFromObject()` (1174) | | |

**Differences:**

- *pending*


## PlayerEntityScriptMethods

<a id="playerentityscriptmethods"></a>
Upstream `src/Core/Entities/PlayerEntityScriptMethods.m` ↔ `Core/Entities/PlayerEntityScriptMethods.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(ScriptMethods) score]` (40) | | |
| `-[PlayerEntity(ScriptMethods) setScore:]` (45) | | |
| `-[PlayerEntity(ScriptMethods) creditBalance]` (50) | | |
| `-[PlayerEntity(ScriptMethods) setCreditBalance:]` (55) | | |
| `-[PlayerEntity(ScriptMethods) dockedStationName]` (60) | | |
| `-[PlayerEntity(ScriptMethods) dockedStationDisplayName]` (65) | | |
| `-[PlayerEntity(ScriptMethods) dockedAtMainStation]` (70) | | |
| `-[PlayerEntity(ScriptMethods) awardCommodityType:amount:]` (75) | | |
| `-[PlayerEntity(ScriptMethods) resetScannerZoom]` (148) | | |
| `-[PlayerEntity(ScriptMethods) currentGalaxyID]` (153) | | |
| `-[PlayerEntity(ScriptMethods) currentSystemID]` (158) | | |
| `-[PlayerEntity(ScriptMethods) setMissionChoice:]` (165) | | |
| `-[PlayerEntity(ScriptMethods) setMissionChoice:withEvent:]` (170) | | |
| `-[PlayerEntity(ScriptMethods) setMissionChoice:keyPress:]` (175) | | |
| `-[PlayerEntity(ScriptMethods) setMissionChoice:keyPress:withEvent:]` (180) | | |
| `-[PlayerEntity(ScriptMethods) allowMissionInterrupt]` (202) | | |
| `-[PlayerEntity(ScriptMethods) scriptTimer]` (207) | | |
| `-[PlayerEntity(ScriptMethods) systemPseudoRandom100]` (215) | | |
| `-[PlayerEntity(ScriptMethods) systemPseudoRandom256]` (221) | | |
| `-[PlayerEntity(ScriptMethods) systemPseudoRandomFloat]` (227) | | |
| `-[PlayerEntity(ScriptMethods) passengerContractMarker:]` (238) | | |
| `-[PlayerEntity(ScriptMethods) parcelContractMarker:]` (248) | | |
| `-[PlayerEntity(ScriptMethods) cargoContractMarker:]` (258) | | |
| `-[PlayerEntity(ScriptMethods) defaultMarker:]` (268) | | |
| `-[PlayerEntity(ScriptMethods) validatedMarker:]` (278) | | |
| `-[PlayerEntity(ScriptMethods) creditsFormattedForSubstitution]` (301) | | |
| `-[PlayerEntity(ScriptMethods) creditsFormattedForLegacySubstitution]` (312) | | |
| `-[PlayerEntity(ScriptMethods) commanderBountyAsString]` (322) | | |
| `-[PlayerEntity(ScriptMethods) commanderKillsAsString]` (328) | | |
| `-[PlayerEntity(ScriptMethods) keyBindingDescription2:]` (334) | | |
| `-[PlayerEntity(ScriptMethods) getKeyBindingDescription:]` (344) | | |
| `-[PlayerEntity(ScriptMethods) keyCodeDescription:]` (369) | | |
| `-[PlayerEntity(ScriptMethods) keyCodeDescriptionShort:]` (468) | | |
| `OOGalacticCoordinatesFromInternal()` (568) | | |
| `OOInternalCoordinatesFromGalactic()` (573) | | |

**Differences:**

- *pending*


## PlayerEntitySound

<a id="playerentitysound"></a>
Upstream `src/Core/Entities/PlayerEntitySound.m` ↔ `Core/Entities/PlayerEntitySound.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(Sound) setUpSound]` (70) | | |
| `-[PlayerEntity(Sound) setUpWeaponSounds]` (101) | | |
| `-[PlayerEntity(Sound) destroySound]` (155) | | |
| `-[PlayerEntity(Sound) playInterfaceBeep:]` (178) | | |
| `-[PlayerEntity(Sound) isBeeping]` (188) | | |
| `-[PlayerEntity(Sound) boop]` (193) | | |
| `-[PlayerEntity(Sound) playIdentOn]` (198) | | |
| `-[PlayerEntity(Sound) playIdentOff]` (203) | | |
| `-[PlayerEntity(Sound) playIdentLockedOn]` (208) | | |
| `-[PlayerEntity(Sound) playMissileArmed]` (213) | | |
| `-[PlayerEntity(Sound) playMineArmed]` (218) | | |
| `-[PlayerEntity(Sound) playMissileSafe]` (223) | | |
| `-[PlayerEntity(Sound) playMissileLockedOn]` (228) | | |
| `-[PlayerEntity(Sound) playNextEquipmentSelected]` (233) | | |
| `-[PlayerEntity(Sound) playNextMissileSelected]` (238) | | |
| `-[PlayerEntity(Sound) playWeaponsOnline]` (243) | | |
| `-[PlayerEntity(Sound) playWeaponsOffline]` (248) | | |
| `-[PlayerEntity(Sound) playCargoJettisioned]` (253) | | |
| `-[PlayerEntity(Sound) playAutopilotOn]` (258) | | |
| `-[PlayerEntity(Sound) playAutopilotOff]` (263) | | |
| `-[PlayerEntity(Sound) playAutopilotOutOfRange]` (271) | | |
| `-[PlayerEntity(Sound) playAutopilotCannotDockWithTarget]` (276) | | |
| `-[PlayerEntity(Sound) playSaveOverwriteYes]` (281) | | |
| `-[PlayerEntity(Sound) playSaveOverwriteNo]` (286) | | |
| `-[PlayerEntity(Sound) playHoldFull]` (291) | | |
| `-[PlayerEntity(Sound) playJumpMassLocked]` (296) | | |
| `-[PlayerEntity(Sound) playTargetLost]` (301) | | |
| `-[PlayerEntity(Sound) playNoTargetInMemory]` (306) | | |
| `-[PlayerEntity(Sound) playTargetSwitched]` (311) | | |
| `-[PlayerEntity(Sound) playHyperspaceNoTarget]` (316) | | |
| `-[PlayerEntity(Sound) playHyperspaceNoFuel]` (321) | | |
| `-[PlayerEntity(Sound) playHyperspaceBlocked]` (326) | | |
| `-[PlayerEntity(Sound) playHyperspaceDistanceTooGreat]` (331) | | |
| `-[PlayerEntity(Sound) playCloakingDeviceOn]` (336) | | |
| `-[PlayerEntity(Sound) playCloakingDeviceOff]` (341) | | |
| `-[PlayerEntity(Sound) playMenuNavigationUp]` (346) | | |
| `-[PlayerEntity(Sound) playMenuNavigationDown]` (351) | | |
| `-[PlayerEntity(Sound) playMenuNavigationNot]` (356) | | |
| `-[PlayerEntity(Sound) playMenuPagePrevious]` (361) | | |
| `-[PlayerEntity(Sound) playMenuPageNext]` (366) | | |
| `-[PlayerEntity(Sound) playDismissedReportScreen]` (371) | | |
| `-[PlayerEntity(Sound) playDismissedMissionScreen]` (376) | | |
| `-[PlayerEntity(Sound) playChangedOption]` (381) | | |
| `-[PlayerEntity(Sound) updateFuelScoopSoundWithInterval:]` (386) | | |
| `-[PlayerEntity(Sound) updateAfterburnerSound]` (407) | | |
| `-[PlayerEntity(Sound) startAfterburnerSound]` (426) | | |
| `-[PlayerEntity(Sound) stopAfterburnerSound]` (434) | | |
| `-[PlayerEntity(Sound) playCloakingDeviceInsufficientEnergy]` (439) | | |
| `-[PlayerEntity(Sound) playBuyCommodity]` (444) | | |
| `-[PlayerEntity(Sound) playBuyShip]` (449) | | |
| `-[PlayerEntity(Sound) playSellCommodity]` (454) | | |
| `-[PlayerEntity(Sound) playCantBuyCommodity]` (459) | | |
| `-[PlayerEntity(Sound) playCantSellCommodity]` (464) | | |
| `-[PlayerEntity(Sound) playCantBuyShip]` (469) | | |
| `-[PlayerEntity(Sound) playStandardHyperspace]` (474) | | |
| `-[PlayerEntity(Sound) playGalacticHyperspace]` (479) | | |
| `-[PlayerEntity(Sound) playHyperspaceAborted]` (484) | | |
| `-[PlayerEntity(Sound) playHitByECMSound]` (489) | | |
| `-[PlayerEntity(Sound) playFiredECMSound]` (495) | | |
| `-[PlayerEntity(Sound) playLaunchFromStation]` (501) | | |
| `-[PlayerEntity(Sound) playDockWithStation]` (506) | | |
| `-[PlayerEntity(Sound) playExitWitchspace]` (511) | | |
| `-[PlayerEntity(Sound) playHostileWarning]` (516) | | |
| `-[PlayerEntity(Sound) playAlertConditionRed]` (521) | | |
| `-[PlayerEntity(Sound) playIncomingMissile:]` (526) | | |
| `-[PlayerEntity(Sound) playEnergyLow]` (531) | | |
| `-[PlayerEntity(Sound) playDockingDenied]` (536) | | |
| `-[PlayerEntity(Sound) playWitchjumpFailure]` (541) | | |
| `-[PlayerEntity(Sound) playWitchjumpMisjump]` (546) | | |
| `-[PlayerEntity(Sound) playWitchjumpBlocked]` (551) | | |
| `-[PlayerEntity(Sound) playWitchjumpDistanceTooGreat]` (556) | | |
| `-[PlayerEntity(Sound) playWitchjumpInsufficientFuel]` (561) | | |
| `-[PlayerEntity(Sound) playFuelLeak]` (566) | | |
| `-[PlayerEntity(Sound) playShieldHit:weaponIdentifier:]` (571) | | |
| `-[PlayerEntity(Sound) playDirectHit:weaponIdentifier:]` (579) | | |
| `-[PlayerEntity(Sound) playScrapeDamage:]` (587) | | |
| `-[PlayerEntity(Sound) playLaserHit:offset:weaponIdentifier:]` (592) | | |
| `-[PlayerEntity(Sound) playWeaponOverheated:]` (608) | | |
| `-[PlayerEntity(Sound) playMissileLaunched:weaponIdentifier:]` (613) | | |
| `-[PlayerEntity(Sound) playMineLaunched:weaponIdentifier:]` (621) | | |
| `-[PlayerEntity(Sound) playEscapePodScooped]` (629) | | |
| `-[PlayerEntity(Sound) playAegisCloseToPlanet]` (634) | | |
| `-[PlayerEntity(Sound) playAegisCloseToStation]` (639) | | |
| `-[PlayerEntity(Sound) playGameOver]` (644) | | |
| `-[PlayerEntity(Sound) playLegacyScriptSound:]` (649) | | |

**Differences:**

- *pending*


## PlayerEntityStickMapper

<a id="playerentitystickmapper"></a>
Upstream `src/Core/Entities/PlayerEntityStickMapper.m` ↔ `Core/Entities/PlayerEntityStickMapper.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(StickMapper) resetStickFunctions]` (48) | | |
| `-[PlayerEntity(StickMapper) setGuiToStickMapperScreen:]` (54) | | |
| `-[PlayerEntity(StickMapper) setGuiToStickMapperScreen:resetCurrentRow:]` (59) | | |
| `-[PlayerEntity(StickMapper) stickMapperInputHandler:]` (124) | | |
| `-[PlayerEntity(StickMapper) updateFunction:]` (201) | | |
| `-[PlayerEntity(StickMapper) checkCustomEquipButtons:ignore:]` (283) | | |
| `-[PlayerEntity(StickMapper) removeFunction:]` (307) | | |
| `-[PlayerEntity(StickMapper) displayFunctionList:]` (358) | | |
| `-[PlayerEntity(StickMapper) describeStickDict:]` (474) | | |
| `-[PlayerEntity(StickMapper) hwToString:]` (499) | | |
| `-[PlayerEntity(StickMapper) stickFunctionList]` (517) | | |
| `-[PlayerEntity(StickMapper) makeStickGuiDict:]` (825) | | |
| `-[PlayerEntity(StickMapper) makeStickGuiDictHeader:]` (846) | | |

**Differences:**

- *pending*


## PlayerEntityStickProfile

<a id="playerentitystickprofile"></a>
Upstream `src/Core/Entities/PlayerEntityStickProfile.m` ↔ `Core/Entities/PlayerEntityStickProfile.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlayerEntity(StickProfile) setGuiToStickProfileScreen:]` (69) | | |
| `-[PlayerEntity(StickProfile) stickProfileInputHandler:]` (76) | | |
| `-[PlayerEntity(StickProfile) stickProfileGraphAxisProfile:screenAt:screenSize:]` (180) | | |
| `-[StickProfileScreen init]` (191) | | |
| `-[StickProfileScreen dealloc]` (207) | | |
| `-[StickProfileScreen startGui:]` (217) | | |
| `-[StickProfileScreen mouseDown:]` (228) | | |
| `-[StickProfileScreen mouseUp]` (253) | | |
| `-[StickProfileScreen deleteSelected]` (262) | | |
| `-[StickProfileScreen(StickProfileInternal) nextAxis]` (280) | | |
| `-[StickProfileScreen(StickProfileInternal) previousAxis]` (290) | | |
| `-[StickProfileScreen(StickProfileInternal) currentAxis]` (300) | | |
| `-[StickProfileScreen(StickProfileInternal) increaseDeadzone]` (315) | | |
| `-[StickProfileScreen(StickProfileInternal) decreaseDeadzone]` (325) | | |
| `-[StickProfileScreen(StickProfileInternal) nextProfileType]` (335) | | |
| `-[StickProfileScreen(StickProfileInternal) previousProfileType]` (355) | | |
| `-[StickProfileScreen(StickProfileInternal) currentProfileIsSpline]` (375) | | |
| `-[StickProfileScreen(StickProfileInternal) IncreasePower]` (383) | | |
| `-[StickProfileScreen(StickProfileInternal) DecreasePower]` (397) | | |
| `-[StickProfileScreen(StickProfileInternal) IncreaseParam]` (411) | | |
| `-[StickProfileScreen(StickProfileInternal) DecreaseParam]` (425) | | |
| `-[StickProfileScreen(StickProfileInternal) graphProfile:at:size:]` (439) | | |
| `-[StickProfileScreen(StickProfileInternal) startEdit]` (487) | | |
| `-[StickProfileScreen(StickProfileInternal) saveSettings]` (504) | | |
| `-[StickProfileScreen(StickProfileInternal) showScreen]` (510) | | |
| `-[StickProfileScreen(StickProfileInternal) profileType]` (584) | | |

**Differences:**

- *pending*


## ProxyPlayerEntity

<a id="proxyplayerentity"></a>
Upstream `src/Core/Entities/ProxyPlayerEntity.m` ↔ `Core/Entities/ProxyPlayerEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[ProxyPlayerEntity initWithKey:definition:]` (30) | | |
| `-[ProxyPlayerEntity copyValuesFromPlayer:]` (44) | | |
| `-[ProxyPlayerEntity isPlayerLikeShip]` (63) | | |
| `-[ProxyPlayerEntity fuelLeakRate]` (68) | | |
| `-[ProxyPlayerEntity setFuelLeakRate:]` (73) | | |
| `-[ProxyPlayerEntity massLocked]` (78) | | |
| `-[ProxyPlayerEntity setMassLocked:]` (83) | | |
| `-[ProxyPlayerEntity atHyperspeed]` (88) | | |
| `-[ProxyPlayerEntity setAtHyperspeed:]` (93) | | |
| `-[ProxyPlayerEntity dialForwardShield]` (98) | | |
| `-[ProxyPlayerEntity setDialForwardShield:]` (103) | | |
| `-[ProxyPlayerEntity dialAftShield]` (108) | | |
| `-[ProxyPlayerEntity setDialAftShield:]` (113) | | |
| `-[ProxyPlayerEntity dialMissileStatus]` (118) | | |
| `-[ProxyPlayerEntity setDialMissileStatus:]` (123) | | |
| `-[ProxyPlayerEntity dialFuelScoopStatus]` (128) | | |
| `-[ProxyPlayerEntity setDialFuelScoopStatus:]` (133) | | |
| `-[ProxyPlayerEntity compassMode]` (138) | | |
| `-[ProxyPlayerEntity setCompassMode:]` (143) | | |
| `-[ProxyPlayerEntity dialIdentEngaged]` (148) | | |
| `-[ProxyPlayerEntity setDialIdentEngaged:]` (153) | | |
| `-[ProxyPlayerEntity alertCondition]` (158) | | |
| `-[ProxyPlayerEntity setAlertCondition:]` (163) | | |
| `-[ProxyPlayerEntity trumbleCount]` (168) | | |
| `-[ProxyPlayerEntity setTrumbleCount:]` (173) | | |
| `-[ProxyPlayerEntity setTradeInFactor:]` (178) | | |
| `-[ProxyPlayerEntity tradeInFactor]` (183) | | |
| `-[Entity(ProxyPlayer) isPlayerLikeShip]` (194) | | |
| `-[PlayerEntity(ProxyPlayer) isPlayerLikeShip]` (203) | | |

**Differences:**

- *pending*

