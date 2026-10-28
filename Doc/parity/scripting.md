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

**Status:** done — 9 rows: 9 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOConstToJSStringInit()` (129) | `OOConstToJSStringInit` OOConstToJSString.cpp:133 | ✅ |
| `OOConstToJSStringDestroy()` (149) | `OOConstToJSStringDestroy` OOConstToJSString.cpp:155 | ✅ |
| `CompareEntries()` (156) | `CompareEntries` OOConstToJSString.cpp:164 | ✅ |
| `InitTable()` (168) | `InitTable` OOConstToJSString.cpp:176 | ✅ |
| `OOJSStringFromConstantPRIVATE()` (182) | `OOJSStringFromConstantPRIVATE` OOConstToJSString.cpp:190 | ✅ |
| `OOConstantFromJSStringPRIVATE()` (206) | `OOConstantFromJSStringPRIVATE` OOConstToJSString.cpp:214 | ✅ |
| `OOConstantFromJSValuePRIVATE()` (233) | `OOConstantFromJSValuePRIVATE` OOConstToJSString.cpp:241 | ✅ |
| *(added)* nine `s…TableEntries[]` tables from `Core/Tables/*.tbl` + `ENTRY` macros (71–107) | same tables OOConstToJSString.cpp:79–113; all nine `.tbl` files byte-identical (diffed) | ✅ |
| *(added)* header inlines `OOJSStringFrom…/…FromJSString/…FromJSValue` + default values (`OOConstToJSString.h`) | `OOConstToJSString.h`: bodies and per-type defaults identical after signature renaming (diffed) | ✅ |

**Differences:**

- ✅ **low** — The binary search has the same latent bug on both sides. When `value` is below the smallest entry, `max = mid - 1` underflows `NSUInteger` and the loop continues, so this was kept, not introduced — upstream `OOConstToJSString.m:189-201` ↔ port `OOConstToJSString.cpp:197-209`
- ✅ **low** — `sInited` exists only under `!NDEBUG` in the port. It is only read by asserts, so there is no release-build effect — upstream `OOConstToJSString.m:60` ↔ port `OOConstToJSString.cpp:62-64`

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

**Status:** done — 17 rows: 15 ✅ 1 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| *(added)* `sEquipmentInfoProperties[]` (83): 38 props, `displayColor`/`effectiveTechLevel` RW_CB, rest RO_CB | `sEquipmentInfoProperties` OOJSEquipmentInfo.cpp:187: same 38 names, IDs and order; RW = Permanent\|Enumerate\|Shared, RO adds ReadOnly | ✅ |
| *(added)* `sEquipmentInfoStaticProperties[]` (126): `allEquipment` RO_CB | OOJSEquipmentInfo.cpp:232 | ✅ |
| *(added)* `sEquipmentInfoMethods[]` (131): `toString`/0 | OOJSEquipmentInfo.cpp:243 (`EquipmentInfoToString` → `OOJSCxxObjectWrapperToString`) | ✅ |
| *(added)* `sEquipmentInfoStaticMethods[]` (137): `infoForKey`/0 | OOJSEquipmentInfo.cpp:251 | ✅ |
| *(added)* `sEquipmentInfoClass` (143) | `sEquipmentInfoClass` OOJSEquipmentInfo.cpp:259 | ✅ |
| `InitOOJSEquipmentInfo()` (162) | `InitOOJSEquipmentInfo` OOJSEquipmentInfo.cpp:297 (converter `EquipmentInfoConverter` :288 gives the type as an Object node) | ✅ |
| `JSValueToEquipmentType()` (169) | `JSValueToEquipmentType` OOJSEquipmentInfo.cpp:305 | ✅ |
| `JSValueToEquipmentKey()` (188) | `JSValueToEquipmentKey` OOJSEquipmentInfo.cpp:324 | ✅ |
| `JSValueToEquipmentKeyRelaxed()` (193) | `JSValueToEquipmentKeyRelaxed` OOJSEquipmentInfo.cpp:329 | ✅ |
| `EquipmentInfoGetProperty()` (224), all cases except the following two rows | `EquipmentInfoGetProperty` OOJSEquipmentInfo.cpp:362 | ✅ |
| `EquipmentInfoGetProperty()`, `provides` case (301) | OOJSEquipmentInfo.cpp:445 | ❌ |
| `EquipmentInfoGetProperty()`, `requiresEquipment`/`requiresAnyEquipment`/`incompatibleEquipment` cases (373–383) | OOJSEquipmentInfo.cpp:518–528 via `KeyArrayOrNull` :113 | ⚠️ |
| `EquipmentInfoSetProperty()` (414) | `EquipmentInfoSetProperty` OOJSEquipmentInfo.cpp:558 | ✅ |
| `EquipmentInfoGetAllEqipment()` (471) | `EquipmentInfoGetAllEqipment` OOJSEquipmentInfo.cpp:621 | ✅ |
| `-[OOEquipmentType(OOJavaScriptExtensions) oo_jsValueInContext:]` (483) | `OOEquipmentType::jsValueInContext` OOJSEquipmentInfo.cpp:636 | ✅ |
| `-[OOEquipmentType(OOJavaScriptExtensions) oo_jsClassName]` (496) | `OOJSEquipmentInfoJSClassName` OOJSEquipmentInfo.cpp:666 (+ `jsDescription` :657, same format as the inherited `oo_jsDescriptionWithClassName:`) | ✅ |
| `-[OOEquipmentType(OOJavaScriptExtensions) oo_clearJSSelf:]` (501) | `OOEquipmentType::clearJSSelf` OOJSEquipmentInfo.cpp:649 | ✅ |
| `EquipmentInfoStaticInfoForKey()` (512) | `EquipmentInfoStaticInfoForKey` OOJSEquipmentInfo.cpp:675 | ✅ |

**Differences:**

- ❌ **med** — `EquipmentInfo.provides` returns `[]` instead of `null` for equipment with no extra-info dictionary (`equipment.plist` entries with only the required fields). Upstream sets `_provides` only inside `if (extra != nil)`, so `[nil copy]` gives JS `null`. The port's `providesForScripting()` returns a `std::vector`, so the result is always an array. Scripts that test `eq.provides` for truthiness or `=== null` take the other branch — upstream `OOJSEquipmentInfo.m:301-303`, `OOEquipmentType.m:214-240,629-632` ↔ port `OOJSEquipmentInfo.cpp:445-447`, `OOEquipmentType.cpp:263-268,603-606`
- ⚠️ **low** — `requiresEquipment`, `requiresAnyEquipment` and `incompatibleEquipment` come back as sorted key arrays instead of `NSSet -allObjects` hash order. The membership is the same and only the array order changes (port comment at :111-112) — upstream `OOJSEquipmentInfo.m:373-383` ↔ port `OOJSEquipmentInfo.cpp:113-118,518-528`
- ✅ note — `effectiveTechLevel` setter: the deprecation text, the `(null)` for a nil identifier, the 0..15 clamp that keeps 99, the `%u` mission-variable format and the warning text all match. `calculatedPrice` keeps the same EQ_FUEL formula, `(70 - fuel) * price * fuelChargeRate`, and the same `EQ_RENOVATION` branch. `installationTime` keeps its `price + 600` fallback. `scriptInfo`, `weaponInfo` and `scriptName` keep the `{}`/`""` fallbacks for nil.

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

**Status:** done — 2 rows: 2 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSFont()` (36) | `InitOOJSFont` OOJSFont.cpp:69 (`defaultFont` READONLY, `measureString`/1 METHOD_READONLY) | ✅ |
| `FontMeasureString()` (44) | `FontMeasureString` OOJSFont.cpp:78 | ✅ |

**Differences:**

- ✅ note — `measureString(null)` measures `""` in the port. Upstream measures a nil string. Both report the same bad-argument text, with an `undefined` argv, when the argument is undefined or missing.

## OOJSFrameCallbacks

<a id="oojsframecallbacks"></a>
Upstream `src/Core/Scripting/OOJSFrameCallbacks.m` ↔ `Core/Scripting/OOJSFrameCallbacks.cpp`

**Status:** done — 13 rows: 13 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitOOJSFrameCallbacks()` (113) | `InitOOJSFrameCallbacks` OOJSFrameCallbacks.cpp:141 | ✅ |
| `OOJSFrameCallbacksInvoke()` (127) | `OOJSFrameCallbacksInvoke` OOJSFrameCallbacks.cpp:155 | ✅ |
| `OOJSFrameCallbacksRemoveAll()` (165) | `OOJSFrameCallbacksRemoveAll` OOJSFrameCallbacks.cpp:194 | ✅ |
| `GlobalAddFrameCallback()` (180) | `GlobalAddFrameCallback` OOJSFrameCallbacks.cpp:209 | ✅ |
| `GlobalRemoveFrameCallback()` (214) | `GlobalRemoveFrameCallback` OOJSFrameCallbacks.cpp:243 | ✅ |
| `GlobalIsValidFrameCallback()` (242) | `GlobalIsValidFrameCallback` OOJSFrameCallbacks.cpp:271 | ✅ |
| `AddCallback()` (265) | `AddCallback` OOJSFrameCallbacks.cpp:295 | ✅ |
| `GrowCallbackList()` (296) | `GrowCallbackList` OOJSFrameCallbacks.cpp:328 | ✅ |
| `GetIndexForTrackingID()` (343) | `GetIndexForTrackingID` OOJSFrameCallbacks.cpp:377 | ✅ |
| `RemoveCallbackWithTrackingID()` (363) | `RemoveCallbackWithTrackingID` OOJSFrameCallbacks.cpp:399 | ✅ |
| `RemoveCallbackAtIndex()` (377) | `RemoveCallbackAtIndex` OOJSFrameCallbacks.cpp:414 | ✅ |
| `QueueDeferredOperation()` (398) | `QueueDeferredOperation` OOJSFrameCallbacks.cpp:436 (a `std::vector<DeferredOperation>` instead of an array of dicts) | ✅ |
| `RunDeferredOperations()` (411) | `RunDeferredOperations` OOJSFrameCallbacks.cpp:446 | ✅ |

**Differences:**

- ✅ note (ordering) — The ID scramble constants are unchanged (`0x2315EB16`, `992699`), and the ID is still seeded from `timeIntervalSinceReferenceDate`. Callbacks run in slot order `0..sCount-1`. Removal swaps the last entry into the removed slot, so ordering after a removal is the same as upstream — upstream `:147-151,386-388` ↔ port `:175-179,423-425`
- ✅ note (mutation during iteration) — While running, add and remove are queued in call order and replayed after the loop. A deferred remove of an unknown ID is still silent (no warning), and a failed deferred add logs `script.frameCallback.deferredAdd.failed` with the same text. Upstream round-trips the ID through `NSNumber numberWithInt` and `oo_intForKey`; the port stores the `uint32_t` directly, which is lossless both ways — upstream `:398-432` ↔ port `:436-465`
- ✅ note — `addFrameCallback` returns `INT_TO_JSVAL`/`int32Value` of a `uint32` on both sides, so IDs ≥ 2³¹ come back negative in both. Argument-error texts are identical ("function", "frame callback tracking ID"), and so is `"removeFrameCallback(): invalid tracking ID."`.

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

**Status:** done — 13 rows: 13 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSGuiScreenKeyDefinition init]` (31) | constructor OOJSGuiScreenKeyDefinition.cpp:64 | ✅ |
| `-[OOJSGuiScreenKeyDefinition deleteJSPointers]` (47) | `deleteJSPointers` OOJSGuiScreenKeyDefinition.cpp:76 | ✅ |
| `-[OOJSGuiScreenKeyDefinition dealloc]` (63) | destructor OOJSGuiScreenKeyDefinition.cpp:91 | ✅ |
| `-[OOJSGuiScreenKeyDefinition name]` (72) | `name` OOJSGuiScreenKeyDefinition.cpp:98 | ✅ |
| `-[OOJSGuiScreenKeyDefinition setName:]` (77) | `setName` OOJSGuiScreenKeyDefinition.cpp:103 | ✅ |
| `-[OOJSGuiScreenKeyDefinition registerKeys]` (83) | `registerKeys` OOJSGuiScreenKeyDefinition.cpp:108 | ✅ |
| `-[OOJSGuiScreenKeyDefinition setRegisterKeys:]` (88) | `setRegisterKeys` OOJSGuiScreenKeyDefinition.cpp:113 (value copy = `-copy`) | ✅ |
| `-[OOJSGuiScreenKeyDefinition callback]` (94) | `callback` OOJSGuiScreenKeyDefinition.cpp:118 | ✅ |
| `-[OOJSGuiScreenKeyDefinition setCallback:]` (99) | `setCallback` OOJSGuiScreenKeyDefinition.cpp:123 | ✅ |
| `-[OOJSGuiScreenKeyDefinition callbackThis]` (108) | `callbackThis` OOJSGuiScreenKeyDefinition.cpp:132 | ✅ |
| `-[OOJSGuiScreenKeyDefinition setCallbackThis:]` (113) | `setCallbackThis` OOJSGuiScreenKeyDefinition.cpp:137 | ✅ |
| `-[OOJSGuiScreenKeyDefinition runCallback:]` (122) | `runCallback` OOJSGuiScreenKeyDefinition.cpp:146 | ✅ |
| `-[OOJSGuiScreenKeyDefinition interfaceCompare:]` (145) | `interfaceCompare` OOJSGuiScreenKeyDefinition.cpp:164 | ✅ |

**Differences:**

- ✅ note — `interfaceCompare` with a nil `_name` returns `OOOrderedSame`, which models a message to nil. A nil other name compares as `""`. The case-folding rules are those of `oo::str::caseInsensitiveCompare` (audited with foundation) — upstream `:145-148` ↔ port `:164-172`

## OOJSInterfaceDefinition

<a id="oojsinterfacedefinition"></a>
Upstream `src/Core/Scripting/OOJSInterfaceDefinition.m` ↔ `Core/Scripting/OOJSInterfaceDefinition.cpp`

**Status:** done — 15 rows: 15 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSInterfaceDefinition init]` (31) | constructor OOJSInterfaceDefinition.cpp:60 | ✅ |
| `-[OOJSInterfaceDefinition deleteJSPointers]` (47) | `deleteJSPointers` OOJSInterfaceDefinition.cpp:72 | ✅ |
| `-[OOJSInterfaceDefinition dealloc]` (63) | destructor OOJSInterfaceDefinition.cpp:87 | ✅ |
| `-[OOJSInterfaceDefinition title]` (72) | `title` OOJSInterfaceDefinition.cpp:94 | ✅ |
| `-[OOJSInterfaceDefinition setTitle:]` (77) | `setTitle` OOJSInterfaceDefinition.cpp:99 | ✅ |
| `-[OOJSInterfaceDefinition category]` (83) | `category` OOJSInterfaceDefinition.cpp:104 | ✅ |
| `-[OOJSInterfaceDefinition setCategory:]` (88) | `setCategory` OOJSInterfaceDefinition.cpp:109 (non-optional; the only caller, `Station.setInterface`, never passes nil) | ✅ |
| `-[OOJSInterfaceDefinition summary]` (94) | `summary` OOJSInterfaceDefinition.cpp:114 | ✅ |
| `-[OOJSInterfaceDefinition setSummary:]` (99) | `setSummary` OOJSInterfaceDefinition.cpp:119 (same as above) | ✅ |
| `-[OOJSInterfaceDefinition callback]` (105) | `callback` OOJSInterfaceDefinition.cpp:124 | ✅ |
| `-[OOJSInterfaceDefinition setCallback:]` (110) | `setCallback` OOJSInterfaceDefinition.cpp:129 | ✅ |
| `-[OOJSInterfaceDefinition callbackThis]` (119) | `callbackThis` OOJSInterfaceDefinition.cpp:138 | ✅ |
| `-[OOJSInterfaceDefinition setCallbackThis:]` (124) | `setCallbackThis` OOJSInterfaceDefinition.cpp:143 | ✅ |
| `-[OOJSInterfaceDefinition runCallback:]` (133) | `runCallback` OOJSInterfaceDefinition.cpp:152 | ✅ |
| `-[OOJSInterfaceDefinition interfaceCompare:]` (156) | `interfaceCompare` OOJSInterfaceDefinition.cpp:170 + `CaseInsensitiveCompare` :46 | ✅ |

**Differences:**

- ✅ note — The nil branches of `CaseInsensitiveCompare` cannot be reached. `Station.setInterface` always supplies a non-empty title, and a non-empty category (defaulting to `interfaces-category-uncategorised`) — upstream `OOJSStation.m:763-808` ↔ port `OOJSStation.cpp:1022-1069`

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

**Status:** done — 8 rows: 8 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJSPopulatorDefinition init]` (33) | constructor OOJSPopulatorDefinition.cpp:61 | ✅ |
| `-[OOJSPopulatorDefinition deleteJSPointers]` (49) | `deleteJSPointers` OOJSPopulatorDefinition.cpp:73 | ✅ |
| `-[OOJSPopulatorDefinition dealloc]` (65) | destructor OOJSPopulatorDefinition.cpp:88 | ✅ |
| `-[OOJSPopulatorDefinition callback]` (74) | `callback` OOJSPopulatorDefinition.cpp:95 | ✅ |
| `-[OOJSPopulatorDefinition setCallback:]` (79) | `setCallback` OOJSPopulatorDefinition.cpp:100 | ✅ |
| `-[OOJSPopulatorDefinition callbackThis]` (88) | `callbackThis` OOJSPopulatorDefinition.cpp:109 | ✅ |
| `-[OOJSPopulatorDefinition setCallbackThis:]` (93) | `setCallbackThis` OOJSPopulatorDefinition.cpp:114 | ✅ |
| `-[OOJSPopulatorDefinition runCallback:]` (102) | `runPopulatorCallback` OOJSPopulatorDefinition.cpp:123 (renamed; same body: `HPVectorToVector` → `VectorToJSValue`, push/pop owner) | ✅ |

**Differences:**

- ✅ note — The port adds `OOJSPopulatorDefinitionToPList`/`…In` (:157, :164) so that a definition can travel inside a `PList` where upstream put the object itself in an `NSDictionary`. These add no behaviour of their own.

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

**Status:** done — 100 rows: 95 ✅ 2 ⚠️ 3 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `sShipClass` JSClass (153) | `sShipClass` ooscript::ClassDef `Core/Scripting/OOJSShip.cpp:204` | ✅ |
| `enum kShip_*` property IDs from -128 (177) | `enum kShip_*` `Core/Scripting/OOJSShip.cpp:231` | ✅ |
| `sShipProperties[]` JSPropertySpec (325) | `sShipProperties[]` `Core/Scripting/OOJSShip.cpp:379` | ⚠️ |
| `sShipMethods[]` JSFunctionSpec (476) | `sShipMethods[]` `Core/Scripting/OOJSShip.cpp:530` | ✅ |
| `sShipStaticMethods[]` (562) | `sShipStaticMethods[]` `Core/Scripting/OOJSShip.cpp:616` | ✅ |
| `JSShipGetShipEntity` (DEFINE_JS_OBJECT_GETTER, 573) | `JSShipGetShipEntity()` `Core/Scripting/OOJSShip.cpp:634` | ✅ |
| `GET_THIS_SHIP` macro (1823) | `GET_THIS_SHIP` `Core/Scripting/OOJSShip.cpp:2034` | ✅ |
| `InitOOJSShip()` (575) | `InitOOJSShip()` `Core/Scripting/OOJSShip.cpp:645` | ✅ |
| `JSShipClass()` (582) | `JSShipClass()` `Core/Scripting/OOJSShip.cpp:652` | ✅ |
| `JSShipPrototype()` (587) | `JSShipPrototype()` `Core/Scripting/OOJSShip.cpp:657` | ✅ |
| `ShipGetProperty()` (592) | `ShipGetProperty()` `Core/Scripting/OOJSShip.cpp:733` | ✅ |
| `ShipSetProperty()` (1177) | `ShipSetProperty()` `Core/Scripting/OOJSShip.cpp:1337` | ⚠️ |
| `ShipSetScript()` (1832) | `ShipSetScript()` `Core/Scripting/OOJSShip.cpp:2043` | ✅ |
| `ShipSetAI()` (1858) | `ShipSetAI()` `Core/Scripting/OOJSShip.cpp:2070` | ✅ |
| `ShipSwitchAI()` (1884) | `ShipSwitchAI()` `Core/Scripting/OOJSShip.cpp:2097` | ✅ |
| `ShipExitAI()` (1910) | `ShipExitAI()` `Core/Scripting/OOJSShip.cpp:2124` | ✅ |
| `ShipReactToAIMessage()` (1941) | `ShipReactToAIMessage()` `Core/Scripting/OOJSShip.cpp:2157` | ✅ |
| `ShipSendAIMessage()` (1967) | `ShipSendAIMessage()` `Core/Scripting/OOJSShip.cpp:2184` | ✅ |
| `ShipDeployEscorts()` (1993) | `ShipDeployEscorts()` `Core/Scripting/OOJSShip.cpp:2211` | ✅ |
| `ShipDockEscorts()` (2008) | `ShipDockEscorts()` `Core/Scripting/OOJSShip.cpp:2227` | ✅ |
| `ShipHasEquipmentProviding()` (2023) | `ShipHasEquipmentProviding()` `Core/Scripting/OOJSShip.cpp:2243` | ✅ |
| `ShipHasRole()` (2045) | `ShipHasRole()` `Core/Scripting/OOJSShip.cpp:2266` | ✅ |
| `ShipEjectItem()` (2067) | `ShipEjectItem()` `Core/Scripting/OOJSShip.cpp:2289` | ✅ |
| `ShipAddCargoEntity()` (2089) | `ShipAddCargoEntity()` `Core/Scripting/OOJSShip.cpp:2312` | ✅ |
| `ShipEjectSpecificItem()` (2135) | `ShipEjectSpecificItem()` `Core/Scripting/OOJSShip.cpp:2360` | ✅ |
| `ShipDumpCargo()` (2157) | `ShipDumpCargo()` `Core/Scripting/OOJSShip.cpp:2383` | ✅ |
| `ShipSpawn()` (2197) | `ShipSpawn()` `Core/Scripting/OOJSShip.cpp:2425` | ✅ |
| `ShipDealEnergyDamage()` (2228) | `ShipDealEnergyDamage()` `Core/Scripting/OOJSShip.cpp:2457` | ✅ |
| `ShipExplode()` (2272) | `ShipExplode()` `Core/Scripting/OOJSShip.cpp:2502` | ✅ |
| `ShipRemove()` (2282) | `ShipRemove()` `Core/Scripting/OOJSShip.cpp:2512` | ✅ |
| `ShipRunLegacyScriptActions()` (2312) | `ShipRunLegacyScriptActions()` `Core/Scripting/OOJSShip.cpp:2543` | ✅ |
| `ShipCommsMessage()` (2346) | `ShipCommsMessage()` `Core/Scripting/OOJSShip.cpp:2577` | ✅ |
| `ShipFireECM()` (2375) | `ShipFireECM()` `Core/Scripting/OOJSShip.cpp:2607` | ✅ |
| `ShipAbandonShip()` (2395) | `ShipAbandonShip()` `Core/Scripting/OOJSShip.cpp:2630` | ✅ |
| `ShipCanAwardEquipment()` (2408) | `ShipCanAwardEquipment()` `Core/Scripting/OOJSShip.cpp:2644` | ✅ |
| `ShipAwardEquipment()` (2454) | `ShipAwardEquipment()` `Core/Scripting/OOJSShip.cpp:2691` | ✅ |
| `ShipRemoveEquipment()` (2523) | `ShipRemoveEquipment()` `Core/Scripting/OOJSShip.cpp:2762` | ✅ |
| `ShipRestoreSubEntities()` (2574) | `ShipRestoreSubEntities()` `Core/Scripting/OOJSShip.cpp:2814` | ✅ |
| `ShipSetEquipmentStatus()` (2603) | `ShipSetEquipmentStatus()` `Core/Scripting/OOJSShip.cpp:2845` | ✅ |
| `ShipEquipmentStatus()` (2683) | `ShipEquipmentStatus()` `Core/Scripting/OOJSShip.cpp:2927` | ✅ |
| `ShipSelectNewMissile()` (2742) | `ShipSelectNewMissile()` `Core/Scripting/OOJSShip.cpp:2987` | ✅ |
| `ShipFireMissile()` (2761) | `ShipFireMissile()` `Core/Scripting/OOJSShip.cpp:3006` | ✅ |
| `ShipFindNearestStation()` (2781) | `ShipFindNearestStation()` `Core/Scripting/OOJSShip.cpp:3027` | ✅ |
| `ShipSetBounty()` (2808) | `ShipSetBounty()` `Core/Scripting/OOJSShip.cpp:3056` | ✅ |
| `ShipSetCargo()` (2836) | `ShipSetCargo()` `Core/Scripting/OOJSShip.cpp:3085` | ✅ |
| `ShipSetCrew()` (2866) | `ShipSetCrew()` `Core/Scripting/OOJSShip.cpp:3117` | ❌ |
| `ShipSetCargoType()` (2902) | `ShipSetCargoType()` `Core/Scripting/OOJSShip.cpp:3155` | ✅ |
| `ShipSetMaterials()` (2941) | `ShipSetMaterials()` `Core/Scripting/OOJSShip.cpp:3195` | ✅ |
| `ShipSetShaders()` (2960) | `ShipSetShaders()` `Core/Scripting/OOJSShip.cpp:3214` | ✅ |
| `ShipSetMaterialsInternal()` (2985) | `ShipSetMaterialsInternal()` `Core/Scripting/OOJSShip.cpp:3240` | ✅ |
| `ShipExitSystem()` (3049) | `ShipExitSystem()` `Core/Scripting/OOJSShip.cpp:3309` | ✅ |
| `ShipUpdateEscortFormation()` (3077) | `ShipUpdateEscortFormation()` `Core/Scripting/OOJSShip.cpp:3338` | ✅ |
| `RemoveOrExplodeShip()` (3090) | `RemoveOrExplodeShip()` `Core/Scripting/OOJSShip.cpp:3353` | ✅ |
| `ShipClearDefenseTargets()` (3133) | `ShipClearDefenseTargets()` `Core/Scripting/OOJSShip.cpp:3400` | ✅ |
| `ShipAddDefenseTarget()` (3146) | `ShipAddDefenseTarget()` `Core/Scripting/OOJSShip.cpp:3414` | ✅ |
| `ShipRemoveDefenseTarget()` (3166) | `ShipRemoveDefenseTarget()` `Core/Scripting/OOJSShip.cpp:3435` | ✅ |
| `ShipAddCollisionException()` (3186) | `ShipAddCollisionException()` `Core/Scripting/OOJSShip.cpp:3456` | ✅ |
| `ShipRemoveCollisionException()` (3210) | `ShipRemoveCollisionException()` `Core/Scripting/OOJSShip.cpp:3482` | ✅ |
| `ShipGetMaterials()` (3233) | `ShipGetMaterials()` `Core/Scripting/OOJSShip.cpp:3507` | ✅ |
| `ShipGetShaders()` (3251) | `ShipGetShaders()` `Core/Scripting/OOJSShip.cpp:3525` | ✅ |
| `ShipBroadcastCascadeImminent()` (3268) | `ShipBroadcastCascadeImminent()` `Core/Scripting/OOJSShip.cpp:3542` | ✅ |
| `ShipBecomeCascadeExplosion()` (3281) | `ShipBecomeCascadeExplosion()` `Core/Scripting/OOJSShip.cpp:3556` | ✅ |
| `ShipOfferToEscort()` (3294) | `ShipOfferToEscort()` `Core/Scripting/OOJSShip.cpp:3570` | ✅ |
| `ShipRequestHelpFromGroup()` (3314) | `ShipRequestHelpFromGroup()` `Core/Scripting/OOJSShip.cpp:3591` | ✅ |
| `ShipPatrolReportIn()` (3329) | `ShipPatrolReportIn()` `Core/Scripting/OOJSShip.cpp:3607` | ✅ |
| `ShipMarkTargetForFines()` (3351) | `ShipMarkTargetForFines()` `Core/Scripting/OOJSShip.cpp:3630` | ✅ |
| `ShipEnterWormhole()` (3370) | `ShipEnterWormhole()` `Core/Scripting/OOJSShip.cpp:3649` | ✅ |
| `ShipNotifyGroupOfWormhole()` (3399) | `ShipNotifyGroupOfWormhole()` `Core/Scripting/OOJSShip.cpp:3680` | ✅ |
| `ShipThrowSpark()` (3413) | `ShipThrowSpark()` `Core/Scripting/OOJSShip.cpp:3695` | ✅ |
| `ShipPerformAttack()` (3426) | `ShipPerformAttack()` `Core/Scripting/OOJSShip.cpp:3709` | ✅ |
| `ShipPerformCollect()` (3439) | `ShipPerformCollect()` `Core/Scripting/OOJSShip.cpp:3723` | ✅ |
| `ShipPerformEscort()` (3452) | `ShipPerformEscort()` `Core/Scripting/OOJSShip.cpp:3737` | ✅ |
| `ShipPerformFaceDestination()` (3465) | `ShipPerformFaceDestination()` `Core/Scripting/OOJSShip.cpp:3751` | ✅ |
| `ShipPerformFlee()` (3478) | `ShipPerformFlee()` `Core/Scripting/OOJSShip.cpp:3765` | ✅ |
| `ShipPerformFlyToRangeFromDestination()` (3491) | `ShipPerformFlyToRangeFromDestination()` `Core/Scripting/OOJSShip.cpp:3779` | ✅ |
| `ShipPerformHold()` (3504) | `ShipPerformHold()` `Core/Scripting/OOJSShip.cpp:3793` | ✅ |
| `ShipPerformIdle()` (3517) | `ShipPerformIdle()` `Core/Scripting/OOJSShip.cpp:3807` | ✅ |
| `ShipPerformIntercept()` (3530) | `ShipPerformIntercept()` `Core/Scripting/OOJSShip.cpp:3821` | ✅ |
| `ShipPerformLandOnPlanet()` (3543) | `ShipPerformLandOnPlanet()` `Core/Scripting/OOJSShip.cpp:3835` | ✅ |
| `ShipPerformMining()` (3556) | `ShipPerformMining()` `Core/Scripting/OOJSShip.cpp:3849` | ✅ |
| `ShipPerformScriptedAI()` (3569) | `ShipPerformScriptedAI()` `Core/Scripting/OOJSShip.cpp:3863` | ✅ |
| `ShipPerformScriptedAttackAI()` (3582) | `ShipPerformScriptedAttackAI()` `Core/Scripting/OOJSShip.cpp:3877` | ✅ |
| `ShipPerformStop()` (3595) | `ShipPerformStop()` `Core/Scripting/OOJSShip.cpp:3891` | ✅ |
| `ShipPerformTumble()` (3608) | `ShipPerformTumble()` `Core/Scripting/OOJSShip.cpp:3905` | ✅ |
| `ShipRequestDockingInstructions()` (3621) | `ShipRequestDockingInstructions()` `Core/Scripting/OOJSShip.cpp:3919` | ✅ |
| `ShipRecallDockingInstructions()` (3639) | `ShipRecallDockingInstructions()` `Core/Scripting/OOJSShip.cpp:3933` | ✅ |
| `ShipBroadcastDistressMessage()` (3657) | `ShipBroadcastDistressMessage()` `Core/Scripting/OOJSShip.cpp:3947` | ✅ |
| `ShipCheckCourseToDestination()` (3670) | `ShipCheckCourseToDestination()` `Core/Scripting/OOJSShip.cpp:3961` | ✅ |
| `ShipGetSafeCourseToDestination()` (3684) | `ShipGetSafeCourseToDestination()` `Core/Scripting/OOJSShip.cpp:3976` | ✅ |
| `ShipCheckScanner()` (3698) | `ShipCheckScanner()` `Core/Scripting/OOJSShip.cpp:3991` | ✅ |
| `ShipAdjustCargo()` (3728) | `ShipAdjustCargo()` `Core/Scripting/OOJSShip.cpp:4023` | ✅ |
| `ShipDamageAssessment()` (3770) | `ShipDamageAssessment()` `Core/Scripting/OOJSShip.cpp:4065` | ✅ |
| `ShipThreatAssessment()` (3797) | `ShipThreatAssessment()` `Core/Scripting/OOJSShip.cpp:4093` | ✅ |
| `ShipThreatAssessmentWeapon()` (3918) | `ShipThreatAssessmentWeapon()` `Core/Scripting/OOJSShip.cpp:4215` | ✅ |
| `ShipStaticKeys()` (3928) | `ShipStaticKeys()` `Core/Scripting/OOJSShip.cpp:4225` | ❌ |
| `ShipStaticKeysForRole()` (3939) | `ShipStaticKeysForRole()` `Core/Scripting/OOJSShip.cpp:4235` | ✅ |
| `ShipStaticRoleIsInCategory()` (3956) | `ShipStaticRoleIsInCategory()` `Core/Scripting/OOJSShip.cpp:4254` | ✅ |
| `ShipStaticRoles()` (3973) | `ShipStaticRoles()` `Core/Scripting/OOJSShip.cpp:4271` | ❌ |
| `ShipStaticShipDataForKey()` (3984) | `ShipStaticShipDataForKey()` `Core/Scripting/OOJSShip.cpp:4281` | ✅ |
| `ShipStaticSetShipDataForKey()` (4000) | `ShipStaticSetShipDataForKey()` `Core/Scripting/OOJSShip.cpp:4298` | ✅ |

**Differences:**

- ⚠️ **med** — The `speed` property is now `OOJS_PROP_READWRITE_CB` (upstream `OOJS_PROP_READONLY_CB`), and `ShipSetProperty` gains a `kShip_speed` case that calls `setSpeed()`. Upstream ignored writes silently (a TypeError in strict mode). In the port an NPC write takes effect. `player.ship.speed = x` throws "player.ship.speed is read-only.", and a negative or NaN value throws "ship.speed must be a number >= 0.". A long code comment justifies the change, but no ADR or bead id is cited. It is the only change in the property table: name, tinyid and flags match for every other entry — upstream `Core/Scripting/OOJSShip.m:450` ↔ port `Core/Scripting/OOJSShip.cpp:504`, `:1693-1734`
- ❌ **low** — `setCrew()`: when `OOCharacter characterWithDictionary` answers nil, upstream `[NSArray arrayWithObject:nil]` raises, and the JS caller gets an exception. The port skips the nil character and sets an empty crew, so the ship becomes unpiloted and the call returns `true` — upstream `Core/Scripting/OOJSShip.m:2889-2890` ↔ port `Core/Scripting/OOJSShip.cpp:3139-3143`
- ❌ **low** — `Ship.keys()` and `Ship.roles()` return array order from `std::map` (sorted ascending, `OOShipRegistry::shipKeys()/shipRoles()`). Upstream used NSDictionary `-allKeys` hash order. The contents are the same but scripts can see the new order. (The order change comes from OOShipRegistry, not this file.) — upstream `Core/Scripting/OOJSShip.m:3933,3978` ↔ port `Core/Scripting/OOJSShip.cpp:4230,4276`
- ✅ note (dict key order) — Getter `roleWeights` and `equipmentStatus(…, true)` build `oo::PList::Dict` (`std::map`). JS object keys therefore enumerate sorted (`EQUIPMENT_DAMAGED` before `EQUIPMENT_OK`), not in NSDictionary order. The values are the same — port `Core/Scripting/OOJSShip.cpp:781-785`, `:2971-2973`
- ✅ note (ShipGetProperty) — I checked all 144 cases. Every case returns the same type, and null vs undefined matches upstream. A stale entity gives `undefined`. A nil object result gives `null` (`StringOrNull`, `EntityObjectNode`, a null PList). `scriptInfo` is `{}`, never null (`ShipEntity::getScriptInfo()`). `crew` is null when the ship is unpiloted. `commodity` is null when the amount is 0. The AI getters give null/false with no AI — port `Core/Scripting/OOJSShip.cpp:733-1335`
- ✅ note (ShipSetProperty) — Validation and error text match case by case. This covers the entityPersonality range, scriptedMisjumpRange and sunGlareFilter messages, max* "cannot be negative", the injectorSpeedFactor bounds, maxEscorts, and "player.ship.%s is read-only.". Two upstream quirks are kept: `kShip_entityPersonality` falls through to `hyperspaceSpinTime` when int conversion fails, and the sunGlareFilter message says "> 0.0 and < 1.0" while the check is inclusive — port `Core/Scripting/OOJSShip.cpp:1489-1498`, `:1819-1829`
- ✅ note (copy-paste quirks kept) — `patrolReportIn()` reports bad arguments as `addDefenseTarget`. `keysForRole` reports as `shipKeysForRole`. `setShipDataForKey` reports as `setShipInfoForKey`. The property table still lists `homeSystem` twice. `setBounty()` returns true without setting rval — port `Core/Scripting/OOJSShip.cpp:3616`, `:4247`, `:4307`, `:435/439`, `:3079`
- ✅ note (nil args) — Where upstream sent nil to a Foundation API, the port passes `""`: `setShipDataForKey` with a non-string key, `adjustCargo` with a nil commodity. `setShipDataForKey` with a non-object second argument uses `toObject` where upstream used raw `JSVAL_TO_OBJECT`. `checkScanner` skips null entries, where upstream `addObject:nil` would raise. Only degenerate inputs reach any of these — port `Core/Scripting/OOJSShip.cpp:4304`, `:4053-4056`, `:4015`
- ✅ note — `InitOOJSShip` registers `OOJSEntityObjectConverter` in place of `OOJSBasicPrivateObjectConverter`, and the class finalizer is `OOJSCxxObjectWrapperFinalize`. Both are documented as the entity-holder replacement (bead oo-9ht.39.3), and the converted value is still the entity's object or null when stale — port `Core/Scripting/OOJSShip.cpp:216`, `:648`

## OOJSShipGroup

<a id="oojsshipgroup"></a>
Upstream `src/Core/Scripting/OOJSShipGroup.m` ↔ `Core/Scripting/OOJSShipGroup.cpp`

**Status:** done — 13 rows: 13 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| *(added)* `sShipGroupClass` (41) | `sShipGroupClass` OOJSShipGroup.cpp:109 | ✅ |
| *(added)* `sShipGroupProperties[]` (64): `count` RO, `leader` RW, `name` RW, `ships` RO (all _CB) | `sShipGroupProperties` OOJSShipGroup.cpp:137 (+ identical raw mirror :152 for error reporters) | ✅ |
| *(added)* `sShipGroupMethods[]` (73): `toString`/0, `addShip`/1, `containsShip`/1, `removeShip`/1 | `sShipGroupMethods` OOJSShipGroup.cpp:163 | ✅ |
| `InitOOJSShipGroup()` (84) | `InitOOJSShipGroup` OOJSShipGroup.cpp:190 (converter :179 gives the group as an Object node) | ✅ |
| `ShipGroupGetProperty()` (90) | `ShipGroupGetProperty` OOJSShipGroup.cpp:200 | ✅ |
| `ShipGroupSetProperty()` (134) | `ShipGroupSetProperty` OOJSShipGroup.cpp:248 | ✅ |
| `ShipGroupConstruct()` (173) | `ShipGroupConstruct` OOJSShipGroup.cpp:297 | ✅ |
| `-[OOShipGroup(OOJavaScriptExtensions) oo_jsValueInContext:]` (208) | `OOShipGroup::jsValueInContext` OOJSShipGroup.cpp:337 | ✅ |
| `-[OOShipGroup(OOJavaScriptExtensions) oo_clearJSSelf:]` (226) | `OOShipGroup::clearJSSelf` OOJSShipGroup.cpp:355 | ✅ |
| *(added)* inherited `oo_jsDescription` (class name `OOShipGroup`, no `oo_jsClassName`) | `OOShipGroup::jsDescription` OOJSShipGroup.cpp:363 + `ShipGroupToString` :375 | ✅ |
| `ShipGroupAddShip()` (237) | `ShipGroupAddShip` OOJSShipGroup.cpp:383 | ✅ |
| `ShipGroupRemoveShip()` (298) | `ShipGroupRemoveShip` OOJSShipGroup.cpp:454 | ✅ |
| `ShipGroupContainsShip()` (324) | `ShipGroupContainsShip` OOJSShipGroup.cpp:484 | ✅ |

**Differences:**

- ✅ note — The escort-group branch of `addShip` is the same. "Already escorting" logs a warning and returns `false`. When an unescorted leader whose escort group is its own group gets a new escort, the port makes a new `"ship group"` group and leaks it at +1, as upstream's `alloc/init` did. The text of the escort-group warning matches, but `%@` of the ship becomes `oo::EntityDescription(ship)` (audited with entities) — upstream `:258-291` ↔ port `:409-446`

## OOJSSound

<a id="oojssound"></a>
Upstream `src/Core/Scripting/OOJSSound.m` ↔ `Core/Scripting/OOJSSound.cpp`

**Status:** done — 15 rows: 14 ✅ 1 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| *(added)* `sSoundClass` (43; setProperty `JS_StrictPropertyStub`) | `sSoundClass` OOJSSound.cpp:118 (setProperty default stub) | ✅ |
| *(added)* `sSoundProperties[]` (63): `name` RO_CB | OOJSSound.cpp:143 | ✅ |
| *(added)* `sSoundMethods[]` (69): `toString`/0 | OOJSSound.cpp:151 (`SoundToString` :304) | ✅ |
| *(added)* `sSoundStaticMethods[]` (79): `load`/1, `musicSoundSource`/0, `playMusic`/1, `stopMusic`/0 | OOJSSound.cpp:164 | ✅ |
| `InitOOJSSound()` (108) | `InitOOJSSound` OOJSSound.cpp:219; converter `SoundConverter` :211 returns null | ⚠️ |
| `SoundFromJSValue()` (114) | `SoundFromJSValue` OOJSSound.cpp:228 (keeps the upstream pause-without-resume of the time limiter) | ✅ |
| `SoundGetProperty()` (131) | `SoundGetProperty` OOJSSound.cpp:254 | ✅ |
| `GetNamedSound()` (156) | `GetNamedSound` OOJSSound.cpp:286 | ✅ |
| `SoundStaticLoad()` (172) | `SoundStaticLoad` OOJSSound.cpp:314 | ✅ |
| `SoundStaticMusicSoundSource()` (195) | `SoundStaticMusicSoundSource` OOJSSound.cpp:339 | ✅ |
| `SoundStaticPlayMusic()` (207) | `SoundStaticPlayMusic` OOJSSound.cpp:354 | ✅ |
| `SoundStaticStopMusic()` (245) | `SoundStaticStopMusic` OOJSSound.cpp:394 | ✅ |
| `-[OOSound(OOJavaScriptExtentions) oo_jsValueInContext:]` (273) | `OOJSSoundJSValueInContext` OOJSSound.cpp:424 (new wrapper per call, as upstream) | ✅ |
| `-[OOSound(OOJavaScriptExtentions) oo_jsDescription]` (289) | `OOJSSoundJSDescription` OOJSSound.cpp:444 (`[Sound "name"]`, `(null)` for nil name) | ✅ |
| `-[OOSound(OOJavaScriptExtentions) oo_jsClassName]` (294) | `OOJSSoundJSClassName` OOJSSound.cpp:449 | ✅ |

**Differences:**

- ⚠️ **low** — The object converter registered for `Sound` returns null instead of the native `OOSound` that `OOJSBasicPrivateObjectConverter` returned. A generic JS→native or plist conversion of a `Sound` object therefore gives null. The sound's own callers use `SoundFromJSValue()`, which reads the slot directly (bead oo-9ht.68, comment at :207-209) — upstream `OOJSSound.m:111` ↔ port `OOJSSound.cpp:211-214,225`
- ✅ note — The texts and arities of the `playMusic` argument errors match, including upstream's second report with count 1 after `OOJSArgumentListGetNumber` fails. The `stopMusic` text `"string or no argument"` matches too.

## OOJSSoundSource

<a id="oojssoundsource"></a>
Upstream `src/Core/Scripting/OOJSSoundSource.m` ↔ `Core/Scripting/OOJSSoundSource.cpp`

**Status:** done — 13 rows: 12 ✅ 1 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| *(added)* `sSoundSourceClass` (42) | `sSoundSourceClass` OOJSSoundSource.cpp:105 | ✅ |
| *(added)* `sSoundSourceProperties[]` (68): `isPlaying` RO, `loop`/`position`/`positional`/`repeatCount`/`sound`/`volume` RW (all _CB) | OOJSSoundSource.cpp:136 | ✅ |
| *(added)* `sSoundSourceMethods[]` (80): `toString`/0, `play`/0, `playOrRepeat`/0, `stop`/0 | OOJSSoundSource.cpp:150 | ✅ |
| `InitOOJSSoundSource()` (110) | `InitOOJSSoundSource` OOJSSoundSource.cpp:202; converter `SoundSourceConverter` :194 returns null | ⚠️ |
| `SoundSourceConstruct()` (116) | `SoundSourceConstruct` OOJSSoundSource.cpp:210 | ✅ |
| `SoundSourceGetProperty()` (132) | `SoundSourceGetProperty` OOJSSoundSource.cpp:230 | ✅ |
| `SoundSourceSetProperty()` (179) | `SoundSourceSetProperty` OOJSSoundSource.cpp:283 (keeps the dead `if (100 < 1)` and the 0..1 volume clamp) | ✅ |
| `SoundSourcePlay()` (255) | `SoundSourcePlay` OOJSSoundSource.cpp:378 | ✅ |
| `SoundSourceStop()` (285) | `SoundSourceStop` OOJSSoundSource.cpp:412 | ✅ |
| `SoundSourcePlayOrRepeat()` (304) | `SoundSourcePlayOrRepeat` OOJSSoundSource.cpp:435 | ✅ |
| `-[OOSoundSource(OOJavaScriptExtentions) oo_jsValueInContext:]` (324) | `OOJSSoundSourceJSValueInContext` OOJSSoundSource.cpp:460 | ✅ |
| `-[OOSoundSource(OOJavaScriptExtentions) oo_jsClassName]` (340) | `OOJSSoundSourceJSClassName` OOJSSoundSource.cpp:480 | ✅ |
| *(added)* inherited `oo_jsDescription` → `[SoundSource <components>]` | `OOJSSoundSourceHolder::jsDescription` OOJSSoundSource.cpp:171 + `SoundSourceToString` :370 | ✅ |

**Differences:**

- ⚠️ **low** — The `SoundSource` object converter returns null instead of the native `OOSoundSource` (bead oo-9ht.88, comment at :191-193). A generic JS→native conversion of a `SoundSource` gives null — upstream `OOJSSoundSource.m:113` ↔ port `OOJSSoundSource.cpp:194-197,206`

## OOJSSpecialFunctions

<a id="oojsspecialfunctions"></a>
Upstream `src/Core/Scripting/OOJSSpecialFunctions.m` ↔ `Core/Scripting/OOJSSpecialFunctions.cpp`

**Status:** done — 5 rows: 5 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| *(added)* `sSpecialFunctionsMethods[]` (33): `jsWarning`/1, `markConsoleEntryPoint`/0 (if `OO_DEBUG_MONITOR_ENABLED`) | OOJSSpecialFunctions.cpp:37 (macro comes from `meson.build` command line, so unaffected by `OO_PLAIN_CPP_UNIT`) | ✅ |
| `InitOOJSSpecialFunctions()` (43) | `InitOOJSSpecialFunctions` OOJSSpecialFunctions.cpp:46 (empty) | ✅ |
| `JSSpecialFunctionsObjectWrapper()` (47) | `JSSpecialFunctionsObjectWrapper` OOJSSpecialFunctions.cpp:50 | ✅ |
| `SpecialJSWarning()` (70) | `SpecialJSWarning` OOJSSpecialFunctions.cpp:73 | ✅ |
| `SpecialMarkConsoleEntryPoint()` (90) | `SpecialMarkConsoleEntryPoint` OOJSSpecialFunctions.cpp:93 | ✅ |

**Differences:**

- *none*

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

**Status:** done — 16 rows: 14 ✅ 0 ⚠️ 2 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| *(added)* `Resources/Config/whitelist.plist` (all 12 tables: `action_methods`, `ai_methods`, `ai_and_action_methods`, `query_methods`, `*_aliases`, …) and `+[ResourceManager whitelistDictionary]` | same file, byte-identical (diffed); `ResourceManager::whitelistDictionary` ResourceManager.cpp:1641 (loaded once, missing cached) | ✅ |
| `OOSanitizeLegacyScript()` (58) | `OOSanitizeLegacyScript` OOLegacyScriptWhitelist.cpp:66 | ✅ |
| `OOSanitizeLegacyScriptInternal()` (65) | `OOSanitizeLegacyScriptInternal` OOLegacyScriptWhitelist.cpp:73 | ❌ |
| `OOSanitizeLegacyScriptConditions()` (101) | `OOSanitizeLegacyScriptConditions` OOLegacyScriptWhitelist.cpp:107 | ✅ |
| `OOSanitizeLegacyScriptConditionsInternal()` (110) | `OOSanitizeLegacyScriptConditionsInternal` OOLegacyScriptWhitelist.cpp:114 | ❌ |
| `OOLegacyConditionsAreSanitized()` (149) | `OOLegacyConditionsAreSanitized` OOLegacyScriptWhitelist.cpp:155 | ✅ |
| `SanitizeCondition()` (156) | `SanitizeCondition` OOLegacyScriptWhitelist.cpp:164 | ✅ |
| `SanitizeConditionalStatement()` (290) | `SanitizeConditionalStatement` OOLegacyScriptWhitelist.cpp:292 | ✅ |
| `SanitizeActionStatement()` (336) | `SanitizeActionStatement` OOLegacyScriptWhitelist.cpp:346 | ✅ |
| `ClassifyLHSConditionSelector()` (375) | `ClassifyLHSConditionSelector` OOLegacyScriptWhitelist.cpp:397 (unused `stack` param dropped) | ✅ |
| `SanitizeQueryMethod()` (406) | `SanitizeQueryMethod` OOLegacyScriptWhitelist.cpp:467 (+ `WhitelistSet` :433, `WhitelistDictionary` :449, `AliasFor` :456) | ✅ |
| `SanitizeActionMethod()` (427) | `SanitizeActionMethod` OOLegacyScriptWhitelist.cpp:486 | ✅ |
| `AlwaysFalseConditions()` (476) | `AlwaysFalseConditions` OOLegacyScriptWhitelist.cpp:539 (upstream inverted-cache bug kept: always returns null) | ✅ |
| `IsAlwaysFalseConditions()` (487) | `IsAlwaysFalseConditions` OOLegacyScriptWhitelist.cpp:555 | ✅ |
| `StringFromStackInternal()` (492) | `StringFromStackInternal` OOLegacyScriptWhitelist.cpp:565 | ✅ |
| `StringFromStack()` (512) | `StringFromStack` OOLegacyScriptWhitelist.cpp:583 | ✅ |

**Differences:**

- ❌ **low** — `script.syntax.statement.invalidType` names the offending statement's type with the PList type name (`integer`, `array`, `bool` …) instead of its Objective-C class (`[statement class]`, e.g. `GSMutableArray`). The log text differs for malformed scripts only — upstream `OOLegacyScriptWhitelist.m:86` ↔ port `OOLegacyScriptWhitelist.cpp:93`
- ❌ **low** — `script.syntax.condition.notString` has the same class-name→type-name change in its log text — upstream `OOLegacyScriptWhitelist.m:129` ↔ port `OOLegacyScriptWhitelist.cpp:133`
- ✅ note (whitelist tables, entry by entry) — `whitelist.plist` is byte-identical. The sets are built the same way:
  - query = `query_methods`.
  - action = `action_methods` ∪ `ai_and_action_methods`.
  - action+AI = `ai_methods` ∪ action.
  - AI aliases = `ai_method_aliases` overlaid by `action_method_aliases` (action wins, as `dictionaryByAddingEntriesFromDictionary:`).

  Aliases are resolved before the membership test, and LHS type is classified by the suffix of the un-aliased selector. Both sides cache the tables for the life of the process — upstream `:406-473` ↔ port `:467-534`
- ✅ note — `AlwaysFalseConditions()` returns nil/null on both sides because of the upstream `!= nil` bug, so a conditions array that fails sanitising drops its whole conditional statement. This is kept on purpose (port comment :541-544, proposed ADR-0043). The comparator table (`morethan`→GREATERTHAN), the rejection of OP_NUMBER+`undefined` and OP_BOOL with anything but equal/notequal, the RHS coalescing, the join and `[credits_number]` rewrite of action arguments, and the 2-element array for actions with no argument all match.

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

