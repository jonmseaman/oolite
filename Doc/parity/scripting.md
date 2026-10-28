# Parity findings: Scripting (JS engine, OOJS*, legacy scripts, JS console)

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## OOJSConsole

<a id="oojsconsole"></a>
Upstream `src/Core/Debug/OOJSConsole.m` ↔ `Core/Debug/OOJSConsole.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSConsole()` (224) | | |
| `OOJSConsoleDestroy()` (233) | | |
| `DebugMonitorToJSConsole()` (238) | | |
| `ConsoleGetProperty()` (287) | | |
| `ConsoleSetProperty()` (382) | | |
| `DoWeDefineAllDebugFlags()` (461) | | |
| `ConsoleFinalize()` (489) | | |
| `ConsoleSettingsDeleteProperty()` (499) | | |
| `ConsoleSettingsGetProperty()` (523) | | |
| `ConsoleSettingsSetProperty()` (553) | | |
| `ConsoleConsoleMessage()` (594) | | |
| `ConsoleClearConsole()` (648) | | |
| `ConsoleScriptStack()` (667) | | |
| `ConsoleInspectEntity()` (677) | | |
| `ConsoleCallObjCMethod()` (698) | | |
| `ConsoleSetUpCallObjC()` (724) | | |
| `ConsoleIsExecutableJavaScript()` (742) | | |
| `ConsoleDisplayMessagesInClass()` (769) | | |
| `ConsoleSetDisplayMessagesInClass()` (782) | | |
| `ConsoleWriteLogMarker()` (799) | | |
| `ConsoleWriteMemoryStats()` (810) | | |
| `ConsoleWriteJSMemoryStats()` (824) | | |
| `ConsoleGarbageCollect()` (838) | | |
| `DumpCallback()` (858) | | |
| `ConsoleDumpNamedRoots()` (880) | | |
| `ConsoleDumpHeap()` (905) | | |
| `ConsoleProfile()` (926) | | |
| `ConsoleGetProfile()` (950) | | |
| `ConsoleTrace()` (974) | | |
| `PerformProfiling()` (997) | | |

**Differences:**

- *pending*


## PlayerEntityLegacyScriptEngine

<a id="playerentitylegacyscriptengine"></a>
Upstream `src/Core/Entities/PlayerEntityLegacyScriptEngine.m` ↔ `Core/Entities/PlayerEntityLegacyScriptEngine.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `CurrentScriptNameOr()` (112) | | |
| `CurrentScriptDesc()` (120) | | |
| `PerformScriptActions()` (130) | | |
| `PerformConditionalStatment()` (142) | | |
| `PerformActionStatment()` (165) | | |
| `TestScriptConditions()` (209) | | |
| `-[PlayerEntity(Scripting) setScriptTarget:]` (223) | | |
| `-[PlayerEntity(Scripting) scriptTarget]` (228) | | |
| `RecursiveRemapStatus()` (233) | | |
| `-[PlayerEntity(Scripting) worldScriptsRequiringTickle]` (247) | | |
| `-[PlayerEntity(Scripting) checkScript]` (266) | | |
| `-[PlayerEntity(Scripting) runScriptActions:withContextName:forTarget:]` (321) | | |
| `-[PlayerEntity(Scripting) runUnsanitizedScriptActions:allowingAIMethods:withContextName:forTarget:]` (349) | | |
| `-[PlayerEntity(Scripting) scriptTestConditions:]` (356) | | |
| `-[PlayerEntity(Scripting) scriptTestCondition:]` (374) | | |
| `-[PlayerEntity(Scripting) expandScriptRightHandSide:]` (542) | | |
| `-[PlayerEntity(Scripting) missionVariables]` (574) | | |
| `-[PlayerEntity(Scripting) missionVariableForKey:]` (579) | | |
| `-[PlayerEntity(Scripting) setMissionVariable:forKey:]` (587) | | |
| `-[PlayerEntity(Scripting) localVariablesForMission:]` (597) | | |
| `-[PlayerEntity(Scripting) localVariableForKey:andMission:]` (613) | | |
| `-[PlayerEntity(Scripting) setLocalVariable:forKey:andMission:]` (618) | | |
| `-[PlayerEntity(Scripting) missionsList]` (632) | | |
| `-[PlayerEntity(Scripting) replaceVariablesInString:]` (691) | | |
| `-[PlayerEntity(Scripting) setMissionDescription:]` (724) | | |
| `-[PlayerEntity(Scripting) setMissionDescription:forMission:]` (729) | | |
| `-[PlayerEntity(Scripting) setMissionInstructions:forMission:]` (742) | | |
| `-[PlayerEntity(Scripting) setMissionInstructionsList:forMission:]` (755) | | |
| `-[PlayerEntity(Scripting) clearMissionDescription]` (777) | | |
| `-[PlayerEntity(Scripting) clearMissionDescriptionForMission:]` (782) | | |
| `-[PlayerEntity(Scripting) mission_string]` (795) | | |
| `-[PlayerEntity(Scripting) status_string]` (800) | | |
| `-[PlayerEntity(Scripting) gui_screen_string]` (805) | | |
| `-[PlayerEntity(Scripting) galaxy_number]` (810) | | |
| `-[PlayerEntity(Scripting) planet_number]` (815) | | |
| `-[PlayerEntity(Scripting) score_number]` (820) | | |
| `-[PlayerEntity(Scripting) credits_number]` (825) | | |
| `-[PlayerEntity(Scripting) scriptTimer_number]` (830) | | |
| `-[PlayerEntity(Scripting) shipsFound_number]` (836) | | |
| `-[PlayerEntity(Scripting) commanderLegalStatus_number]` (841) | | |
| `-[PlayerEntity(Scripting) setLegalStatus:]` (846) | | |
| `-[PlayerEntity(Scripting) commanderLegalStatus_string]` (851) | | |
| `-[PlayerEntity(Scripting) d100_number]` (856) | | |
| `-[PlayerEntity(Scripting) pseudoFixedD100_number]` (862) | | |
| `-[PlayerEntity(Scripting) d256_number]` (867) | | |
| `-[PlayerEntity(Scripting) pseudoFixedD256_number]` (873) | | |
| `-[PlayerEntity(Scripting) clock_number]` (878) | | |
| `-[PlayerEntity(Scripting) clock_secs_number]` (883) | | |
| `-[PlayerEntity(Scripting) clock_mins_number]` (888) | | |
| `-[PlayerEntity(Scripting) clock_hours_number]` (893) | | |
| `-[PlayerEntity(Scripting) clock_days_number]` (898) | | |
| `-[PlayerEntity(Scripting) fuelLevel_number]` (903) | | |
| `-[PlayerEntity(Scripting) dockedAtMainStation_bool]` (908) | | |
| `-[PlayerEntity(Scripting) foundEquipment_bool]` (916) | | |
| `-[PlayerEntity(Scripting) sunWillGoNova_bool]` (921) | | |
| `-[PlayerEntity(Scripting) sunGoneNova_bool]` (926) | | |
| `-[PlayerEntity(Scripting) missionChoice_string]` (931) | | |
| `-[PlayerEntity(Scripting) missionKeyPress_string]` (936) | | |
| `-[PlayerEntity(Scripting) dockedTechLevel_number]` (941) | | |
| `-[PlayerEntity(Scripting) dockedStationName_string]` (950) | | |
| `-[PlayerEntity(Scripting) systemGovernment_string]` (962) | | |
| `-[PlayerEntity(Scripting) systemGovernment_number]` (972) | | |
| `-[PlayerEntity(Scripting) systemEconomy_string]` (978) | | |
| `-[PlayerEntity(Scripting) systemEconomy_number]` (988) | | |
| `-[PlayerEntity(Scripting) systemTechLevel_number]` (994) | | |
| `-[PlayerEntity(Scripting) systemPopulation_number]` (1000) | | |
| `-[PlayerEntity(Scripting) systemProductivity_number]` (1006) | | |
| `-[PlayerEntity(Scripting) commanderName_string]` (1012) | | |
| `-[PlayerEntity(Scripting) commanderRank_string]` (1017) | | |
| `-[PlayerEntity(Scripting) commanderShip_string]` (1022) | | |
| `-[PlayerEntity(Scripting) commanderShipDisplayName_string]` (1027) | | |
| `-[PlayerEntity(Scripting) expandMessage:]` (1034) | | |
| `-[PlayerEntity(Scripting) commsMessage:]` (1048) | | |
| `-[PlayerEntity(Scripting) commsMessageByUnpiloted:]` (1056) | | |
| `-[PlayerEntity(Scripting) consoleMessage3s:]` (1061) | | |
| `-[PlayerEntity(Scripting) consoleMessage6s:]` (1066) | | |
| `-[PlayerEntity(Scripting) awardCredits:]` (1071) | | |
| `-[PlayerEntity(Scripting) awardShipKills:]` (1088) | | |
| `-[PlayerEntity(Scripting) awardEquipment:]` (1098) | | |
| `-[PlayerEntity(Scripting) removeEquipment:]` (1120) | | |
| `-[PlayerEntity(Scripting) setPlanetinfo:]` (1140) | | |
| `-[PlayerEntity(Scripting) setSpecificPlanetInfo:]` (1160) | | |
| `-[PlayerEntity(Scripting) awardCargo:]` (1180) | | |
| `-[PlayerEntity(Scripting) removeAllCargo]` (1216) | | |
| `-[PlayerEntity(Scripting) removeAllCargo:]` (1221) | | |
| `-[PlayerEntity(Scripting) useSpecialCargo:]` (1259) | | |
| `-[PlayerEntity(Scripting) testForEquipment:]` (1269) | | |
| `-[PlayerEntity(Scripting) awardFuel:]` (1274) | | |
| `-[PlayerEntity(Scripting) messageShipAIs:]` (1286) | | |
| `-[PlayerEntity(Scripting) ejectItem:]` (1312) | | |
| `-[PlayerEntity(Scripting) addShips:]` (1319) | | |
| `-[PlayerEntity(Scripting) addSystemShips:]` (1345) | | |
| `-[PlayerEntity(Scripting) addShipsAt:]` (1374) | | |
| `-[PlayerEntity(Scripting) addShipsAtPrecisely:]` (1412) | | |
| `-[PlayerEntity(Scripting) addShipsWithinRadius:]` (1450) | | |
| `-[PlayerEntity(Scripting) spawnShip:]` (1480) | | |
| `-[PlayerEntity(Scripting) set:]` (1489) | | |
| `-[PlayerEntity(Scripting) reset:]` (1522) | | |
| `-[PlayerEntity(Scripting) increment:]` (1539) | | |
| `-[PlayerEntity(Scripting) decrement:]` (1560) | | |
| `-[PlayerEntity(Scripting) add:]` (1581) | | |
| `-[PlayerEntity(Scripting) subtract:]` (1614) | | |
| `-[PlayerEntity(Scripting) checkForShips:]` (1647) | | |
| `-[PlayerEntity(Scripting) resetScriptTimer]` (1652) | | |
| `-[PlayerEntity(Scripting) addMissionText:]` (1659) | | |
| `-[PlayerEntity(Scripting) addLiteralMissionText:]` (1678) | | |
| `-[PlayerEntity(Scripting) setMissionChoiceByTextEntry:]` (1690) | | |
| `-[PlayerEntity(Scripting) setMissionChoices:]` (1697) | | |
| `-[PlayerEntity(Scripting) setMissionChoicesDictionary:]` (1706) | | |
| `-[PlayerEntity(Scripting) resetMissionChoice]` (1828) | | |
| `-[PlayerEntity(Scripting) clearMissionScreen]` (1833) | | |
| `-[PlayerEntity(Scripting) addMissionDestination:]` (1843) | | |
| `-[PlayerEntity(Scripting) removeMissionDestination:]` (1858) | | |
| `-[PlayerEntity(Scripting) showShipModel:]` (1873) | | |
| `-[PlayerEntity(Scripting) setMissionMusic:]` (1884) | | |
| `-[PlayerEntity(Scripting) missionTitle]` (1892) | | |
| `-[PlayerEntity(Scripting) setMissionTitle:]` (1897) | | |
| `-[PlayerEntity(Scripting) setMissionImage:]` (1905) | | |
| `-[PlayerEntity(Scripting) setMissionBackground:]` (1914) | | |
| `-[PlayerEntity(Scripting) setFuelLeak:]` (1923) | | |
| `-[PlayerEntity(Scripting) fuelLeakRate_number]` (1938) | | |
| `-[PlayerEntity(Scripting) setSunNovaIn:]` (1943) | | |
| `-[PlayerEntity(Scripting) launchFromStation]` (1949) | | |
| `-[PlayerEntity(Scripting) blowUpStation]` (1959) | | |
| `-[PlayerEntity(Scripting) sendAllShipsAway]` (1970) | | |
| `-[PlayerEntity(Scripting) addPlanet:]` (2005) | | |
| `-[PlayerEntity(Scripting) addMoon:]` (2049) | | |
| `-[PlayerEntity(Scripting) debugOn]` (2091) | | |
| `-[PlayerEntity(Scripting) debugOff]` (2097) | | |
| `-[PlayerEntity(Scripting) debugMessage:]` (2103) | | |
| `-[PlayerEntity(Scripting) playSound:]` (2108) | | |
| `-[PlayerEntity(Scripting) doMissionCallback]` (2115) | | |
| `-[PlayerEntity(Scripting) clearMissionScreenID]` (2122) | | |
| `-[PlayerEntity(Scripting) setMissionScreenID:]` (2128) | | |
| `-[PlayerEntity(Scripting) missionScreenID]` (2133) | | |
| `-[PlayerEntity(Scripting) endMissionScreenAndNoteOpportunity]` (2138) | | |
| `-[PlayerEntity(Scripting) setGuiToMissionScreen]` (2149) | | |
| `-[PlayerEntity(Scripting) refreshMissionScreenTextEntry]` (2160) | | |
| `-[PlayerEntity(Scripting) setGuiToMissionScreenWithCallback:]` (2176) | | |
| `-[PlayerEntity(Scripting) setBackgroundFromDescriptionsKey:]` (2228) | | |
| `-[PlayerEntity(Scripting) addScene:atOffset:]` (2240) | | |
| `-[PlayerEntity(Scripting) processSceneDictionary:atOffset:]` (2259) | | |
| `-[PlayerEntity(Scripting) processSceneString:atOffset:]` (2292) | | |
| `-[PlayerEntity(Scripting) addEqScriptForKey:]` (2483) | | |
| `-[PlayerEntity(Scripting) removeEqScriptForKey:]` (2520) | | |
| `-[PlayerEntity(Scripting) eqScriptIndexForKey:]` (2545) | | |
| `-[PlayerEntity(Scripting) targetNearestHostile]` (2560) | | |
| `-[PlayerEntity(Scripting) targetNearestIncomingMissile]` (2571) | | |
| `-[PlayerEntity(Scripting) setGalacticHyperspaceBehaviourTo:]` (2582) | | |
| `-[PlayerEntity(Scripting) setGalacticHyperspaceFixedCoordsTo:]` (2592) | | |
| `OOComparisonTypeToString()` (2608) | | |

**Differences:**

- *pending*


## EntityOOJavaScriptExtensions

<a id="entityoojavascriptextensions"></a>
Upstream `src/Core/Scripting/EntityOOJavaScriptExtensions.m` ↔ `Core/Scripting/EntityOOJavaScriptExtensions.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[Entity(OOJavaScriptExtensions) isVisibleToScripts]` (39) | | |
| `-[Entity(OOJavaScriptExtensions) oo_jsClassName]` (44) | | |
| `-[Entity(OOJavaScriptExtensions) oo_jsValueInContext:]` (49) | | |
| `-[Entity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (81) | | |
| `-[Entity(OOJavaScriptExtensions) deleteJSSelf]` (87) | | |
| `-[ShipEntity(OOJavaScriptExtensions) isVisibleToScripts]` (105) | | |
| `-[ShipEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (110) | | |
| `-[ShipEntity(OOJavaScriptExtensions) oo_jsClassName]` (116) | | |
| `-[ShipEntity(OOJavaScriptExtensions) subEntitiesForScript]` (121) | | |
| `-[ShipEntity(OOJavaScriptExtensions) setTargetForScript:]` (126) | | |

**Differences:**

- *pending*


## OOConstToJSString

<a id="ooconsttojsstring"></a>
Upstream `src/Core/Scripting/OOConstToJSString.m` ↔ `Core/Scripting/OOConstToJSString.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOConstToJSStringInit()` (129) | | |
| `OOConstToJSStringDestroy()` (149) | | |
| `CompareEntries()` (156) | | |
| `InitTable()` (168) | | |
| `OOJSStringFromConstantPRIVATE()` (182) | | |
| `OOConstantFromJSStringPRIVATE()` (206) | | |
| `OOConstantFromJSValuePRIVATE()` (233) | | |

**Differences:**

- *pending*


## OOJSCall

<a id="oojscall"></a>
Upstream `src/Core/Scripting/OOJSCall.m` ↔ `Core/Scripting/OOJSCall.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOJSCallObjCObjectMethod()` (66) | | |
| `SignatureMatch()` (186) | | |
| `GetMethodType()` (194) | | |
| `-[OOJSCallMethodSignatureTemplateClass voidVoidMethod]` (214) | | |
| `-[OOJSCallMethodSignatureTemplateClass voidObjectMethod:]` (218) | | |
| `-[OOJSCallMethodSignatureTemplateClass objectObjectMethod:]` (222) | | |

**Differences:**

- *pending*


## OOJSClock

<a id="oojsclock"></a>
Upstream `src/Core/Scripting/OOJSClock.m` ↔ `Core/Scripting/OOJSClock.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSClock()` (100) | | |
| `ClockGetProperty()` (106) | | |
| `JSClockToString()` (175) | | |
| `ClockClockStringForTime()` (185) | | |
| `ClockAddSeconds()` (205) | | |

**Differences:**

- *pending*


## OOJSDock

<a id="oojsdock"></a>
Upstream `src/Core/Scripting/OOJSDock.m` ↔ `Core/Scripting/OOJSDock.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSDock()` (82) | | |
| `JSDockGetDockEntity()` (89) | | |
| `JSDockGetShipEntity()` (113) | | |
| `-[DockEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (139) | | |
| `-[DockEntity(OOJavaScriptExtensions) oo_jsClassName]` (145) | | |
| `DockGetProperty()` (152) | | |
| `DockSetProperty()` (195) | | |
| `DockIsQueued()` (246) | | |

**Differences:**

- *pending*


## OOJSEngineDebuggerHelpers

<a id="oojsenginedebuggerhelpers"></a>
Upstream `src/Core/Scripting/OOJSEngineDebuggerHelpers.m` ↔ `Core/Scripting/OOJSEngineDebuggerHelpers.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `JSValueToStrDbg()` (85) | | |
| `JSObjectToStrDbg()` (94) | | |
| `JSStringToStrDbg()` (101) | | |
| `JSValueTypeDbg()` (108) | | |
| `JSValueToStrSafeDbg()` (155) | | |
| `JSObjectToStrSafeDbg()` (187) | | |
| `JSStringToStrSafeDbg()` (194) | | |
| `JSIDToStrSafeDbg()` (201) | | |

**Differences:**

- *pending*


## OOJSEngineTimeManagement

<a id="oojsenginetimemanagement"></a>
Upstream `src/Core/Scripting/OOJSEngineTimeManagement.m` ↔ `Core/Scripting/OOJSEngineTimeManagement.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOJSPauseTimeLimiter()` (129) | | |
| `OOJSResumeTimeLimiter()` (137) | | |
| `OOJSCopyTimeLimiterNominalStartTime()` (151) | | |
| `OOJSResetTimeLimiter()` (156) | | |
| `OOJSGetTimeLimiterLimit()` (164) | | |
| `OOJSSetTimeLimiterLimit()` (169) | | |
| `-[OOJavaScriptEngine(WatchdogTimer) watchdogTimerThread]` (177) | | |
| `OperationCallback()` (204) | | |
| `ContextCallback()` (228) | | |
| `OOJSTimeManagementInit()` (240) | | |
| `OOJSBeginProfiling()` (293) | | |
| `OOJSEndProfiling()` (313) | | |
| `OOJSIsProfiling()` (354) | | |
| `CleanUpJSFrame()` (366) | | |
| `TraceEnterJSFunction()` (371) | | |
| `FunctionCallback()` (430) | | |
| `OOJSProfileEnter()` (490) | | |
| `OOJSProfileExit()` (510) | | |
| `UpdateProfileForFrame()` (553) | | |
| `-[OOTimeProfile dealloc]` (581) | | |
| `-[OOTimeProfile description]` (588) | | |
| `-[OOTimeProfile totalTime]` (624) | | |
| `-[OOTimeProfile setTotalTime:]` (629) | | |
| `-[OOTimeProfile javaScriptTime]` (634) | | |
| `-[OOTimeProfile setJavaScriptTime:]` (644) | | |
| `-[OOTimeProfile nativeTime]` (650) | | |
| `-[OOTimeProfile setNativeTime:]` (655) | | |
| `-[OOTimeProfile extensionTime]` (660) | | |
| `-[OOTimeProfile setExtensionTime:]` (665) | | |
| `-[OOTimeProfile nonExtensionTime]` (670) | | |
| `-[OOTimeProfile profilerOverhead]` (675) | | |
| `-[OOTimeProfile setProfilerOverhead:]` (680) | | |
| `-[OOTimeProfile profileEntries]` (685) | | |
| `-[OOTimeProfile setProfileEntries:]` (690) | | |
| `-[OOTimeProfile oo_jsValueInContext:]` (698) | | |
| `-[OOTimeProfile propertyListRepresentation]` (703) | | |
| `-[OOTimeProfileEntry initWithCName:]` (727) | | |
| `-[OOTimeProfileEntry initWithJSFunction:context:]` (741) | | |
| `-[OOTimeProfileEntry dealloc]` (776) | | |
| `-[OOTimeProfileEntry addSampleWithTotalTime:selfTime:]` (783) | | |
| `-[OOTimeProfileEntry description]` (792) | | |
| `-[OOTimeProfileEntry function]` (818) | | |
| `-[OOTimeProfileEntry hitCount]` (823) | | |
| `-[OOTimeProfileEntry totalTimeSum]` (828) | | |
| `-[OOTimeProfileEntry selfTimeSum]` (833) | | |
| `-[OOTimeProfileEntry totalTimeAverage]` (838) | | |
| `-[OOTimeProfileEntry selfTimeAverage]` (843) | | |
| `-[OOTimeProfileEntry totalTimeMax]` (848) | | |
| `-[OOTimeProfileEntry selfTimeMax]` (853) | | |
| `-[OOTimeProfileEntry isJavaScriptFrame]` (858) | | |
| `-[OOTimeProfileEntry compareByTotalTime:]` (867) | | |
| `-[OOTimeProfileEntry compareByTotalTimeReverse:]` (872) | | |
| `-[OOTimeProfileEntry compareBySelfTime:]` (884) | | |
| `-[OOTimeProfileEntry compareBySelfTimeReverse:]` (889) | | |
| `-[OOTimeProfileEntry oo_jsValueInContext:]` (901) | | |
| `-[OOTimeProfileEntry propertyListRepresentation]` (906) | | |

**Differences:**

- *pending*


## OOJSEntity

<a id="oojsentity"></a>
Upstream `src/Core/Scripting/OOJSEntity.m` ↔ `Core/Scripting/OOJSEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSEntity()` (127) | | |
| `JSValueToEntity()` (133) | | |
| `EntityFromArgumentList()` (142) | | |
| `EntityGetProperty()` (171) | | |
| `EntitySetProperty()` (296) | | |
| `EntityDumpState()` (376) | | |

**Differences:**

- *pending*


## OOJSEquipmentInfo

<a id="oojsequipmentinfo"></a>
Upstream `src/Core/Scripting/OOJSEquipmentInfo.m` ↔ `Core/Scripting/OOJSEquipmentInfo.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSEquipmentInfo()` (162) | | |
| `JSValueToEquipmentType()` (169) | | |
| `JSValueToEquipmentKey()` (188) | | |
| `JSValueToEquipmentKeyRelaxed()` (193) | | |
| `EquipmentInfoGetProperty()` (224) | | |
| `EquipmentInfoSetProperty()` (414) | | |
| `EquipmentInfoGetAllEqipment()` (471) | | |
| `-[OOEquipmentType(OOJavaScriptExtensions) oo_jsValueInContext:]` (483) | | |
| `-[OOEquipmentType(OOJavaScriptExtensions) oo_jsClassName]` (496) | | |
| `-[OOEquipmentType(OOJavaScriptExtensions) oo_clearJSSelf:]` (501) | | |
| `EquipmentInfoStaticInfoForKey()` (512) | | |

**Differences:**

- *pending*


## OOJSExhaustPlume

<a id="oojsexhaustplume"></a>
Upstream `src/Core/Scripting/OOJSExhaustPlume.m` ↔ `Core/Scripting/OOJSExhaustPlume.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSExhaustPlume()` (74) | | |
| `JSExhaustPlumeGetExhaustPlumeEntity()` (81) | | |
| `-[OOExhaustPlumeEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (107) | | |
| `-[OOExhaustPlumeEntity(OOJavaScriptExtensions) oo_jsClassName]` (113) | | |
| `-[OOExhaustPlumeEntity(OOJavaScriptExtensions) isVisibleToScripts]` (118) | | |
| `ExhaustPlumeGetProperty()` (125) | | |
| `ExhaustPlumeSetProperty()` (157) | | |
| `ExhaustPlumeRemove()` (201) | | |

**Differences:**

- *pending*


## OOJSFlasher

<a id="oojsflasher"></a>
Upstream `src/Core/Scripting/OOJSFlasher.m` ↔ `Core/Scripting/OOJSFlasher.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSFlasher()` (85) | | |
| `JSFlasherGetFlasherEntity()` (92) | | |
| `-[OOFlasherEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (118) | | |
| `-[OOFlasherEntity(OOJavaScriptExtensions) oo_jsClassName]` (124) | | |
| `-[OOFlasherEntity(OOJavaScriptExtensions) isVisibleToScripts]` (129) | | |
| `FlasherGetProperty()` (136) | | |
| `FlasherSetProperty()` (185) | | |
| `FlasherRemove()` (273) | | |

**Differences:**

- *pending*


## OOJSFont

<a id="oojsfont"></a>
Upstream `src/Core/Scripting/OOJSFont.m` ↔ `Core/Scripting/OOJSFont.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSFont()` (36) | | |
| `FontMeasureString()` (44) | | |

**Differences:**

- *pending*


## OOJSFrameCallbacks

<a id="oojsframecallbacks"></a>
Upstream `src/Core/Scripting/OOJSFrameCallbacks.m` ↔ `Core/Scripting/OOJSFrameCallbacks.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSFrameCallbacks()` (113) | | |
| `OOJSFrameCallbacksInvoke()` (127) | | |
| `OOJSFrameCallbacksRemoveAll()` (165) | | |
| `GlobalAddFrameCallback()` (180) | | |
| `GlobalRemoveFrameCallback()` (214) | | |
| `GlobalIsValidFrameCallback()` (242) | | |
| `AddCallback()` (265) | | |
| `GrowCallbackList()` (296) | | |
| `GetIndexForTrackingID()` (343) | | |
| `RemoveCallbackWithTrackingID()` (363) | | |
| `RemoveCallbackAtIndex()` (377) | | |
| `QueueDeferredOperation()` (398) | | |
| `RunDeferredOperations()` (411) | | |

**Differences:**

- *pending*


## OOJSFunction

<a id="oojsfunction"></a>
Upstream `src/Core/Scripting/OOJSFunction.m` ↔ `Core/Scripting/OOJSFunction.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSFunction initWithFunction:context:]` (32) | | |
| `-[OOJSFunction initWithName:]` (55) | | |
| `-[OOJSFunction deleteJSValue]` (114) | | |
| `-[OOJSFunction dealloc]` (128) | | |
| `-[OOJSFunction descriptionComponents]` (136) | | |
| `-[OOJSFunction name]` (144) | | |
| `-[OOJSFunction function]` (149) | | |
| `-[OOJSFunction functionValue]` (154) | | |
| `-[OOJSFunction evaluateWithContext:]` (163) | | |
| `-[OOJSFunction evaluateWithContext:]` (179) | | |
| `-[OOJSFunction evaluateWithContext:]` (211) | | |
| `-[OOJSFunction evaluatePredicateWithContext:]` (226) | | |

**Differences:**

- *pending*


## OOJSGlobal

<a id="oojsglobal"></a>
Upstream `src/Core/Scripting/OOJSGlobal.m` ↔ `Core/Scripting/OOJSGlobal.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `CreateOOJSGlobal()` (151) | | |
| `SetUpOOJSGlobal()` (161) | | |
| `GlobalGetProperty()` (167) | | |
| `GlobalSetProperty()` (199) | | |
| `GlobalLog()` (230) | | |
| `GlobalExpandDescription()` (269) | | |
| `GlobalKeyBindingDescription()` (295) | | |
| `GlobalExpandMissionText()` (319) | | |
| `GlobalDisplayNameForCommodity()` (345) | | |
| `GlobalRandomName()` (363) | | |
| `GlobalRandomInhabitantsDescription()` (384) | | |
| `GlobalClearExtraGuiScreenKeys()` (404) | | |
| `GlobalSetExtraGuiScreenKeys()` (436) | | |
| `GlobalSetScreenBackground()` (517) | | |
| `GlobalGetScreenBackgroundForKey()` (547) | | |
| `GlobalSetScreenBackgroundForKey()` (568) | | |
| `GlobalSetScreenOverlay()` (598) | | |
| `GlobalGetGuiColorSettingForKey()` (624) | | |
| `GlobalSetGuiColorSettingForKey()` (651) | | |
| `GlobalTakeSnapShot()` (693) | | |
| `GlobalAutoAIForRole()` (734) | | |
| `GlobalPauseGame()` (756) | | |
| `GlobalQuitGame()` (778) | | |

**Differences:**

- *pending*


## OOJSGuiScreenKeyDefinition

<a id="oojsguiscreenkeydefinition"></a>
Upstream `src/Core/Scripting/OOJSGuiScreenKeyDefinition.m` ↔ `Core/Scripting/OOJSGuiScreenKeyDefinition.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSGuiScreenKeyDefinition init]` (31) | | |
| `-[OOJSGuiScreenKeyDefinition deleteJSPointers]` (47) | | |
| `-[OOJSGuiScreenKeyDefinition dealloc]` (63) | | |
| `-[OOJSGuiScreenKeyDefinition name]` (72) | | |
| `-[OOJSGuiScreenKeyDefinition setName:]` (77) | | |
| `-[OOJSGuiScreenKeyDefinition registerKeys]` (83) | | |
| `-[OOJSGuiScreenKeyDefinition setRegisterKeys:]` (88) | | |
| `-[OOJSGuiScreenKeyDefinition callback]` (94) | | |
| `-[OOJSGuiScreenKeyDefinition setCallback:]` (99) | | |
| `-[OOJSGuiScreenKeyDefinition callbackThis]` (108) | | |
| `-[OOJSGuiScreenKeyDefinition setCallbackThis:]` (113) | | |
| `-[OOJSGuiScreenKeyDefinition runCallback:]` (122) | | |
| `-[OOJSGuiScreenKeyDefinition interfaceCompare:]` (145) | | |

**Differences:**

- *pending*


## OOJSInterfaceDefinition

<a id="oojsinterfacedefinition"></a>
Upstream `src/Core/Scripting/OOJSInterfaceDefinition.m` ↔ `Core/Scripting/OOJSInterfaceDefinition.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSInterfaceDefinition init]` (31) | | |
| `-[OOJSInterfaceDefinition deleteJSPointers]` (47) | | |
| `-[OOJSInterfaceDefinition dealloc]` (63) | | |
| `-[OOJSInterfaceDefinition title]` (72) | | |
| `-[OOJSInterfaceDefinition setTitle:]` (77) | | |
| `-[OOJSInterfaceDefinition category]` (83) | | |
| `-[OOJSInterfaceDefinition setCategory:]` (88) | | |
| `-[OOJSInterfaceDefinition summary]` (94) | | |
| `-[OOJSInterfaceDefinition setSummary:]` (99) | | |
| `-[OOJSInterfaceDefinition callback]` (105) | | |
| `-[OOJSInterfaceDefinition setCallback:]` (110) | | |
| `-[OOJSInterfaceDefinition callbackThis]` (119) | | |
| `-[OOJSInterfaceDefinition setCallbackThis:]` (124) | | |
| `-[OOJSInterfaceDefinition runCallback:]` (133) | | |
| `-[OOJSInterfaceDefinition interfaceCompare:]` (156) | | |

**Differences:**

- *pending*


## OOJSManifest

<a id="oojsmanifest"></a>
Upstream `src/Core/Scripting/OOJSManifest.m` ↔ `Core/Scripting/OOJSManifest.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOManifest dealloc]` (87) | | |
| `-[OOManifest oo_jsClassName]` (92) | | |
| `-[OOManifest oo_jsValueInContext:]` (97) | | |
| `InitOOJSManifest()` (115) | | |
| `ManifestDeleteProperty()` (129) | | |
| `ManifestGetProperty()` (135) | | |
| `ManifestSetProperty()` (172) | | |
| `ManifestComment()` (202) | | |
| `ManifestSetComment()` (224) | | |
| `ManifestShortComment()` (248) | | |
| `ManifestSetShortComment()` (270) | | |

**Differences:**

- *pending*


## OOJSMission

<a id="oojsmission"></a>
Upstream `src/Core/Scripting/OOJSMission.m` ↔ `Core/Scripting/OOJSMission.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSMission()` (100) | | |
| `MissionRunCallback()` (113) | | |
| `MissionGetProperty()` (176) | | |
| `MissionSetProperty()` (213) | | |
| `MissionMarkSystem()` (242) | | |
| `MissionUnmarkSystem()` (283) | | |
| `MissionAddMessageText()` (330) | | |
| `MissionSetInstructionsKey()` (352) | | |
| `MissionSetInstructions()` (358) | | |
| `MissionSetInstructionsInternal()` (363) | | |
| `GetParameterDictionary()` (406) | | |
| `GetParameterString()` (417) | | |
| `GetParameterImageDescriptor()` (426) | | |
| `MissionRunScreen()` (437) | | |
| `MissionRunShipLibrary()` (652) | | |

**Differences:**

- *pending*


## OOJSMissionVariables

<a id="oojsmissionvariables"></a>
Upstream `src/Core/Scripting/OOJSMissionVariables.m` ↔ `Core/Scripting/OOJSMissionVariables.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `KeyForPropertyID()` (34) | | |
| `InitOOJSMissionVariables()` (67) | | |
| `MissionVariablesConverter()` (78) | | |
| `MissionVariablesDeleteProperty()` (84) | | |
| `MissionVariablesGetProperty()` (99) | | |
| `MissionVariablesSetProperty()` (126) | | |
| `MissionVariablesEnumerate()` (150) | | |

**Differences:**

- *pending*


## OOJSOolite

<a id="oojsoolite"></a>
Upstream `src/Core/Scripting/OOJSOolite.m` ↔ `Core/Scripting/OOJSOolite.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSOolite()` (100) | | |
| `OoliteGetProperty()` (106) | | |
| `OoliteSetProperty()` (185) | | |
| `VersionString()` (262) | | |
| `VersionComponents()` (267) | | |
| `OoliteCompareVersion()` (279) | | |

**Differences:**

- *pending*


## OOJSPlanet

<a id="oojsplanet"></a>
Upstream `src/Core/Scripting/OOJSPlanet.m` ↔ `Core/Scripting/OOJSPlanet.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSPlanet()` (87) | | |
| `-[OOPlanetEntity(OOJavaScriptExtensions) isVisibleToScripts]` (96) | | |
| `-[OOPlanetEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (102) | | |
| `-[OOPlanetEntity(OOJavaScriptExtensions) oo_jsClassName]` (108) | | |
| `PlanetGetProperty()` (122) | | |
| `PlanetSetProperty()` (184) | | |

**Differences:**

- *pending*


## OOJSPlayer

<a id="oojsplayer"></a>
Upstream `src/Core/Scripting/OOJSPlayer.m` ↔ `Core/Scripting/OOJSPlayer.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSPlayer()` (151) | | |
| `JSPlayerClass()` (160) | | |
| `JSPlayerPrototype()` (165) | | |
| `JSPlayerObject()` (170) | | |
| `OOPlayerForScripting()` (175) | | |
| `PlayerGetProperty()` (183) | | |
| `PlayerSetProperty()` (288) | | |
| `PlayerCommsMessage()` (354) | | |
| `PlayerConsoleMessage()` (379) | | |
| `PlayerEndScenario()` (403) | | |
| `PlayerIncreaseContractReputation()` (422) | | |
| `PlayerDecreaseContractReputation()` (433) | | |
| `PlayerIncreaseParcelReputation()` (444) | | |
| `PlayerDecreaseParcelReputation()` (455) | | |
| `PlayerIncreasePassengerReputation()` (466) | | |
| `PlayerDecreasePassengerReputation()` (477) | | |
| `PlayerAddMessageToArrivalReport()` (488) | | |
| `PlayerAudioMessage()` (508) | | |
| `PlayerReplaceShip()` (530) | | |
| `PlayerSetEscapePodDestination()` (571) | | |
| `PlayerSetPlayerRole()` (648) | | |
| `PlayerStopAudioMessage()` (675) | | |

**Differences:**

- *pending*


## OOJSPlayerShip

<a id="oojsplayership"></a>
Upstream `src/Core/Scripting/OOJSPlayerShip.m` ↔ `Core/Scripting/OOJSPlayerShip.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSPlayerShip()` (269) | | |
| `JSPlayerShipClass()` (284) | | |
| `JSPlayerShipPrototype()` (289) | | |
| `JSPlayerShipObject()` (294) | | |
| `-[PlayerEntity(OOJavaScriptExtensions) oo_jsClassName]` (301) | | |
| `-[PlayerEntity(OOJavaScriptExtensions) setJSSelf:context:]` (306) | | |
| `-[PlayerEntity(OOJavaScriptExtensions) javaScriptEngineWillReset:]` (317) | | |
| `PlayerShipGetProperty()` (334) | | |
| `PlayerShipSetProperty()` (606) | | |
| `PlayerShipLaunch()` (1003) | | |
| `PlayerShipRemoveAllCargo()` (1017) | | |
| `PlayerShipUseSpecialCargo()` (1038) | | |
| `PlayerShipEngageAutopilotToStation()` (1062) | | |
| `PlayerShipDisengageAutopilot()` (1085) | | |
| `PlayerShipRequestDockingClearance()` (1098) | | |
| `PlayerShipCancelDockingRequest()` (1121) | | |
| `PlayerShipAwardEquipmentToCurrentPylon()` (1145) | | |
| `PlayerShipAddPassenger()` (1171) | | |
| `PlayerShipRemovePassenger()` (1206) | | |
| `PlayerShipAddParcel()` (1231) | | |
| `PlayerShipRemoveParcel()` (1264) | | |
| `PlayerShipAwardContract()` (1289) | | |
| `PlayerShipRemoveContract()` (1325) | | |
| `PlayerShipSetCustomView()` (1357) | | |
| `PlayerShipResetCustomView()` (1411) | | |
| `PlayerShipResetScannerZoom()` (1431) | | |
| `PlayerShipTakeInternalDamage()` (1444) | | |
| `PlayerShipBeginHyperspaceCountdown()` (1457) | | |
| `PlayerShipCancelHyperspaceCountdown()` (1491) | | |
| `PlayerShipBeginGalacticHyperspaceCountdown()` (1508) | | |
| `PlayerShipSetMultiFunctionDisplay()` (1542) | | |
| `PlayerShipSetMultiFunctionText()` (1570) | | |
| `PlayerShipSetPrimedEquipment()` (1608) | | |
| `PlayerShipSetCustomHUDDial()` (1634) | | |
| `PlayerShipHideHUDSelector()` (1662) | | |
| `PlayerShipShowHUDSelector()` (1683) | | |
| `ValidateContracts()` (1704) | | |

**Differences:**

- *pending*


## OOJSPopulatorDefinition

<a id="oojspopulatordefinition"></a>
Upstream `src/Core/Scripting/OOJSPopulatorDefinition.m` ↔ `Core/Scripting/OOJSPopulatorDefinition.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSPopulatorDefinition init]` (33) | | |
| `-[OOJSPopulatorDefinition deleteJSPointers]` (49) | | |
| `-[OOJSPopulatorDefinition dealloc]` (65) | | |
| `-[OOJSPopulatorDefinition callback]` (74) | | |
| `-[OOJSPopulatorDefinition setCallback:]` (79) | | |
| `-[OOJSPopulatorDefinition callbackThis]` (88) | | |
| `-[OOJSPopulatorDefinition setCallbackThis:]` (93) | | |
| `-[OOJSPopulatorDefinition runCallback:]` (102) | | |

**Differences:**

- *pending*


## OOJSQuaternion

<a id="oojsquaternion"></a>
Upstream `src/Core/Scripting/OOJSQuaternion.m` ↔ `Core/Scripting/OOJSQuaternion.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSQuaternion()` (185) | | |
| `JSQuaternionWithQuaternion()` (190) | | |
| `QuaternionToJSValue()` (218) | | |
| `JSValueToQuaternion()` (236) | | |
| `-[PlayerEntity(JSQuaternionStatistics) reportJSQuaternionStatistics]` (262) | | |
| `-[PlayerEntity(JSQuaternionStatistics) clearJSQuaternionStatistics]` (286) | | |
| `JSObjectGetQuaternion()` (306) | | |
| `GetThisQuaternion()` (384) | | |
| `JSQuaternionSetQuaternion()` (394) | | |
| `QuaternionFromArgumentListNoErrorInternal()` (420) | | |
| `QuaternionFromArgumentList()` (470) | | |
| `QuaternionFromArgumentListNoError()` (482) | | |
| `QuaternionGetProperty()` (489) | | |
| `QuaternionSetProperty()` (529) | | |
| `QuaternionFinalize()` (573) | | |
| `QuaternionConstruct()` (584) | | |
| `QuaternionToString()` (626) | | |
| `QuaternionToSource()` (641) | | |
| `QuaternionMultiply()` (657) | | |
| `QuaternionDot()` (676) | | |
| `QuaternionRotate()` (696) | | |
| `QuaternionRotateX()` (725) | | |
| `QuaternionRotateY()` (745) | | |
| `QuaternionRotateZ()` (765) | | |
| `QuaternionNormalize()` (785) | | |
| `QuaternionConjugate()` (802) | | |
| `QuaternionVectorForward()` (819) | | |
| `QuaternionVectorUp()` (837) | | |
| `QuaternionVectorRight()` (855) | | |
| `QuaternionToArray()` (873) | | |
| `QuaternionStaticRandom()` (918) | | |

**Differences:**

- *pending*


## OOJSScript

<a id="oojsscript"></a>
Upstream `src/Core/Scripting/OOJSScript.m` ↔ `Core/Scripting/OOJSScript.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOJSScript scriptWithPath:properties:]` (104) | | |
| `-[OOJSScript initWithPath:properties:]` (109) | | |
| `-[OOJSScript dealloc]` (256) | | |
| `-[OOJSScript oo_jsClassName]` (281) | | |
| `-[OOJSScript descriptionComponents]` (286) | | |
| `-[OOJSScript javaScriptEngineWillReset:]` (294) | | |
| `+[OOJSScript currentlyRunningScript]` (305) | | |
| `+[OOJSScript scriptStack]` (312) | | |
| `-[OOJSScript weakRetain]` (321) | | |
| `-[OOJSScript weakRefDied:]` (328) | | |
| `-[OOJSScript name]` (334) | | |
| `-[OOJSScript scriptDescription]` (343) | | |
| `-[OOJSScript version]` (348) | | |
| `-[OOJSScript runWithTarget:]` (353) | | |
| `-[OOJSScript callMethod:]` (357) | | |
| `-[OOJSScript propertyWithID:inContext:]` (417) | | |
| `-[OOJSScript setProperty:withID:inContext:]` (430) | | |
| `-[OOJSScript defineProperty:withID:inContext:]` (440) | | |
| `-[OOJSScript propertyNamed:]` (450) | | |
| `-[OOJSScript setProperty:named:]` (464) | | |
| `-[OOJSScript defineProperty:named:]` (478) | | |
| `-[OOJSScript oo_jsValueInContext:]` (492) | | |
| `+[OOJSScript pushScript:]` (499) | | |
| `+[OOJSScript popScript:]` (512) | | |
| `-[OOJSScript(OOPrivate) scriptNameFromPath:]` (541) | | |
| `-[OOJSScript(OOPrivate) defaultPropertiesFromPath:]` (573) | | |
| `-[OOScript(JavaScriptEvents) callMethod:]` (603) | | |
| `InitOOJSScript()` (614) | | |
| `ScriptAddProperty()` (620) | | |
| `AddStackToArrayReversed()` (635) | | |
| `LoadScriptWithName()` (643) | | |
| `CompiledScriptData()` (707) | | |
| `ScriptWithCompiledData()` (728) | | |
| `StrippedName()` (754) | | |

**Differences:**

- *pending*


## OOJSShip

<a id="oojsship"></a>
Upstream `src/Core/Scripting/OOJSShip.m` ↔ `Core/Scripting/OOJSShip.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSShip()` (575) | | |
| `JSShipClass()` (582) | | |
| `JSShipPrototype()` (587) | | |
| `ShipGetProperty()` (592) | | |
| `ShipSetProperty()` (1177) | | |
| `ShipSetScript()` (1832) | | |
| `ShipSetAI()` (1858) | | |
| `ShipSwitchAI()` (1884) | | |
| `ShipExitAI()` (1910) | | |
| `ShipReactToAIMessage()` (1941) | | |
| `ShipSendAIMessage()` (1967) | | |
| `ShipDeployEscorts()` (1993) | | |
| `ShipDockEscorts()` (2008) | | |
| `ShipHasEquipmentProviding()` (2023) | | |
| `ShipHasRole()` (2045) | | |
| `ShipEjectItem()` (2067) | | |
| `ShipAddCargoEntity()` (2089) | | |
| `ShipEjectSpecificItem()` (2135) | | |
| `ShipDumpCargo()` (2157) | | |
| `ShipSpawn()` (2197) | | |
| `ShipDealEnergyDamage()` (2228) | | |
| `ShipExplode()` (2272) | | |
| `ShipRemove()` (2282) | | |
| `ShipRunLegacyScriptActions()` (2312) | | |
| `ShipCommsMessage()` (2346) | | |
| `ShipFireECM()` (2375) | | |
| `ShipAbandonShip()` (2395) | | |
| `ShipCanAwardEquipment()` (2408) | | |
| `ShipAwardEquipment()` (2454) | | |
| `ShipRemoveEquipment()` (2523) | | |
| `ShipRestoreSubEntities()` (2574) | | |
| `ShipSetEquipmentStatus()` (2603) | | |
| `ShipEquipmentStatus()` (2683) | | |
| `ShipSelectNewMissile()` (2742) | | |
| `ShipFireMissile()` (2761) | | |
| `ShipFindNearestStation()` (2781) | | |
| `ShipSetBounty()` (2808) | | |
| `ShipSetCargo()` (2836) | | |
| `ShipSetCrew()` (2866) | | |
| `ShipSetCargoType()` (2902) | | |
| `ShipSetMaterials()` (2941) | | |
| `ShipSetShaders()` (2960) | | |
| `ShipSetMaterialsInternal()` (2985) | | |
| `ShipExitSystem()` (3049) | | |
| `ShipUpdateEscortFormation()` (3077) | | |
| `RemoveOrExplodeShip()` (3090) | | |
| `ShipClearDefenseTargets()` (3133) | | |
| `ShipAddDefenseTarget()` (3146) | | |
| `ShipRemoveDefenseTarget()` (3166) | | |
| `ShipAddCollisionException()` (3186) | | |
| `ShipRemoveCollisionException()` (3210) | | |
| `ShipGetMaterials()` (3233) | | |
| `ShipGetShaders()` (3251) | | |
| `ShipBroadcastCascadeImminent()` (3268) | | |
| `ShipBecomeCascadeExplosion()` (3281) | | |
| `ShipOfferToEscort()` (3294) | | |
| `ShipRequestHelpFromGroup()` (3314) | | |
| `ShipPatrolReportIn()` (3329) | | |
| `ShipMarkTargetForFines()` (3351) | | |
| `ShipEnterWormhole()` (3370) | | |
| `ShipNotifyGroupOfWormhole()` (3399) | | |
| `ShipThrowSpark()` (3413) | | |
| `ShipPerformAttack()` (3426) | | |
| `ShipPerformCollect()` (3439) | | |
| `ShipPerformEscort()` (3452) | | |
| `ShipPerformFaceDestination()` (3465) | | |
| `ShipPerformFlee()` (3478) | | |
| `ShipPerformFlyToRangeFromDestination()` (3491) | | |
| `ShipPerformHold()` (3504) | | |
| `ShipPerformIdle()` (3517) | | |
| `ShipPerformIntercept()` (3530) | | |
| `ShipPerformLandOnPlanet()` (3543) | | |
| `ShipPerformMining()` (3556) | | |
| `ShipPerformScriptedAI()` (3569) | | |
| `ShipPerformScriptedAttackAI()` (3582) | | |
| `ShipPerformStop()` (3595) | | |
| `ShipPerformTumble()` (3608) | | |
| `ShipRequestDockingInstructions()` (3621) | | |
| `ShipRecallDockingInstructions()` (3639) | | |
| `ShipBroadcastDistressMessage()` (3657) | | |
| `ShipCheckCourseToDestination()` (3670) | | |
| `ShipGetSafeCourseToDestination()` (3684) | | |
| `ShipCheckScanner()` (3698) | | |
| `ShipAdjustCargo()` (3728) | | |
| `ShipDamageAssessment()` (3770) | | |
| `ShipThreatAssessment()` (3797) | | |
| `ShipThreatAssessmentWeapon()` (3918) | | |
| `ShipStaticKeys()` (3928) | | |
| `ShipStaticKeysForRole()` (3939) | | |
| `ShipStaticRoleIsInCategory()` (3956) | | |
| `ShipStaticRoles()` (3973) | | |
| `ShipStaticShipDataForKey()` (3984) | | |
| `ShipStaticSetShipDataForKey()` (4000) | | |

**Differences:**

- *pending*


## OOJSShipGroup

<a id="oojsshipgroup"></a>
Upstream `src/Core/Scripting/OOJSShipGroup.m` ↔ `Core/Scripting/OOJSShipGroup.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSShipGroup()` (84) | | |
| `ShipGroupGetProperty()` (90) | | |
| `ShipGroupSetProperty()` (134) | | |
| `ShipGroupConstruct()` (173) | | |
| `-[OOShipGroup(OOJavaScriptExtensions) oo_jsValueInContext:]` (208) | | |
| `-[OOShipGroup(OOJavaScriptExtensions) oo_clearJSSelf:]` (226) | | |
| `ShipGroupAddShip()` (237) | | |
| `ShipGroupRemoveShip()` (298) | | |
| `ShipGroupContainsShip()` (324) | | |

**Differences:**

- *pending*


## OOJSSound

<a id="oojssound"></a>
Upstream `src/Core/Scripting/OOJSSound.m` ↔ `Core/Scripting/OOJSSound.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSSound()` (108) | | |
| `SoundFromJSValue()` (114) | | |
| `SoundGetProperty()` (131) | | |
| `GetNamedSound()` (156) | | |
| `SoundStaticLoad()` (172) | | |
| `SoundStaticMusicSoundSource()` (195) | | |
| `SoundStaticPlayMusic()` (207) | | |
| `SoundStaticStopMusic()` (245) | | |
| `-[OOSound(OOJavaScriptExtentions) oo_jsValueInContext:]` (273) | | |
| `-[OOSound(OOJavaScriptExtentions) oo_jsDescription]` (289) | | |
| `-[OOSound(OOJavaScriptExtentions) oo_jsClassName]` (294) | | |

**Differences:**

- *pending*


## OOJSSoundSource

<a id="oojssoundsource"></a>
Upstream `src/Core/Scripting/OOJSSoundSource.m` ↔ `Core/Scripting/OOJSSoundSource.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSSoundSource()` (110) | | |
| `SoundSourceConstruct()` (116) | | |
| `SoundSourceGetProperty()` (132) | | |
| `SoundSourceSetProperty()` (179) | | |
| `SoundSourcePlay()` (255) | | |
| `SoundSourceStop()` (285) | | |
| `SoundSourcePlayOrRepeat()` (304) | | |
| `-[OOSoundSource(OOJavaScriptExtentions) oo_jsValueInContext:]` (324) | | |
| `-[OOSoundSource(OOJavaScriptExtentions) oo_jsClassName]` (340) | | |

**Differences:**

- *pending*


## OOJSSpecialFunctions

<a id="oojsspecialfunctions"></a>
Upstream `src/Core/Scripting/OOJSSpecialFunctions.m` ↔ `Core/Scripting/OOJSSpecialFunctions.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSSpecialFunctions()` (43) | | |
| `JSSpecialFunctionsObjectWrapper()` (47) | | |
| `SpecialJSWarning()` (70) | | |
| `SpecialMarkConsoleEntryPoint()` (90) | | |

**Differences:**

- *pending*


## OOJSStation

<a id="oojsstation"></a>
Upstream `src/Core/Scripting/OOJSStation.m` ↔ `Core/Scripting/OOJSStation.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSStation()` (151) | | |
| `JSStationGetStationEntity()` (158) | | |
| `JSStationGetShipEntity()` (182) | | |
| `-[StationEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (208) | | |
| `-[StationEntity(OOJavaScriptExtensions) oo_jsClassName]` (214) | | |
| `StationGetProperty()` (221) | | |
| `StationSetProperty()` (330) | | |
| `StationAbortAllDockings()` (434) | | |
| `StationAbortDockingForShip()` (447) | | |
| `StationCanDockShip()` (472) | | |
| `StationDockPlayer()` (503) | | |
| `StationIncreaseAlertLevel()` (531) | | |
| `StationDecreaseAlertLevel()` (548) | | |
| `StationLaunchShipWithRole()` (566) | | |
| `StationLaunchDefenseShip()` (598) | | |
| `StationLaunchEscort()` (615) | | |
| `StationLaunchScavenger()` (632) | | |
| `StationLaunchMiner()` (649) | | |
| `StationLaunchPirateShip()` (665) | | |
| `StationLaunchShuttle()` (682) | | |
| `StationLaunchPatrol()` (698) | | |
| `StationLaunchPolice()` (714) | | |
| `StationSetInterface()` (730) | | |
| `StationSetMarketPrice()` (827) | | |
| `StationSetMarketQuantity()` (864) | | |
| `StationAddShipToShipyard()` (901) | | |
| `StationRemoveShipFromShipyard()` (1048) | | |

**Differences:**

- *pending*


## OOJSSun

<a id="oojssun"></a>
Upstream `src/Core/Scripting/OOJSSun.m` ↔ `Core/Scripting/OOJSSun.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSSun()` (79) | | |
| `-[OOSunEntity(OOJavaScriptExtensions) isVisibleToScripts]` (88) | | |
| `-[OOSunEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (93) | | |
| `-[OOSunEntity(OOJavaScriptExtensions) oo_jsClassName]` (99) | | |
| `SunGetProperty()` (106) | | |
| `SunGoNova()` (145) | | |
| `SunCancelNova()` (164) | | |

**Differences:**

- *pending*


## OOJSSystem

<a id="oojssystem"></a>
Upstream `src/Core/Scripting/OOJSSystem.m` ↔ `Core/Scripting/OOJSSystem.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSSystem()` (214) | | |
| `SystemGetProperty()` (222) | | |
| `SystemSetProperty()` (439) | | |
| `SystemToString()` (571) | | |
| `SystemAddPlanet()` (585) | | |
| `SystemAddMoon()` (610) | | |
| `SystemSendAllShipsAway()` (635) | | |
| `SystemCountShipsWithPrimaryRole()` (648) | | |
| `SystemCountShipsWithRole()` (680) | | |
| `SystemShipsWithPrimaryRole()` (712) | | |
| `SystemShipsWithRole()` (745) | | |
| `SystemCountEntitiesWithScanClass()` (778) | | |
| `SystemEntitiesWithScanClass()` (810) | | |
| `SystemFilteredEntities()` (843) | | |
| `SystemLocationFromCode()` (881) | | |
| `SystemAddShips()` (908) | | |
| `SystemAddGroup()` (914) | | |
| `SystemAddShipsToRoute()` (920) | | |
| `SystemAddGroupToRoute()` (926) | | |
| `SystemLegacyAddShips()` (932) | | |
| `SystemLegacyAddSystemShips()` (958) | | |
| `SystemLegacyAddShipsAt()` (985) | | |
| `SystemLegacyAddShipsAtPrecisely()` (1016) | | |
| `SystemLegacyAddShipsWithinRadius()` (1047) | | |
| `SystemLegacySpawnShip()` (1081) | | |
| `SystemStaticSystemNameForID()` (1108) | | |
| `SystemStaticSystemIDForName()` (1129) | | |
| `SystemStaticInfoForSystem()` (1155) | | |
| `SystemAddVisualEffect()` (1182) | | |
| `SystemSetPopulator()` (1216) | | |
| `SystemSetWaypoint()` (1273) | | |
| `SystemAddShipsOrGroup()` (1324) | | |
| `SystemAddShipsOrGroupToRoute()` (1383) | | |
| `GetRelativeToAndRange()` (1445) | | |
| `FindJSVisibleEntities()` (1477) | | |
| `FindShips()` (1502) | | |
| `CompareEntitiesByDistance()` (1515) | | |

**Differences:**

- *pending*


## OOJSSystemInfo

<a id="oojssysteminfo"></a>
Upstream `src/Core/Scripting/OOJSSystemInfo.m` ↔ `Core/Scripting/OOJSSystemInfo.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSystemInfo init]` (128) | | |
| `-[OOSystemInfo initWithGalaxy:system:]` (134) | | |
| `-[OOSystemInfo dealloc]` (149) | | |
| `-[OOSystemInfo descriptionComponents]` (156) | | |
| `-[OOSystemInfo shortDescriptionComponents]` (161) | | |
| `-[OOSystemInfo oo_jsClassName]` (166) | | |
| `-[OOSystemInfo isEqual:]` (171) | | |
| `-[OOSystemInfo hash]` (176) | | |
| `-[OOSystemInfo valueForKey:]` (184) | | |
| `-[OOSystemInfo setValue:forKey:]` (192) | | |
| `-[OOSystemInfo allKeys]` (199) | | |
| `-[OOSystemInfo galaxy]` (207) | | |
| `-[OOSystemInfo system]` (212) | | |
| `-[OOSystemInfo coordinates]` (223) | | |
| `-[OOSystemInfo oo_jsValueInContext:]` (231) | | |
| `InitOOJSSystemInfo()` (249) | | |
| `GetJSSystemInfoForSystem()` (255) | | |
| `SystemInfoFinalize()` (287) | | |
| `SystemInfoEnumerate()` (301) | | |
| `SystemInfoDeleteProperty()` (350) | | |
| `SystemInfoGetProperty()` (360) | | |
| `SystemInfoSetProperty()` (435) | | |
| `SystemInfoDistanceToSystem()` (456) | | |
| `SystemInfoRouteToSystem()` (486) | | |
| `SystemInfoSamplePrice()` (523) | | |
| `SystemInfoSetPropertyMethod()` (550) | | |
| `SystemInfoStaticFilteredSystems()` (597) | | |
| `SystemInfoStaticSetInterstellarProperty()` (662) | | |

**Differences:**

- *pending*


## OOJSTimer

<a id="oojstimer"></a>
Upstream `src/Core/Scripting/OOJSTimer.m` ↔ `Core/Scripting/OOJSTimer.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSTimer initWithDelay:]` (48) | | |
| `-[OOJSTimer deleteJSPointers]` (85) | | |
| `-[OOJSTimer dealloc]` (104) | | |
| `-[OOJSTimer descriptionComponents]` (113) | | |
| `-[OOJSTimer oo_jsClassName]` (133) | | |
| `-[OOJSTimer timerFired]` (138) | | |
| `-[OOJSTimer oo_jsValueInContext:]` (171) | | |
| `InitOOJSTimer()` (227) | | |
| `TimerGetProperty()` (233) | | |
| `TimerSetProperty()` (264) | | |
| `TimerFinalize()` (305) | | |
| `TimerConstruct()` (324) | | |
| `TimerStart()` (391) | | |
| `TimerStop()` (406) | | |

**Differences:**

- *pending*


## OOJSVector

<a id="oojsvector"></a>
Upstream `src/Core/Scripting/OOJSVector.m` ↔ `Core/Scripting/OOJSVector.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSVector()` (231) | | |
| `JSVectorWithVector()` (236) | | |
| `VectorToJSValue()` (264) | | |
| `JSVectorWithHPVector()` (282) | | |
| `HPVectorToJSValue()` (310) | | |
| `NSPointToVectorJSValue()` (328) | | |
| `JSValueToHPVector()` (333) | | |
| `JSValueToVector()` (341) | | |
| `-[PlayerEntity(JSVectorStatistics) reportJSVectorStatistics]` (369) | | |
| `-[PlayerEntity(JSVectorStatistics) clearJSVectorStatistics]` (395) | | |
| `JSObjectGetVector()` (415) | | |
| `GetThisVector()` (483) | | |
| `JSVectorSetVector()` (493) | | |
| `JSVectorSetHPVector()` (498) | | |
| `VectorFromArgumentListNoErrorInternal()` (525) | | |
| `VectorFromArgumentList()` (573) | | |
| `VectorFromArgumentListNoError()` (585) | | |
| `VectorGetProperty()` (592) | | |
| `VectorSetProperty()` (628) | | |
| `VectorFinalize()` (668) | | |
| `VectorConstruct()` (683) | | |
| `VectorToString()` (725) | | |
| `VectorToSource()` (740) | | |
| `VectorAdd()` (756) | | |
| `VectorSubtract()` (775) | | |
| `VectorDistanceTo()` (794) | | |
| `VectorSquaredDistanceTo()` (814) | | |
| `VectorMultiply()` (834) | | |
| `VectorDot()` (854) | | |
| `VectorAngleTo()` (874) | | |
| `VectorCross()` (901) | | |
| `VectorTripleProduct()` (920) | | |
| `VectorDirection()` (946) | | |
| `VectorMagnitude()` (963) | | |
| `VectorSquaredMagnitude()` (981) | | |
| `VectorRotationTo()` (999) | | |
| `VectorRotateBy()` (1036) | | |
| `VectorToArray()` (1056) | | |
| `VectorToCoordinateSystem()` (1085) | | |
| `VectorFromCoordinateSystem()` (1111) | | |
| `VectorStaticInterpolate()` (1140) | | |
| `VectorStaticRandom()` (1183) | | |
| `VectorStaticRandomDirection()` (1198) | | |
| `VectorStaticRandomDirectionAndLength()` (1213) | | |

**Differences:**

- *pending*


## OOJSVisualEffect

<a id="oojsvisualeffect"></a>
Upstream `src/Core/Scripting/OOJSVisualEffect.m` ↔ `Core/Scripting/OOJSVisualEffect.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSVisualEffect()` (132) | | |
| `JSVisualEffectGetVisualEffectEntity()` (139) | | |
| `-[OOVisualEffectEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (165) | | |
| `-[OOVisualEffectEntity(OOJavaScriptExtensions) oo_jsClassName]` (171) | | |
| `-[OOVisualEffectEntity(OOJavaScriptExtensions) isVisibleToScripts]` (176) | | |
| `-[OOVisualEffectEntity(OOJavaScriptExtensions) subEntitiesForScript]` (181) | | |
| `VisualEffectGetProperty()` (188) | | |
| `VisualEffectSetProperty()` (297) | | |
| `VisualEffectRemove()` (467) | | |
| `VisualEffectGetMaterials()` (487) | | |
| `VisualEffectGetShaders()` (505) | | |
| `VisualEffectSetMaterials()` (523) | | |
| `VisualEffectSetShaders()` (542) | | |
| `VisualEffectSetMaterialsInternal()` (569) | | |
| `VisualEffectScale()` (632) | | |
| `VisualEffectRestoreSubEntities()` (662) | | |

**Differences:**

- *pending*


## OOJSWaypoint

<a id="oojswaypoint"></a>
Upstream `src/Core/Scripting/OOJSWaypoint.m` ↔ `Core/Scripting/OOJSWaypoint.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSWaypoint()` (77) | | |
| `JSWaypointGetWaypointEntity()` (84) | | |
| `-[OOWaypointEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (110) | | |
| `-[OOWaypointEntity(OOJavaScriptExtensions) oo_jsClassName]` (116) | | |
| `-[OOWaypointEntity(OOJavaScriptExtensions) isVisibleToScripts]` (121) | | |
| `WaypointGetProperty()` (128) | | |
| `WaypointSetProperty()` (176) | | |

**Differences:**

- *pending*


## OOJSWorldScripts

<a id="oojsworldscripts"></a>
Upstream `src/Core/Scripting/OOJSWorldScripts.m` ↔ `Core/Scripting/OOJSWorldScripts.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSWorldScripts()` (48) | | |
| `WorldScriptsGetProperty()` (53) | | |
| `WorldScriptsEnumerate()` (84) | | |

**Differences:**

- *pending*


## OOJSWormhole

<a id="oojswormhole"></a>
Upstream `src/Core/Scripting/OOJSWormhole.m` ↔ `Core/Scripting/OOJSWormhole.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSWormhole()` (77) | | |
| `JSWormholeGetWormholeEntity()` (84) | | |
| `-[WormholeEntity(OOJavaScriptExtensions) getJSClass:andPrototype:]` (110) | | |
| `-[WormholeEntity(OOJavaScriptExtensions) oo_jsClassName]` (116) | | |
| `-[WormholeEntity(OOJavaScriptExtensions) isVisibleToScripts]` (121) | | |
| `WormholeGetProperty()` (128) | | |
| `WormholeSetProperty()` (169) | | |

**Differences:**

- *pending*


## OOJavaScriptEngine

<a id="oojavascriptengine"></a>
Upstream `src/Core/Scripting/OOJavaScriptEngine.m` ↔ `Core/Scripting/OOJavaScriptEngine.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `ReportJSError()` (135) | | |
| `+[OOJavaScriptEngine sharedEngine]` (237) | | |
| `-[OOJavaScriptEngine runMissionCallback]` (245) | | |
| `-[OOJavaScriptEngine init]` (250) | | |
| `-[OOJavaScriptEngine createMainThreadContext]` (291) | | |
| `-[OOJavaScriptEngine destroyMainThreadContext]` (378) | | |
| `-[OOJavaScriptEngine reset]` (403) | | |
| `-[OOJavaScriptEngine dealloc]` (450) | | |
| `-[OOJavaScriptEngine globalObject]` (462) | | |
| `-[OOJavaScriptEngine callJSFunction:]` (467) | | |
| `-[OOJavaScriptEngine removeGCObjectRoot:]` (490) | | |
| `-[OOJavaScriptEngine removeGCValueRoot:]` (497) | | |
| `-[OOJavaScriptEngine garbageCollectionOpportunity:]` (504) | | |
| `-[OOJavaScriptEngine showErrorLocations]` (515) | | |
| `-[OOJavaScriptEngine setShowErrorLocations:]` (520) | | |
| `-[OOJavaScriptEngine objectClass]` (525) | | |
| `-[OOJavaScriptEngine stringClass]` (530) | | |
| `-[OOJavaScriptEngine arrayClass]` (535) | | |
| `-[OOJavaScriptEngine numberClass]` (540) | | |
| `-[OOJavaScriptEngine booleanClass]` (545) | | |
| `-[OOJavaScriptEngine lookUpStandardClassPointers]` (550) | | |
| `-[OOJavaScriptEngine registerStandardObjectConverters]` (579) | | |
| `DebuggerHook()` (589) | | |
| `-[OOJavaScriptEngine dumpStackForErrors]` (601) | | |
| `-[OOJavaScriptEngine setDumpStackForErrors:]` (606) | | |
| `-[OOJavaScriptEngine dumpStackForWarnings]` (611) | | |
| `-[OOJavaScriptEngine setDumpStackForWarnings:]` (616) | | |
| `-[OOJavaScriptEngine enableDebuggerStatement]` (621) | | |
| `-[OOJavaScriptEngine(OOMonitorSupport) setMonitor:]` (633) | | |
| `-[OOJavaScriptEngine(OOMonitorSupportInternal) sendMonitorError:]` (643) | | |
| `-[OOJavaScriptEngine(OOMonitorSupportInternal) sendMonitorLogMessage:]` (652) | | |
| `DumpVariable()` (667) | | |
| `OOJSDumpStack()` (694) | | |
| `GetLocationNameAndLine()` (804) | | |
| `OOJSDescribeLocation()` (823) | | |
| `OOJSMarkConsoleEvalLocation()` (851) | | |
| `OOJSInitJSIDCachePRIVATE()` (857) | | |
| `OOJSIDFromString()` (873) | | |
| `OOStringFromJSID()` (906) | | |
| `CallerPrefix()` (921) | | |
| `OOJSReportError()` (930) | | |
| `OOJSReportErrorForCaller()` (939) | | |
| `OOJSReportErrorWithArguments()` (957) | | |
| `OOJSReportWrappedException()` (973) | | |
| `OOJSUnreachable()` (986) | | |
| `OOJSReportWarning()` (994) | | |
| `OOJSReportWarningForCaller()` (1003) | | |
| `OOJSReportWarningWithArguments()` (1021) | | |
| `OOJSReportBadPropertySelector()` (1035) | | |
| `OOJSReportBadPropertyValue()` (1043) | | |
| `OOJSReportBadArguments()` (1052) | | |
| `OOJSSetWarningOrErrorStackSkip()` (1068) | | |
| `OOJSArgumentListGetNumber()` (1073) | | |
| `OOJSArgumentListGetNumberNoError()` (1084) | | |
| `JSArrayFromNSArray()` (1108) | | |
| `JSNewNSArrayValue()` (1147) | | |
| `JSObjectFromNSDictionary()` (1179) | | |
| `JSNewNSDictionaryValue()` (1249) | | |
| `-[NSObject(OOJavaScriptConversion) oo_jsValueInContext:]` (1279) | | |
| `-[NSObject(OOJavaScriptConversion) oo_jsClassName]` (1284) | | |
| `-[NSObject(OOJavaScriptConversion) oo_jsDescription]` (1289) | | |
| `-[NSObject(OOJavaScriptConversion) oo_jsDescriptionWithClassName:]` (1294) | | |
| `-[NSObject(OOJavaScriptConversion) oo_clearJSSelf:]` (1316) | | |
| `OOJSObjectFromNativeObject()` (1322) | | |
| `+[OOJSValue valueWithJSValue:inContext:]` (1333) | | |
| `+[OOJSValue valueWithJSObject:inContext:]` (1342) | | |
| `-[OOJSValue initWithJSValue:inContext:]` (1351) | | |
| `-[OOJSValue initWithJSObject:inContext:]` (1381) | | |
| `-[OOJSValue deleteJSValue]` (1386) | | |
| `-[OOJSValue dealloc]` (1400) | | |
| `-[OOJSValue oo_jsValueInContext:]` (1406) | | |
| `OOJSStrLiteralCachePRIVATE()` (1413) | | |
| `OOStringFromJSString()` (1430) | | |
| `OOStringFromJSValueEvenIfNull()` (1449) | | |
| `OOStringFromJSValue()` (1461) | | |
| `OOStringFromJSPropertyIDAndSpec()` (1473) | | |
| `DescribeValue()` (1493) | | |
| `OOJSDescribeValue()` (1578) | | |
| `+[NSString(OOJavaScriptExtensions) stringWithJavaScriptParameters:count:inContext:]` (1585) | | |
| `-[NSString(OOJavaScriptExtensions) oo_jsValueInContext:]` (1607) | | |
| `+[NSString(OOJavaScriptExtensions) concatenationOfStringsFromJavaScriptValues:count:separator:inContext:]` (1634) | | |
| `-[NSString(OOJavaScriptExtensions) escapedForJavaScriptLiteral]` (1663) | | |
| `-[NSString(OOJavaScriptExtensions) oo_jsClassName]` (1726) | | |
| `-[NSArray(OOJavaScriptConversion) oo_jsValueInContext:]` (1735) | | |
| `-[OONativeVector(OOJavaScriptConversion) oo_jsValueInContext:]` (1746) | | |
| `-[NSDictionary(OOJavaScriptConversion) oo_jsValueInContext:]` (1757) | | |
| `-[NSNumber(OOJavaScriptConversion) oo_jsValueInContext:]` (1768) | | |
| `-[NSNumber(OOJavaScriptConversion) oo_jsClassName]` (1797) | | |
| `-[NSNull(OOJavaScriptConversion) oo_jsValueInContext:]` (1806) | | |
| `OOJSUnconstructableConstruct()` (1813) | | |
| `OOJSObjectWrapperFinalize()` (1826) | | |
| `OOJSObjectWrapperToString()` (1840) | | |
| `JSFunctionPredicate()` (1868) | | |
| `JSEntityIsJavaScriptVisiblePredicate()` (1908) | | |
| `JSEntityIsJavaScriptSearchablePredicate()` (1917) | | |
| `JSEntityIsDemoShipPredicate()` (1949) | | |
| `OOJSRegisterSubclass()` (1956) | | |
| `UnregisterSubclasses()` (1969) | | |
| `OOJSIsSubclass()` (1975) | | |
| `OOJSDictionaryFromJSValue()` (2039) | | |
| `OOJSDictionaryFromJSObject()` (2052) | | |
| `OOJSDictionaryFromStringTable()` (2103) | | |
| `OOJSNativeObjectFromJSValue()` (2155) | | |
| `OOJSNativeObjectFromJSObject()` (2182) | | |
| `OOJSNativeObjectOfClassFromJSValue()` (2207) | | |
| `OOJSNativeObjectOfClassFromJSObject()` (2215) | | |
| `OOJSBasicPrivateObjectConverter()` (2223) | | |
| `OOJSRegisterObjectConverter()` (2235) | | |
| `UnregisterObjectConverters()` (2254) | | |
| `JSArrayConverter()` (2259) | | |
| `JSStringConverter()` (2296) | | |
| `JSNumberConverter()` (2301) | | |
| `JSBooleanConverter()` (2310) | | |

**Differences:**

- *pending*


## OOLegacyScriptWhitelist

<a id="oolegacyscriptwhitelist"></a>
Upstream `src/Core/Scripting/OOLegacyScriptWhitelist.m` ↔ `Core/Scripting/OOLegacyScriptWhitelist.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOSanitizeLegacyScript()` (58) | | |
| `OOSanitizeLegacyScriptInternal()` (65) | | |
| `OOSanitizeLegacyScriptConditions()` (101) | | |
| `OOSanitizeLegacyScriptConditionsInternal()` (110) | | |
| `OOLegacyConditionsAreSanitized()` (149) | | |
| `SanitizeCondition()` (156) | | |
| `SanitizeConditionalStatement()` (290) | | |
| `SanitizeActionStatement()` (336) | | |
| `ClassifyLHSConditionSelector()` (375) | | |
| `SanitizeQueryMethod()` (406) | | |
| `SanitizeActionMethod()` (427) | | |
| `AlwaysFalseConditions()` (476) | | |
| `IsAlwaysFalseConditions()` (487) | | |
| `StringFromStackInternal()` (492) | | |
| `StringFromStack()` (512) | | |

**Differences:**

- *pending*


## OOPListScript

<a id="ooplistscript"></a>
Upstream `src/Core/Scripting/OOPListScript.m` ↔ `Core/Scripting/OOPListScript.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOPListScript scriptsInPListFile:]` (50) | | |
| `-[OOPListScript dealloc]` (63) | | |
| `-[OOPListScript name]` (71) | | |
| `-[OOPListScript scriptDescription]` (76) | | |
| `-[OOPListScript version]` (81) | | |
| `-[OOPListScript requiresTickle]` (86) | | |
| `-[OOPListScript runWithTarget:]` (91) | | |
| `+[OOPListScript(SetUp) scriptsFromDictionaryOfScripts:filePath:]` (112) | | |
| `+[OOPListScript(SetUp) loadCachedScripts:]` (152) | | |
| `-[OOPListScript(SetUp) initWithName:scriptArray:metadata:]` (173) | | |

**Differences:**

- *pending*


## OOScript

<a id="ooscript"></a>
Upstream `src/Core/Scripting/OOScript.m` ↔ `Core/Scripting/OOScript.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOScript worldScriptsAtPath:]` (43) | | |
| `+[OOScript scriptsFromFileNamed:]` (117) | | |
| `+[OOScript scriptsFromList:]` (132) | | |
| `+[OOScript scriptsFromFileAtPath:]` (149) | | |
| `+[OOScript jsScriptFromFileNamed:properties:]` (175) | | |
| `+[OOScript jsAIScriptFromFileNamed:properties:]` (200) | | |
| `-[OOScript descriptionComponents]` (225) | | |
| `-[OOScript name]` (230) | | |
| `-[OOScript scriptDescription]` (236) | | |
| `-[OOScript version]` (242) | | |
| `-[OOScript displayName]` (248) | | |
| `-[OOScript requiresTickle]` (261) | | |
| `-[OOScript runWithTarget:]` (266) | | |

**Differences:**

- *pending*


## OOScriptTimer

<a id="ooscripttimer"></a>
Upstream `src/Core/Scripting/OOScriptTimer.m` ↔ `Core/Scripting/OOScriptTimer.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOScriptTimer initWithNextTime:]` (39) | | |
| `-[OOScriptTimer initOneShotTimerWithDelay:]` (66) | | |
| `-[OOScriptTimer dealloc]` (71) | | |
| `-[OOScriptTimer descriptionComponents]` (79) | | |
| `-[OOScriptTimer nextTime]` (91) | | |
| `-[OOScriptTimer setNextTime:]` (96) | | |
| `-[OOScriptTimer interval]` (105) | | |
| `-[OOScriptTimer setInterval:]` (110) | | |
| `-[OOScriptTimer timerFired]` (117) | | |
| `-[OOScriptTimer scheduleTimer]` (122) | | |
| `-[OOScriptTimer unscheduleTimer]` (143) | | |
| `-[OOScriptTimer isScheduled]` (150) | | |
| `+[OOScriptTimer updateTimers]` (155) | | |
| `+[OOScriptTimer noteGameReset]` (189) | | |
| `-[OOScriptTimer isValidForScheduling]` (201) | | |
| `-[OOScriptTimer compareByNextFireTime:]` (224) | | |

**Differences:**

- *pending*

