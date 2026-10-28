# Parity findings: OXP verifier

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## OOAIStateMachineVerifierStage

<a id="ooaistatemachineverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOAIStateMachineVerifierStage.m` ↔ `Core/OXPVerifier/OOAIStateMachineVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOAIStateMachineVerifierStage dealloc]` (44) | | |
| `-[OOAIStateMachineVerifierStage name]` (52) | | |
| `-[OOAIStateMachineVerifierStage shouldRun]` (57) | | |
| `-[OOAIStateMachineVerifierStage run]` (62) | | |
| `+[OOAIStateMachineVerifierStage nameForReverseDependencyForVerifier:]` (85) | | |
| `-[OOAIStateMachineVerifierStage stateMachineNamed:usedByShip:]` (90) | | |
| `-[OOAIStateMachineVerifierStage(Private) validateAI:]` (115) | | |

**Differences:**

- *pending*


## OOCheckDemoShipsPListVerifierStage

<a id="oocheckdemoshipsplistverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOCheckDemoShipsPListVerifierStage.m` ↔ `Core/OXPVerifier/OOCheckDemoShipsPListVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCheckDemoShipsPListVerifierStage name]` (42) | | |
| `-[OOCheckDemoShipsPListVerifierStage shouldRun]` (47) | | |
| `-[OOCheckDemoShipsPListVerifierStage run]` (58) | | |
| `-[OOCheckDemoShipsPListVerifierStage(OOPrivate) runCheckWithDemoShips:shipData:]` (101) | | |

**Differences:**

- *pending*


## OOCheckEquipmentPListVerifierStage

<a id="oocheckequipmentplistverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOCheckEquipmentPListVerifierStage.m` ↔ `Core/OXPVerifier/OOCheckEquipmentPListVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCheckEquipmentPListVerifierStage name]` (44) | | |
| `-[OOCheckEquipmentPListVerifierStage shouldRun]` (49) | | |
| `-[OOCheckEquipmentPListVerifierStage run]` (60) | | |
| `-[OOCheckEquipmentPListVerifierStage(OOPrivate) runCheckWithEquipment:]` (88) | | |

**Differences:**

- *pending*


## OOCheckJSSyntaxVerifierStage

<a id="oocheckjssyntaxverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOCheckJSSyntaxVerifierStage.m` ↔ `Core/OXPVerifier/OOCheckJSSyntaxVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCheckJSSyntaxVerifierStage name]` (40) | | |
| `-[OOCheckJSSyntaxVerifierStage shouldRun]` (45) | | |
| `-[OOCheckJSSyntaxVerifierStage run]` (53) | | |

**Differences:**

- *pending*


## OOCheckPListSyntaxVerifierStage

<a id="oocheckplistsyntaxverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOCheckPListSyntaxVerifierStage.m` ↔ `Core/OXPVerifier/OOCheckPListSyntaxVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCheckPListSyntaxVerifierStage name]` (37) | | |
| `-[OOCheckPListSyntaxVerifierStage shouldRun]` (42) | | |
| `-[OOCheckPListSyntaxVerifierStage run]` (47) | | |

**Differences:**

- *pending*


## OOCheckRequiresPListVerifierStage

<a id="oocheckrequiresplistverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOCheckRequiresPListVerifierStage.m` ↔ `Core/OXPVerifier/OOCheckRequiresPListVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCheckRequiresPListVerifierStage name]` (37) | | |
| `-[OOCheckRequiresPListVerifierStage shouldRun]` (42) | | |
| `-[OOCheckRequiresPListVerifierStage run]` (53) | | |

**Differences:**

- *pending*


## OOCheckShipDataPListVerifierStage

<a id="oocheckshipdataplistverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOCheckShipDataPListVerifierStage.m` ↔ `Core/OXPVerifier/OOCheckShipDataPListVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCheckShipDataPListVerifierStage name]` (58) | | |
| `-[OOCheckShipDataPListVerifierStage dependents]` (63) | | |
| `-[OOCheckShipDataPListVerifierStage shouldRun]` (71) | | |
| `-[OOCheckShipDataPListVerifierStage run]` (82) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) verifyShipInfo:withName:]` (163) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) message:]` (194) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) verboseMessage:]` (209) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) getRoles]` (227) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) checkKeys]` (249) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) checkSchema]` (277) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) checkModel]` (282) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) rolesFromString:]` (308) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) verifier:]` (337) | | |
| `-[OOCheckShipDataPListVerifierStage(OOPrivate) verifier:]` (349) | | |

**Differences:**

- *pending*


## OOFileScannerVerifierStage

<a id="oofilescannerverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOFileScannerVerifierStage.m` ↔ `Core/OXPVerifier/OOFileScannerVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOFileScannerVerifierStage dealloc]` (83) | | |
| `-[OOFileScannerVerifierStage name]` (95) | | |
| `-[OOFileScannerVerifierStage run]` (100) | | |
| `+[OOFileScannerVerifierStage nameForDependencyForVerifier:]` (118) | | |
| `-[OOFileScannerVerifierStage fileExists:]` (130) | | |
| `-[OOFileScannerVerifierStage pathForFile:]` (138) | | |
| `-[OOFileScannerVerifierStage dataForFile:]` (209) | | |
| `-[OOFileScannerVerifierStage plistNamed:]` (223) | | |
| `-[OOFileScannerVerifierStage displayNameForFile:andFolder:]` (283) | | |
| `-[OOFileScannerVerifierStage filesInFolder:]` (290) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) scanForFiles]` (301) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) checkRootFolders]` (374) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) checkConfigFiles]` (398) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) checkKnownFiles]` (431) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) lowercaseMap:]` (474) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) scanDirectory:]` (497) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) checkPListFormat:file:folder:]` (547) | | |
| `-[OOFileScannerVerifierStage(OOPrivate) constructReadMeNames]` (585) | | |
| `-[OOListUnusedFilesStage name]` (624) | | |
| `-[OOListUnusedFilesStage dependencies]` (629) | | |
| `-[OOListUnusedFilesStage run]` (634) | | |
| `+[OOListUnusedFilesStage nameForReverseDependencyForVerifier:]` (639) | | |
| `-[OOOXPVerifier(OOFileScannerVerifierStage) fileScannerStage]` (655) | | |
| `-[OOFileHandlingVerifierStage dependencies]` (664) | | |
| `-[OOFileHandlingVerifierStage dependents]` (669) | | |
| `CheckNameConflict()` (676) | | |

**Differences:**

- *pending*


## OOModelVerifierStage

<a id="oomodelverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOModelVerifierStage.m` ↔ `Core/OXPVerifier/OOModelVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOModelVerifierStage init]` (47) | | |
| `-[OOModelVerifierStage dealloc]` (57) | | |
| `+[OOModelVerifierStage nameForReverseDependencyForVerifier:]` (64) | | |
| `-[OOModelVerifierStage name]` (76) | | |
| `-[OOModelVerifierStage shouldRun]` (81) | | |
| `-[OOModelVerifierStage run]` (86) | | |
| `-[OOModelVerifierStage modelNamed:]` (122) | | |
| `-[OOModelVerifierStage(OOPrivate) checkModel:]` (171) | | |
| `-[OOOXPVerifier(OOModelVerifierStage) modelVerifierStage]` (184) | | |

**Differences:**

- *pending*


## OOOXPVerifier

<a id="oooxpverifier"></a>
Upstream `src/Core/OXPVerifier/OOOXPVerifier.m` ↔ `Core/OXPVerifier/OOOXPVerifier.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOOXPVerifier runVerificationIfRequested]` (95) | | |
| `-[OOOXPVerifier dealloc]` (147) | | |
| `-[OOOXPVerifier registerStage:]` (158) | | |
| `-[OOOXPVerifier oxpPath]` (198) | | |
| `-[OOOXPVerifier oxpDisplayName]` (203) | | |
| `-[OOOXPVerifier stageWithName:]` (208) | | |
| `-[OOOXPVerifier configurationValueForKey:]` (216) | | |
| `-[OOOXPVerifier configurationArrayForKey:]` (221) | | |
| `-[OOOXPVerifier configurationDictionaryForKey:]` (226) | | |
| `-[OOOXPVerifier configurationStringForKey:]` (231) | | |
| `-[OOOXPVerifier configurationSetForKey:]` (236) | | |
| `-[OOOXPVerifier(OOPrivate) initWithPath:]` (246) | | |
| `-[OOOXPVerifier(OOPrivate) run]` (275) | | |
| `-[OOOXPVerifier(OOPrivate) setUpLogOverrides]` (305) | | |
| `-[OOOXPVerifier(OOPrivate) registerBaseStages]` (328) | | |
| `-[OOOXPVerifier(OOPrivate) buildDependencyGraph]` (362) | | |
| `-[OOOXPVerifier(OOPrivate) runStages]` (467) | | |
| `-[OOOXPVerifier(OOPrivate) setUpDependencies:]` (533) | | |
| `-[OOOXPVerifier(OOPrivate) setUpDependents:]` (559) | | |
| `-[OOOXPVerifier(OOPrivate) dumpDebugGraphviz]` (582) | | |
| `SwitchLogFile()` (648) | | |
| `NoteVerificationStage()` (660) | | |
| `OpenLogFile()` (665) | | |

**Differences:**

- *pending*


## OOOXPVerifierStage

<a id="oooxpverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOOXPVerifierStage.m` ↔ `Core/OXPVerifier/OOOXPVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOOXPVerifierStage init]` (43) | | |
| `-[OOOXPVerifierStage dealloc]` (57) | | |
| `-[OOOXPVerifierStage description]` (66) | | |
| `-[OOOXPVerifierStage verifier]` (71) | | |
| `-[OOOXPVerifierStage completed]` (76) | | |
| `-[OOOXPVerifierStage name]` (81) | | |
| `-[OOOXPVerifierStage dependencies]` (87) | | |
| `-[OOOXPVerifierStage dependents]` (92) | | |
| `-[OOOXPVerifierStage shouldRun]` (97) | | |
| `-[OOOXPVerifierStage run]` (102) | | |
| `-[OOOXPVerifierStage(OOInternal) setVerifier:]` (111) | | |
| `-[OOOXPVerifierStage(OOInternal) isDependentOf:]` (116) | | |
| `-[OOOXPVerifierStage(OOInternal) registerDependency:]` (136) | | |
| `-[OOOXPVerifierStage(OOInternal) canRun]` (144) | | |
| `-[OOOXPVerifierStage(OOInternal) performRun]` (149) | | |
| `-[OOOXPVerifierStage(OOInternal) noteSkipped]` (167) | | |
| `-[OOOXPVerifierStage(OOInternal) dependencyRegistrationComplete]` (176) | | |
| `-[OOOXPVerifierStage(OOInternal) resolvedDependencies]` (181) | | |
| `-[OOOXPVerifierStage(OOInternal) resolvedDependents]` (186) | | |
| `-[OOOXPVerifierStage(OOPrivate) registerDepedent:]` (195) | | |
| `-[OOOXPVerifierStage(OOPrivate) dependencyCompleted:]` (202) | | |

**Differences:**

- *pending*


## OOPListSchemaVerifier

<a id="ooplistschemaverifier"></a>
Upstream `src/Core/OXPVerifier/OOPListSchemaVerifier.m` ↔ `Core/OXPVerifier/OOPListSchemaVerifier.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `BackLink()` (137) | | |
| `BackLinkIndex()` (143) | | |
| `BackLinkRoot()` (149) | | |
| `+[OOPListSchemaVerifier verifierWithSchema:]` (229) | | |
| `-[OOPListSchemaVerifier initWithSchema:]` (234) | | |
| `-[OOPListSchemaVerifier dealloc]` (253) | | |
| `-[OOPListSchemaVerifier setDelegate:]` (261) | | |
| `-[OOPListSchemaVerifier delegate]` (269) | | |
| `-[OOPListSchemaVerifier verifyPropertyList:named:]` (274) | | |
| `+[OOPListSchemaVerifier descriptionForKeyPath:]` (291) | | |
| `-[OOPListSchemaVerifier(OOPrivate) delegateVerifierWithPropertyList:]` (323) | | |
| `-[OOPListSchemaVerifier(OOPrivate) delegateVerifierWithPropertyList:]` (367) | | |
| `-[OOPListSchemaVerifier(OOPrivate) verifyPList:]` (395) | | |
| `-[OOPListSchemaVerifier(OOPrivate) resolveSchemaType:]` (476) | | |
| `StringToSchemaType()` (525) | | |
| `ApplyStringFilter()` (562) | | |
| `ApplyStringTest()` (623) | | |
| `KeyPathToArray()` (660) | | |
| `KeyPathToString()` (674) | | |
| `StringForErrorReport()` (679) | | |
| `ArrayForErrorReport()` (700) | | |
| `SetForErrorReport()` (731) | | |
| `StringOrArrayForErrorReport()` (736) | | |
| `Verify_String()` (764) | | |
| `Verify_Array()` (825) | | |
| `Verify_Dictionary()` (879) | | |
| `Verify_Integer()` (983) | | |
| `Verify_PositiveInteger()` (1011) | | |
| `Verify_Float()` (1039) | | |
| `Verify_PositiveFloat()` (1067) | | |
| `Verify_OneOf()` (1099) | | |
| `Verify_Enumeration()` (1143) | | |
| `Verify_Boolean()` (1171) | | |
| `Verify_FuzzyBoolean()` (1182) | | |
| `Verify_Vector()` (1195) | | |
| `Verify_Quaternion()` (1206) | | |
| `Verify_DelegatedType()` (1217) | | |
| `-[NSString(OOPListSchemaVerifierHelpers) ooPListVerifierHasSubString:]` (1253) | | |
| `-[NSError(OOPListSchemaVerifierConveniences) plistKeyPath]` (1262) | | |
| `-[NSError(OOPListSchemaVerifierConveniences) plistKeyPathDescription]` (1267) | | |
| `-[NSError(OOPListSchemaVerifierConveniences) missingRequiredKeys]` (1272) | | |
| `-[NSError(OOPListSchemaVerifierConveniences) expectedClass]` (1277) | | |
| `-[NSError(OOPListSchemaVerifierConveniences) expectedClassName]` (1282) | | |
| `Error()` (1292) | | |
| `ErrorWithProperty()` (1304) | | |
| `ErrorWithDictionary()` (1320) | | |
| `ErrorWithDictionaryAndArguments()` (1332) | | |
| `ErrorTypeMismatch()` (1350) | | |
| `ErrorFailureAlreadyReported()` (1383) | | |
| `IsFailureAlreadyReportedError()` (1388) | | |

**Differences:**

- *pending*


## OOTextureVerifierStage

<a id="ootextureverifierstage"></a>
Upstream `src/Core/OXPVerifier/OOTextureVerifierStage.m` ↔ `Core/OXPVerifier/OOTextureVerifierStage.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOTextureVerifierStage init]` (44) | | |
| `-[OOTextureVerifierStage dealloc]` (53) | | |
| `+[OOTextureVerifierStage nameForReverseDependencyForVerifier:]` (60) | | |
| `-[OOTextureVerifierStage name]` (65) | | |
| `-[OOTextureVerifierStage shouldRun]` (70) | | |
| `-[OOTextureVerifierStage run]` (75) | | |
| `-[OOTextureVerifierStage textureNamed:usedInContext:]` (96) | | |
| `-[OOTextureVerifierStage(OOPrivate) checkTextureNamed:inFolder:]` (119) | | |
| `-[OOTextureHandlingStage dependents]` (169) | | |
| `-[OOOXPVerifier(OOTextureVerifierStage) textureVerifierStage]` (180) | | |

**Differences:**

- *pending*

