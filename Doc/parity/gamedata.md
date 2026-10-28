# Parity findings: Game data, resources, registries, AI, strings

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## AI

<a id="ai"></a>
Upstream `src/Core/AI.m` ↔ `Core/AI.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[AI currentlyRunningAI]` (98) | | |
| `+[AI currentlyRunningAIDescription]` (103) | | |
| `-[AI init]` (112) | | |
| `-[AI initWithStateMachine:andState:]` (124) | | |
| `-[AI dealloc]` (136) | | |
| `-[AI descriptionComponents]` (154) | | |
| `-[AI shortDescriptionComponents]` (159) | | |
| `-[AI owner]` (164) | | |
| `-[AI setOwner:]` (175) | | |
| `-[AI reportStackOverflow]` (182) | | |
| `-[AI preserveCurrentStateMachine]` (204) | | |
| `-[AI restorePreviousStateMachine]` (237) | | |
| `-[AI hasSuspendedStateMachines]` (263) | | |
| `-[AI exitStateMachineWithMessage:]` (268) | | |
| `-[AI setStateMachine:withJSScript:]` (278) | | |
| `-[AI setState:]` (303) | | |
| `-[AI setStateMachine:afterDelay:]` (319) | | |
| `-[AI setState:afterDelay:]` (324) | | |
| `-[AI name]` (329) | | |
| `-[AI associatedJS]` (334) | | |
| `-[AI state]` (339) | | |
| `-[AI stackDepth]` (344) | | |
| `-[AI reactToMessage:context:]` (363) | | |
| `-[AI takeAction:]` (461) | | |
| `-[AI think]` (514) | | |
| `-[AI message:]` (536) | | |
| `-[AI dropMessage:]` (552) | | |
| `-[AI pendingMessages]` (557) | | |
| `-[AI debugDumpPendingMessages]` (566) | | |
| `-[AI setNextThinkTime:]` (581) | | |
| `-[AI nextThinkTime]` (586) | | |
| `-[AI setThinkTimeInterval:]` (594) | | |
| `-[AI thinkTimeInterval]` (599) | | |
| `-[AI clearStack]` (604) | | |
| `-[AI clearAllData]` (609) | | |
| `-[AI dumpState]` (617) | | |
| `-[AI(OOPrivate) performDeferredCall:withObject:afterDelay:]` (635) | | |
| `+[AI(OOPrivate) deferredCallTrampolineWithInfo:]` (654) | | |
| `-[AI(OOPrivate) refreshOwnerDesc]` (669) | | |
| `-[AI(OOPrivate) directSetStateMachine:name:]` (682) | | |
| `-[AI(OOPrivate) directSetState:]` (694) | | |
| `-[AI(OOPrivate) loadStateMachine:jsName:]` (702) | | |
| `-[AI(OOPrivate) cleanHandlers:forState:stateMachine:]` (779) | | |
| `-[AI(OOPrivate) cleanActions:forHandler:state:stateMachine:]` (802) | | |
| `-[OOPreservedAIStateMachine initWithStateMachine:]` (877) | | |
| `-[OOPreservedAIStateMachine dealloc]` (894) | | |
| `-[OOPreservedAIStateMachine stateMachine]` (905) | | |
| `-[OOPreservedAIStateMachine name]` (910) | | |
| `-[OOPreservedAIStateMachine state]` (915) | | |
| `-[OOPreservedAIStateMachine pendingMessages]` (920) | | |
| `-[OOPreservedAIStateMachine jsScript]` (925) | | |

**Differences:**

- *pending*


## AIGraphViz

<a id="aigraphviz"></a>
Upstream `src/Core/AIGraphViz.m` ↔ `Core/AIGraphViz.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `GenerateGraphVizForAIStateMachine()` (38) | | |
| `HandlerToken()` (104) | | |
| `HandleOneCommand()` (124) | | |
| `AddSimpleSpecialNodeLink()` (172) | | |
| `AddExitAINode()` (181) | | |
| `AddChangeAINode()` (197) | | |

**Differences:**

- *pending*


## OOCharacter

<a id="oocharacter"></a>
Upstream `src/Core/OOCharacter.m` ↔ `Core/OOCharacter.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCharacter descriptionComponents]` (45) | | |
| `-[OOCharacter oo_jsClassName]` (50) | | |
| `-[OOCharacter dealloc]` (55) | | |
| `-[OOCharacter initWithGenSeed:andOriginalSystem:]` (65) | | |
| `-[OOCharacter initWithRole:andOriginalSystem:]` (77) | | |
| `+[OOCharacter characterWithRole:andOriginalSystem:]` (89) | | |
| `+[OOCharacter randomCharacterWithRole:andOriginalSystem:]` (94) | | |
| `+[OOCharacter characterWithDictionary:]` (111) | | |
| `-[OOCharacter planetOfOrigin]` (119) | | |
| `-[OOCharacter planetIDOfOrigin]` (126) | | |
| `-[OOCharacter species]` (132) | | |
| `-[OOCharacter basicSetUp]` (149) | | |
| `-[OOCharacter castInRole:]` (219) | | |
| `-[OOCharacter name]` (297) | | |
| `-[OOCharacter shortDescription]` (302) | | |
| `-[OOCharacter genSeed]` (307) | | |
| `-[OOCharacter legalStatus]` (312) | | |
| `-[OOCharacter insuranceCredits]` (317) | | |
| `-[OOCharacter legacyScript]` (322) | | |
| `-[OOCharacter infoForScripting]` (327) | | |
| `-[OOCharacter setName:]` (339) | | |
| `-[OOCharacter setShortDescription:]` (345) | | |
| `-[OOCharacter setOriginSystem:]` (351) | | |
| `-[OOCharacter setGenSeed:]` (356) | | |
| `-[OOCharacter setLegalStatus:]` (361) | | |
| `-[OOCharacter setInsuranceCredits:]` (366) | | |
| `-[OOCharacter setLegacyScript:]` (371) | | |
| `-[OOCharacter script]` (377) | | |
| `-[OOCharacter setCharacterScript:]` (382) | | |
| `-[OOCharacter doScriptEvent:]` (390) | | |
| `-[OOCharacter setCharacterFromDictionary:]` (397) | | |

**Differences:**

- *pending*


## OOColor

<a id="oocolor"></a>
Upstream `src/Core/OOColor.m` ↔ `Core/OOColor.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOColor setRed:green:blue:alpha:]` (32) | | |
| `-[OOColor setHue:saturation:brightness:alpha:]` (40) | | |
| `-[OOColor copyWithZone:]` (94) | | |
| `+[OOColor colorWithHue:saturation:brightness:alpha:]` (100) | | |
| `+[OOColor colorWithRed:green:blue:alpha:]` (107) | | |
| `+[OOColor colorWithWhite:alpha:]` (114) | | |
| `+[OOColor colorWithRGBAComponents:]` (119) | | |
| `+[OOColor colorWithHSBAComponents:]` (127) | | |
| `+[OOColor colorWithDescription:]` (135) | | |
| `+[OOColor colorWithDescription:saturationFactor:]` (140) | | |
| `+[OOColor brightColorWithDescription:]` (202) | | |
| `+[OOColor colorFromString:]` (211) | | |
| `+[OOColor blackColor]` (234) | | |
| `+[OOColor darkGrayColor]` (239) | | |
| `+[OOColor lightGrayColor]` (244) | | |
| `+[OOColor whiteColor]` (249) | | |
| `+[OOColor grayColor]` (254) | | |
| `+[OOColor redColor]` (259) | | |
| `+[OOColor greenColor]` (264) | | |
| `+[OOColor blueColor]` (269) | | |
| `+[OOColor cyanColor]` (274) | | |
| `+[OOColor yellowColor]` (279) | | |
| `+[OOColor magentaColor]` (284) | | |
| `+[OOColor orangeColor]` (289) | | |
| `+[OOColor purpleColor]` (294) | | |
| `+[OOColor brownColor]` (299) | | |
| `+[OOColor clearColor]` (304) | | |
| `-[OOColor blendedColorWithFraction:ofColor:]` (309) | | |
| `-[OOColor descriptionComponents]` (323) | | |
| `-[OOColor redComponent]` (329) | | |
| `-[OOColor greenComponent]` (334) | | |
| `-[OOColor blueComponent]` (339) | | |
| `-[OOColor getRed:green:blue:alpha:]` (344) | | |
| `-[OOColor rgbaComponents]` (354) | | |
| `-[OOColor isBlack]` (360) | | |
| `-[OOColor isWhite]` (365) | | |
| `-[OOColor hueComponent]` (371) | | |
| `-[OOColor saturationComponent]` (390) | | |
| `-[OOColor brightnessComponent]` (397) | | |
| `-[OOColor getHue:saturation:brightness:alpha:]` (403) | | |
| `-[OOColor hsbaComponents]` (431) | | |
| `-[OOColor alphaComponent]` (442) | | |
| `-[OOColor premultipliedColor]` (447) | | |
| `-[OOColor colorWithBrightnessFactor:]` (457) | | |
| `-[OOColor normalizedArray]` (465) | | |
| `-[OOColor rgbaDescription]` (477) | | |
| `-[OOColor hsbaDescription]` (482) | | |
| `OORGBAComponentsDescription()` (489) | | |
| `OOHSBAComponentsDescription()` (494) | | |

**Differences:**

- *pending*


## OOCommodities

<a id="oocommodities"></a>
Upstream `src/Core/OOCommodities.m` ↔ `Core/OOCommodities.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOCommodities legacyCommodityType:]` (55) | | |
| `-[OOCommodities init]` (97) | | |
| `-[OOCommodities dealloc]` (122) | | |
| `-[OOCommodities generateManifestForPlayer]` (129) | | |
| `-[OOCommodities generateBlankMarket]` (150) | | |
| `-[OOCommodities createDefinitionFrom:price:andQuantity:forKey:atStation:inSystem:]` (169) | | |
| `-[OOCommodities modifyGood:withScript:atStation:inSystem:localMode:]` (196) | | |
| `-[OOCommodities generateMarketForSystemWithEconomy:andScript:]` (247) | | |
| `-[OOCommodities generateMarketForStation:]` (275) | | |
| `-[OOCommodities count]` (333) | | |
| `-[OOCommodities goods]` (338) | | |
| `-[OOCommodities goodDefined:]` (343) | | |
| `-[OOCommodities goodNamed:]` (348) | | |
| `-[OOCommodities getRandomCommodity]` (361) | | |
| `-[OOCommodities massUnitForGood:]` (368) | | |
| `-[OOCommodities generateQuantityForGood:inEconomy:]` (377) | | |
| `-[OOCommodities generatePriceForGood:inEconomy:]` (392) | | |
| `-[OOCommodities samplePriceForCommodity:inEconomy:withScript:inSystem:]` (407) | | |
| `-[OOCommodities economicBiasForGood:inEconomy:]` (426) | | |
| `-[OOCommodities firstModifierForGood:inClasses:fromList:]` (448) | | |
| `-[OOCommodities adjustPrice:byRule:]` (469) | | |
| `-[OOCommodities adjustQuantity:byRule:]` (489) | | |
| `-[OOCommodities updateInfoFor:byRule:maxCapacity:]` (510) | | |

**Differences:**

- *pending*


## OOCommodityMarket

<a id="oocommoditymarket"></a>
Upstream `src/Core/OOCommodityMarket.m` ↔ `Core/OOCommodityMarket.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCommodityMarket init]` (34) | | |
| `-[OOCommodityMarket dealloc]` (47) | | |
| `-[OOCommodityMarket count]` (54) | | |
| `-[OOCommodityMarket setGood:withInfo:]` (59) | | |
| `-[OOCommodityMarket goods]` (66) | | |
| `-[OOCommodityMarket dictionaryForScripting]` (75) | | |
| `-[OOCommodityMarket setPrice:forGood:]` (80) | | |
| `-[OOCommodityMarket setQuantity:forGood:]` (90) | | |
| `-[OOCommodityMarket addQuantity:forGood:]` (100) | | |
| `-[OOCommodityMarket removeQuantity:forGood:]` (110) | | |
| `-[OOCommodityMarket removeAllGoods]` (120) | | |
| `-[OOCommodityMarket setComment:forGood:]` (128) | | |
| `-[OOCommodityMarket setShortComment:forGood:]` (138) | | |
| `-[OOCommodityMarket nameForGood:]` (148) | | |
| `-[OOCommodityMarket commentForGood:]` (157) | | |
| `-[OOCommodityMarket shortCommentForGood:]` (166) | | |
| `-[OOCommodityMarket priceForGood:]` (175) | | |
| `-[OOCommodityMarket quantityForGood:]` (184) | | |
| `-[OOCommodityMarket massUnitForGood:]` (193) | | |
| `-[OOCommodityMarket exportLegalityForGood:]` (202) | | |
| `-[OOCommodityMarket importLegalityForGood:]` (211) | | |
| `-[OOCommodityMarket capacityForGood:]` (220) | | |
| `-[OOCommodityMarket trumbleOpinionForGood:]` (231) | | |
| `-[OOCommodityMarket definitionForGood:]` (240) | | |
| `-[OOCommodityMarket savePlayerAmounts]` (245) | | |
| `-[OOCommodityMarket loadPlayerAmounts:]` (255) | | |
| `-[OOCommodityMarket saveStationAmounts]` (292) | | |
| `-[OOCommodityMarket loadStationAmounts:]` (302) | | |
| `goodsSorter()` (341) | | |

**Differences:**

- *pending*


## OOConstToString

<a id="ooconsttostring"></a>
Upstream `src/Core/OOConstToString.m` ↔ `Core/OOConstToString.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOStringFromEntityStatus()` (50) | | |
| `OOStringFromBehaviour()` (58) | | |
| `OOStringFromCompassMode()` (67) | | |
| `OOStringFromLongRangeChartMode()` (76) | | |
| `OOStringFromGalacticHyperspaceBehaviour()` (85) | | |
| `OOStringFromGUIScreenID()` (94) | | |
| `OOStringFromScanClass()` (103) | | |
| `OOStringFromShipDamageType()` (112) | | |
| `OOStringFromLegalStatusReason()` (121) | | |
| `OOEntityStatusFromString()` (140) | | |
| `OOCompassModeFromString()` (147) | | |
| `OOGalacticHyperspaceBehaviourFromString()` (154) | | |
| `OOGUIScreenIDFromString()` (168) | | |
| `OOScanClassFromString()` (175) | | |
| `OOLongRangeChartModeFromString()` (182) | | |
| `RouteTypeToString()` (192) | | |
| `OODisplayStringFromGovernmentID()` (203) | | |
| `OODisplayStringFromEconomyID()` (218) | | |
| `JSTypeToString()` (233) | | |
| `OOStringFromWeaponType()` (249) | | |
| `OOWeaponTypeFromString()` (258) | | |
| `OOEquipmentIdentifierFromWeaponType()` (263) | | |
| `OOWeaponTypeFromEquipmentIdentifierSloppy()` (268) | | |
| `OOWeaponTypeFromEquipmentIdentifierLegacy()` (285) | | |
| `OOWeaponTypeFromEquipmentIdentifierStrict()` (306) | | |
| `CargoTypeToString()` (312) | | |
| `StringToCargoType()` (327) | | |
| `EnergyUnitTypeToString()` (345) | | |
| `StringToEnergyUnitType()` (362) | | |
| `OODisplayRatingStringFromKillCount()` (373) | | |
| `KillCountToRatingAndKillString()` (399) | | |
| `OODisplayStringFromLegalStatus()` (404) | | |
| `OODisplayStringFromAlertCondition()` (424) | | |
| `OODisplayStringFromShaderSetting()` (430) | | |
| `OOStringFromShaderSetting()` (446) | | |
| `OOShaderSettingFromString()` (458) | | |
| `CommodityDisplayNameForSymbolicName()` (468) | | |
| `CommodityDisplayNameForCommodityArray()` (474) | | |
| `DisplayStringForMassUnit()` (479) | | |
| `DisplayStringForMassUnitForCommodity()` (493) | | |
| `StringToRouteType()` (498) | | |
| `DockingClearanceStatusToString()` (507) | | |
| `OOStringFromGraphicsDetail()` (520) | | |
| `OOGraphicsDetailFromString()` (532) | | |
| `OOStringFromHDRToneMapper()` (542) | | |
| `OOHDRToneMapperFromString()` (555) | | |
| `OOStringFromSDRToneMapper()` (566) | | |
| `OOSDRToneMapperFromString()` (581) | | |

**Differences:**

- *pending*


## OOConvertSystemDescriptions

<a id="ooconvertsystemdescriptions"></a>
Upstream `src/Core/OOConvertSystemDescriptions.m` ↔ `Core/OOConvertSystemDescriptions.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `CompileSystemDescriptions()` (45) | | |
| `ExportSystemDescriptions()` (94) | | |
| `OOConvertSystemDescriptionsToArrayFormat()` (135) | | |
| `OOConvertSystemDescriptionsToDictionaryFormat()` (177) | | |
| `OOStringifySystemDescriptionLine()` (200) | | |
| `InitKeyToIndexDict()` (232) | | |
| `IndexToKey()` (256) | | |
| `ConvertIndicesToKeys()` (265) | | |
| `KeyToIndex()` (279) | | |
| `ConvertKeysToIndices()` (300) | | |
| `HighestIndex()` (337) | | |

**Differences:**

- *pending*


## OOEntityFilterPredicate

<a id="ooentityfilterpredicate"></a>
Upstream `src/Core/OOEntityFilterPredicate.m` ↔ `Core/OOEntityFilterPredicate.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `YESPredicate()` (34) | | |
| `NOPredicate()` (39) | | |
| `NOTPredicate()` (44) | | |
| `ANDPredicate()` (53) | | |
| `ORPredicate()` (64) | | |
| `NORPredicate()` (75) | | |
| `XORPredicate()` (86) | | |
| `NANDPredicate()` (97) | | |
| `HasScanClassPredicate()` (108) | | |
| `HasClassPredicate()` (113) | | |
| `IsShipPredicate()` (118) | | |
| `IsStationPredicate()` (123) | | |
| `IsPlanetPredicate()` (128) | | |
| `IsSunPredicate()` (136) | | |
| `IsVisualEffectPredicate()` (141) | | |
| `HasRolePredicate()` (146) | | |
| `HasPrimaryRolePredicate()` (151) | | |
| `HasRoleInSetPredicate()` (156) | | |
| `HasPrimaryRoleInSetPredicate()` (161) | | |
| `IsHostileAgainstTargetPredicate()` (166) | | |

**Differences:**

- *pending*


## OOEquipmentType

<a id="ooequipmenttype"></a>
Upstream `src/Core/OOEquipmentType.m` ↔ `Core/OOEquipmentType.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOEquipmentType loadEquipment]` (50) | | |
| `+[OOEquipmentType addEquipmentWithInfo:]` (104) | | |
| `+[OOEquipmentType getMissileRegistryRoleForShip:]` (127) | | |
| `+[OOEquipmentType setMissileRegistryRole:forShip:]` (132) | | |
| `+[OOEquipmentType allEquipmentTypes]` (143) | | |
| `+[OOEquipmentType equipmentEnumerator]` (148) | | |
| `+[OOEquipmentType reverseEquipmentEnumerator]` (153) | | |
| `+[OOEquipmentType equipmentEnumeratorOutfitting]` (158) | | |
| `+[OOEquipmentType equipmentTypeWithIdentifier:]` (163) | | |
| `-[OOEquipmentType initWithInfo:]` (168) | | |
| `-[OOEquipmentType dealloc]` (370) | | |
| `-[OOEquipmentType copyWithZone:]` (391) | | |
| `-[OOEquipmentType descriptionComponents]` (397) | | |
| `-[OOEquipmentType identifier]` (402) | | |
| `-[OOEquipmentType damagedIdentifier]` (407) | | |
| `-[OOEquipmentType name]` (412) | | |
| `-[OOEquipmentType descriptiveText]` (417) | | |
| `-[OOEquipmentType techLevel]` (422) | | |
| `-[OOEquipmentType price]` (427) | | |
| `-[OOEquipmentType isAvailableToAll]` (432) | | |
| `-[OOEquipmentType requiresEmptyPylon]` (437) | | |
| `-[OOEquipmentType requiresMountedPylon]` (442) | | |
| `-[OOEquipmentType requiresCleanLegalRecord]` (447) | | |
| `-[OOEquipmentType requiresNonCleanLegalRecord]` (452) | | |
| `-[OOEquipmentType requiresFreePassengerBerth]` (457) | | |
| `-[OOEquipmentType requiresFullFuel]` (462) | | |
| `-[OOEquipmentType requiresNonFullFuel]` (467) | | |
| `-[OOEquipmentType isPrimaryWeapon]` (472) | | |
| `-[OOEquipmentType isMissileOrMine]` (477) | | |
| `-[OOEquipmentType isPortableBetweenShips]` (482) | | |
| `-[OOEquipmentType canCarryMultiple]` (487) | | |
| `-[OOEquipmentType damageProbability]` (504) | | |
| `-[OOEquipmentType canBeDamaged]` (512) | | |
| `-[OOEquipmentType isVisible]` (524) | | |
| `-[OOEquipmentType hideValues]` (529) | | |
| `-[OOEquipmentType isAvailableToPlayer]` (534) | | |
| `-[OOEquipmentType isAvailableToNPCs]` (539) | | |
| `-[OOEquipmentType requiredCargoSpace]` (544) | | |
| `-[OOEquipmentType requiresEquipment]` (549) | | |
| `-[OOEquipmentType requiresAnyEquipment]` (554) | | |
| `-[OOEquipmentType incompatibleEquipment]` (559) | | |
| `-[OOEquipmentType displayColor]` (564) | | |
| `-[OOEquipmentType setDisplayColor:]` (569) | | |
| `-[OOEquipmentType conditions]` (575) | | |
| `-[OOEquipmentType conditionScript]` (580) | | |
| `-[OOEquipmentType scriptInfo]` (585) | | |
| `-[OOEquipmentType scriptName]` (590) | | |
| `-[OOEquipmentType fastAffinityDefensive]` (595) | | |
| `-[OOEquipmentType fastAffinityOffensive]` (600) | | |
| `-[OOEquipmentType defaultActivateKey]` (605) | | |
| `-[OOEquipmentType defaultModeKey]` (610) | | |
| `-[OOEquipmentType installTime]` (615) | | |
| `-[OOEquipmentType repairTime]` (620) | | |
| `-[OOEquipmentType providesForScripting]` (629) | | |
| `-[OOEquipmentType provides:]` (634) | | |
| `-[OOEquipmentType isTurretLaser]` (640) | | |
| `-[OOEquipmentType isMiningLaser]` (645) | | |
| `-[OOEquipmentType weaponInfo]` (650) | | |
| `-[OOEquipmentType weaponRange]` (655) | | |
| `-[OOEquipmentType weaponEnergyUse]` (660) | | |
| `-[OOEquipmentType weaponDamage]` (665) | | |
| `-[OOEquipmentType weaponRechargeRate]` (670) | | |
| `-[OOEquipmentType weaponShotTemperature]` (675) | | |
| `-[OOEquipmentType weaponThreatAssessment]` (680) | | |
| `-[OOEquipmentType weaponColor]` (685) | | |
| `-[OOEquipmentType fxShotMissName]` (690) | | |
| `-[OOEquipmentType fxShotHitName]` (695) | | |
| `-[OOEquipmentType fxShieldHitName]` (700) | | |
| `-[OOEquipmentType fxUnshieldedHitName]` (705) | | |
| `-[OOEquipmentType fxWeaponLaunchedName]` (710) | | |
| `-[OOEquipmentType suppressClangStuff]` (720) | | |
| `-[OOEquipmentType(Conveniences) effectiveTechLevel]` (731) | | |

**Differences:**

- *pending*


## OOOXZManager

<a id="oooxzmanager"></a>
Upstream `src/Core/OOOXZManager.m` ↔ `Core/OOOXZManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOOXZManager sharedManager]` (176) | | |
| `-[OOOXZManager init]` (184) | | |
| `-[OOOXZManager dealloc]` (204) | | |
| `-[OOOXZManager installPath]` (220) | | |
| `-[OOOXZManager extractAddOnsPath]` (246) | | |
| `-[OOOXZManager additionalAddOnsPaths]` (266) | | |
| `-[OOOXZManager extractionBasePathForIdentifier:andVersion:]` (277) | | |
| `-[OOOXZManager ensureInstallPath]` (286) | | |
| `-[OOOXZManager manifestPath]` (308) | | |
| `-[OOOXZManager downloadPath]` (316) | | |
| `-[OOOXZManager dataURL]` (325) | | |
| `-[OOOXZManager humanSize:]` (335) | | |
| `-[OOOXZManager setOXZList:]` (348) | | |
| `-[OOOXZManager setFilteredList:]` (358) | | |
| `-[OOOXZManager setFilter:]` (364) | | |
| `-[OOOXZManager setOXPURL:]` (370) | | |
| `-[OOOXZManager applyCurrentFilter:]` (376) | | |
| `-[OOOXZManager applyFilterByNoFilter:]` (425) | | |
| `-[OOOXZManager applyFilterByUpdateRequired:]` (430) | | |
| `-[OOOXZManager applyFilterByInstallable:]` (435) | | |
| `-[OOOXZManager applyFilterByKeyword:keyword:]` (440) | | |
| `-[OOOXZManager applyFilterByAuthor:author:]` (464) | | |
| `-[OOOXZManager applyFilterByDays:days:]` (473) | | |
| `-[OOOXZManager applyFilterByTag:tag:]` (485) | | |
| `-[OOOXZManager applyFilterByCategory:category:]` (502) | | |
| `-[OOOXZManager validateFilter:]` (513) | | |
| `-[OOOXZManager setCurrentDownload:withLabel:]` (531) | | |
| `-[OOOXZManager setProgressStatus:]` (541) | | |
| `-[OOOXZManager updateManifests]` (547) | | |
| `-[OOOXZManager beginDownload:]` (560) | | |
| `-[OOOXZManager cancelUpdate]` (588) | | |
| `-[OOOXZManager manifests]` (610) | | |
| `-[OOOXZManager managedOXZs]` (615) | | |
| `-[OOOXZManager processDownloadedManifests]` (660) | | |
| `-[OOOXZManager processDownloadedOXZ]` (686) | | |
| `-[OOOXZManager installedManifestForIdentifier:]` (885) | | |
| `-[OOOXZManager installableState:]` (897) | | |
| `-[OOOXZManager colorForManifest:]` (951) | | |
| `-[OOOXZManager installStatusForManifest:]` (974) | | |
| `-[OOOXZManager gui]` (997) | | |
| `-[OOOXZManager isRestarting]` (1195) | | |
| `-[OOOXZManager processSelection]` (1211) | | |
| `-[OOOXZManager isAcceptingTextInput]` (1301) | | |
| `-[OOOXZManager isAcceptingGUIInput]` (1306) | | |
| `-[OOOXZManager processTextInput:]` (1311) | | |
| `-[OOOXZManager refreshTextInput:]` (1323) | | |
| `-[OOOXZManager processFilterKey]` (1334) | | |
| `-[OOOXZManager processShowInfoKey]` (1347) | | |
| `-[OOOXZManager processCopyUrlKey]` (1417) | | |
| `-[OOOXZManager processExtractKey]` (1443) | | |
| `-[OOOXZManager installOXZ:]` (1462) | | |
| `-[OOOXZManager updateAllOXZ]` (1493) | | |
| `-[OOOXZManager installOptions]` (1519) | | |
| `-[OOOXZManager showInstallOptions]` (1533) | | |
| `-[OOOXZManager removeOXZ:]` (1679) | | |
| `-[OOOXZManager removeOptions]` (1703) | | |
| `-[OOOXZManager showRemoveOptions]` (1721) | | |
| `-[OOOXZManager showOptionsUpdate]` (1815) | | |
| `-[OOOXZManager showOptionsPrev]` (1831) | | |
| `-[OOOXZManager processOptionsPrev]` (1841) | | |
| `-[OOOXZManager processOptionsNext]` (1851) | | |
| `-[OOOXZManager showOptionsNext]` (1860) | | |
| `-[OOOXZManager extractOXZ:]` (1870) | | |
| `-[OOOXZManager connection:didReceiveResponse:]` (1979) | | |
| `-[OOOXZManager connection:didReceiveData:]` (1995) | | |
| `-[OOOXZManager connectionDidFinishLoading:]` (2022) | | |
| `-[OOOXZManager connection:didFailWithError:]` (2044) | | |
| `oxzSort()` (2056) | | |

**Differences:**

- *pending*


## OOProbabilitySet

<a id="ooprobabilityset"></a>
Upstream `src/Core/OOProbabilitySet.m` ↔ `Core/OOProbabilitySet.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOProbabilitySet probabilitySet]` (121) | | |
| `+[OOProbabilitySet probabilitySetWithObjects:weights:count:]` (126) | | |
| `+[OOProbabilitySet probabilitySetWithPropertyListRepresentation:]` (131) | | |
| `-[OOProbabilitySet init]` (136) | | |
| `-[OOProbabilitySet initWithObjects:weights:count:]` (142) | | |
| `-[OOProbabilitySet initWithPropertyListRepresentation:]` (164) | | |
| `-[OOProbabilitySet initPriv]` (212) | | |
| `-[OOProbabilitySet descriptionComponents]` (217) | | |
| `-[OOProbabilitySet propertyListRepresentation]` (222) | | |
| `-[OOProbabilitySet randomObject]` (227) | | |
| `-[OOProbabilitySet weightForObject:]` (232) | | |
| `-[OOProbabilitySet sumOfWeights]` (237) | | |
| `-[OOProbabilitySet count]` (242) | | |
| `-[OOProbabilitySet allObjects]` (247) | | |
| `-[OOProbabilitySet copyWithZone:]` (252) | | |
| `-[OOProbabilitySet mutableCopyWithZone:]` (261) | | |
| `-[OOProbabilitySet(OOExtendedProbabilitySet) containsObject:]` (270) | | |
| `-[OOProbabilitySet(OOExtendedProbabilitySet) objectEnumerator]` (275) | | |
| `-[OOProbabilitySet(OOExtendedProbabilitySet) probabilityForObject:]` (280) | | |
| `+[OOEmptyProbabilitySet singleton]` (295) | | |
| `-[OOEmptyProbabilitySet propertyListRepresentation]` (304) | | |
| `-[OOEmptyProbabilitySet randomObject]` (310) | | |
| `-[OOEmptyProbabilitySet weightForObject:]` (315) | | |
| `-[OOEmptyProbabilitySet sumOfWeights]` (320) | | |
| `-[OOEmptyProbabilitySet count]` (325) | | |
| `-[OOEmptyProbabilitySet allObjects]` (330) | | |
| `-[OOEmptyProbabilitySet mutableCopyWithZone:]` (335) | | |
| `+[OOEmptyProbabilitySet(Singleton) allocWithZone:]` (352) | | |
| `-[OOEmptyProbabilitySet(Singleton) copyWithZone:]` (361) | | |
| `-[OOEmptyProbabilitySet(Singleton) retain]` (366) | | |
| `-[OOEmptyProbabilitySet(Singleton) retainCount]` (371) | | |
| `-[OOEmptyProbabilitySet(Singleton) release]` (376) | | |
| `-[OOEmptyProbabilitySet(Singleton) autorelease]` (380) | | |
| `-[OOSingleObjectProbabilitySet initWithObject:weight:]` (389) | | |
| `-[OOSingleObjectProbabilitySet dealloc]` (404) | | |
| `-[OOSingleObjectProbabilitySet propertyListRepresentation]` (411) | | |
| `-[OOSingleObjectProbabilitySet randomObject]` (419) | | |
| `-[OOSingleObjectProbabilitySet weightForObject:]` (424) | | |
| `-[OOSingleObjectProbabilitySet sumOfWeights]` (432) | | |
| `-[OOSingleObjectProbabilitySet count]` (437) | | |
| `-[OOSingleObjectProbabilitySet allObjects]` (442) | | |
| `-[OOSingleObjectProbabilitySet mutableCopyWithZone:]` (447) | | |
| `-[OOConcreteProbabilitySet initWithObjects:weights:count:]` (456) | | |
| `-[OOConcreteProbabilitySet dealloc]` (485) | | |
| `-[OOConcreteProbabilitySet propertyListRepresentation]` (505) | | |
| `-[OOConcreteProbabilitySet count]` (526) | | |
| `-[OOConcreteProbabilitySet privObjectForWeight:]` (531) | | |
| `-[OOConcreteProbabilitySet randomObject]` (568) | | |
| `-[OOConcreteProbabilitySet weightForObject:]` (575) | | |
| `-[OOConcreteProbabilitySet sumOfWeights]` (595) | | |
| `-[OOConcreteProbabilitySet allObjects]` (600) | | |
| `-[OOConcreteProbabilitySet objectEnumerator]` (605) | | |
| `-[OOConcreteProbabilitySet privObjectAtIndex:]` (610) | | |
| `-[OOConcreteProbabilitySet mutableCopyWithZone:]` (615) | | |
| `+[OOMutableProbabilitySet probabilitySet]` (643) | | |
| `-[OOMutableProbabilitySet init]` (648) | | |
| `-[OOMutableProbabilitySet initWithObjects:weights:count:]` (655) | | |
| `-[OOMutableProbabilitySet initWithPropertyListRepresentation:]` (662) | | |
| `-[OOMutableProbabilitySet copyWithZone:]` (669) | | |
| `-[OOMutableProbabilitySet setWeight:forObject:]` (674) | | |
| `-[OOMutableProbabilitySet removeObject:]` (679) | | |
| `-[OOConcreteMutableProbabilitySet initPriv]` (688) | | |
| `-[OOConcreteMutableProbabilitySet initPrivWithObjectArray:weightsArray:sum:]` (699) | | |
| `-[OOConcreteMutableProbabilitySet initWithObjects:weights:count:]` (712) | | |
| `-[OOConcreteMutableProbabilitySet initWithPropertyListRepresentation:]` (732) | | |
| `-[OOConcreteMutableProbabilitySet dealloc]` (768) | | |
| `-[OOConcreteMutableProbabilitySet propertyListRepresentation]` (776) | | |
| `-[OOConcreteMutableProbabilitySet count]` (784) | | |
| `-[OOConcreteMutableProbabilitySet randomObject]` (789) | | |
| `-[OOConcreteMutableProbabilitySet weightForObject:]` (810) | | |
| `-[OOConcreteMutableProbabilitySet sumOfWeights]` (825) | | |
| `-[OOConcreteMutableProbabilitySet allObjects]` (839) | | |
| `-[OOConcreteMutableProbabilitySet objectEnumerator]` (844) | | |
| `-[OOConcreteMutableProbabilitySet setWeight:forObject:]` (849) | | |
| `-[OOConcreteMutableProbabilitySet removeObject:]` (869) | | |
| `-[OOConcreteMutableProbabilitySet copyWithZone:]` (882) | | |
| `-[OOConcreteMutableProbabilitySet mutableCopyWithZone:]` (913) | | |
| `-[OOProbabilitySetEnumerator initWithEnumerable:]` (924) | | |
| `-[OOProbabilitySetEnumerator dealloc]` (933) | | |
| `-[OOProbabilitySetEnumerator nextObject]` (940) | | |
| `ThrowAbstractionViolationException()` (953) | | |

**Differences:**

- *pending*


## OORoleSet

<a id="ooroleset"></a>
Upstream `src/Core/OORoleSet.m` ↔ `Core/OORoleSet.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OORoleSet roleSetWithString:]` (42) | | |
| `+[OORoleSet roleSetWithRole:probability:]` (47) | | |
| `-[OORoleSet initWithRoleString:]` (52) | | |
| `-[OORoleSet initWithRole:probability:]` (60) | | |
| `-[OORoleSet dealloc]` (70) | | |
| `-[OORoleSet description]` (79) | | |
| `-[OORoleSet isEqual:]` (84) | | |
| `-[OORoleSet hash]` (92) | | |
| `-[OORoleSet copyWithZone:]` (97) | | |
| `-[OORoleSet roleString]` (103) | | |
| `-[OORoleSet hasRole:]` (135) | | |
| `-[OORoleSet probabilityForRole:]` (140) | | |
| `-[OORoleSet intersectsSet:]` (145) | | |
| `-[OORoleSet roles]` (155) | | |
| `-[OORoleSet sortedRoles]` (163) | | |
| `-[OORoleSet rolesAndProbabilities]` (168) | | |
| `-[OORoleSet anyRole]` (173) | | |
| `-[OORoleSet roleSetWithAddedRoleIfNotSet:probability:]` (197) | | |
| `-[OORoleSet roleSetWithAddedRole:probability:]` (210) | | |
| `-[OORoleSet roleSetWithRemovedRole:]` (223) | | |
| `-[OORoleSet(OOPrivate) initWithRolesAndProbabilities:]` (239) | | |
| `OOParseRolesFromString()` (286) | | |

**Differences:**

- *pending*


## OOShipGroup

<a id="ooshipgroup"></a>
Upstream `src/Core/OOShipGroup.m` ↔ `Core/OOShipGroup.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOShipGroup init]` (73) | | |
| `-[OOShipGroup initWithName:]` (78) | | |
| `+[OOShipGroup groupWithName:]` (94) | | |
| `+[OOShipGroup groupWithName:leader:]` (99) | | |
| `-[OOShipGroup dealloc]` (106) | | |
| `-[OOShipGroup descriptionComponents]` (119) | | |
| `-[OOShipGroup name]` (131) | | |
| `-[OOShipGroup setName:]` (136) | | |
| `-[OOShipGroup leader]` (146) | | |
| `-[OOShipGroup setLeader:]` (159) | | |
| `-[OOShipGroup objectEnumerator]` (170) | | |
| `-[OOShipGroup mutationSafeEnumerator]` (175) | | |
| `-[OOShipGroup members]` (180) | | |
| `-[OOShipGroup membersExcludingLeader]` (185) | | |
| `-[OOShipGroup memberArray]` (190) | | |
| `-[OOShipGroup memberArrayExcludingLeader]` (210) | | |
| `-[OOShipGroup containsShip:]` (234) | | |
| `-[OOShipGroup addShip:]` (247) | | |
| `-[OOShipGroup removeShip:]` (268) | | |
| `-[OOShipGroup count]` (304) | | |
| `-[OOShipGroup isEmpty]` (320) | | |
| `-[OOShipGroup resizeTo:]` (328) | | |
| `-[OOShipGroup cleanUp]` (344) | | |
| `-[OOShipGroup updateCount]` (364) | | |
| `ShipGroupIterate()` (369) | | |
| `-[OOShipGroup countByEnumeratingWithState:objects:count:]` (404) | | |
| `-[OOShipGroup suppressClangStuff]` (437) | | |
| `-[OOShipGroupEnumerator initWithShipGroup:]` (446) | | |
| `-[OOShipGroupEnumerator dealloc]` (459) | | |
| `-[OOShipGroupEnumerator nextObject]` (466) | | |
| `-[OOShipGroupEnumerator index]` (471) | | |
| `-[OOShipGroupEnumerator setPerformCleanup:]` (476) | | |

**Differences:**

- *pending*


## OOShipLibraryDescriptions

<a id="ooshiplibrarydescriptions"></a>
Upstream `src/Core/OOShipLibraryDescriptions.m` ↔ `Core/OOShipLibraryDescriptions.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOShipLibraryCategorySingular()` (29) | | |
| `OOShipLibraryCategoryPlural()` (34) | | |
| `OOShipLibrarySpeed()` (39) | | |
| `OOShipLibraryTurnRate()` (59) | | |
| `OOShipLibraryCargo()` (77) | | |
| `OOShipLibraryGenerator()` (89) | | |
| `OOShipLibraryShields()` (103) | | |
| `OOShipLibraryWitchspace()` (122) | | |
| `OOShipLibraryWeapons()` (131) | | |
| `OOShipLibraryTurrets()` (142) | | |
| `OOShipLibrarySize()` (152) | | |

**Differences:**

- *pending*


## OOShipRegistry

<a id="ooshipregistry"></a>
Upstream `src/Core/OOShipRegistry.m` ↔ `Core/OOShipRegistry.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOShipRegistry sharedRegistry]` (119) | | |
| `+[OOShipRegistry reload]` (128) | | |
| `-[OOShipRegistry init]` (141) | | |
| `-[OOShipRegistry dealloc]` (180) | | |
| `-[OOShipRegistry shipInfoForKey:]` (190) | | |
| `-[OOShipRegistry setShipInfoForKey:with:]` (195) | | |
| `-[OOShipRegistry effectInfoForKey:]` (203) | | |
| `-[OOShipRegistry shipyardInfoForKey:]` (208) | | |
| `-[OOShipRegistry probabilitySetForRole:]` (213) | | |
| `-[OOShipRegistry demoShipKeys]` (220) | | |
| `-[OOShipRegistry playerShipKeys]` (228) | | |
| `-[OOShipRegistry(OOConveniences) shipKeys]` (237) | | |
| `-[OOShipRegistry(OOConveniences) shipRoles]` (242) | | |
| `-[OOShipRegistry(OOConveniences) shipKeysWithRole:]` (247) | | |
| `-[OOShipRegistry(OOConveniences) randomShipKeyForRole:]` (252) | | |
| `-[OOShipRegistry(OODataLoader) loadShipData]` (271) | | |
| `-[OOShipRegistry(OODataLoader) loadDemoShipConditions]` (383) | | |
| `-[OOShipRegistry(OODataLoader) loadDemoShips]` (411) | | |
| `-[OOShipRegistry(OODataLoader) loadCachedRoleProbabilitySets]` (511) | | |
| `-[OOShipRegistry(OODataLoader) buildRoleProbabilitySets]` (530) | | |
| `-[OOShipRegistry(OODataLoader) applyLikeShips:withKey:]` (578) | | |
| `-[OOShipRegistry(OODataLoader) mergeShip:withParent:]` (642) | | |
| `-[OOShipRegistry(OODataLoader) makeShipEntriesMutable:]` (696) | | |
| `-[OOShipRegistry(OODataLoader) loadAndApplyShipDataOverrides:]` (717) | | |
| `-[OOShipRegistry(OODataLoader) stripPrivateKeys:]` (745) | | |
| `-[OOShipRegistry(OODataLoader) loadAndMergeShipyard:]` (773) | | |
| `-[OOShipRegistry(OODataLoader) canonicalizeAndTagSubentities:]` (826) | | |
| `-[OOShipRegistry(OODataLoader) removeUnusableEntries:shipMode:]` (906) | | |
| `-[OOShipRegistry(OODataLoader) sanitizeConditions:]` (947) | | |
| `-[OOShipRegistry(OODataLoader) preloadShipMeshes:]` (1050) | | |
| `-[OOShipRegistry(OODataLoader) mergeShipRoles:]` (1095) | | |
| `-[OOShipRegistry(OODataLoader) canonicalizeSubentityDeclaration:]` (1136) | | |
| `-[OOShipRegistry(OODataLoader) translateOldStyleSubentityDeclaration:]` (1178) | | |
| `-[OOShipRegistry(OODataLoader) translateOldStyleFlasherDeclaration:]` (1215) | | |
| `-[OOShipRegistry(OODataLoader) translateOldStandardBasicSubentityDeclaration:]` (1249) | | |
| `-[OOShipRegistry(OODataLoader) validateNewStyleSubentityDeclaration:]` (1297) | | |
| `-[OOShipRegistry(OODataLoader) validateNewStyleFlasherDeclaration:]` (1318) | | |
| `-[OOShipRegistry(OODataLoader) validateNewStyleStandardSubentityDeclaration:]` (1395) | | |
| `-[OOShipRegistry(OODataLoader) shipIsBallTurretForKey:inShipData:]` (1484) | | |
| `+[OOShipRegistry(Singleton) allocWithZone:]` (1517) | | |
| `-[OOShipRegistry(Singleton) copyWithZone:]` (1527) | | |
| `-[OOShipRegistry(Singleton) retain]` (1532) | | |
| `-[OOShipRegistry(Singleton) retainCount]` (1537) | | |
| `-[OOShipRegistry(Singleton) release]` (1542) | | |
| `-[OOShipRegistry(Singleton) autorelease]` (1546) | | |
| `DumpStringAddrs()` (1557) | | |
| `GatherStringAddrsDict()` (1586) | | |
| `GatherStringAddrsArray()` (1597) | | |
| `GatherStringAddrs()` (1606) | | |
| `SortDemoShipsByName()` (1618) | | |
| `SortDemoCategoriesByName()` (1623) | | |

**Differences:**

- *pending*


## OOStringExpander

<a id="oostringexpander"></a>
Upstream `src/Core/OOStringExpander.m` ↔ `Core/OOStringExpander.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOExpandDescriptionString()` (160) | | |
| `OOGenerateSystemDescription()` (220) | | |
| `OOStringExpanderDefaultRandomSeed()` (226) | | |
| `Expand()` (243) | | |
| `ExpandKey()` (349) | | |
| `ApplyOperators()` (410) | | |
| `Operator_cr()` (428) | | |
| `Operator_dcr()` (433) | | |
| `Operator_icr()` (438) | | |
| `Operator_idcr()` (443) | | |
| `Operator_precision()` (448) | | |
| `Operator_multiply()` (453) | | |
| `Operator_add()` (458) | | |
| `ApplyOneOperator()` (472) | | |
| `ExpandDigitKey()` (508) | | |
| `ExpandStringKey()` (563) | | |
| `ExpandStringKeyOverride()` (613) | | |
| `ExpandStringKeySpecial()` (636) | | |
| `ExpandStringKeyKeyboardBinding()` (659) | | |
| `SpecialSubstitutionSelectors()` (674) | | |
| `ExpandStringKeyFromDescriptions()` (712) | | |
| `ExpandStringKeyMissionVariable()` (738) | | |
| `ExpandStringKeyLegacyLocalVariable()` (753) | | |
| `ExpandLegacyScriptSelectorKey()` (764) | | |
| `LookUpLegacySelector()` (782) | | |
| `ReportWarningForUnknownKey()` (834) | | |
| `ExpandPercentEscape()` (864) | | |
| `ExpandPercentR()` (934) | | |
| `ExpandSystemNameForGalaxyEscape()` (967) | | |
| `ExpandSystemNameEscape()` (1014) | | |
| `AppendCharacters()` (1047) | | |
| `GetSystemName()` (1075) | | |
| `GetSystemNameIan()` (1085) | | |
| `GetRandomNameN()` (1096) | | |
| `GetRandomNameR()` (1107) | | |
| `GetSystemDescriptions()` (1118) | | |
| `OldRandomDigrams()` (1134) | | |
| `NewRandomDigrams()` (1152) | | |
| `SyntaxIssue()` (1168) | | |

**Differences:**

- *pending*


## OOStringParsing

<a id="oostringparsing"></a>
Upstream `src/Core/OOStringParsing.m` ↔ `Core/OOStringParsing.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `ScanTokensFromString()` (44) | | |
| `ScanVectorFromString()` (70) | | |
| `ScanHPVectorFromString()` (100) | | |
| `ScanQuaternionFromString()` (112) | | |
| `ScanVectorAndQuaternionFromString()` (146) | | |
| `VectorFromString()` (183) | | |
| `QuaternionFromString()` (191) | | |
| `StringFromPoint()` (199) | | |
| `PointFromString()` (204) | | |
| `RandomSeedFromString()` (217) | | |
| `StringFromRandomSeed()` (248) | | |
| `OOPadStringToEms()` (253) | | |
| `OOStringFromDeciCredits()` (270) | | |
| `-[NSString(OOUtilities) pathHasExtension:]` (323) | | |
| `-[NSString(OOUtilities) pathHasExtensionInArray:]` (328) | | |
| `ComponentsFromVersionString()` (342) | | |
| `CompareVersions()` (369) | | |
| `ClockToString()` (402) | | |
| `EscapedGraphVizString()` (424) | | |
| `GraphVizTokenString()` (459) | | |
| `NameIsTaken()` (516) | | |

**Differences:**

- *pending*


## OOSystemDescriptionManager

<a id="oosystemdescriptionmanager"></a>
Upstream `src/Core/OOSystemDescriptionManager.m` ↔ `Core/OOSystemDescriptionManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSystemDescriptionManager init]` (62) | | |
| `-[OOSystemDescriptionManager dealloc]` (82) | | |
| `-[OOSystemDescriptionManager buildRouteCache]` (96) | | |
| `-[OOSystemDescriptionManager setUniversalProperties:]` (120) | | |
| `-[OOSystemDescriptionManager setInterstellarProperties:]` (129) | | |
| `-[OOSystemDescriptionManager setProperties:forSystemKey:]` (134) | | |
| `-[OOSystemDescriptionManager setProperty:forSystemKey:andLayer:toValue:fromManifest:]` (158) | | |
| `-[OOSystemDescriptionManager saveScriptedChangeToProperty:forSystemKey:andLayer:toValue:fromManifest:]` (188) | | |
| `-[OOSystemDescriptionManager importScriptedChanges:]` (206) | | |
| `-[OOSystemDescriptionManager importLegacyScriptedChanges:]` (235) | | |
| `-[OOSystemDescriptionManager exportScriptedChanges]` (271) | | |
| `-[OOSystemDescriptionManager getPropertiesForCurrentSystem]` (276) | | |
| `-[OOSystemDescriptionManager getPropertiesForSystemKey:]` (293) | | |
| `-[OOSystemDescriptionManager getPropertiesForSystem:inGalaxy:]` (310) | | |
| `-[OOSystemDescriptionManager getProperty:forSystemKey:]` (320) | | |
| `-[OOSystemDescriptionManager getProperty:forSystem:inGalaxy:]` (325) | | |
| `-[OOSystemDescriptionManager getProperty:forSystemKey:withUniversal:]` (339) | | |
| `-[OOSystemDescriptionManager getProperty:orProperty:forSystemKey:withUniversal:]` (367) | | |
| `-[OOSystemDescriptionManager setProperties:inDescription:]` (407) | | |
| `-[OOSystemDescriptionManager calculatePropertiesForSystemKey:]` (424) | | |
| `-[OOSystemDescriptionManager updateCacheEntry:]` (448) | | |
| `-[OOSystemDescriptionManager updateCacheEntry:forProperty:]` (458) | | |
| `-[OOSystemDescriptionManager getCoordinatesForSystem:inGalaxy:]` (470) | | |
| `-[OOSystemDescriptionManager getNeighbourIDsForSystem:inGalaxy:]` (484) | | |
| `-[OOSystemDescriptionManager getRandomSeedForCurrentSystem]` (499) | | |
| `-[OOSystemDescriptionManager getRandomSeedForSystem:inGalaxy:]` (514) | | |
| `-[OOSystemDescriptionEntry init]` (536) | | |
| `-[OOSystemDescriptionEntry dealloc]` (547) | | |
| `-[OOSystemDescriptionEntry setProperty:forLayer:toValue:]` (555) | | |
| `-[OOSystemDescriptionEntry getProperty:forLayer:]` (570) | | |
| `-[OOSystemDescriptionEntry validateProperty:withValue:]` (580) | | |

**Differences:**

- *pending*


## OOTrumble

<a id="ootrumble"></a>
Upstream `src/Core/OOTrumble.m` ↔ `Core/OOTrumble.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOTrumble init]` (41) | | |
| `-[OOTrumble initForPlayer:]` (54) | | |
| `-[OOTrumble initForPlayer:digram:]` (63) | | |
| `-[OOTrumble setupForPlayer:digram:]` (72) | | |
| `-[OOTrumble dealloc]` (210) | | |
| `-[OOTrumble spawnFrom:]` (217) | | |
| `-[OOTrumble calcGrowthRate]` (252) | | |
| `-[OOTrumble digram]` (258) | | |
| `-[OOTrumble position]` (263) | | |
| `-[OOTrumble movement]` (268) | | |
| `-[OOTrumble rotation]` (273) | | |
| `-[OOTrumble size]` (278) | | |
| `-[OOTrumble hunger]` (283) | | |
| `-[OOTrumble discomfort]` (288) | | |
| `-[OOTrumble actionIdle]` (294) | | |
| `-[OOTrumble actionBlink]` (300) | | |
| `-[OOTrumble actionSnarl]` (306) | | |
| `-[OOTrumble actionProot]` (312) | | |
| `-[OOTrumble actionShudder]` (318) | | |
| `-[OOTrumble actionStoned]` (324) | | |
| `-[OOTrumble actionPop]` (330) | | |
| `-[OOTrumble actionSleep]` (336) | | |
| `-[OOTrumble actionSpawn]` (342) | | |
| `-[OOTrumble randomizeMotionX]` (348) | | |
| `-[OOTrumble randomizeMotionY]` (355) | | |
| `-[OOTrumble drawTrumble:]` (362) | | |
| `-[OOTrumble updateTrumble:]` (490) | | |
| `-[OOTrumble updateIdle:]` (652) | | |
| `-[OOTrumble updateBlink:]` (689) | | |
| `-[OOTrumble updateSnarl:]` (706) | | |
| `-[OOTrumble updateProot:]` (742) | | |
| `-[OOTrumble updateShudder:]` (775) | | |
| `-[OOTrumble updateStoned:]` (808) | | |
| `-[OOTrumble updatePop:]` (812) | | |
| `-[OOTrumble updateSleep:]` (850) | | |
| `-[OOTrumble updateSpawn:]` (880) | | |
| `-[OOTrumble dictionary]` (906) | | |
| `-[OOTrumble setFromDictionary:]` (923) | | |
| `InitTrumbleSounds()` (943) | | |
| `PlayTrumbleIdle()` (952) | | |
| `PlayTrumbleSqueal()` (962) | | |

**Differences:**

- *pending*


## ResourceManager

<a id="resourcemanager"></a>
Upstream `src/Core/ResourceManager.m` ↔ `Core/ResourceManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[ResourceManager reset]` (102) | | |
| `+[ResourceManager resetManifestKnowledgeForOXZManager]` (114) | | |
| `+[ResourceManager errors]` (123) | | |
| `+[ResourceManager rootPaths]` (151) | | |
| `+[ResourceManager userRootPaths]` (163) | | |
| `+[ResourceManager builtInPath]` (192) | | |
| `+[ResourceManager pathsWithAddOns]` (212) | | |
| `+[ResourceManager preloadFileLists]` (332) | | |
| `+[ResourceManager preloadFileListFromOXZ:forFolders:]` (349) | | |
| `+[ResourceManager preloadFileListFromFolder:forFolders:]` (388) | | |
| `+[ResourceManager preloadFilePathFor:inFolder:atPath:]` (406) | | |
| `+[ResourceManager paths]` (418) | | |
| `+[ResourceManager maskUserNameInPathArray:]` (426) | | |
| `+[ResourceManager maskUserName:inPath:]` (444) | | |
| `+[ResourceManager useAddOns]` (455) | | |
| `+[ResourceManager setUseAddOns:]` (460) | | |
| `+[ResourceManager addExternalPath:]` (494) | | |
| `+[ResourceManager pathEnumerator]` (507) | | |
| `+[ResourceManager reversePathEnumerator]` (512) | | |
| `+[ResourceManager OXPsWithMessagesFound]` (517) | | |
| `+[ResourceManager manifestForIdentifier:]` (522) | | |
| `+[ResourceManager checkOXPMessagesInPath:]` (527) | | |
| `+[ResourceManager checkPotentialPath:searchPaths:]` (546) | | |
| `+[ResourceManager validateManifest:forOXP:]` (590) | | |
| `+[ResourceManager checkVersionCompatibility:forOXP:]` (647) | | |
| `+[ResourceManager areRequirementsFulfilled:forOXP:andFile:]` (659) | | |
| `+[ResourceManager manifestHasConflicts:logErrors:]` (719) | | |
| `+[ResourceManager filterSearchPathsForConflicts:]` (749) | | |
| `+[ResourceManager manifestHasMissingDependencies:logErrors:]` (769) | | |
| `+[ResourceManager manifest:HasUnmetDependency:logErrors:]` (787) | | |
| `+[ResourceManager filterSearchPathsForRequirements:]` (828) | | |
| `+[ResourceManager matchVersions:withVersion:]` (853) | | |
| `+[ResourceManager filterSearchPathsToExcludeScenarioOnlyPaths:]` (877) | | |
| `+[ResourceManager filterSearchPathsByScenario:]` (896) | | |
| `+[ResourceManager manifestAllowedByScenario:]` (916) | | |
| `+[ResourceManager manifestAllowedByScenario:withIdentifier:]` (947) | | |
| `+[ResourceManager manifestAllowedByScenario:withTag:]` (962) | | |
| `+[ResourceManager addErrorWithKey:param1:param2:]` (985) | | |
| `+[ResourceManager checkCacheUpToDateForPaths:]` (994) | | |
| `+[ResourceManager corePlist:excludedAt:]` (1061) | | |
| `+[ResourceManager dictionaryFromFilesNamed:]` (1081) | | |
| `+[ResourceManager dictionaryFromFilesNamed:]` (1088) | | |
| `+[ResourceManager arrayFromFilesNamed:inFolder:andMerge:]` (1191) | | |
| `+[ResourceManager arrayFromFilesNamed:inFolder:andMerge:cache:]` (1196) | | |
| `+[ResourceManager handleEquipmentListMerging:forLookupIndex:]` (1300) | | |
| `+[ResourceManager handleEquipmentOverrides:]` (1331) | | |
| `+[ResourceManager handleStarNebulaListMerging:]` (1414) | | |
| `+[ResourceManager whitelistDictionary]` (1466) | | |
| `LogClassKeyRoot()` (1484) | | |
| `+[ResourceManager logControlDictionary]` (1494) | | |
| `+[ResourceManager roleCategoriesDictionary]` (1558) | | |
| `+[ResourceManager mergeRoleCategories:intoDictionary:]` (1589) | | |
| `+[ResourceManager systemDescriptionManager]` (1607) | | |
| `+[ResourceManager shaderBindingTypesDictionary]` (1646) | | |
| `+[ResourceManager pathForFileNamed:inFolder:]` (1687) | | |
| `+[ResourceManager pathForFileNamed:inFolder:cache:]` (1693) | | |
| `+[ResourceManager retrieveFileNamed:]` (1743) | | |
| `+[ResourceManager ooMusicNamed:inFolder:]` (1777) | | |
| `+[ResourceManager ooSoundNamed:inFolder:]` (1787) | | |
| `+[ResourceManager stringFromFilesNamed:inFolder:]` (1797) | | |
| `+[ResourceManager stringFromFilesNamed:inFolder:cache:]` (1802) | | |
| `+[ResourceManager loadScripts]` (1831) | | |
| `+[ResourceManager writeDiagnosticData:toFileNamed:]` (1898) | | |
| `+[ResourceManager writeDiagnosticString:toFileNamed:]` (1925) | | |
| `+[ResourceManager writeDiagnosticPList:toFileNamed:]` (1930) | | |
| `+[ResourceManager materialDefaults]` (1941) | | |
| `+[ResourceManager directoryExists:create:]` (1946) | | |
| `+[ResourceManager diagnosticFileLocation]` (1969) | | |
| `+[ResourceManager logPaths]` (1974) | | |
| `+[ResourceManager clearCaches]` (1988) | | |

**Differences:**

- *pending*

