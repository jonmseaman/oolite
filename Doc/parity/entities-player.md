# Parity findings: Player entity and its categories

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## PlayerEntity

<a id="playerentity"></a>
Upstream `src/Core/Entities/PlayerEntity.m` ↔ `Core/Entities/PlayerEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[PlayerEntity sharedPlayer]` (168) | | |
| `-[PlayerEntity setName:]` (176) | | |
| `-[PlayerEntity baseMass]` (181) | | |
| `-[PlayerEntity unloadAllCargoPodsForType:toManifest:]` (199) | | |
| `-[PlayerEntity unloadCargoPodsForType:amount:]` (223) | | |
| `-[PlayerEntity unloadCargoPods]` (263) | | |
| `-[PlayerEntity createCargoPodWithType:andAmount:]` (282) | | |
| `-[PlayerEntity loadCargoPodsForType:fromManifest:]` (299) | | |
| `-[PlayerEntity loadCargoPodsForType:amount:]` (359) | | |
| `-[PlayerEntity loadCargoPods]` (410) | | |
| `-[PlayerEntity shipCommodityData]` (421) | | |
| `-[PlayerEntity deciCredits]` (426) | | |
| `-[PlayerEntity random_factor]` (431) | | |
| `-[PlayerEntity setRandom_factor:]` (436) | | |
| `-[PlayerEntity galaxyNumber]` (441) | | |
| `-[PlayerEntity galaxy_coordinates]` (446) | | |
| `-[PlayerEntity setGalaxyCoordinates:]` (451) | | |
| `-[PlayerEntity cursor_coordinates]` (457) | | |
| `-[PlayerEntity chart_centre_coordinates]` (462) | | |
| `-[PlayerEntity chart_zoom]` (467) | | |
| `-[PlayerEntity custom_chart_zoom]` (479) | | |
| `-[PlayerEntity setCustomChartZoom:]` (484) | | |
| `-[PlayerEntity custom_chart_centre_coordinates]` (489) | | |
| `-[PlayerEntity setCustomChartCentre:]` (494) | | |
| `-[PlayerEntity adjusted_chart_centre]` (500) | | |
| `-[PlayerEntity ANAMode]` (558) | | |
| `-[PlayerEntity systemID]` (563) | | |
| `-[PlayerEntity setSystemID:]` (568) | | |
| `-[PlayerEntity previousSystemID]` (576) | | |
| `-[PlayerEntity setPreviousSystemID:]` (581) | | |
| `-[PlayerEntity targetSystemID]` (586) | | |
| `-[PlayerEntity setTargetSystemID:]` (591) | | |
| `-[PlayerEntity nextHopTargetSystemID]` (598) | | |
| `-[PlayerEntity infoSystemID]` (621) | | |
| `-[PlayerEntity setInfoSystemID:moveChart:]` (626) | | |
| `-[PlayerEntity nextInfoSystem]` (653) | | |
| `-[PlayerEntity previousInfoSystem]` (680) | | |
| `-[PlayerEntity homeInfoSystem]` (707) | | |
| `-[PlayerEntity targetInfoSystem]` (713) | | |
| `-[PlayerEntity infoSystemOnRoute]` (719) | | |
| `-[PlayerEntity wormhole]` (734) | | |
| `-[PlayerEntity setWormhole:]` (739) | | |
| `-[PlayerEntity commanderDataDictionary]` (749) | | |
| `-[PlayerEntity setCommanderDataFromDictionary:]` (1012) | | |
| `-[PlayerEntity init]` (1595) | | |
| `-[PlayerEntity deferredInit]` (1601) | | |
| `-[PlayerEntity setUpAndConfirmOK:]` (1639) | | |
| `-[PlayerEntity setUpAndConfirmOK:saveGame:]` (1644) | | |
| `-[PlayerEntity completeSetUp]` (2022) | | |
| `-[PlayerEntity completeSetUpAndSetTarget:]` (2027) | | |
| `-[PlayerEntity startUpComplete]` (2043) | | |
| `-[PlayerEntity setUpShipFromDictionary:]` (2050) | | |
| `-[PlayerEntity dealloc]` (2145) | | |
| `-[PlayerEntity sessionID]` (2231) | | |
| `-[PlayerEntity warnAboutHostiles]` (2237) | | |
| `-[PlayerEntity canCollide]` (2242) | | |
| `-[PlayerEntity compareZeroDistance:]` (2257) | | |
| `-[PlayerEntity validForAddToUniverse]` (2262) | | |
| `-[PlayerEntity lookingAtSunWithThresholdAngleCos:]` (2267) | | |
| `-[PlayerEntity insideAtmosphereFraction]` (2342) | | |
| `-[PlayerEntity update:]` (2380) | | |
| `-[PlayerEntity doBookkeeping:]` (2461) | | |
| `-[PlayerEntity updateMovementFlags]` (2885) | | |
| `-[PlayerEntity updateAlertConditionForNearbyEntities]` (2893) | | |
| `-[PlayerEntity setMaxFlightPitch:]` (2996) | | |
| `-[PlayerEntity setMaxFlightRoll:]` (3002) | | |
| `-[PlayerEntity setMaxFlightYaw:]` (3008) | | |
| `-[PlayerEntity checkEntityForMassLock:withScanClass:]` (3014) | | |
| `-[PlayerEntity updateAlertCondition]` (3068) | | |
| `-[PlayerEntity updateFuelScoops:]` (3099) | | |
| `-[PlayerEntity updateClocks:]` (3110) | | |
| `-[PlayerEntity checkScriptsIfAppropriate]` (3160) | | |
| `-[PlayerEntity updateTrumbles:]` (3211) | | |
| `-[PlayerEntity performAutopilotUpdates:]` (3222) | | |
| `-[PlayerEntity performDockingRequest:]` (3229) | | |
| `-[PlayerEntity requestDockingClearance:]` (3251) | | |
| `-[PlayerEntity cancelDockingRequest:]` (3258) | | |
| `-[PlayerEntity engageAutopilotToStation:]` (3279) | | |
| `-[PlayerEntity disengageAutopilot]` (3314) | | |
| `-[PlayerEntity resetAutopilotAI]` (3333) | | |
| `-[PlayerEntity hyperspeedFactor]` (3351) | | |
| `-[PlayerEntity injectorsEngaged]` (3357) | | |
| `-[PlayerEntity hyperspeedEngaged]` (3362) | | |
| `-[PlayerEntity performInFlightUpdates:]` (3367) | | |
| `-[PlayerEntity performWitchspaceCountdownUpdates:]` (3422) | | |
| `-[PlayerEntity performWitchspaceExitUpdates:]` (3479) | | |
| `-[PlayerEntity performLaunchingUpdates:]` (3519) | | |
| `-[PlayerEntity performDockingUpdates:]` (3539) | | |
| `-[PlayerEntity performDeadUpdates:]` (3551) | | |
| `-[PlayerEntity gameOverFadeToBW]` (3566) | | |
| `-[PlayerEntity isValidTarget:]` (3595) | | |
| `-[PlayerEntity showGameOver]` (3630) | | |
| `-[PlayerEntity showShipModelWithKey:shipData:personality:factorX:factorY:factorZ:inContext:]` (3646) | | |
| `-[PlayerEntity updateTargeting]` (3699) | | |
| `-[PlayerEntity orientationChanged]` (3803) | | |
| `-[PlayerEntity applyAttitudeChanges:]` (3814) | | |
| `-[PlayerEntity applyRoll:andClimb:]` (3820) | | |
| `-[PlayerEntity applyYaw:]` (3849) | | |
| `-[PlayerEntity drawRotationMatrix]` (3856) | | |
| `-[PlayerEntity drawTransformationMatrix]` (3861) | | |
| `-[PlayerEntity normalOrientation]` (3868) | | |
| `-[PlayerEntity setNormalOrientation:]` (3873) | | |
| `-[PlayerEntity moveForward:]` (3878) | | |
| `-[PlayerEntity breakPatternPosition]` (3884) | | |
| `-[PlayerEntity viewpointOffset]` (3889) | | |
| `-[PlayerEntity viewpointOffsetAft]` (3916) | | |
| `-[PlayerEntity viewpointOffsetForward]` (3921) | | |
| `-[PlayerEntity viewpointOffsetPort]` (3926) | | |
| `-[PlayerEntity viewpointOffsetStarboard]` (3931) | | |
| `-[PlayerEntity viewpointPosition]` (3938) | | |
| `-[PlayerEntity drawImmediate:translucent:]` (3962) | | |
| `-[PlayerEntity setMassLockable:]` (3979) | | |
| `-[PlayerEntity massLockable]` (3985) | | |
| `-[PlayerEntity massLocked]` (3990) | | |
| `-[PlayerEntity atHyperspeed]` (3995) | | |
| `-[PlayerEntity occlusionLevel]` (4000) | | |
| `-[PlayerEntity setOcclusionLevel:]` (4005) | | |
| `-[PlayerEntity setDockedAtMainStation]` (4010) | | |
| `-[PlayerEntity dockedStation]` (4017) | | |
| `-[PlayerEntity setDockedStation:]` (4022) | | |
| `-[PlayerEntity setTargetDockStationTo:]` (4028) | | |
| `-[PlayerEntity getTargetDockStation]` (4033) | | |
| `-[PlayerEntity hud]` (4038) | | |
| `-[PlayerEntity resetHud]` (4043) | | |
| `-[PlayerEntity switchHudTo:]` (4052) | | |
| `-[PlayerEntity dialCustomFloat:]` (4112) | | |
| `-[PlayerEntity dialCustomString:]` (4117) | | |
| `-[PlayerEntity dialCustomColor:]` (4122) | | |
| `-[PlayerEntity setDialCustom:forKey:]` (4127) | | |
| `-[PlayerEntity setShowDemoShips:]` (4132) | | |
| `-[PlayerEntity showDemoShips]` (4137) | | |
| `-[PlayerEntity maxForwardShieldLevel]` (4142) | | |
| `-[PlayerEntity maxAftShieldLevel]` (4147) | | |
| `-[PlayerEntity forwardShieldRechargeRate]` (4152) | | |
| `-[PlayerEntity aftShieldRechargeRate]` (4157) | | |
| `-[PlayerEntity setMaxForwardShieldLevel:]` (4162) | | |
| `-[PlayerEntity setMaxAftShieldLevel:]` (4167) | | |
| `-[PlayerEntity setForwardShieldRechargeRate:]` (4172) | | |
| `-[PlayerEntity setAftShieldRechargeRate:]` (4177) | | |
| `-[PlayerEntity forwardShieldLevel]` (4182) | | |
| `-[PlayerEntity aftShieldLevel]` (4187) | | |
| `-[PlayerEntity setForwardShieldLevel:]` (4192) | | |
| `-[PlayerEntity setAftShieldLevel:]` (4197) | | |
| `-[PlayerEntity keyConfig]` (4202) | | |
| `-[PlayerEntity isMouseControlOn]` (4208) | | |
| `-[PlayerEntity dialRoll]` (4213) | | |
| `-[PlayerEntity dialPitch]` (4223) | | |
| `-[PlayerEntity dialYaw]` (4233) | | |
| `-[PlayerEntity dialSpeed]` (4243) | | |
| `-[PlayerEntity dialHyperSpeed]` (4249) | | |
| `-[PlayerEntity dialForwardShield]` (4254) | | |
| `-[PlayerEntity dialAftShield]` (4263) | | |
| `-[PlayerEntity dialEnergy]` (4272) | | |
| `-[PlayerEntity dialMaxEnergy]` (4278) | | |
| `-[PlayerEntity dialFuel]` (4283) | | |
| `-[PlayerEntity dialHyperRange]` (4292) | | |
| `-[PlayerEntity laserHeatLevel]` (4299) | | |
| `-[PlayerEntity laserHeatLevelAft]` (4305) | | |
| `-[PlayerEntity laserHeatLevelForward]` (4311) | | |
| `-[PlayerEntity laserHeatLevelPort]` (4318) | | |
| `-[PlayerEntity laserHeatLevelStarboard]` (4324) | | |
| `-[PlayerEntity dialAltitude]` (4330) | | |
| `-[PlayerEntity clockTime]` (4349) | | |
| `-[PlayerEntity clockTimeAdjusted]` (4354) | | |
| `-[PlayerEntity clockAdjusting]` (4359) | | |
| `-[PlayerEntity addToAdjustTime:]` (4364) | | |
| `-[PlayerEntity escapePodRescueTime]` (4369) | | |
| `-[PlayerEntity setEscapePodRescueTime:]` (4374) | | |
| `-[PlayerEntity dial_clock]` (4379) | | |
| `-[PlayerEntity dial_clock_adjusted]` (4384) | | |
| `-[PlayerEntity dial_fpsinfo]` (4389) | | |
| `-[PlayerEntity dial_objinfo]` (4395) | | |
| `-[PlayerEntity countMissiles]` (4405) | | |
| `-[PlayerEntity dialMissileStatus]` (4416) | | |
| `-[PlayerEntity canScoop:]` (4430) | | |
| `-[PlayerEntity dialFuelScoopStatus]` (4437) | | |
| `-[PlayerEntity fuelLeakRate]` (4452) | | |
| `-[PlayerEntity setFuelLeakRate:]` (4457) | | |
| `-[PlayerEntity commLog]` (4462) | | |
| `-[PlayerEntity roleWeights]` (4478) | | |
| `-[PlayerEntity addRoleForAggression:]` (4483) | | |
| `-[PlayerEntity addRoleForMining]` (4510) | | |
| `-[PlayerEntity addRoleToPlayer:]` (4522) | | |
| `-[PlayerEntity addRoleToPlayer:inSlot:]` (4528) | | |
| `-[PlayerEntity clearRoleFromPlayer:]` (4540) | | |
| `-[PlayerEntity clearRolesFromPlayer:]` (4553) | | |
| `-[PlayerEntity maxPlayerRoles]` (4563) | | |
| `-[PlayerEntity updateSystemMemory]` (4574) | | |
| `-[PlayerEntity compassTarget]` (4594) | | |
| `-[PlayerEntity setCompassTarget:]` (4604) | | |
| `-[PlayerEntity validateCompassTarget]` (4610) | | |
| `-[PlayerEntity compassTargetLabel]` (4668) | | |
| `-[PlayerEntity compassMode]` (4694) | | |
| `-[PlayerEntity setCompassMode:]` (4699) | | |
| `-[PlayerEntity setPrevCompassMode]` (4704) | | |
| `-[PlayerEntity setNextCompassMode]` (4761) | | |
| `-[PlayerEntity activeMissile]` (4816) | | |
| `-[PlayerEntity setActiveMissile:]` (4821) | | |
| `-[PlayerEntity dialMaxMissiles]` (4826) | | |
| `-[PlayerEntity dialIdentEngaged]` (4831) | | |
| `-[PlayerEntity setDialIdentEngaged:]` (4836) | | |
| `-[PlayerEntity specialCargo]` (4841) | | |
| `-[PlayerEntity dialTargetName]` (4846) | | |
| `-[PlayerEntity multiFunctionDisplayList]` (4865) | | |
| `-[PlayerEntity multiFunctionText:]` (4870) | | |
| `-[PlayerEntity setMultiFunctionText:forKey:]` (4880) | | |
| `-[PlayerEntity setMultiFunctionDisplay:toKey:]` (4894) | | |
| `-[PlayerEntity cycleNextMultiFunctionDisplay:]` (4916) | | |
| `-[PlayerEntity cyclePreviousMultiFunctionDisplay:]` (4946) | | |
| `-[PlayerEntity selectNextMultiFunctionDisplay]` (4976) | | |
| `-[PlayerEntity selectPreviousMultiFunctionDisplay]` (4988) | | |
| `-[PlayerEntity activeMFD]` (5004) | | |
| `-[PlayerEntity missileForPylon:]` (5009) | | |
| `-[PlayerEntity safeAllMissiles]` (5016) | | |
| `-[PlayerEntity tidyMissilePylons]` (5028) | | |
| `-[PlayerEntity selectNextMissile]` (5050) | | |
| `-[PlayerEntity clearAlertFlags]` (5096) | | |
| `-[PlayerEntity alertFlags]` (5101) | | |
| `-[PlayerEntity setAlertFlag:to:]` (5106) | | |
| `-[PlayerEntity realAlertCondition]` (5117) | | |
| `-[PlayerEntity alertCondition]` (5122) | | |
| `-[PlayerEntity fleeingStatus]` (5146) | | |
| `-[PlayerEntity interpretAIMessage:]` (5153) | | |
| `-[PlayerEntity mountMissile:]` (5203) | | |
| `-[PlayerEntity mountMissileWithRole:]` (5223) | | |
| `-[PlayerEntity fireMissile]` (5230) | | |
| `-[PlayerEntity launchMine:]` (5282) | | |
| `-[PlayerEntity assignToActivePylon:]` (5302) | | |
| `-[PlayerEntity activateCloakingDevice]` (5342) | | |
| `-[PlayerEntity deactivateCloakingDevice]` (5359) | | |
| `-[PlayerEntity scannerFuzziness]` (5372) | | |
| `-[PlayerEntity noticeECM]` (5388) | | |
| `-[PlayerEntity fireECM]` (5393) | | |
| `-[PlayerEntity installedEnergyUnitType]` (5404) | | |
| `-[PlayerEntity energyUnitType]` (5413) | | |
| `-[PlayerEntity currentWeaponStats]` (5426) | | |
| `-[PlayerEntity weaponsOnline]` (5435) | | |
| `-[PlayerEntity setWeaponsOnline:]` (5440) | | |
| `-[PlayerEntity currentLaserOffset]` (5447) | | |
| `-[PlayerEntity fireMainWeapon]` (5452) | | |
| `-[PlayerEntity weaponForFacing:]` (5529) | | |
| `-[PlayerEntity currentWeapon]` (5550) | | |
| `-[PlayerEntity doesHitLine:v1:hitEntity:]` (5558) | | |
| `-[PlayerEntity takeEnergyDamage:from:becauseOf:weaponIdentifier:]` (5599) | | |
| `-[PlayerEntity takeScrapeDamage:from:]` (5701) | | |
| `-[PlayerEntity takeHeatDamage:]` (5755) | | |
| `-[PlayerEntity createDoppelganger]` (5785) | | |
| `-[PlayerEntity launchEscapeCapsule]` (5806) | | |
| `-[PlayerEntity dumpCargo]` (5928) | | |
| `-[PlayerEntity rotateCargo]` (5944) | | |
| `-[PlayerEntity setBounty:]` (5982) | | |
| `-[PlayerEntity setBounty:withReason:]` (5987) | | |
| `-[PlayerEntity setBounty:withReasonAsString:]` (5993) | | |
| `-[PlayerEntity bounty]` (6010) | | |
| `-[PlayerEntity legalStatus]` (6015) | | |
| `-[PlayerEntity markAsOffender:]` (6020) | | |
| `-[PlayerEntity markAsOffender:withReason:]` (6025) | | |
| `-[PlayerEntity collectBountyFor:]` (6044) | | |
| `-[PlayerEntity takeInternalDamage]` (6099) | | |
| `-[PlayerEntity getDestroyedBy:damageType:]` (6189) | | |
| `-[PlayerEntity loseTargetStatus]` (6227) | | |
| `-[PlayerEntity endScenario:]` (6251) | | |
| `-[PlayerEntity enterDock:]` (6260) | | |
| `-[PlayerEntity docked]` (6299) | | |
| `-[PlayerEntity leaveDock:]` (6394) | | |
| `-[PlayerEntity witchStart]` (6490) | | |
| `-[PlayerEntity witchEnd]` (6533) | | |
| `-[PlayerEntity witchJumpChecklist:]` (6546) | | |
| `-[PlayerEntity setJumpType:]` (6617) | | |
| `-[PlayerEntity hyperspaceJumpDistance]` (6626) | | |
| `-[PlayerEntity fuelRequiredForJump]` (6632) | | |
| `-[PlayerEntity hasSufficientFuelForJump]` (6637) | | |
| `-[PlayerEntity noteCompassLostTarget]` (6642) | | |
| `-[PlayerEntity enterGalacticWitchspace]` (6655) | | |
| `-[PlayerEntity enterWormhole:]` (6747) | | |
| `-[PlayerEntity enterWitchspace]` (6774) | | |
| `-[PlayerEntity witchJumpTo:misjump:]` (6863) | | |
| `-[PlayerEntity leaveWitchspace]` (6915) | | |
| `-[PlayerEntity setGuiToStatusScreen]` (7006) | | |
| `-[PlayerEntity equipmentList]` (7146) | | |
| `-[PlayerEntity primedEquipmentCount]` (7245) | | |
| `-[PlayerEntity primedEquipmentName:]` (7250) | | |
| `-[PlayerEntity currentPrimedEquipment]` (7261) | | |
| `-[PlayerEntity setPrimedEquipment:showMessage:]` (7271) | | |
| `-[PlayerEntity activatePrimableEquipment:withMode:]` (7291) | | |
| `-[PlayerEntity fastEquipmentA]` (7311) | | |
| `-[PlayerEntity fastEquipmentB]` (7316) | | |
| `-[PlayerEntity setFastEquipmentA:]` (7321) | | |
| `-[PlayerEntity setFastEquipmentB:]` (7327) | | |
| `-[PlayerEntity weaponTypeForFacing:strict:]` (7333) | | |
| `-[PlayerEntity missilesList]` (7361) | | |
| `-[PlayerEntity cargoList]` (7367) | | |
| `-[PlayerEntity cargoListForScripting]` (7393) | | |
| `-[PlayerEntity legalStatusOfCargoList]` (7432) | | |
| `-[PlayerEntity contractsListForScriptingFromArray:forCargo:]` (7445) | | |
| `-[PlayerEntity passengerListForScripting]` (7483) | | |
| `-[PlayerEntity parcelListForScripting]` (7488) | | |
| `-[PlayerEntity contractListForScripting]` (7493) | | |
| `-[PlayerEntity setGuiToSystemDataScreen]` (7498) | | |
| `-[PlayerEntity setGuiToSystemDataScreenRefreshBackground:]` (7503) | | |
| `-[PlayerEntity prepareMarkedDestination:marker:]` (7699) | | |
| `-[PlayerEntity markedDestinations]` (7711) | | |
| `-[PlayerEntity setGuiToLongRangeChartScreen]` (7746) | | |
| `-[PlayerEntity setGuiToShortRangeChartScreen]` (7758) | | |
| `-[PlayerEntity setGuiToChartScreenFrom:]` (7769) | | |
| `SliderString()` (7823) | | |
| `-[PlayerEntity setGuiToGameOptionsScreen]` (7830) | | |
| `-[PlayerEntity setGuiToLoadSaveScreen]` (8061) | | |
| `-[PlayerEntity highlightEquipShipScreenKey:]` (8157) | | |
| `-[PlayerEntity availableFacings]` (8197) | | |
| `-[PlayerEntity setGuiToEquipShipScreen:selectingFacingFor:]` (8206) | | |
| `-[PlayerEntity setGuiToEquipShipScreen:]` (8547) | | |
| `-[PlayerEntity showInformationForSelectedUpgrade]` (8552) | | |
| `-[PlayerEntity showInformationForSelectedUpgradeWithFormatString:]` (8557) | | |
| `-[PlayerEntity setGuiToInterfacesScreen:]` (8588) | | |
| `-[PlayerEntity showInformationForSelectedInterface]` (8698) | | |
| `-[PlayerEntity activateSelectedInterface]` (8719) | | |
| `-[PlayerEntity setupStartScreenGui]` (8747) | | |
| `-[PlayerEntity setGuiToIntroFirstGo:]` (8819) | | |
| `-[PlayerEntity setGuiToOXZManager]` (8922) | | |
| `-[PlayerEntity noteGUIWillChangeTo:]` (8940) | | |
| `-[PlayerEntity noteGUIDidChangeFrom:to:]` (8947) | | |
| `-[PlayerEntity noteGUIDidChangeFrom:to:refresh:]` (8952) | | |
| `-[PlayerEntity noteViewDidChangeFrom:toView:]` (8985) | | |
| `-[PlayerEntity buySelectedItem]` (8990) | | |
| `-[PlayerEntity adjustPriceByScriptForEqKey:withCurrent:]` (9070) | | |
| `-[PlayerEntity tryBuyingItem:]` (9104) | | |
| `-[PlayerEntity setWeaponMount:toWeapon:]` (9345) | | |
| `-[PlayerEntity setWeaponMount:toWeapon:inContext:]` (9350) | | |
| `-[PlayerEntity changePassengerBerths:]` (9395) | | |
| `-[PlayerEntity removeMissiles]` (9410) | | |
| `-[PlayerEntity doTradeIn:forPriceFactor:]` (9431) | | |
| `-[PlayerEntity cargoQuantityForType:]` (9440) | | |
| `-[PlayerEntity setCargoQuantityForType:amount:]` (9461) | | |
| `-[PlayerEntity calculateCurrentCargo]` (9503) | | |
| `-[PlayerEntity cargoQuantityOnBoard]` (9508) | | |
| `-[PlayerEntity localMarket]` (9547) | | |
| `-[PlayerEntity applyMarketFilter:onMarket:]` (9569) | | |
| `-[PlayerEntity applyMarketSorter:onMarket:]` (9612) | | |
| `-[PlayerEntity showMarketScreenHeaders]` (9632) | | |
| `-[PlayerEntity showMarketScreenDataLine:forGood:inMarket:holdQuantity:]` (9654) | | |
| `-[PlayerEntity marketScreenTitle]` (9700) | | |
| `-[PlayerEntity setGuiToMarketScreen]` (9728) | | |
| `-[PlayerEntity setGuiToMarketInfoScreen]` (9895) | | |
| `-[PlayerEntity showMarketCashAndLoadLine]` (9979) | | |
| `-[PlayerEntity guiScreen]` (9988) | | |
| `-[PlayerEntity tryBuyingCommodity:all:]` (9993) | | |
| `-[PlayerEntity trySellingCommodity:all:]` (10074) | | |
| `-[PlayerEntity isMining]` (10126) | | |
| `-[PlayerEntity isSpeechOn]` (10131) | | |
| `-[PlayerEntity canAddEquipment:inContext:]` (10136) | | |
| `-[PlayerEntity addEquipmentItem:inContext:]` (10150) | | |
| `-[PlayerEntity addEquipmentItem:withValidation:inContext:]` (10155) | | |
| `-[PlayerEntity customEquipmentActivation]` (10186) | | |
| `-[PlayerEntity addEquipmentWithScriptToCustomKeyArray:]` (10191) | | |
| `-[PlayerEntity validateCustomEquipActivationArray]` (10233) | | |
| `-[PlayerEntity removeEquipmentItem:]` (10256) | | |
| `-[PlayerEntity addEquipmentFromCollection:]` (10268) | | |
| `-[PlayerEntity hasOneEquipmentItem:includeMissiles:]` (10335) | | |
| `-[PlayerEntity hasPrimaryWeapon:]` (10357) | | |
| `-[PlayerEntity removeExternalStore:]` (10369) | | |
| `-[PlayerEntity removeFromPylon:]` (10386) | | |
| `-[PlayerEntity parcelCount]` (10419) | | |
| `-[PlayerEntity passengerCount]` (10424) | | |
| `-[PlayerEntity passengerCapacity]` (10429) | | |
| `-[PlayerEntity hasHostileTarget]` (10434) | | |
| `-[PlayerEntity receiveCommsMessage:from:]` (10440) | | |
| `-[PlayerEntity getFined]` (10450) | | |
| `-[PlayerEntity adjustTradeInFactorBy:]` (10476) | | |
| `-[PlayerEntity tradeInFactor]` (10485) | | |
| `-[PlayerEntity renovationCosts]` (10490) | | |
| `-[PlayerEntity renovationFactor]` (10501) | | |
| `-[PlayerEntity setDefaultViewOffsets]` (10508) | | |
| `-[PlayerEntity setDefaultCustomViews]` (10520) | | |
| `-[PlayerEntity weaponViewOffset]` (10532) | | |
| `-[PlayerEntity setUpTrumbles]` (10551) | | |
| `-[PlayerEntity addTrumble:]` (10580) | | |
| `-[PlayerEntity removeTrumble:]` (10590) | | |
| `-[PlayerEntity trumbleArray]` (10611) | | |
| `-[PlayerEntity trumbleCount]` (10616) | | |
| `-[PlayerEntity trumbleValue]` (10621) | | |
| `-[PlayerEntity setTrumbleValueFrom:]` (10643) | | |
| `-[PlayerEntity trumbleAppetiteAccumulator]` (10741) | | |
| `-[PlayerEntity setTrumbleAppetiteAccumulator:]` (10746) | | |
| `-[PlayerEntity mungChecksumWithNSString:]` (10751) | | |
| `-[PlayerEntity screenModeStringForWidth:height:refreshRate:]` (10762) | | |
| `-[PlayerEntity suppressTargetLost]` (10771) | | |
| `-[PlayerEntity setScoopsActive]` (10776) | | |
| `-[PlayerEntity setFoundTarget:]` (10782) | | |
| `-[PlayerEntity addTarget:]` (10794) | | |
| `-[PlayerEntity clearTargetMemory]` (10847) | | |
| `-[PlayerEntity targetMemory]` (10860) | | |
| `-[PlayerEntity moveTargetMemoryBy:]` (10865) | | |
| `-[PlayerEntity printIdentLockedOnForMissile:]` (10911) | | |
| `-[PlayerEntity customViewQuaternion]` (10921) | | |
| `-[PlayerEntity setCustomViewQuaternion:]` (10926) | | |
| `-[PlayerEntity customViewMatrix]` (10932) | | |
| `-[PlayerEntity customViewOffset]` (10937) | | |
| `-[PlayerEntity setCustomViewOffset:]` (10942) | | |
| `-[PlayerEntity customViewRotationCenter]` (10947) | | |
| `-[PlayerEntity setCustomViewRotationCenter:]` (10952) | | |
| `-[PlayerEntity customViewZoomIn:]` (10957) | | |
| `-[PlayerEntity customViewZoomOut:]` (10968) | | |
| `-[PlayerEntity customViewRotateLeft:]` (10979) | | |
| `-[PlayerEntity customViewRotateRight:]` (10990) | | |
| `-[PlayerEntity customViewRotateUp:]` (11001) | | |
| `-[PlayerEntity customViewRotateDown:]` (11012) | | |
| `-[PlayerEntity customViewRollRight:]` (11023) | | |
| `-[PlayerEntity customViewRollLeft:]` (11034) | | |
| `-[PlayerEntity customViewPanUp:]` (11045) | | |
| `-[PlayerEntity customViewPanDown:]` (11052) | | |
| `-[PlayerEntity customViewPanLeft:]` (11059) | | |
| `-[PlayerEntity customViewPanRight:]` (11066) | | |
| `-[PlayerEntity customViewForwardVector]` (11073) | | |
| `-[PlayerEntity customViewUpVector]` (11078) | | |
| `-[PlayerEntity customViewRightVector]` (11083) | | |
| `-[PlayerEntity customViewDescription]` (11088) | | |
| `-[PlayerEntity resetCustomView]` (11093) | | |
| `-[PlayerEntity setCustomViewData]` (11098) | | |
| `-[PlayerEntity setCustomViewDataFromDictionary:withScaling:]` (11109) | | |
| `-[PlayerEntity showInfoFlag]` (11143) | | |
| `-[PlayerEntity missionOverlayDescriptor]` (11148) | | |
| `-[PlayerEntity missionOverlayDescriptorOrDefault]` (11153) | | |
| `-[PlayerEntity setMissionOverlayDescriptor:]` (11167) | | |
| `-[PlayerEntity missionBackgroundDescriptor]` (11175) | | |
| `-[PlayerEntity missionBackgroundDescriptorOrDefault]` (11180) | | |
| `-[PlayerEntity setMissionBackgroundDescriptor:]` (11190) | | |
| `-[PlayerEntity missionBackgroundSpecial]` (11198) | | |
| `-[PlayerEntity setMissionBackgroundSpecial:]` (11203) | | |
| `-[PlayerEntity setMissionExitScreen:]` (11254) | | |
| `-[PlayerEntity missionExitScreen]` (11259) | | |
| `-[PlayerEntity equipScreenBackgroundDescriptor]` (11264) | | |
| `-[PlayerEntity setEquipScreenBackgroundDescriptor:]` (11269) | | |
| `-[PlayerEntity scriptsLoaded]` (11277) | | |
| `-[PlayerEntity worldScriptNames]` (11282) | | |
| `-[PlayerEntity worldScriptsByName]` (11287) | | |
| `-[PlayerEntity commodityScriptNamed:]` (11292) | | |
| `-[PlayerEntity doScriptEvent:inContext:withArguments:count:]` (11311) | | |
| `-[PlayerEntity doWorldEventUntilMissionScreen:]` (11317) | | |
| `-[PlayerEntity doWorldScriptEvent:inContext:withArguments:count:timeLimit:]` (11344) | | |
| `-[PlayerEntity setGalacticHyperspaceBehaviour:]` (11357) | | |
| `-[PlayerEntity galacticHyperspaceBehaviour]` (11364) | | |
| `-[PlayerEntity setGalacticHyperspaceFixedCoords:]` (11369) | | |
| `-[PlayerEntity setGalacticHyperspaceFixedCoordsX:y:]` (11374) | | |
| `-[PlayerEntity galacticHyperspaceFixedCoords]` (11380) | | |
| `-[PlayerEntity setWitchspaceCountdown:]` (11385) | | |
| `-[PlayerEntity longRangeChartMode]` (11390) | | |
| `-[PlayerEntity setLongRangeChartMode:]` (11395) | | |
| `-[PlayerEntity scoopOverride]` (11400) | | |
| `-[PlayerEntity setScoopOverride:]` (11405) | | |
| `-[PlayerEntity fuelChargeRate]` (11413) | | |
| `-[PlayerEntity setDockTarget:]` (11429) | | |
| `-[PlayerEntity jumpCause]` (11438) | | |
| `-[PlayerEntity setJumpCause:]` (11443) | | |
| `-[PlayerEntity commanderName]` (11450) | | |
| `-[PlayerEntity lastsaveName]` (11455) | | |
| `-[PlayerEntity setCommanderName:]` (11460) | | |
| `-[PlayerEntity setLastsaveName:]` (11467) | | |
| `-[PlayerEntity isDocked]` (11475) | | |
| `-[PlayerEntity clearedToDock]` (11529) | | |
| `-[PlayerEntity setDockingClearanceStatus:]` (11534) | | |
| `-[PlayerEntity getDockingClearanceStatus]` (11550) | | |
| `-[PlayerEntity penaltyForUnauthorizedDocking]` (11555) | | |
| `-[PlayerEntity addScannedWormhole:]` (11572) | | |
| `-[PlayerEntity updateWormholes]` (11590) | | |
| `-[PlayerEntity scannedWormholes]` (11620) | | |
| `-[PlayerEntity initialiseMissionDestinations:andLegacy:]` (11625) | | |
| `-[PlayerEntity markerKey:]` (11657) | | |
| `-[PlayerEntity addMissionDestinationMarker:]` (11662) | | |
| `-[PlayerEntity removeMissionDestinationMarker:]` (11672) | | |
| `-[PlayerEntity getMissionDestinations]` (11686) | | |
| `-[PlayerEntity shipyardRecord]` (11691) | | |
| `-[PlayerEntity setLastShot:]` (11696) | | |
| `-[PlayerEntity clearExtraMissionKeys]` (11701) | | |
| `-[PlayerEntity setExtraMissionKeys:]` (11707) | | |
| `-[PlayerEntity clearExtraGuiScreenKeys:key:]` (11718) | | |
| `-[PlayerEntity setExtraGuiScreenKeys:definition:]` (11733) | | |
| `-[PlayerEntity dumpSelfState]` (11794) | | |
| `-[PlayerEntity suppressClangStuff]` (11856) | | |
| `marketSorterByName()` (11868) | | |
| `marketSorterByPrice()` (11874) | | |
| `marketSorterByQuantity()` (11887) | | |
| `marketSorterByMassUnit()` (11900) | | |

**Differences:**

- *pending*


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

