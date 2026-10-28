# Parity findings: Ship, station, dock entities

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## DockEntity

<a id="dockentity"></a>
Upstream `src/Core/Entities/DockEntity.m` ↔ `Core/Entities/DockEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[DockEntity pruneAndCountShipsOnApproach]` (60) | | |
| `-[DockEntity abortAllDockings]` (89) | | |
| `-[DockEntity abortAllLaunches]` (129) | | |
| `-[DockEntity autoDockShipsInQueue:]` (135) | | |
| `-[DockEntity autoDockShipsOnApproach]` (148) | | |
| `-[DockEntity allowsDocking]` (153) | | |
| `-[DockEntity setAllowsDocking:]` (158) | | |
| `-[DockEntity disallowedDockingCollides]` (166) | | |
| `-[DockEntity allowsLaunching]` (171) | | |
| `-[DockEntity setAllowsLaunching:]` (176) | | |
| `-[DockEntity setDisallowedDockingCollides:]` (184) | | |
| `-[DockEntity setVirtual]` (189) | | |
| `-[DockEntity canAcceptShipForDocking:]` (194) | | |
| `-[DockEntity isOffCentre]` (234) | | |
| `-[DockEntity dockingInstructionsForShip:]` (246) | | |
| `-[DockEntity addShipToShipsOnApproach:]` (400) | | |
| `-[DockEntity noteDockingForShip:]` (522) | | |
| `-[DockEntity abortDockingForShip:]` (531) | | |
| `-[DockEntity portUpVectorForShipsBoundingBox:]` (560) | | |
| `-[DockEntity shipIsInDockingQueue:]` (571) | | |
| `-[DockEntity countOfShipsInDockingQueue]` (591) | | |
| `-[DockEntity countOfShipsInLaunchQueue]` (596) | | |
| `-[DockEntity shipIsInDockingCorridor:]` (601) | | |
| `-[DockEntity pullInShipIfPermitted:]` (773) | | |
| `-[DockEntity addShipToLaunchQueue:withPriority:]` (782) | | |
| `-[DockEntity launchShip:]` (801) | | |
| `-[DockEntity countOfShipsInLaunchQueueWithPrimaryRole:]` (854) | | |
| `-[DockEntity allowsLaunchingOf:]` (865) | | |
| `-[DockEntity clear]` (894) | | |
| `-[DockEntity dockingCorridorIsEmpty]` (900) | | |
| `-[DockEntity clearDockingCorridor]` (966) | | |
| `-[DockEntity setDimensionsAndCorridor:ddc:launching:]` (1032) | | |
| `-[DockEntity isDock]` (1058) | | |
| `-[DockEntity initWithKey:definition:]` (1063) | | |
| `-[DockEntity dealloc]` (1082) | | |
| `-[DockEntity clearIdLocks:]` (1091) | | |
| `-[DockEntity clearAllIdLocks]` (1101) | | |
| `-[DockEntity setUpShipFromDictionary:]` (1109) | | |
| `-[DockEntity update:]` (1124) | | |
| `-[DockEntity noteTakingDamage:from:type:]` (1142) | | |
| `-[DockEntity takeEnergyDamage:from:becauseOf:weaponIdentifier:]` (1151) | | |
| `-[DockEntity drawImmediate:translucent:]` (1161) | | |

**Differences:**

- *pending*


## ShipEntity

<a id="shipentity"></a>
Upstream `src/Core/Entities/ShipEntity.m` ↔ `Core/Entities/ShipEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `calcFuelChargeRate()` (104) | | |
| `-[ShipEntity init]` (174) | | |
| `-[ShipEntity initBypassForPlayer]` (184) | | |
| `-[ShipEntity initWithKey:definition:]` (190) | | |
| `-[ShipEntity setUpFromDictionary:]` (235) | | |
| `-[ShipEntity setUpShipFromDictionary:]` (500) | | |
| `-[ShipEntity setSubIdx:]` (724) | | |
| `-[ShipEntity subIdx]` (729) | | |
| `-[ShipEntity maxShipSubEntities]` (734) | | |
| `-[ShipEntity repeatString:times:]` (739) | | |
| `-[ShipEntity serializeShipSubEntities]` (753) | | |
| `-[ShipEntity deserializeShipSubEntitiesFrom:]` (771) | | |
| `-[ShipEntity setUpSubEntities]` (790) | | |
| `-[ShipEntity frustumRadius]` (822) | | |
| `-[ShipEntity setUpOneSubentity:]` (835) | | |
| `-[ShipEntity setUpOneFlasher:]` (851) | | |
| `-[ShipEntity setUpOneStandardSubentity:asTurret:]` (860) | | |
| `-[ShipEntity isTemplateCargoPod]` (932) | | |
| `-[ShipEntity setUpCargoType:]` (937) | | |
| `-[ShipEntity dealloc]` (986) | | |
| `-[ShipEntity removeScript]` (1060) | | |
| `-[ShipEntity clearSubEntities]` (1066) | | |
| `-[ShipEntity subEntityRotationalVelocity]` (1080) | | |
| `-[ShipEntity setSubEntityRotationalVelocity:]` (1085) | | |
| `-[ShipEntity descriptionComponents]` (1090) | | |
| `-[ShipEntity shortDescriptionComponents]` (1106) | | |
| `-[ShipEntity sunGlareFilter]` (1111) | | |
| `-[ShipEntity setSunGlareFilter:]` (1116) | | |
| `-[ShipEntity accuracy]` (1121) | | |
| `-[ShipEntity setAccuracy:]` (1126) | | |
| `-[ShipEntity mesh]` (1145) | | |
| `-[ShipEntity setMesh:]` (1150) | | |
| `-[ShipEntity totalBoundingBox]` (1159) | | |
| `-[ShipEntity forwardVector]` (1164) | | |
| `-[ShipEntity upVector]` (1169) | | |
| `-[ShipEntity rightVector]` (1174) | | |
| `-[ShipEntity scriptedMisjump]` (1179) | | |
| `-[ShipEntity setScriptedMisjump:]` (1184) | | |
| `-[ShipEntity scriptedMisjumpRange]` (1189) | | |
| `-[ShipEntity setScriptedMisjumpRange:]` (1194) | | |
| `-[ShipEntity subEntities]` (1199) | | |
| `-[ShipEntity subEntityCount]` (1204) | | |
| `-[ShipEntity hasSubEntity:]` (1209) | | |
| `-[ShipEntity subEntityEnumerator]` (1214) | | |
| `-[ShipEntity shipSubEntityEnumerator]` (1219) | | |
| `-[ShipEntity flasherEnumerator]` (1224) | | |
| `-[ShipEntity exhaustEnumerator]` (1229) | | |
| `-[ShipEntity subEntityTakingDamage]` (1234) | | |
| `-[ShipEntity setSubEntityTakingDamage:]` (1252) | | |
| `-[ShipEntity shipScript]` (1271) | | |
| `-[ShipEntity shipAIScript]` (1276) | | |
| `-[ShipEntity shipAIScriptWakeTime]` (1281) | | |
| `-[ShipEntity setAIScriptWakeTime:]` (1286) | | |
| `-[ShipEntity findBoundingBoxRelativeToPosition:i:j:k:]` (1291) | | |
| `-[ShipEntity octree]` (1304) | | |
| `-[ShipEntity volume]` (1309) | | |
| `-[ShipEntity doesHitLine:v1:]` (1314) | | |
| `-[ShipEntity doesHitLine:v1:hitEntity:]` (1323) | | |
| `-[ShipEntity doesHitLine:v1:withPosition:i:j:k:]` (1359) | | |
| `-[ShipEntity wasAddedToUniverse]` (1368) | | |
| `-[ShipEntity wasRemovedFromUniverse]` (1395) | | |
| `-[ShipEntity absoluteTractorPosition]` (1400) | | |
| `-[ShipEntity beaconCode]` (1405) | | |
| `-[ShipEntity setBeaconCode:]` (1410) | | |
| `-[ShipEntity beaconLabel]` (1427) | | |
| `-[ShipEntity setBeaconLabel:]` (1432) | | |
| `-[ShipEntity isVisible]` (1443) | | |
| `-[ShipEntity isBeacon]` (1448) | | |
| `-[ShipEntity beaconDrawable]` (1453) | | |
| `-[ShipEntity prevBeacon]` (1476) | | |
| `-[ShipEntity nextBeacon]` (1481) | | |
| `-[ShipEntity setPrevBeacon:]` (1486) | | |
| `-[ShipEntity setNextBeacon:]` (1494) | | |
| `-[ShipEntity setIsBoulder:]` (1504) | | |
| `-[ShipEntity isBoulder]` (1512) | | |
| `-[ShipEntity isMinable]` (1517) | | |
| `-[ShipEntity countsAsKill]` (1527) | | |
| `-[ShipEntity setUpEscorts]` (1532) | | |
| `-[ShipEntity setUpMixedEscorts]` (1619) | | |
| `-[ShipEntity setUpOneEscort:inGroup:withRole:atPosition:andCount:]` (1684) | | |
| `-[ShipEntity shipDataKey]` (1774) | | |
| `-[ShipEntity shipDataKeyAutoRole]` (1779) | | |
| `-[ShipEntity setShipDataKey:]` (1784) | | |
| `-[ShipEntity shipInfoDictionary]` (1790) | | |
| `-[ShipEntity getWeaponOffsetFrom:withKey:inMode:]` (1797) | | |
| `-[ShipEntity aftWeaponOffset]` (1819) | | |
| `-[ShipEntity forwardWeaponOffset]` (1824) | | |
| `-[ShipEntity portWeaponOffset]` (1829) | | |
| `-[ShipEntity starboardWeaponOffset]` (1834) | | |
| `-[ShipEntity isFrangible]` (1839) | | |
| `-[ShipEntity suppressFlightNotifications]` (1844) | | |
| `-[ShipEntity scanClass]` (1849) | | |
| `-[ShipEntity canCollide]` (1858) | | |
| `doOctreesCollide()` (1878) | | |
| `-[ShipEntity checkCloseCollisionWith:]` (1944) | | |
| `-[ShipEntity findSubentityBoundingBox]` (2002) | | |
| `-[ShipEntity absoluteIJKForSubentity]` (2007) | | |
| `-[ShipEntity addSubentityToCollisionRadius:]` (2028) | | |
| `-[ShipEntity launchPodWithCrew:]` (2047) | | |
| `-[ShipEntity validForAddToUniverse]` (2074) | | |
| `-[ShipEntity update:]` (2083) | | |
| `-[ShipEntity processBehaviour:]` (2485) | | |
| `-[ShipEntity noteFrustration:]` (2641) | | |
| `-[ShipEntity respondToAttackFrom:becauseOf:]` (2647) | | |
| `-[ShipEntity hasOneEquipmentItem:includeWeapons:whileLoading:]` (2699) | | |
| `-[ShipEntity hasOneEquipmentItem:includeMissiles:whileLoading:]` (2722) | | |
| `-[ShipEntity hasPrimaryWeapon:]` (2746) | | |
| `-[ShipEntity countEquipmentItem:]` (2766) | | |
| `-[ShipEntity hasEquipmentItem:includeWeapons:whileLoading:]` (2778) | | |
| `-[ShipEntity hasEquipmentItem:]` (2796) | | |
| `-[ShipEntity hasEquipmentItemProviding:]` (2803) | | |
| `-[ShipEntity equipmentItemProviding:]` (2820) | | |
| `-[ShipEntity hasAllEquipment:includeWeapons:whileLoading:]` (2837) | | |
| `-[ShipEntity hasAllEquipment:]` (2859) | | |
| `-[ShipEntity hasHyperspaceMotor]` (2864) | | |
| `-[ShipEntity hyperspaceSpinTime]` (2869) | | |
| `-[ShipEntity setHyperspaceSpinTime:]` (2874) | | |
| `-[ShipEntity canAddEquipment:inContext:]` (2879) | | |
| `-[ShipEntity weaponFacings]` (2901) | | |
| `-[ShipEntity weaponTypeIDForFacing:strict:]` (2906) | | |
| `-[ShipEntity weaponTypeForFacing:strict:]` (2943) | | |
| `-[ShipEntity missilesList]` (2950) | | |
| `-[ShipEntity passengerListForScripting]` (2956) | | |
| `-[ShipEntity parcelListForScripting]` (2961) | | |
| `-[ShipEntity contractListForScripting]` (2966) | | |
| `-[ShipEntity generateMissileEquipmentTypeFrom:]` (2971) | | |
| `-[ShipEntity equipmentListForScripting]` (2993) | | |
| `-[ShipEntity equipmentValidToAdd:inContext:]` (3028) | | |
| `-[ShipEntity equipmentValidToAdd:whileLoading:inContext:]` (3033) | | |
| `-[ShipEntity setWeaponMount:toWeapon:]` (3133) | | |
| `-[ShipEntity addEquipmentItem:inContext:]` (3165) | | |
| `-[ShipEntity addEquipmentItem:withValidation:inContext:]` (3170) | | |
| `-[ShipEntity equipmentEnumerator]` (3253) | | |
| `-[ShipEntity equipmentCount]` (3258) | | |
| `-[ShipEntity removeEquipmentItem:]` (3263) | | |
| `-[ShipEntity removeExternalStore:]` (3333) | | |
| `-[ShipEntity verifiedMissileTypeFromRole:]` (3351) | | |
| `-[ShipEntity selectMissile]` (3429) | | |
| `-[ShipEntity removeAllEquipment]` (3485) | | |
| `-[ShipEntity removeMissiles]` (3491) | | |
| `-[ShipEntity parcelCount]` (3497) | | |
| `-[ShipEntity passengerCount]` (3502) | | |
| `-[ShipEntity passengerCapacity]` (3507) | | |
| `-[ShipEntity missileCount]` (3512) | | |
| `-[ShipEntity missileCapacity]` (3517) | | |
| `-[ShipEntity extraCargo]` (3522) | | |
| `-[ShipEntity hasScoop]` (3528) | | |
| `-[ShipEntity hasFuelScoop]` (3533) | | |
| `-[ShipEntity hasCargoScoop]` (3539) | | |
| `-[ShipEntity hasECM]` (3544) | | |
| `-[ShipEntity hasCloakingDevice]` (3549) | | |
| `-[ShipEntity hasMilitaryScannerFilter]` (3555) | | |
| `-[ShipEntity hasMilitaryJammer]` (3564) | | |
| `-[ShipEntity hasExpandedCargoBay]` (3573) | | |
| `-[ShipEntity hasShieldBooster]` (3579) | | |
| `-[ShipEntity hasMilitaryShieldEnhancer]` (3585) | | |
| `-[ShipEntity hasHeatShield]` (3591) | | |
| `-[ShipEntity hasFuelInjection]` (3596) | | |
| `-[ShipEntity hasCascadeMine]` (3601) | | |
| `-[ShipEntity hasEscapePod]` (3609) | | |
| `-[ShipEntity hasDockingComputer]` (3614) | | |
| `-[ShipEntity hasGalacticHyperdrive]` (3619) | | |
| `-[ShipEntity shieldBoostFactor]` (3624) | | |
| `-[ShipEntity maxForwardShieldLevel]` (3637) | | |
| `-[ShipEntity maxAftShieldLevel]` (3642) | | |
| `-[ShipEntity shieldRechargeRate]` (3647) | | |
| `-[ShipEntity maxHyperspaceDistance]` (3652) | | |
| `-[ShipEntity afterburnerFactor]` (3657) | | |
| `-[ShipEntity afterburnerRate]` (3662) | | |
| `-[ShipEntity setAfterburnerFactor:]` (3667) | | |
| `-[ShipEntity setAfterburnerRate:]` (3672) | | |
| `-[ShipEntity maxThrust]` (3677) | | |
| `-[ShipEntity setMaxThrust:]` (3682) | | |
| `-[ShipEntity thrust]` (3687) | | |
| `-[ShipEntity behaviour_stop_still:]` (3696) | | |
| `-[ShipEntity behaviour_idle:]` (3704) | | |
| `-[ShipEntity behaviour_tumble:]` (3720) | | |
| `-[ShipEntity behaviour_tractored:]` (3725) | | |
| `-[ShipEntity behaviour_track_target:]` (3801) | | |
| `-[ShipEntity behaviour_intercept_target:]` (3813) | | |
| `-[ShipEntity behaviour_attack_break_off_target:]` (3882) | | |
| `-[ShipEntity behaviour_attack_slow_dogfight:]` (3940) | | |
| `-[ShipEntity behaviour_evasive_action:]` (3992) | | |
| `-[ShipEntity behaviour_attack_target:]` (4034) | | |
| `-[ShipEntity behaviour_attack_broadside:]` (4183) | | |
| `-[ShipEntity behaviour_attack_broadside_left:]` (4231) | | |
| `-[ShipEntity behaviour_attack_broadside_right:]` (4236) | | |
| `-[ShipEntity behaviour_attack_broadside_target:leftside:]` (4241) | | |
| `-[ShipEntity behaviour_close_to_broadside_range:]` (4334) | | |
| `-[ShipEntity behaviour_close_with_target:]` (4365) | | |
| `-[ShipEntity behaviour_attack_sniper:]` (4393) | | |
| `-[ShipEntity behaviour_fly_to_target_six:]` (4457) | | |
| `-[ShipEntity behaviour_attack_mining_target:]` (4578) | | |
| `-[ShipEntity behaviour_attack_fly_to_target:]` (4607) | | |
| `-[ShipEntity behaviour_attack_fly_from_target:]` (4729) | | |
| `-[ShipEntity behaviour_running_defense:]` (4797) | | |
| `-[ShipEntity behaviour_flee_target:]` (4836) | | |
| `-[ShipEntity behaviour_fly_range_from_destination:]` (4899) | | |
| `-[ShipEntity behaviour_face_destination:]` (4916) | | |
| `-[ShipEntity behaviour_land_on_planet:]` (4963) | | |
| `-[ShipEntity behaviour_formation_form_up:]` (4998) | | |
| `-[ShipEntity behaviour_fly_to_destination:]` (5045) | | |
| `-[ShipEntity behaviour_fly_from_destination:]` (5132) | | |
| `-[ShipEntity behaviour_avoid_collision:]` (5151) | | |
| `-[ShipEntity behaviour_track_as_turret:]` (5169) | | |
| `-[ShipEntity behaviour_fly_thru_navpoints:]` (5219) | | |
| `-[ShipEntity behaviour_scripted_ai:]` (5296) | | |
| `-[ShipEntity reactionTime]` (5397) | | |
| `-[ShipEntity setReactionTime:]` (5402) | | |
| `-[ShipEntity calculateTargetPosition]` (5407) | | |
| `-[ShipEntity startTrackingCurve]` (5420) | | |
| `-[ShipEntity updateTrackingCurve]` (5439) | | |
| `-[ShipEntity calculateTrackingCurve]` (5464) | | |
| `-[ShipEntity drawImmediate:translucent:]` (5486) | | |
| `-[ShipEntity drawDebugStuff]` (5523) | | |
| `-[ShipEntity drawSubEntityImmediate:translucent:]` (5549) | | |
| `-[ShipEntity scannerDisplayColorForShip:isHostile:flash:scannerDisplayColor1:scannerDisplayColor2:scannerDisplayColorH1:scannerDisplayColorH2:]` (5587) | | |
| `-[ShipEntity setScannerDisplayColor1:]` (5685) | | |
| `-[ShipEntity setScannerDisplayColor2:]` (5694) | | |
| `-[ShipEntity scannerDisplayColor1]` (5703) | | |
| `-[ShipEntity scannerDisplayColor2]` (5708) | | |
| `-[ShipEntity setScannerDisplayColorHostile1:]` (5713) | | |
| `-[ShipEntity setScannerDisplayColorHostile2:]` (5722) | | |
| `-[ShipEntity scannerDisplayColorHostile1]` (5731) | | |
| `-[ShipEntity scannerDisplayColorHostile2]` (5736) | | |
| `-[ShipEntity isCloaked]` (5741) | | |
| `-[ShipEntity cloakPassive]` (5746) | | |
| `-[ShipEntity setCloaked:]` (5751) | | |
| `-[ShipEntity hasAutoCloak]` (5759) | | |
| `-[ShipEntity setAutoCloak:]` (5764) | | |
| `-[ShipEntity isJammingScanning]` (5769) | | |
| `-[ShipEntity addSubEntity:]` (5774) | | |
| `-[ShipEntity setOwner:]` (5789) | | |
| `-[ShipEntity applyThrust:]` (5803) | | |
| `-[ShipEntity orientationChanged]` (5851) | | |
| `-[ShipEntity applyRoll:andClimb:]` (5860) | | |
| `-[ShipEntity applyRoll:climb:andYaw:]` (5876) | | |
| `-[ShipEntity applyAttitudeChanges:]` (5894) | | |
| `-[ShipEntity avoidCollision]` (5899) | | |
| `-[ShipEntity resumePostProximityAlert]` (5933) | | |
| `-[ShipEntity messageTime]` (5955) | | |
| `-[ShipEntity setMessageTime:]` (5960) | | |
| `-[ShipEntity group]` (5965) | | |
| `-[ShipEntity setGroup:]` (5970) | | |
| `-[ShipEntity escortGroup]` (5986) | | |
| `-[ShipEntity setEscortGroup:]` (5996) | | |
| `-[ShipEntity rawEscortGroup]` (6007) | | |
| `-[ShipEntity stationGroup]` (6013) | | |
| `-[ShipEntity hasEscorts]` (6023) | | |
| `-[ShipEntity escortEnumerator]` (6030) | | |
| `-[ShipEntity escortArray]` (6037) | | |
| `-[ShipEntity escortCount]` (6044) | | |
| `-[ShipEntity pendingEscortCount]` (6051) | | |
| `-[ShipEntity setPendingEscortCount:]` (6056) | | |
| `-[ShipEntity maxEscortCount]` (6061) | | |
| `-[ShipEntity setMaxEscortCount:]` (6066) | | |
| `-[ShipEntity turretCount]` (6071) | | |
| `-[ShipEntity proximityAlert]` (6084) | | |
| `-[ShipEntity setProximityAlert:]` (6093) | | |
| `-[ShipEntity name]` (6143) | | |
| `-[ShipEntity shipUniqueName]` (6148) | | |
| `-[ShipEntity shipClassName]` (6153) | | |
| `-[ShipEntity displayName]` (6158) | | |
| `-[ShipEntity scanDescriptionForScripting]` (6179) | | |
| `-[ShipEntity scanDescription]` (6184) | | |
| `-[ShipEntity setName:]` (6219) | | |
| `-[ShipEntity setShipUniqueName:]` (6225) | | |
| `-[ShipEntity setShipClassName:]` (6231) | | |
| `-[ShipEntity setDisplayName:]` (6237) | | |
| `-[ShipEntity setScanDescription:]` (6243) | | |
| `-[ShipEntity identFromShip:]` (6249) | | |
| `-[ShipEntity hasRole:]` (6257) | | |
| `-[ShipEntity roleSet]` (6262) | | |
| `-[ShipEntity addRole:]` (6269) | | |
| `-[ShipEntity addRole:withProbability:]` (6274) | | |
| `-[ShipEntity removeRole:]` (6289) | | |
| `-[ShipEntity primaryRole]` (6300) | | |
| `-[ShipEntity setPrimaryRole:]` (6314) | | |
| `-[ShipEntity hasPrimaryRole:]` (6322) | | |
| `-[ShipEntity isPolice]` (6327) | | |
| `-[ShipEntity isThargoid]` (6333) | | |
| `-[ShipEntity isTrader]` (6338) | | |
| `-[ShipEntity isPirate]` (6343) | | |
| `-[ShipEntity isMissile]` (6348) | | |
| `-[ShipEntity isMine]` (6353) | | |
| `-[ShipEntity isWeapon]` (6358) | | |
| `-[ShipEntity isEscort]` (6363) | | |
| `-[ShipEntity isShuttle]` (6368) | | |
| `-[ShipEntity isTurret]` (6373) | | |
| `-[ShipEntity isPirateVictim]` (6378) | | |
| `-[ShipEntity isExplicitlyUnpiloted]` (6383) | | |
| `-[ShipEntity isUnpiloted]` (6388) | | |
| `IsBehaviourHostile()` (6393) | | |
| `-[ShipEntity hasHostileTarget]` (6425) | | |
| `-[ShipEntity isHostileTo:]` (6441) | | |
| `-[ShipEntity weaponRange]` (6446) | | |
| `-[ShipEntity setWeaponRange:]` (6451) | | |
| `-[ShipEntity setWeaponDataFromType:]` (6456) | | |
| `-[ShipEntity energyRechargeRate]` (6472) | | |
| `-[ShipEntity setEnergyRechargeRate:]` (6477) | | |
| `-[ShipEntity weaponRechargeRate]` (6482) | | |
| `-[ShipEntity setWeaponRechargeRate:]` (6487) | | |
| `-[ShipEntity setWeaponEnergy:]` (6492) | | |
| `-[ShipEntity currentWeaponFacing]` (6497) | | |
| `-[ShipEntity scannerRange]` (6502) | | |
| `-[ShipEntity setScannerRange:]` (6507) | | |
| `-[ShipEntity reference]` (6512) | | |
| `-[ShipEntity setReference:]` (6517) | | |
| `-[ShipEntity reportAIMessages]` (6522) | | |
| `-[ShipEntity setReportAIMessages:]` (6527) | | |
| `-[ShipEntity transitionToAegisNone]` (6532) | | |
| `SurfaceDistanceSqaredV()` (6553) | | |
| `SurfaceDistanceSqared()` (6563) | | |
| `ComparePlanetsBySurfaceDistance()` (6568) | | |
| `-[ShipEntity findNearestPlanet]` (6585) | | |
| `-[ShipEntity findNearestStellarBody]` (6614) | | |
| `-[ShipEntity findNearestPlanetExcludingMoons]` (6628) | | |
| `-[ShipEntity checkForAegis]` (6654) | | |
| `-[ShipEntity forceAegisCheck]` (6784) | | |
| `-[ShipEntity withinStationAegis]` (6789) | | |
| `-[ShipEntity lastAegisLock]` (6794) | | |
| `-[ShipEntity setLastAegisLock:]` (6805) | | |
| `-[ShipEntity homeSystem]` (6811) | | |
| `-[ShipEntity destinationSystem]` (6816) | | |
| `-[ShipEntity setHomeSystem:]` (6821) | | |
| `-[ShipEntity setDestinationSystem:]` (6826) | | |
| `-[ShipEntity setStatus:]` (6831) | | |
| `-[ShipEntity setLaunchDelay:]` (6841) | | |
| `-[ShipEntity crew]` (6846) | | |
| `-[ShipEntity setCrew:]` (6851) | | |
| `-[ShipEntity setSingleCrewWithRole:]` (6865) | | |
| `-[ShipEntity crewForScripting]` (6874) | | |
| `-[ShipEntity setStateMachine:]` (6888) | | |
| `-[ShipEntity setAI:]` (6893) | | |
| `-[ShipEntity getAI]` (6903) | | |
| `-[ShipEntity hasAutoAI]` (6908) | | |
| `-[ShipEntity hasNewAI]` (6913) | | |
| `-[ShipEntity hasAutoWeapons]` (6918) | | |
| `-[ShipEntity setShipScript:]` (6923) | | |
| `-[ShipEntity frustration]` (6973) | | |
| `-[ShipEntity fuel]` (6978) | | |
| `-[ShipEntity setFuel:]` (6983) | | |
| `-[ShipEntity fuelCapacity]` (6991) | | |
| `-[ShipEntity fuelChargeRate]` (6997) | | |
| `-[ShipEntity applySticks:]` (7013) | | |
| `-[ShipEntity setRoll:]` (7062) | | |
| `-[ShipEntity setRawRoll:]` (7067) | | |
| `-[ShipEntity setPitch:]` (7072) | | |
| `-[ShipEntity setYaw:]` (7077) | | |
| `-[ShipEntity setThrust:]` (7082) | | |
| `-[ShipEntity setThrustForDemo:]` (7087) | | |
| `-[ShipEntity setBounty:]` (7092) | | |
| `-[ShipEntity setBounty:withReason:]` (7097) | | |
| `-[ShipEntity setBounty:withReasonAsString:]` (7113) | | |
| `-[ShipEntity bounty]` (7133) | | |
| `-[ShipEntity legalStatus]` (7142) | | |
| `-[ShipEntity setCommodity:andAmount:]` (7151) | | |
| `-[ShipEntity setCommodityForPod:andAmount:]` (7164) | | |
| `-[ShipEntity commodityType]` (7184) | | |
| `-[ShipEntity commodityAmount]` (7189) | | |
| `-[ShipEntity maxAvailableCargoSpace]` (7194) | | |
| `-[ShipEntity setMaxAvailableCargoSpace:]` (7199) | | |
| `-[ShipEntity availableCargoSpace]` (7204) | | |
| `-[ShipEntity cargoQuantityOnBoard]` (7212) | | |
| `-[ShipEntity cargoType]` (7219) | | |
| `-[ShipEntity cargo]` (7226) | | |
| `-[ShipEntity cargoListForScripting]` (7231) | | |
| `-[ShipEntity setCargo:]` (7265) | | |
| `-[ShipEntity addCargo:]` (7271) | | |
| `-[ShipEntity removeCargo:amount:]` (7281) | | |
| `-[ShipEntity showScoopMessage]` (7309) | | |
| `-[ShipEntity cargoFlag]` (7314) | | |
| `-[ShipEntity setCargoFlag:]` (7319) | | |
| `-[ShipEntity setSpeed:]` (7371) | | |
| `-[ShipEntity setDesiredSpeed:]` (7376) | | |
| `-[ShipEntity desiredSpeed]` (7381) | | |
| `-[ShipEntity desiredRange]` (7386) | | |
| `-[ShipEntity setDesiredRange:]` (7391) | | |
| `-[ShipEntity cruiseSpeed]` (7396) | | |
| `-[ShipEntity increase_flight_speed:]` (7401) | | |
| `-[ShipEntity decrease_flight_speed:]` (7413) | | |
| `-[ShipEntity increase_flight_roll:]` (7427) | | |
| `-[ShipEntity decrease_flight_roll:]` (7436) | | |
| `-[ShipEntity increase_flight_pitch:]` (7445) | | |
| `-[ShipEntity decrease_flight_pitch:]` (7454) | | |
| `-[ShipEntity increase_flight_yaw:]` (7463) | | |
| `-[ShipEntity decrease_flight_yaw:]` (7472) | | |
| `-[ShipEntity flightRoll]` (7481) | | |
| `-[ShipEntity flightPitch]` (7486) | | |
| `-[ShipEntity flightYaw]` (7491) | | |
| `-[ShipEntity flightSpeed]` (7496) | | |
| `-[ShipEntity maxFlightPitch]` (7501) | | |
| `-[ShipEntity maxFlightSpeed]` (7506) | | |
| `-[ShipEntity maxFlightRoll]` (7511) | | |
| `-[ShipEntity maxFlightYaw]` (7516) | | |
| `-[ShipEntity setMaxFlightPitch:]` (7521) | | |
| `-[ShipEntity setMaxFlightSpeed:]` (7526) | | |
| `-[ShipEntity setMaxFlightRoll:]` (7531) | | |
| `-[ShipEntity setMaxFlightYaw:]` (7536) | | |
| `-[ShipEntity speedFactor]` (7541) | | |
| `-[ShipEntity temperature]` (7548) | | |
| `-[ShipEntity setTemperature:]` (7553) | | |
| `-[ShipEntity randomEjectaTemperature]` (7558) | | |
| `-[ShipEntity randomEjectaTemperatureWithMaxFactor:]` (7563) | | |
| `-[ShipEntity heatInsulation]` (7580) | | |
| `-[ShipEntity setHeatInsulation:]` (7585) | | |
| `-[ShipEntity damage]` (7590) | | |
| `-[ShipEntity dealEnergyDamage:atRange:withBias:]` (7595) | | |
| `-[ShipEntity dealEnergyDamageWithinDesiredRange]` (7658) | | |
| `-[ShipEntity dealMomentumWithinDesiredRange:]` (7676) | | |
| `-[ShipEntity isHulk]` (7698) | | |
| `-[ShipEntity setHulk:]` (7703) | | |
| `-[ShipEntity noteTakingDamage:from:type:]` (7710) | | |
| `-[ShipEntity noteKilledBy:damageType:]` (7740) | | |
| `-[ShipEntity getDestroyedBy:damageType:]` (7764) | | |
| `-[ShipEntity rescaleBy:]` (7771) | | |
| `-[ShipEntity rescaleBy:writeToCache:]` (7776) | | |
| `-[ShipEntity releaseCargoPodsDebris]` (7810) | | |
| `-[ShipEntity setIsWreckage:]` (7877) | | |
| `-[ShipEntity showDamage]` (7882) | | |
| `-[ShipEntity becomeExplosion]` (7887) | | |
| `-[ShipEntity becomeEnergyBlast]` (8142) | | |
| `-[ShipEntity broadcastEnergyBlastImminent]` (8151) | | |
| `-[ShipEntity removeExhaust:]` (8169) | | |
| `-[ShipEntity removeFlasher:]` (8175) | | |
| `-[ShipEntity subEntityDied:]` (8181) | | |
| `-[ShipEntity subEntityReallyDied:]` (8193) | | |
| `-[ShipEntity positionOffsetForAlignment:]` (8206) | | |
| `positionOffsetForShipInRotationToAlignment()` (8249) | | |
| `-[ShipEntity becomeLargeExplosion:]` (8296) | | |
| `-[ShipEntity collectBountyFor:]` (8334) | | |
| `-[ShipEntity compareBeaconCodeWith:]` (8342) | | |
| `-[ShipEntity weaponRecoveryTime]` (8348) | | |
| `-[ShipEntity laserHeatLevel]` (8354) | | |
| `-[ShipEntity laserHeatLevelAft]` (8360) | | |
| `-[ShipEntity laserHeatLevelForward]` (8366) | | |
| `-[ShipEntity laserHeatLevelPort]` (8383) | | |
| `-[ShipEntity laserHeatLevelStarboard]` (8389) | | |
| `-[ShipEntity hullHeatLevel]` (8395) | | |
| `-[ShipEntity entityPersonality]` (8401) | | |
| `-[ShipEntity entityPersonalityInt]` (8406) | | |
| `-[ShipEntity randomSeedForShaders]` (8411) | | |
| `-[ShipEntity setEntityPersonalityInt:]` (8416) | | |
| `-[ShipEntity setSuppressExplosion:]` (8424) | | |
| `-[ShipEntity resetExhaustPlumes]` (8429) | | |
| `-[ShipEntity checkScanner]` (8444) | | |
| `-[ShipEntity checkScannerIgnoringUnpowered]` (8482) | | |
| `-[ShipEntity scannedShips]` (8524) | | |
| `-[ShipEntity numberOfScannedShips]` (8530) | | |
| `-[ShipEntity foundTarget]` (8535) | | |
| `-[ShipEntity setFoundTarget:]` (8545) | | |
| `-[ShipEntity primaryAggressor]` (8551) | | |
| `-[ShipEntity setPrimaryAggressor:]` (8561) | | |
| `-[ShipEntity lastEscortTarget]` (8567) | | |
| `-[ShipEntity setLastEscortTarget:]` (8577) | | |
| `-[ShipEntity thankedShip]` (8583) | | |
| `-[ShipEntity setThankedShip:]` (8593) | | |
| `-[ShipEntity rememberedShip]` (8599) | | |
| `-[ShipEntity setRememberedShip:]` (8609) | | |
| `-[ShipEntity targetStation]` (8615) | | |
| `-[ShipEntity setTargetStation:]` (8625) | | |
| `-[ShipEntity isValidTarget:]` (8634) | | |
| `-[ShipEntity addTarget:]` (8654) | | |
| `-[ShipEntity removeTarget:]` (8669) | | |
| `-[ShipEntity canStillTrackPrimaryTarget]` (8692) | | |
| `-[ShipEntity primaryTarget]` (8714) | | |
| `-[ShipEntity primaryTargetWithoutValidityCheck]` (8733) | | |
| `-[ShipEntity isFriendlyTo:]` (8744) | | |
| `-[ShipEntity shipHitByLaser]` (8757) | | |
| `-[ShipEntity setShipHitByLaser:]` (8762) | | |
| `-[ShipEntity noteLostTarget]` (8770) | | |
| `-[ShipEntity noteLostTargetAndGoIdle]` (8795) | | |
| `-[ShipEntity noteTargetDestroyed:]` (8802) | | |
| `-[ShipEntity behaviour]` (8817) | | |
| `-[ShipEntity setBehaviour:]` (8822) | | |
| `-[ShipEntity destination]` (8830) | | |
| `-[ShipEntity coordinates]` (8835) | | |
| `-[ShipEntity setCoordinate:]` (8840) | | |
| `-[ShipEntity distance_six:]` (8845) | | |
| `-[ShipEntity distance_twelve:withOffset:]` (8854) | | |
| `-[ShipEntity trackOntoTarget:withDForward:]` (8866) | | |
| `-[ShipEntity ballTrackLeadingTarget:atTarget:]` (8903) | | |
| `-[ShipEntity setEvasiveJink:]` (8976) | | |
| `-[ShipEntity evasiveAction:]` (8999) | | |
| `-[ShipEntity trackPrimaryTarget:retreat:]` (9030) | | |
| `-[ShipEntity trackSideTarget:leftside:]` (9242) | | |
| `-[ShipEntity missileTrackPrimaryTarget:]` (9352) | | |
| `-[ShipEntity trackDestination:retreat:]` (9446) | | |
| `-[ShipEntity rollToMatchUp:rotating:]` (9589) | | |
| `-[ShipEntity rangeToDestination]` (9613) | | |
| `-[ShipEntity collisionExceptions]` (9618) | | |
| `-[ShipEntity addCollisionException:]` (9626) | | |
| `-[ShipEntity removeCollisionException:]` (9635) | | |
| `-[ShipEntity collisionExceptedFor:]` (9642) | | |
| `-[ShipEntity defenseTargetCount]` (9650) | | |
| `-[ShipEntity allDefenseTargets]` (9655) | | |
| `-[ShipEntity defenseTargetEnumerator]` (9660) | | |
| `-[ShipEntity addDefenseTarget:]` (9665) | | |
| `-[ShipEntity validateDefenseTargets]` (9683) | | |
| `-[ShipEntity isDefenseTarget:]` (9698) | | |
| `-[ShipEntity removeAllDefenseTargets]` (9704) | | |
| `-[ShipEntity removeDefenseTarget:]` (9709) | | |
| `-[ShipEntity rangeToPrimaryTarget]` (9714) | | |
| `-[ShipEntity rangeToSecondaryTarget:]` (9719) | | |
| `-[ShipEntity approachAspectToPrimaryTarget]` (9732) | | |
| `-[ShipEntity hasProximityAlertIgnoringTarget:]` (9747) | | |
| `-[ShipEntity currentAimTolerance]` (9757) | | |
| `-[ShipEntity lookingAtSunWithThresholdAngleCos:]` (9809) | | |
| `-[ShipEntity onTarget:withWeapon:]` (9847) | | |
| `-[ShipEntity fireWeapon:direction:range:]` (9908) | | |
| `-[ShipEntity fireMainWeapon:]` (10005) | | |
| `-[ShipEntity fireAftWeapon:]` (10044) | | |
| `-[ShipEntity firePortWeapon:]` (10053) | | |
| `-[ShipEntity fireStarboardWeapon:]` (10062) | | |
| `-[ShipEntity shotTime]` (10071) | | |
| `-[ShipEntity resetShotTime]` (10076) | | |
| `-[ShipEntity fireTurretCannon:]` (10081) | | |
| `-[ShipEntity setLaserColor:]` (10132) | | |
| `-[ShipEntity setExhaustEmissiveColor:]` (10140) | | |
| `-[ShipEntity laserColor]` (10148) | | |
| `-[ShipEntity exhaustEmissiveColor]` (10153) | | |
| `-[ShipEntity fireSubentityLaserShot:]` (10158) | | |
| `-[ShipEntity fireDirectLaserShot:]` (10241) | | |
| `-[ShipEntity fireDirectLaserDefensiveShot]` (10253) | | |
| `-[ShipEntity fireDirectLaserShotAt:]` (10273) | | |
| `-[ShipEntity laserPortOffset:]` (10332) | | |
| `-[ShipEntity fireLaserShotInDirection:weaponIdentifier:]` (10356) | | |
| `-[ShipEntity adjustMissedShots:]` (10464) | | |
| `-[ShipEntity missedShots]` (10476) | | |
| `-[ShipEntity throwSparks]` (10485) | | |
| `-[ShipEntity considerFiringMissile:]` (10517) | | |
| `-[ShipEntity missileLaunchPosition]` (10530) | | |
| `-[ShipEntity fireMissile]` (10554) | | |
| `-[ShipEntity fireMissileWithIdentifier:andTarget:]` (10559) | | |
| `-[ShipEntity isMissileFlagSet]` (10689) | | |
| `-[ShipEntity setIsMissileFlag:]` (10694) | | |
| `-[ShipEntity missileLoadTime]` (10699) | | |
| `-[ShipEntity setMissileLoadTime:]` (10704) | | |
| `-[ShipEntity noticeECM]` (10710) | | |
| `-[ShipEntity fireECM]` (10720) | | |
| `-[ShipEntity activateCloakingDevice]` (10731) | | |
| `-[ShipEntity deactivateCloakingDevice]` (10743) | | |
| `-[ShipEntity launchCascadeMine]` (10751) | | |
| `-[ShipEntity launchEscapeCapsule]` (10804) | | |
| `-[ShipEntity dumpCargo]` (10852) | | |
| `-[ShipEntity dumpCargoItem:]` (10862) | | |
| `-[ShipEntity dumpItem:]` (10903) | | |
| `-[ShipEntity manageCollisions]` (10987) | | |
| `-[ShipEntity collideWithShip:]` (11016) | | |
| `-[ShipEntity thrustVector]` (11154) | | |
| `-[ShipEntity velocity]` (11159) | | |
| `-[ShipEntity setTotalVelocity:]` (11164) | | |
| `-[ShipEntity adjustVelocity:]` (11169) | | |
| `-[ShipEntity addImpactMoment:fraction:]` (11174) | | |
| `-[ShipEntity canScoop:]` (11179) | | |
| `-[ShipEntity getTractoredBy:]` (11207) | | |
| `-[ShipEntity scoopIn:]` (11231) | | |
| `-[ShipEntity suppressTargetLost]` (11236) | | |
| `-[ShipEntity scoopUp:]` (11240) | | |
| `-[ShipEntity scoopUpProcess:processEvents:processMessages:]` (11245) | | |
| `-[ShipEntity cascadeIfAppropriateWithDamageAmount:cascadeOwner:]` (11377) | | |
| `-[ShipEntity takeEnergyDamage:from:becauseOf:weaponIdentifier:]` (11414) | | |
| `-[ShipEntity abandonShip]` (11572) | | |
| `-[ShipEntity takeScrapeDamage:from:]` (11637) | | |
| `-[ShipEntity takeHeatDamage:]` (11674) | | |
| `-[ShipEntity enterDock:]` (11702) | | |
| `-[ShipEntity leaveDock:]` (11717) | | |
| `-[ShipEntity enterWormhole:]` (11726) | | |
| `-[ShipEntity enterWormhole:replacing:]` (11731) | | |
| `-[ShipEntity enterWitchspace]` (11761) | | |
| `-[ShipEntity leaveWitchspace]` (11774) | | |
| `-[ShipEntity witchspaceLeavingEffects]` (11796) | | |
| `-[ShipEntity markAsOffender:]` (11823) | | |
| `-[ShipEntity markAsOffender:withReason:]` (11828) | | |
| `-[ShipEntity switchLightsOn]` (11855) | | |
| `-[ShipEntity switchLightsOff]` (11871) | | |
| `-[ShipEntity lightsActive]` (11886) | | |
| `-[ShipEntity setDestination:]` (11891) | | |
| `-[ShipEntity setEscortDestination:]` (11897) | | |
| `-[ShipEntity canAcceptEscort:]` (11902) | | |
| `-[ShipEntity acceptAsEscort:]` (11923) | | |
| `-[ShipEntity updateEscortFormation]` (11983) | | |
| `-[ShipEntity refreshEscortPositions]` (11993) | | |
| `-[ShipEntity coordinatesForEscortPosition:]` (12024) | | |
| `-[ShipEntity deployEscorts]` (12042) | | |
| `-[ShipEntity dockEscorts]` (12099) | | |
| `-[ShipEntity setTargetToNearestStationIncludingHostiles:]` (12133) | | |
| `-[ShipEntity setTargetToNearestFriendlyStation]` (12177) | | |
| `-[ShipEntity setTargetToNearestStation]` (12183) | | |
| `-[ShipEntity setTargetToSystemStation]` (12189) | | |
| `-[ShipEntity landOnPlanet:]` (12214) | | |
| `-[ShipEntity abortDocking]` (12231) | | |
| `-[ShipEntity dockingInstructions]` (12241) | | |
| `-[ShipEntity broadcastThargoidDestroyed]` (12246) | | |
| `AuthorityPredicate()` (12256) | | |
| `-[ShipEntity broadcastHitByLaserFrom:]` (12274) | | |
| `-[ShipEntity sendMessage:toShip:withUnpilotedOverride:]` (12301) | | |
| `-[ShipEntity sendExpandedMessage:toShip:]` (12323) | | |
| `-[ShipEntity broadcastAIMessage:]` (12356) | | |
| `-[ShipEntity broadcastMessage:withUnpilotedOverride:]` (12368) | | |
| `-[ShipEntity setCommsMessageColor]` (12394) | | |
| `-[ShipEntity receiveCommsMessage:from:]` (12404) | | |
| `-[ShipEntity commsMessage:withUnpilotedOverride:]` (12410) | | |
| `-[ShipEntity markedForFines]` (12424) | | |
| `-[ShipEntity markForFines]` (12429) | | |
| `-[ShipEntity isMining]` (12437) | | |
| `-[ShipEntity interpretAIMessage:]` (12442) | | |
| `-[ShipEntity findBoundingBoxRelativeTo:i:j:k:]` (12466) | | |
| `-[ShipEntity spawn:]` (12473) | | |
| `-[ShipEntity checkShipsInVicinityForWitchJumpExit]` (12493) | | |
| `-[ShipEntity trackCloseContacts]` (12533) | | |
| `-[ShipEntity setTrackCloseContacts:]` (12538) | | |
| `-[ShipEntity claimAsSalvage]` (12555) | | |
| `-[ShipEntity sendCoordinatesToPilot]` (12583) | | |
| `-[ShipEntity pilotArrived]` (12624) | | |
| `-[ShipEntity dumpSelfState]` (12632) | | |
| `-[ShipEntity script]` (12709) | | |
| `-[ShipEntity scriptInfo]` (12714) | | |
| `-[ShipEntity overrideScriptInfo:]` (12719) | | |
| `-[ShipEntity entityForShaderProperties]` (12731) | | |
| `-[ShipEntity setDemoShip:]` (12736) | | |
| `-[ShipEntity isDemoShip]` (12745) | | |
| `-[ShipEntity setDemoStartTime:]` (12750) | | |
| `-[ShipEntity getDemoStartTime]` (12755) | | |
| `-[ShipEntity doScriptEvent:]` (12761) | | |
| `-[ShipEntity doScriptEvent:withArgument:]` (12768) | | |
| `-[ShipEntity doScriptEvent:]` (12778) | | |
| `-[ShipEntity doScriptEvent:withArguments:]` (12790) | | |
| `-[ShipEntity doScriptEvent:withArguments:count:]` (12822) | | |
| `-[ShipEntity doScriptEvent:inContext:withArguments:count:]` (12829) | | |
| `-[ShipEntity reactToAIMessage:context:]` (12836) | | |
| `-[ShipEntity sendAIMessage:]` (12841) | | |
| `-[ShipEntity doScriptEvent:andReactToAIMessage:]` (12846) | | |
| `-[ShipEntity doScriptEvent:withArgument:andReactToAIMessage:]` (12852) | | |
| `-[ShipEntity alertCondition]` (12860) | | |
| `-[ShipEntity realAlertCondition]` (12871) | | |
| `-[ShipEntity doNothing]` (12927) | | |
| `-[ShipEntity descriptionForObjDump]` (12932) | | |
| `-[Entity(SubEntityRelationship) isShipWithSubEntityShip:]` (12947) | | |
| `-[Entity(SubEntityRelationship) drawSubEntityImmediate:translucent:]` (12952) | | |
| `-[ShipEntity(SubEntityRelationship) isShipWithSubEntityShip:]` (12961) | | |
| `OODefaultShipShaderMacros()` (12986) | | |
| `OOUniformBindingPermitted()` (12998) | | |
| `getWeaponRangeFromType()` (13033) | | |
| `isWeaponNone()` (13038) | | |

**Differences:**

- *pending*


## ShipEntityAI

<a id="shipentityai"></a>
Upstream `src/Core/Entities/ShipEntityAI.m` ↔ `Core/Entities/ShipEntityAI.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[ShipEntity(AI) setAITo:]` (245) | | |
| `-[ShipEntity(AI) setAIScript:]` (268) | | |
| `-[ShipEntity(AI) switchAITo:]` (287) | | |
| `-[ShipEntity(AI) scanForHostiles]` (293) | | |
| `-[ShipEntity(AI) groupAttackTarget]` (315) | | |
| `-[ShipEntity(AI) performAttack]` (350) | | |
| `-[ShipEntity(AI) performCollect]` (359) | | |
| `-[ShipEntity(AI) performEscort]` (365) | | |
| `-[ShipEntity(AI) performFaceDestination]` (373) | | |
| `-[ShipEntity(AI) performFlee]` (379) | | |
| `-[ShipEntity(AI) performFlyToRangeFromDestination]` (394) | | |
| `-[ShipEntity(AI) performHold]` (400) | | |
| `-[ShipEntity(AI) performIdle]` (407) | | |
| `-[ShipEntity(AI) performIntercept]` (413) | | |
| `-[ShipEntity(AI) performLandOnPlanet]` (419) | | |
| `-[ShipEntity(AI) performMining]` (434) | | |
| `-[ShipEntity(AI) performScriptedAI]` (446) | | |
| `-[ShipEntity(AI) performScriptedAttackAI]` (452) | | |
| `-[ShipEntity(AI) performBuoyTumble]` (458) | | |
| `-[ShipEntity(AI) performStop]` (466) | | |
| `-[ShipEntity(AI) performTumble]` (473) | | |
| `-[ShipEntity(AI) performHyperSpaceToSpecificSystem:]` (481) | | |
| `-[ShipEntity(AI) requestDockingCoordinates]` (486) | | |
| `-[ShipEntity(AI) recallDockingInstructions]` (536) | | |
| `-[ShipEntity(AI) scanForNearestIncomingMissile]` (555) | | |
| `-[ShipEntity(AI) enterPlayerWormhole]` (564) | | |
| `-[ShipEntity(AI) enterTargetWormhole]` (569) | | |
| `-[ShipEntity(AI) wormholeEscorts]` (609) | | |
| `-[ShipEntity(AI) wormholeEntireGroup]` (635) | | |
| `-[ShipEntity(AI) suggestEscortTo:]` (641) | | |
| `-[ShipEntity(AI) broadcastDistressMessage]` (677) | | |
| `-[ShipEntity(AI) broadcastDistressMessageWithDumping:]` (683) | | |
| `-[ShipEntity(PureAI) setStateTo:]` (752) | | |
| `-[ShipEntity(PureAI) pauseAI:]` (757) | | |
| `-[ShipEntity(PureAI) randomPauseAI:]` (762) | | |
| `-[ShipEntity(PureAI) dropMessages:]` (778) | | |
| `-[ShipEntity(PureAI) debugDumpPendingMessages]` (790) | | |
| `-[ShipEntity(PureAI) setDestinationToCurrentLocation]` (795) | | |
| `-[ShipEntity(PureAI) setDestinationToJinkPosition]` (801) | | |
| `-[ShipEntity(PureAI) setDesiredRangeTo:]` (808) | | |
| `-[ShipEntity(PureAI) setDesiredRangeForWaypoint]` (813) | | |
| `-[ShipEntity(PureAI) setSpeedTo:]` (818) | | |
| `-[ShipEntity(PureAI) setSpeedFactorTo:]` (823) | | |
| `-[ShipEntity(PureAI) setSpeedToCruiseSpeed]` (828) | | |
| `-[ShipEntity(PureAI) setThrustFactorTo:]` (833) | | |
| `-[ShipEntity(PureAI) setTargetToPrimaryAggressor]` (838) | | |
| `-[ShipEntity(PureAI) addPrimaryAggressorAsDefenseTarget]` (871) | | |
| `-[ShipEntity(PureAI) scanForNearestMerchantman]` (884) | | |
| `-[ShipEntity(PureAI) scanForRandomMerchantman]` (913) | | |
| `-[ShipEntity(PureAI) scanForLoot]` (937) | | |
| `-[ShipEntity(PureAI) scanForRandomLoot]` (979) | | |
| `-[ShipEntity(PureAI) setTargetToFoundTarget]` (1007) | | |
| `-[ShipEntity(PureAI) addFoundTargetAsDefenseTarget]` (1016) | | |
| `-[ShipEntity(PureAI) checkForFullHold]` (1026) | | |
| `-[ShipEntity(PureAI) getWitchspaceEntryCoordinates]` (1037) | | |
| `-[ShipEntity(PureAI) setDestinationFromCoordinates]` (1065) | | |
| `-[ShipEntity(PureAI) setCoordinatesFromPosition]` (1070) | | |
| `-[ShipEntity(PureAI) fightOrFleeMissile]` (1075) | | |
| `-[ShipEntity(PureAI) setCourseToPlanet]` (1144) | | |
| `-[ShipEntity(PureAI) setTakeOffFromPlanet]` (1165) | | |
| `-[ShipEntity(PureAI) landOnPlanet]` (1177) | | |
| `-[ShipEntity(PureAI) checkTargetLegalStatus]` (1183) | | |
| `-[ShipEntity(PureAI) checkOwnLegalStatus]` (1207) | | |
| `-[ShipEntity(PureAI) exitAIWithMessage:]` (1229) | | |
| `-[ShipEntity(PureAI) setDestinationToTarget]` (1236) | | |
| `-[ShipEntity(PureAI) setDestinationWithinTarget]` (1243) | | |
| `-[ShipEntity(PureAI) checkCourseToDestination]` (1256) | | |
| `-[ShipEntity(PureAI) checkAegis]` (1272) | | |
| `-[ShipEntity(PureAI) checkEnergy]` (1308) | | |
| `-[ShipEntity(PureAI) checkHeatInsulation]` (1325) | | |
| `-[ShipEntity(PureAI) findNewDefenseTarget]` (1336) | | |
| `-[ShipEntity(PureAI) scanForOffenders]` (1351) | | |
| `-[ShipEntity(PureAI) setCourseToWitchpoint]` (1388) | | |
| `-[ShipEntity(PureAI) setDestinationToWitchpoint]` (1396) | | |
| `-[ShipEntity(PureAI) setDestinationToStationBeacon]` (1401) | | |
| `-[ShipEntity(PureAI) performHyperSpaceExit]` (1408) | | |
| `-[ShipEntity(PureAI) performHyperSpaceExitWithoutReplacing]` (1413) | | |
| `-[ShipEntity(PureAI) disengageAutopilot]` (1418) | | |
| `-[ShipEntity(PureAI) wormholeGroup]` (1423) | | |
| `-[ShipEntity(PureAI) commsMessage:]` (1439) | | |
| `-[ShipEntity(PureAI) commsMessageByUnpiloted:]` (1444) | | |
| `-[ShipEntity(PureAI) ejectCargo]` (1449) | | |
| `-[ShipEntity(PureAI) scanForThargoid]` (1461) | | |
| `-[ShipEntity(PureAI) scanForNonThargoid]` (1466) | | |
| `-[ShipEntity(PureAI) thargonCheckMother]` (1488) | | |
| `-[ShipEntity(PureAI) becomeUncontrolledThargon]` (1512) | | |
| `-[ShipEntity(PureAI) checkDistanceTravelled]` (1536) | | |
| `-[ShipEntity(PureAI) fightOrFleeHostiles]` (1542) | | |
| `-[ShipEntity(PureAI) suggestEscort]` (1585) | | |
| `-[ShipEntity(PureAI) escortCheckMother]` (1591) | | |
| `-[ShipEntity(PureAI) checkGroupOddsVersusTarget]` (1607) | | |
| `-[ShipEntity(PureAI) scanForFormationLeader]` (1621) | | |
| `-[ShipEntity(PureAI) messageMother:]` (1652) | | |
| `-[ShipEntity(PureAI) messageSelf:]` (1665) | | |
| `-[ShipEntity(PureAI) setPlanetPatrolCoordinates]` (1670) | | |
| `-[ShipEntity(PureAI) setSunSkimStartCoordinates]` (1732) | | |
| `-[ShipEntity(PureAI) setSunSkimEndCoordinates]` (1749) | | |
| `-[ShipEntity(PureAI) setSunSkimExitCoordinates]` (1760) | | |
| `-[ShipEntity(PureAI) patrolReportIn]` (1779) | | |
| `-[ShipEntity(PureAI) checkForMotherStation]` (1788) | | |
| `-[ShipEntity(PureAI) sendTargetCommsMessage:]` (1804) | | |
| `-[ShipEntity(PureAI) markTargetForFines]` (1814) | | |
| `-[ShipEntity(PureAI) markTargetForOffence:]` (1825) | | |
| `-[ShipEntity(PureAI) storeTarget]` (1838) | | |
| `-[ShipEntity(PureAI) recallStoredTarget]` (1849) | | |
| `-[ShipEntity(PureAI) scanForRocks]` (1871) | | |
| `-[ShipEntity(PureAI) setDestinationToDockingAbort]` (1907) | | |
| `-[ShipEntity(PureAI) requestNewTarget]` (1936) | | |
| `-[ShipEntity(PureAI) rollD:]` (1965) | | |
| `-[ShipEntity(PureAI) scanForNearestShipWithPrimaryRole:]` (1977) | | |
| `-[ShipEntity(PureAI) scanForNearestShipHavingRole:]` (1982) | | |
| `-[ShipEntity(PureAI) scanForNearestShipWithAnyPrimaryRole:]` (1987) | | |
| `-[ShipEntity(PureAI) scanForNearestShipHavingAnyRole:]` (1993) | | |
| `-[ShipEntity(PureAI) scanForNearestShipWithScanClass:]` (1999) | | |
| `-[ShipEntity(PureAI) scanForNearestShipWithoutPrimaryRole:]` (2005) | | |
| `-[ShipEntity(PureAI) scanForNearestShipNotHavingRole:]` (2010) | | |
| `-[ShipEntity(PureAI) scanForNearestShipWithoutAnyPrimaryRole:]` (2015) | | |
| `-[ShipEntity(PureAI) scanForNearestShipNotHavingAnyRole:]` (2021) | | |
| `-[ShipEntity(PureAI) scanForNearestShipWithoutScanClass:]` (2027) | | |
| `-[ShipEntity(PureAI) scanForNearestShipMatchingPredicate:]` (2033) | | |
| `-[ShipEntity(PureAI) setCoordinates:]` (2126) | | |
| `-[ShipEntity(PureAI) checkForNormalSpace]` (2158) | | |
| `-[ShipEntity(PureAI) setTargetToRandomStation]` (2166) | | |
| `-[ShipEntity(PureAI) setTargetToLastStation]` (2210) | | |
| `-[ShipEntity(PureAI) addFuel:]` (2222) | | |
| `-[ShipEntity(PureAI) scriptActionOnTarget:]` (2227) | | |
| `-[ShipEntity(PureAI) safeScriptActionOnTarget:]` (2256) | | |
| `-[ShipEntity(PureAI) sendScriptMessage:]` (2274) | | |
| `-[ShipEntity(PureAI) ai_throwSparks]` (2287) | | |
| `-[ShipEntity(PureAI) explodeSelf]` (2292) | | |
| `-[ShipEntity(PureAI) ai_debugMessage:]` (2297) | | |
| `-[ShipEntity(PureAI) targetFirstBeaconWithCode:]` (2306) | | |
| `-[ShipEntity(PureAI) targetNextBeaconWithCode:]` (2316) | | |
| `-[ShipEntity(PureAI) setRacepointsFromTarget]` (2347) | | |
| `-[ShipEntity(PureAI) performFlyRacepoints]` (2367) | | |
| `-[ShipEntity(OOAIPrivate) checkFoundTarget]` (2378) | | |
| `-[ShipEntity(OOAIPrivate) performHyperSpaceExitReplace:]` (2387) | | |
| `-[ShipEntity(OOAIPrivate) performHyperSpaceExitReplace:toSystem:]` (2392) | | |
| `-[ShipEntity(OOAIPrivate) scanForNearestShipWithPredicate:parameter:]` (2466) | | |
| `-[ShipEntity(OOAIPrivate) scanForNearestShipWithNegatedPredicate:parameter:]` (2492) | | |
| `-[ShipEntity(OOAIPrivate) acceptDistressMessageFrom:]` (2498) | | |
| `-[StationEntity(OOAIPrivate) acceptDistressMessageFrom:]` (2517) | | |
| `-[ShipEntity(OOAIStationStubs) launchPatrol]` (2550) | | |

**Differences:**

- *pending*


## ShipEntityLoadRestore

<a id="shipentityloadrestore"></a>
Upstream `src/Core/Entities/ShipEntityLoadRestore.m` ↔ `Core/Entities/ShipEntityLoadRestore.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[ShipEntity(LoadRestore) savedShipDictionaryWithContext:]` (75) | | |
| `+[ShipEntity(LoadRestore) shipRestoredFromDictionary:]` (163) | | |
| `-[ShipEntity(LoadRestore) simplifyShipdata:andGetDeletes:]` (251) | | |
| `StripIgnoredKeys()` (292) | | |
| `GroupIDForGroup()` (304) | | |
| `GroupForGroupID()` (341) | | |

**Differences:**

- *pending*


## ShipEntityScriptMethods

<a id="shipentityscriptmethods"></a>
Upstream `src/Core/Entities/ShipEntityScriptMethods.m` ↔ `Core/Entities/ShipEntityScriptMethods.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[ShipEntity(ScriptMethods) ejectShipOfType:]` (34) | | |
| `-[ShipEntity(ScriptMethods) ejectShipOfRole:]` (47) | | |
| `-[ShipEntity(ScriptMethods) spawnShipsWithRole:count:]` (60) | | |

**Differences:**

- *pending*


## StationEntity

<a id="stationentity"></a>
Upstream `src/Core/Entities/StationEntity.m` ↔ `Core/Entities/StationEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[StationEntity isUnpiloted]` (76) | | |
| `-[StationEntity equivalentTechLevel]` (81) | | |
| `-[StationEntity setEquivalentTechLevel:]` (90) | | |
| `-[StationEntity virtualPortDimensions]` (95) | | |
| `-[StationEntity playerReservedDock]` (100) | | |
| `-[StationEntity beaconPosition]` (105) | | |
| `-[StationEntity equipmentPriceFactor]` (114) | | |
| `-[StationEntity marketCapacity]` (119) | | |
| `-[StationEntity marketDefinition]` (124) | | |
| `-[StationEntity marketScriptName]` (129) | | |
| `-[StationEntity marketMonitored]` (134) | | |
| `-[StationEntity marketBroadcast]` (142) | | |
| `-[StationEntity legalStatusOfManifest:export:]` (150) | | |
| `-[StationEntity localMarket]` (166) | | |
| `-[StationEntity setLocalMarket:]` (179) | | |
| `-[StationEntity localMarketForScripting]` (184) | | |
| `-[StationEntity setPrice:forCommodity:]` (189) | | |
| `-[StationEntity setQuantity:forCommodity:]` (194) | | |
| `-[StationEntity localShipyard]` (199) | | |
| `-[StationEntity setLocalShipyard:]` (204) | | |
| `-[StationEntity localInterfaces]` (211) | | |
| `-[StationEntity setInterfaceDefinition:forKey:]` (216) | | |
| `-[StationEntity initialiseLocalMarket]` (225) | | |
| `-[StationEntity setPlanet:]` (232) | | |
| `-[StationEntity planet]` (240) | | |
| `-[StationEntity countOfDockedContractors]` (245) | | |
| `-[StationEntity countOfDockedPolice]` (250) | | |
| `-[StationEntity countOfDockedDefenders]` (255) | | |
| `-[StationEntity dockSubEntityEnumerator]` (260) | | |
| `-[StationEntity sanityCheckShipsOnApproach]` (265) | | |
| `-[StationEntity launchShip:]` (283) | | |
| `-[StationEntity abortAllDockings]` (312) | | |
| `-[StationEntity autoDockShipsOnHold]` (343) | | |
| `-[StationEntity autoDockShipsOnApproach]` (354) | | |
| `-[StationEntity portUpVectorForShip:]` (368) | | |
| `OOMakeDockingInstructions()` (380) | | |
| `-[StationEntity dockingInstructionsForShip:]` (401) | | |
| `-[StationEntity holdPositionInstructionForShip:]` (503) | | |
| `-[StationEntity abortDockingForShip:]` (513) | | |
| `-[StationEntity initWithKey:definition:]` (535) | | |
| `-[StationEntity dealloc]` (551) | | |
| `-[StationEntity setUpShipFromDictionary:]` (566) | | |
| `-[StationEntity setUpSubEntities]` (655) | | |
| `-[StationEntity shipIsInDockingCorridor:]` (700) | | |
| `-[StationEntity pullInShipIfPermitted:]` (717) | | |
| `-[StationEntity dockingCorridorIsEmpty]` (722) | | |
| `-[StationEntity clearDockingCorridor]` (737) | | |
| `-[StationEntity update:]` (751) | | |
| `-[StationEntity clear]` (853) | | |
| `-[StationEntity hasMultipleDocks]` (864) | | |
| `-[StationEntity hasClearDock]` (880) | | |
| `-[StationEntity hasEligibleDock]` (894) | | |
| `-[StationEntity hasLaunchDock]` (908) | | |
| `-[StationEntity selectDockForDocking]` (921) | | |
| `-[StationEntity addShipToLaunchQueue:withPriority:]` (933) | | |
| `-[StationEntity countOfShipsInLaunchQueueWithPrimaryRole:]` (984) | | |
| `-[StationEntity fitsInDock:]` (995) | | |
| `-[StationEntity fitsInDock:andLogNoFit:]` (1000) | | |
| `-[StationEntity noteDockedShip:]` (1019) | | |
| `-[StationEntity addShipToStationCount:]` (1065) | | |
| `-[StationEntity interstellarUndockingAllowed]` (1081) | | |
| `-[StationEntity hasNPCTraffic]` (1086) | | |
| `-[StationEntity setHasNPCTraffic:]` (1091) | | |
| `-[StationEntity collideWithShip:]` (1096) | | |
| `-[StationEntity hasHostileTarget]` (1108) | | |
| `-[StationEntity takeEnergyDamage:from:becauseOf:weaponIdentifier:]` (1113) | | |
| `-[StationEntity adjustVelocity:]` (1156) | | |
| `-[StationEntity takeScrapeDamage:from:]` (1162) | | |
| `-[StationEntity takeHeatDamage:]` (1169) | | |
| `-[StationEntity allegiance]` (1176) | | |
| `-[StationEntity setAllegiance:]` (1181) | | |
| `-[StationEntity alertLevel]` (1187) | | |
| `-[StationEntity setAlertLevel:signallingScript:]` (1192) | | |
| `-[StationEntity launchIndependentShip:]` (1222) | | |
| `-[StationEntity increaseAlertLevel]` (1297) | | |
| `-[StationEntity decreaseAlertLevel]` (1303) | | |
| `-[StationEntity launchPolice]` (1309) | | |
| `-[StationEntity launchDefenseShip]` (1366) | | |
| `-[StationEntity launchScavenger]` (1461) | | |
| `-[StationEntity launchMiner]` (1503) | | |
| `-[StationEntity launchPirateShip]` (1549) | | |
| `-[StationEntity launchShuttle]` (1603) | | |
| `-[StationEntity launchEscort]` (1636) | | |
| `-[StationEntity launchPatrol]` (1662) | | |
| `-[StationEntity launchShipWithRole:]` (1712) | | |
| `-[StationEntity becomeExplosion]` (1734) | | |
| `-[StationEntity becomeEnergyBlast]` (1761) | | |
| `-[StationEntity becomeLargeExplosion:]` (1768) | | |
| `-[StationEntity acceptPatrolReportFrom:]` (1775) | | |
| `-[StationEntity acceptDockingClearanceRequestFrom:]` (1782) | | |
| `-[StationEntity currentlyInDockingQueues]` (1974) | | |
| `-[StationEntity currentlyInLaunchingQueues]` (1986) | | |
| `-[StationEntity requiresDockingClearance]` (1997) | | |
| `-[StationEntity setRequiresDockingClearance:]` (2002) | | |
| `-[StationEntity allowsFastDocking]` (2007) | | |
| `-[StationEntity setAllowsFastDocking:]` (2012) | | |
| `-[StationEntity allowsAutoDocking]` (2017) | | |
| `-[StationEntity setAllowsAutoDocking:]` (2022) | | |
| `-[StationEntity allowsSaving]` (2027) | | |
| `-[StationEntity isRotatingStation]` (2033) | | |
| `-[StationEntity marketOverrideName]` (2040) | | |
| `-[StationEntity hasShipyard]` (2050) | | |
| `-[StationEntity generateShipyard]` (2071) | | |
| `-[StationEntity generateShipyard:]` (2076) | | |
| `-[StationEntity suppressArrivalReports]` (2095) | | |
| `-[StationEntity setSuppressArrivalReports:]` (2100) | | |
| `-[StationEntity hasBreakPattern]` (2105) | | |
| `-[StationEntity setHasBreakPattern:]` (2110) | | |
| `-[StationEntity descriptionComponents]` (2115) | | |
| `-[StationEntity dumpSelfState]` (2120) | | |

**Differences:**

- *pending*

