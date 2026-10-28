# Parity findings: Universe and GameController

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## GameController

<a id="gamecontroller"></a>
Upstream `src/Core/GameController.m` ↔ `Core/GameController.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[GameController sharedController]` (70) | | |
| `-[GameController init]` (78) | | |
| `-[GameController dealloc]` (101) | | |
| `-[GameController isGamePaused]` (118) | | |
| `-[GameController setGamePaused:]` (123) | | |
| `-[GameController setEcoQoS:]` (139) | | |
| `-[GameController mouseInteractionMode]` (162) | | |
| `-[GameController setMouseInteractionMode:]` (167) | | |
| `-[GameController setMouseInteractionModeForFlight]` (186) | | |
| `-[GameController setMouseInteractionModeForUIWithMouseInteraction:]` (191) | | |
| `-[GameController gameView]` (196) | | |
| `-[GameController setGameView:]` (201) | | |
| `-[GameController applicationDidFinishLaunching:]` (209) | | |
| `-[GameController finishedLaunching]` (283) | | |
| `-[GameController loadPlayerIfRequired]` (288) | | |
| `-[GameController beginSplashScreen]` (300) | | |
| `-[GameController performGameTick:]` (316) | | |
| `-[GameController performGameTick:]` (324) | | |
| `-[GameController doPerformGameTick]` (336) | | |
| `-[GameController startAnimationTimer]` (369) | | |
| `-[GameController stopAnimationTimer]` (384) | | |
| `-[GameController recenterVirtualJoystick]` (395) | | |
| `-[GameController showLogAction:]` (402) | | |
| `-[GameController showLogFolderAction:]` (407) | | |
| `GetPreference()` (413) | | |
| `SetPreference()` (422) | | |
| `RemovePreference()` (427) | | |
| `-[GameController snapshotsURLCreatingIfNeeded:]` (435) | | |
| `-[GameController showSnapshotsAction:]` (497) | | |
| `-[GameController showAddOnsAction:]` (502) | | |
| `-[GameController isDirectoryAtPath:]` (530) | | |
| `-[GameController addOnsExistAtPath:]` (536) | | |
| `-[GameController openPath:]` (552) | | |
| `-[GameController validateMenuItem:]` (557) | | |
| `-[GameController applicationDockMenu:]` (592) | | |
| `-[GameController snapshotsURLCreatingIfNeeded:]` (599) | | |
| `-[GameController logProgress:]` (617) | | |
| `-[GameController debugMessageTrackingIsOn]` (633) | | |
| `-[GameController debugMessageCurrentString]` (638) | | |
| `-[GameController debugMessageTrackingIsOn]` (643) | | |
| `-[GameController debugMessageCurrentString]` (648) | | |
| `-[GameController debugLogProgress:]` (654) | | |
| `-[GameController debugLogProgress:arguments:]` (662) | | |
| `-[GameController debugPushProgressMessage:]` (670) | | |
| `-[GameController debugPopProgressMessage]` (686) | | |
| `-[GameController endSplashScreen]` (700) | | |
| `-[GameController awakeFromNib]` (720) | | |
| `-[GameController application:openFile:]` (735) | | |
| `-[GameController exitAppWithContext:]` (756) | | |
| `-[GameController applicationShouldTerminate:]` (763) | | |
| `-[GameController exitAppWithContext:]` (773) | | |
| `-[GameController exitAppCommandQ]` (799) | | |
| `-[GameController windowDidResize:]` (804) | | |
| `-[GameController playerFileToLoad]` (809) | | |
| `-[GameController setPlayerFileToLoad:]` (814) | | |
| `-[GameController playerFileDirectory]` (823) | | |
| `-[GameController setPlayerFileDirectory:]` (839) | | |
| `-[GameController reportUnhandledStartupException:]` (854) | | |
| `-[GameController suppressClangStuff]` (869) | | |
| `SetUpSparkle()` (879) | | |

**Differences:**

- *pending*


## Universe

<a id="universe"></a>
Upstream `src/Core/Universe.m` ↔ `Core/Universe.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[RouteElement elementWithLocation:parent:cost:distance:time:jumps:]` (164) | | |
| `-[RouteElement parent]` (178) | | |
| `-[RouteElement location]` (182) | | |
| `-[RouteElement cost]` (186) | | |
| `-[RouteElement distance]` (190) | | |
| `-[RouteElement time]` (194) | | |
| `-[RouteElement jumps]` (198) | | |
| `-[Universe bloom]` (284) | | |
| `-[Universe setBloom:]` (289) | | |
| `-[Universe currentPostFX]` (294) | | |
| `-[Universe setCurrentPostFX:]` (299) | | |
| `-[Universe terminatePostFX:]` (312) | | |
| `-[Universe nextColorblindMode:]` (319) | | |
| `-[Universe prevColorblindMode:]` (327) | | |
| `-[Universe colorblindMode]` (335) | | |
| `-[Universe initTargetFramebufferWithViewSize:]` (340) | | |
| `-[Universe deleteOpenGLObjects]` (520) | | |
| `-[Universe resizeTargetFramebufferWithViewSize:]` (540) | | |
| `-[Universe drawTargetTextureIntoDefaultFramebuffer]` (579) | | |
| `-[Universe initWithGameView:]` (673) | | |
| `-[Universe dealloc]` (839) | | |
| `-[Universe sessionID]` (903) | | |
| `-[Universe doingStartUp]` (908) | | |
| `-[Universe doProcedurallyTexturedPlanets]` (913) | | |
| `-[Universe setDoProcedurallyTexturedPlanets:]` (918) | | |
| `-[Universe useAddOns]` (924) | | |
| `-[Universe setUseAddOns:fromSaveGame:]` (929) | | |
| `-[Universe setUseAddOns:fromSaveGame:forceReinit:]` (934) | | |
| `-[Universe entityCount]` (945) | | |
| `-[Universe debugDumpEntities]` (951) | | |
| `-[Universe entityList]` (972) | | |
| `-[Universe pauseGame]` (978) | | |
| `-[Universe quitGame]` (1005) | | |
| `-[Universe carryPlayerOn:inWormhole:]` (1011) | | |
| `-[Universe setUpUniverseFromStation]` (1069) | | |
| `-[Universe setUpUniverseFromWitchspace]` (1143) | | |
| `-[Universe setUpUniverseFromMisjump]` (1179) | | |
| `-[Universe setUpWitchspace]` (1210) | | |
| `-[Universe setUpWitchspaceBetweenSystem:andSystem:]` (1215) | | |
| `-[Universe setUpPlanet]` (1279) | | |
| `-[Universe setUpSpace]` (1315) | | |
| `-[Universe populateNormalSpace]` (1603) | | |
| `-[Universe clearSystemPopulator]` (1663) | | |
| `-[Universe getPopulatorSettings]` (1669) | | |
| `-[Universe setPopulatorSetting:to:]` (1674) | | |
| `-[Universe deterministicPopulation]` (1683) | | |
| `-[Universe populateSystemFromDictionariesWithSun:andPlanet:]` (1688) | | |
| `-[Universe locationByCode:withSun:andPlanet:]` (1764) | | |
| `-[Universe setAmbientLightLevel:]` (1847) | | |
| `-[Universe ambientLightLevel]` (1855) | | |
| `-[Universe setLighting]` (1860) | | |
| `-[Universe forceLightSwitch]` (1941) | | |
| `-[Universe setMainLightPosition:]` (1946) | | |
| `-[Universe addShipWithRole:launchPos:rfactor:]` (1954) | | |
| `-[Universe addShipWithRole:nearRouteOneAt:]` (1988) | | |
| `-[Universe coordinatesForPosition:withCoordinateSystem:returningScalar:]` (2002) | | |
| `-[Universe expressPosition:inCoordinateSystem:]` (2144) | | |
| `-[Universe legacyPositionFrom:asCoordinateSystem:]` (2150) | | |
| `-[Universe coordinatesFromCoordinateSystemString:]` (2254) | | |
| `-[Universe addShipWithRole:nearPosition:withCoordinateSystem:]` (2265) | | |
| `-[Universe addShips:withRole:atPosition:withCoordinateSystem:]` (2280) | | |
| `-[Universe addShips:withRole:nearPosition:withCoordinateSystem:]` (2354) | | |
| `-[Universe addShips:withRole:nearPosition:withCoordinateSystem:withinRadius:]` (2371) | | |
| `-[Universe addShips:withRole:intoBoundingBox:]` (2386) | | |
| `-[Universe spawnShip:]` (2428) | | |
| `-[Universe witchspaceShipWithPrimaryRole:]` (2498) | | |
| `-[Universe spawnShipWithRole:near:]` (2549) | | |
| `-[Universe addVisualEffectAt:withKey:]` (2568) | | |
| `-[Universe addShipAt:withRole:withinRadius:]` (2589) | | |
| `-[Universe addShipsAt:withRole:quantity:withinRadius:asGroup:]` (2679) | | |
| `-[Universe addShipsToRoute:withRole:quantity:routeFraction:asGroup:]` (2709) | | |
| `-[Universe roleIsPirateVictim:]` (2784) | | |
| `-[Universe role:isInCategory:]` (2789) | | |
| `-[Universe forceWitchspaceEntries]` (2799) | | |
| `-[Universe addWitchspaceJumpEffectForShip:]` (2815) | | |
| `-[Universe safeWitchspaceExitDistance]` (2824) | | |
| `-[Universe setUpBreakPattern:orientation:forDocking:]` (2835) | | |
| `-[Universe witchspaceBreakPattern]` (2907) | | |
| `-[Universe setWitchspaceBreakPattern:]` (2912) | | |
| `-[Universe dockingClearanceProtocolActive]` (2917) | | |
| `-[Universe setDockingClearanceProtocolActive:]` (2922) | | |
| `-[Universe handleGameOver]` (2942) | | |
| `-[Universe setupIntroFirstGo:]` (2951) | | |
| `-[Universe demoShipData]` (3038) | | |
| `-[Universe setLibraryTextForDemoShip]` (3043) | | |
| `-[Universe selectIntro2Previous]` (3242) | | |
| `-[Universe selectIntro2PreviousCategory]` (3250) | | |
| `-[Universe selectIntro2NextCategory]` (3258) | | |
| `-[Universe selectIntro2Next]` (3266) | | |
| `IsCandidateMainStationPredicate()` (3272) | | |
| `IsFriendlyStationPredicate()` (3277) | | |
| `-[Universe station]` (3282) | | |
| `-[Universe stationWithRole:andPosition:]` (3291) | | |
| `-[Universe stationFriendlyTo:]` (3311) | | |
| `-[Universe planet]` (3318) | | |
| `-[Universe sun]` (3326) | | |
| `-[Universe planets]` (3334) | | |
| `-[Universe stations]` (3339) | | |
| `-[Universe wormholes]` (3344) | | |
| `-[Universe unMagicMainStation]` (3349) | | |
| `-[Universe resetBeacons]` (3370) | | |
| `-[Universe firstBeacon]` (3384) | | |
| `-[Universe setFirstBeacon:]` (3389) | | |
| `-[Universe lastBeacon]` (3400) | | |
| `-[Universe setLastBeacon:]` (3405) | | |
| `-[Universe setNextBeacon:]` (3416) | | |
| `-[Universe clearBeacon:]` (3427) | | |
| `-[Universe currentWaypoints]` (3448) | | |
| `-[Universe defineWaypoint:forKey:]` (3453) | | |
| `-[Universe skyClearColor]` (3478) | | |
| `-[Universe setSkyColorRed:green:blue:alpha:]` (3483) | | |
| `-[Universe breakPatternOver]` (3492) | | |
| `-[Universe breakPatternHide]` (3497) | | |
| `-[Universe canInstantiateShip:]` (3505) | | |
| `-[Universe randomShipKeyForRoleRespectingConditions:]` (3551) | | |
| `-[Universe newShipWithRole:]` (3602) | | |
| `-[Universe newVisualEffectWithName:]` (3647) | | |
| `-[Universe newSubentityWithName:andScaleFactor:]` (3673) | | |
| `-[Universe newShipWithName:usePlayerProxy:]` (3678) | | |
| `-[Universe newShipWithName:usePlayerProxy:isSubentity:]` (3683) | | |
| `-[Universe newShipWithName:usePlayerProxy:isSubentity:andScaleFactor:]` (3688) | | |
| `-[Universe newDockWithName:andScaleFactor:]` (3735) | | |
| `-[Universe newShipWithName:]` (3772) | | |
| `-[Universe shipClassForShipDictionary:]` (3777) | | |
| `-[Universe defaultAIForRole:]` (3801) | | |
| `-[Universe maxCargoForShip:]` (3806) | | |
| `-[Universe getEquipmentPriceForKey:]` (3814) | | |
| `-[Universe commodities]` (3827) | | |
| `-[Universe reifyCargoPod:]` (3833) | | |
| `-[Universe cargoPodFromTemplate:]` (3842) | | |
| `-[Universe getContainersOfGoods:scarce:legal:]` (3859) | | |
| `-[Universe getContainersOfCommodity:how_much:]` (3919) | | |
| `-[Universe fillCargopodWithRandomCargo:]` (3939) | | |
| `-[Universe getRandomCommodity]` (3951) | | |
| `-[Universe getRandomAmountOfCommodity:]` (3956) | | |
| `-[Universe commodityDataForType:]` (3977) | | |
| `-[Universe displayNameForCommodity:]` (3982) | | |
| `-[Universe describeCommodity:amount:]` (3987) | | |
| `-[Universe setGameView:]` (4032) | | |
| `-[Universe gameView]` (4038) | | |
| `-[Universe gameController]` (4043) | | |
| `-[Universe gameSettings]` (4048) | | |
| `-[Universe useGUILightSource:]` (4103) | | |
| `-[Universe lightForEntity:]` (4128) | | |
| `-[Universe getActiveViewMatrix:forwardVector:upVector:]` (4177) | | |
| `-[Universe activeViewMatrix]` (4220) | | |
| `-[Universe defineFrustum]` (4234) | | |
| `-[Universe viewFrustumIntersectsSphereAt:withRadius:]` (4320) | | |
| `-[Universe drawUniverse]` (4332) | | |
| `-[Universe prepareToRenderIntoDefaultFramebuffer]` (4761) | | |
| `-[Universe framesDoneThisUpdate]` (4774) | | |
| `-[Universe resetFramesDoneThisUpdate]` (4779) | | |
| `-[Universe viewMatrix]` (4784) | | |
| `-[Universe drawMessage]` (4789) | | |
| `-[Universe drawWatermarkString:]` (4810) | | |
| `-[Universe entityForUniversalID:]` (4820) | | |
| `MaintainLinkedLists()` (4846) | | |
| `-[Universe addEntity:]` (4969) | | |
| `-[Universe removeEntity:]` (5107) | | |
| `-[Universe ensureEntityReallyRemoved:]` (5126) | | |
| `-[Universe removeAllEntitiesExceptPlayer]` (5134) | | |
| `-[Universe removeDemoShips]` (5180) | | |
| `-[Universe makeDemoShipWithRole:spinning:]` (5196) | | |
| `-[Universe isVectorClearFromEntity:toDistance:fromPoint:]` (5232) | | |
| `-[Universe hazardOnRouteFromEntity:toDistance:fromPoint:]` (5296) | | |
| `-[Universe getSafeVectorFromEntity:toDistance:fromPoint:]` (5358) | | |
| `-[Universe addWreckageFrom:withRole:at:scale:lifetime:]` (5476) | | |
| `-[Universe addLaserHitEffectsAt:against:damage:color:]` (5507) | | |
| `-[Universe firstShipHitByLaserFromShip:inDirection:offset:gettingRangeFound:]` (5528) | | |
| `-[Universe firstEntityTargetedByPlayer]` (5628) | | |
| `-[Universe firstEntityTargetedByPlayerPrecisely]` (5702) | | |
| `-[Universe entitiesWithinRange:ofEntity:]` (5738) | | |
| `-[Universe countShipsWithRole:inRange:ofEntity:]` (5749) | | |
| `-[Universe countShipsWithRole:]` (5757) | | |
| `-[Universe countShipsWithPrimaryRole:inRange:ofEntity:]` (5762) | | |
| `-[Universe countShipsWithScanClass:inRange:ofEntity:]` (5770) | | |
| `-[Universe countShipsWithPrimaryRole:]` (5778) | | |
| `-[Universe countEntitiesMatchingPredicate:]` (5783) | | |
| `-[Universe countShipsMatchingPredicate:]` (5818) | | |
| `EntityInRange()` (5841) | | |
| `-[Universe findEntitiesMatchingPredicate:]` (5851) | | |
| `-[Universe findOneEntityMatchingPredicate:]` (5889) | | |
| `-[Universe findShipsMatchingPredicate:]` (5911) | | |
| `-[Universe findVisualEffectsMatchingPredicate:]` (5934) | | |
| `-[Universe nearestEntityMatchingPredicate:]` (5957) | | |
| `-[Universe nearestShipMatchingPredicate:]` (5987) | | |
| `-[Universe getTime]` (6007) | | |
| `-[Universe getTimeDelta]` (6012) | | |
| `-[Universe findCollisionsAndShadows]` (6017) | | |
| `-[Universe collisionDescription]` (6035) | | |
| `-[Universe dumpCollisions]` (6043) | | |
| `-[Universe viewDirection]` (6048) | | |
| `-[Universe setViewDirection:]` (6053) | | |
| `-[Universe enterGUIViewModeWithMouseInteraction:]` (6117) | | |
| `-[Universe soundNameForCustomSoundKey:]` (6130) | | |
| `-[Universe screenTextureDescriptorForKey:]` (6183) | | |
| `-[Universe setScreenTextureDescriptorForKey:descriptor:]` (6201) | | |
| `-[Universe clearPreviousMessage]` (6214) | | |
| `-[Universe setMessageGuiBackgroundColor:]` (6221) | | |
| `-[Universe displayMessage:forCount:]` (6226) | | |
| `-[Universe displayCountdownMessage:forCount:]` (6237) | | |
| `-[Universe addDelayedMessage:forCount:afterDelay:]` (6248) | | |
| `-[Universe addDelayedMessage:]` (6256) | | |
| `-[Universe addMessage:forCount:]` (6269) | | |
| `-[Universe speakWithSubstitutions:]` (6274) | | |
| `-[Universe addMessage:forCount:forceDisplay:]` (6319) | | |
| `-[Universe addCommsMessage:forCount:]` (6336) | | |
| `-[Universe addCommsMessage:forCount:andShowComms:logOnly:]` (6341) | | |
| `-[Universe showCommsLog:]` (6372) | | |
| `-[Universe showGUIMessage:withScroll:andColor:overDuration:]` (6379) | | |
| `-[Universe repopulateSystem]` (6389) | | |
| `-[Universe update:]` (6400) | | |
| `-[Universe timeAccelerationFactor]` (6659) | | |
| `-[Universe setTimeAccelerationFactor:]` (6664) | | |
| `-[Universe timeAccelerationFactor]` (6672) | | |
| `-[Universe setTimeAccelerationFactor:]` (6677) | | |
| `-[Universe ECMVisualFXEnabled]` (6682) | | |
| `-[Universe setECMVisualFXEnabled:]` (6687) | | |
| `-[Universe filterSortedLists]` (6692) | | |
| `-[Universe setGalaxyTo:]` (7126) | | |
| `-[Universe setGalaxyTo:andReinit:]` (7131) | | |
| `-[Universe setSystemTo:]` (7152) | | |
| `-[Universe currentSystemID]` (7172) | | |
| `-[Universe descriptions]` (7177) | | |
| `VerifyDescString()` (7193) | | |
| `VerifyDescArray()` (7201) | | |
| `VerifyDesc()` (7209) | | |
| `-[Universe verifyDescriptions]` (7223) | | |
| `-[Universe loadDescriptions]` (7246) | | |
| `-[Universe explosionSetting:]` (7253) | | |
| `-[Universe scenarios]` (7258) | | |
| `-[Universe loadScenarios]` (7263) | | |
| `-[Universe characters]` (7269) | | |
| `-[Universe missiontext]` (7274) | | |
| `-[Universe descriptionForKey:]` (7279) | | |
| `-[Universe descriptionForArrayKey:index:]` (7284) | | |
| `-[Universe descriptionBooleanForKey:]` (7292) | | |
| `-[Universe systemManager]` (7297) | | |
| `-[Universe keyForPlanetOverridesForSystem:inGalaxy:]` (7302) | | |
| `-[Universe keyForInterstellarOverridesForSystems:s2:inGalaxy:]` (7307) | | |
| `-[Universe generateSystemData:]` (7312) | | |
| `-[Universe generateSystemData:useCache:]` (7318) | | |
| `-[Universe currentSystemData]` (7332) | | |
| `-[Universe inInterstellarSpace]` (7364) | | |
| `-[Universe setSystemDataKey:value:fromManifest:]` (7371) | | |
| `-[Universe setSystemDataForGalaxy:planet:key:value:fromManifest:forLayer:]` (7376) | | |
| `-[Universe generateSystemDataForGalaxy:planet:]` (7486) | | |
| `-[Universe systemDataKeysForGalaxy:planet:]` (7492) | | |
| `-[Universe systemDataForGalaxy:planet:key:]` (7498) | | |
| `-[Universe getSystemName:]` (7503) | | |
| `-[Universe getSystemName:forGalaxy:]` (7508) | | |
| `-[Universe getSystemGovernment:]` (7513) | | |
| `-[Universe getSystemInhabitants:]` (7518) | | |
| `-[Universe getSystemInhabitants:plural:]` (7523) | | |
| `-[Universe coordinatesForSystem:]` (7537) | | |
| `-[Universe findSystemFromName:]` (7542) | | |
| `-[Universe findSystemAtCoords:withGalaxy:]` (7559) | | |
| `-[Universe nearbyDestinationsWithinRange:]` (7565) | | |
| `-[Universe findNeighbouringSystemToCoords:withGalaxy:]` (7590) | | |
| `-[Universe findConnectedSystemAtCoords:withGalaxy:]` (7632) | | |
| `-[Universe findSystemNumberAtCoords:withGalaxy:includingHidden:]` (7672) | | |
| `-[Universe findSystemCoordinatesWithPrefix:]` (7719) | | |
| `-[Universe findSystemCoordinatesWithPrefix:exactMatch:]` (7724) | | |
| `-[Universe systemsFound]` (7752) | | |
| `-[Universe systemNameIndex:]` (7757) | | |
| `-[Universe routeFromSystem:toSystem:optimizedBy:]` (7762) | | |
| `-[Universe neighboursToSystem:]` (7885) | | |
| `-[Universe preloadPlanetTexturesForSystem:]` (7928) | | |
| `-[Universe globalSettings]` (7960) | | |
| `-[Universe equipmentData]` (7965) | | |
| `-[Universe equipmentDataOutfitting]` (7970) | | |
| `-[Universe commodityMarket]` (7975) | | |
| `-[Universe timeDescription:]` (7980) | | |
| `-[Universe shortTimeDescription:]` (8007) | | |
| `-[Universe makeSunSkimmer:andSetAI:]` (8041) | | |
| `-[Universe marketSeed]` (8052) | | |
| `-[Universe loadStationMarkets:]` (8069) | | |
| `-[Universe getStationMarkets]` (8094) | | |
| `-[Universe shipsForSaleForSystem:withTL:atTime:]` (8120) | | |
| `compareName()` (8486) | | |
| `comparePrice()` (8500) | | |
| `-[Universe tradeInValueForCommanderDictionary:]` (8508) | | |
| `-[Universe brochureDescriptionWithDictionary:standardEquipment:optionalEquipment:]` (8644) | | |
| `-[Universe getWitchspaceExitPosition]` (8744) | | |
| `-[Universe getWitchspaceExitRotation]` (8749) | | |
| `-[Universe getSunSkimStartPositionForShip:]` (8768) | | |
| `-[Universe getSunSkimEndPositionForShip:]` (8801) | | |
| `-[Universe listBeaconsWithCode:]` (8859) | | |
| `-[Universe allShipsDoScriptEvent:andReactToAIMessage:]` (8875) | | |
| `-[Universe gui]` (8898) | | |
| `-[Universe commLogGUI]` (8903) | | |
| `-[Universe messageGUI]` (8908) | | |
| `-[Universe clearGUIs]` (8913) | | |
| `-[Universe resetCommsLogColor]` (8926) | | |
| `-[Universe setDisplayText:]` (8931) | | |
| `-[Universe displayGUI]` (8936) | | |
| `-[Universe setDisplayFPS:]` (8941) | | |
| `-[Universe displayFPS]` (8946) | | |
| `-[Universe setAutoSave:]` (8951) | | |
| `-[Universe autoSave]` (8957) | | |
| `-[Universe setAutoSaveNow:]` (8962) | | |
| `-[Universe autoSaveNow]` (8967) | | |
| `-[Universe setWireframeGraphics:]` (8972) | | |
| `-[Universe wireframeGraphics]` (8978) | | |
| `-[Universe reducedDetail]` (8983) | | |
| `-[Universe setDetailLevelDirectly:]` (8989) | | |
| `-[Universe setDetailLevel:]` (9002) | | |
| `-[Universe detailLevel]` (9015) | | |
| `-[Universe useShaders]` (9020) | | |
| `-[Universe handleOoliteException:]` (9025) | | |
| `-[Universe airResistanceFactor]` (9042) | | |
| `-[Universe setAirResistanceFactor:]` (9047) | | |
| `-[Universe startSpeakingString:]` (9055) | | |
| `-[Universe stopSpeaking]` (9060) | | |
| `-[Universe isSpeaking]` (9069) | | |
| `-[Universe startSpeakingString:]` (9076) | | |
| `-[Universe stopSpeaking]` (9087) | | |
| `-[Universe isSpeaking]` (9092) | | |
| `-[Universe voiceName:]` (9097) | | |
| `-[Universe voiceNumber:]` (9104) | | |
| `-[Universe nextVoice:]` (9119) | | |
| `-[Universe prevVoice:]` (9126) | | |
| `-[Universe setVoice:withGenderM:]` (9133) | | |
| `-[Universe startSpeakingString:]` (9148) | | |
| `-[Universe stopSpeaking]` (9152) | | |
| `-[Universe isSpeaking]` (9156) | | |
| `-[Universe pauseMessageVisible]` (9162) | | |
| `-[Universe setPauseMessageVisible:]` (9167) | | |
| `-[Universe permanentMessageLog]` (9172) | | |
| `-[Universe setPermanentMessageLog:]` (9177) | | |
| `-[Universe autoMessageLogBg]` (9182) | | |
| `-[Universe setAutoMessageLogBg:]` (9187) | | |
| `-[Universe permanentCommLog]` (9192) | | |
| `-[Universe setPermanentCommLog:]` (9197) | | |
| `-[Universe setAutoCommLog:]` (9202) | | |
| `-[Universe blockJSPlayerShipProps]` (9207) | | |
| `-[Universe setBlockJSPlayerShipProps:]` (9212) | | |
| `-[Universe setUpSettings]` (9221) | | |
| `-[Universe setUpCargoPods]` (9307) | | |
| `-[Universe verifyEntitySessionIDs]` (9323) | | |
| `-[Universe reinitAndShowDemo:]` (9347) | | |
| `-[Universe setUpInitialUniverse]` (9451) | | |
| `-[Universe randomDistanceWithinScanner]` (9486) | | |
| `-[Universe randomPlaceWithinScannerFrom:alongRoute:withOffset:]` (9491) | | |
| `-[Universe fractionalPositionFrom:to:withFraction:]` (9500) | | |
| `-[Universe doRemoveEntity:]` (9514) | | |
| `PreloadOneSound()` (9615) | | |
| `-[Universe preloadSounds]` (9622) | | |
| `-[Universe populateSpaceFromActiveWormholes]` (9645) | | |
| `-[Universe chooseStringForKey:inDictionary:]` (9668) | | |
| `-[Universe dumpDebugGraphViz]` (9681) | | |
| `-[Universe dumpSystemDescriptionGraphViz]` (9688) | | |
| `-[Universe addNumericRefsInString:toGraphViz:fromNode:nodeCount:]` (9791) | | |
| `-[Universe runLocalizationTools]` (9842) | | |
| `-[Universe prunePreloadingPlanetMaterials]` (9872) | | |
| `-[Universe loadConditionScripts]` (9885) | | |
| `-[Universe addConditionScripts:]` (9897) | | |
| `-[Universe getConditionScript:]` (9910) | | |
| `+[OOSound(OOCustomSounds) soundWithCustomSoundKey:]` (9919) | | |
| `-[OOSound(OOCustomSounds) initWithCustomSoundKey:]` (9927) | | |
| `+[OOSoundSource(OOCustomSounds) sourceWithCustomSoundKey:]` (9937) | | |
| `-[OOSoundSource(OOCustomSounds) initWithCustomSoundKey:]` (9942) | | |
| `-[OOSoundSource(OOCustomSounds) playCustomSoundWithKey:]` (9954) | | |
| `populatorPrioritySort()` (9963) | | |
| `equipmentSort()` (9976) | | |
| `equipmentSortOutfitting()` (10007) | | |
| `OOLookUpDescriptionPRIV()` (10038) | | |
| `OOLookUpPluralDescriptionPRIV()` (10047) | | |

**Differences:**

- *pending*

