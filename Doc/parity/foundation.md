# Parity findings: Foundation replacements, logging, collections, async

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## NSBundle+Override

<a id="nsbundleoverride"></a>
Upstream `src/Core/NSBundle+Override.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSBundle(Override) infoDictionary]` (20) | | |

**Differences:**

- *pending*


## NSDataOOExtensions

<a id="nsdataooextensions"></a>
Upstream `src/Core/NSDataOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[NSData(OOExtensions) oo_dataWithOXZFile:]` (37) | | |

**Differences:**

- *pending*


## NSDictionaryOOExtensions

<a id="nsdictionaryooextensions"></a>
Upstream `src/Core/NSDictionaryOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSDictionary(OOExtensions) dictionaryByAddingObject:forKey:]` (32) | | |
| `-[NSDictionary(OOExtensions) dictionaryByRemovingObjectForKey:]` (46) | | |
| `-[NSDictionary(OOExtensions) dictionaryByAddingEntriesFromDictionary:]` (60) | | |

**Differences:**

- *pending*


## NSFileManagerOOExtensions

<a id="nsfilemanagerooextensions"></a>
Upstream `src/Core/NSFileManagerOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSFileManager(OOExtensions) commanderContentsOfPath:]` (39) | | |
| `-[NSFileManager(OOExtensions) defaultCommanderPath]` (79) | | |
| `-[NSFileManager(OOExtensions) oo_directoryContentsAtPath:]` (113) | | |
| `-[NSFileManager(OOExtensions) oo_createDirectoryAtPath:attributes:]` (118) | | |
| `-[NSFileManager(OOExtensions) oo_fileAttributesAtPath:traverseLink:]` (123) | | |
| `-[NSFileManager(OOExtensions) oo_fileSystemAttributesAtPath:]` (137) | | |
| `-[NSFileManager(OOExtensions) oo_removeItemAtPath:]` (142) | | |
| `-[NSFileManager(OOExtensions) oo_moveItemAtPath:toPath:]` (147) | | |
| `-[NSFileManager(OOExtensions) oo_directoryContentsAtPath:]` (154) | | |
| `-[NSFileManager(OOExtensions) oo_createDirectoryAtPath:attributes:]` (159) | | |
| `-[NSFileManager(OOExtensions) oo_fileAttributesAtPath:traverseLink:]` (164) | | |
| `-[NSFileManager(OOExtensions) oo_fileSystemAttributesAtPath:]` (169) | | |
| `-[NSFileManager(OOExtensions) oo_removeItemAtPath:]` (174) | | |
| `-[NSFileManager(OOExtensions) oo_moveItemAtPath:toPath:]` (179) | | |
| `-[NSFileManager(OOExtensions) chdirToSnapshotPath]` (187) | | |
| `-[NSFileManager(OOExtensions) oo_oxzFileExistsAtPath:]` (215) | | |

**Differences:**

- *pending*


## NSMutableDictionaryOOExtensions

<a id="nsmutabledictionaryooextensions"></a>
Upstream `src/Core/NSMutableDictionaryOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSMutableDictionary(OOExtensions) mergeEntriesFromDictionary:]` (30) | | |

**Differences:**

- *pending*


## NSNumberOOExtensions

<a id="nsnumberooextensions"></a>
Upstream `src/Core/NSNumberOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSNumber(OOExtensions) oo_isFloatingPointNumber]` (33) | | |
| `-[NSNumber(OOExtensions) oo_isBoolean]` (47) | | |

**Differences:**

- *pending*


## NSObjectOOExtensions

<a id="nsobjectooextensions"></a>
Upstream `src/Core/NSObjectOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[NSObject(OOExtensions) oo_instanceSize]` (38) | | |
| `-[NSObject(OOExtensions) oo_objectSize]` (43) | | |

**Differences:**

- *pending*


## NSScannerOOExtensions

<a id="nsscannerooextensions"></a>
Upstream `src/Core/NSScannerOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSScanner(OOExtensions) ooliteScanCharactersFromSet:intoString:]` (29) | | |
| `-[NSScanner(OOExtensions) ooliteScanUpToCharactersFromSet:intoString:]` (54) | | |

**Differences:**

- *pending*


## NSStringOOExtensions

<a id="nsstringooextensions"></a>
Upstream `src/Core/NSStringOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[NSString(OOExtensions) stringWithContentsOfUnicodeFile:]` (31) | | |
| `+[NSString(OOExtensions) stringWithUTF16String:]` (86) | | |
| `-[NSString(OOExtensions) utf16DataWithBOM:]` (102) | | |
| `-[NSString(OOExtensions) oo_hash]` (133) | | |
| `-[NSString(OOExtensions) stringByTrimmingLeadingCharactersInSet:]` (143) | | |
| `-[NSString(OOExtensions) stringByTrimmingLeadingWhitespaceAndNewlineCharacters]` (152) | | |
| `-[NSString(OOExtensions) stringByTrimmingTrailingCharactersInSet:]` (157) | | |
| `-[NSString(OOExtensions) stringByTrimmingTrailingWhitespaceAndNewlineCharacters]` (166) | | |
| `-[NSMutableString(OOExtensions) appendLine:]` (175) | | |
| `-[NSMutableString(OOExtensions) appendFormatLine:]` (180) | | |
| `-[NSMutableString(OOExtensions) appendFormatLine:arguments:]` (188) | | |
| `-[NSMutableString(OOExtensions) deleteCharacterAtIndex:]` (195) | | |
| `OOTabString()` (202) | | |

**Differences:**

- *pending*


## NSThreadOOExtensions

<a id="nsthreadooextensions"></a>
Upstream `src/Core/NSThreadOOExtensions.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[NSThread(OOExtensions) ooSetCurrentThreadName:]` (36) | | |

**Differences:**

- *pending*


## NSUserDefaults+Override

<a id="nsuserdefaultsoverride"></a>
Upstream `src/Core/NSUserDefaults+Override.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSUserDefaults(Override) writeDictionary:]` (36) | | |

**Differences:**

- *pending*


## OOAsyncQueue

<a id="ooasyncqueue"></a>
Upstream `src/Core/OOAsyncQueue.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `AllocElement()` (62) | | |
| `FreeElement()` (67) | | |
| `-[OOAsyncQueue init]` (82) | | |
| `-[OOAsyncQueue dealloc]` (97) | | |
| `-[OOAsyncQueue description]` (121) | | |
| `-[OOAsyncQueue enqueue:]` (127) | | |
| `-[OOAsyncQueue dequeue]` (175) | | |
| `-[OOAsyncQueue tryDequeue]` (181) | | |
| `-[OOAsyncQueue empty]` (202) | | |
| `-[OOAsyncQueue count]` (207) | | |
| `-[OOAsyncQueue emptyQueue]` (212) | | |
| `-[OOAsyncQueue(OOPrivate) doEmptyQueueWithAcquiredLock]` (225) | | |
| `-[OOAsyncQueue(OOPrivate) doDequeAndUnlockWithAcquiredLock]` (246) | | |
| `-[OOAsyncQueue(OOPrivate) recycleElementWithAcquiredLock:]` (280) | | |

**Differences:**

- *pending*


## OOAsyncWorkManager

<a id="ooasyncworkmanager"></a>
Upstream `src/Core/OOAsyncWorkManager.m` ↔ `Core/OOAsyncWorkManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitAsyncWorkManager()` (95) | | |
| `+[OOAsyncWorkManager initialize]` (121) | | |
| `+[OOAsyncWorkManager sharedAsyncWorkManager]` (130) | | |
| `+[OOAsyncWorkManager allocWithZone:]` (148) | | |
| `-[OOAsyncWorkManager dealloc]` (157) | | |
| `-[OOAsyncWorkManager release]` (163) | | |
| `-[OOAsyncWorkManager retain]` (167) | | |
| `-[OOAsyncWorkManager retainCount]` (172) | | |
| `-[OOAsyncWorkManager addTask:priority:]` (177) | | |
| `-[OOAsyncWorkManager completePendingTasks]` (183) | | |
| `-[OOAsyncWorkManager waitForTaskToComplete:]` (188) | | |
| `-[OOAsyncWorkManagerInternal init]` (198) | | |
| `-[OOAsyncWorkManagerInternal completePendingTasks]` (220) | | |
| `-[OOAsyncWorkManagerInternal waitForTaskToComplete:]` (236) | | |
| `-[OOAsyncWorkManagerInternal queueResult:]` (268) | | |
| `-[OOAsyncWorkManagerInternal noteTaskQueued:]` (275) | | |
| `-[OOManualDispatchAsyncWorkManager init]` (293) | | |
| `-[OOManualDispatchAsyncWorkManager addTask:priority:]` (318) | | |
| `-[OOManualDispatchAsyncWorkManager queueTask:]` (329) | | |
| `+[OOOperationQueueAsyncWorkManager canBeUsed]` (363) | | |
| `-[OOOperationQueueAsyncWorkManager init]` (371) | | |
| `-[OOOperationQueueAsyncWorkManager dealloc]` (385) | | |
| `-[OOOperationQueueAsyncWorkManager addTask:priority:]` (392) | | |
| `-[OOOperationQueueAsyncWorkManager dispatchTask:]` (413) | | |

**Differences:**

- *pending*


## OOCPUInfo

<a id="oocpuinfo"></a>
Upstream `src/Core/OOCPUInfo.m` ↔ `Core/OOCPUInfo.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOCPUInfoInit()` (60) | | |
| `OOCPUCount()` (99) | | |
| `OOCPUID()` (115) | | |
| `OOCPUDescription()` (132) | | |
| `OOSystemMemoryStatus()` (154) | | |
| `operatingSystemFullVersion()` (181) | | |
| `is64BitSystem()` (210) | | |

**Differences:**

- *pending*


## OOCache

<a id="oocache"></a>
Upstream `src/Core/OOCache.m` ↔ `Core/OOCache.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCache dealloc]` (185) | | |
| `-[OOCache description]` (193) | | |
| `-[OOCache init]` (198) | | |
| `-[OOCache initWithPList:]` (203) | | |
| `-[OOCache pListRepresentation]` (235) | | |
| `-[OOCache objectForKey:]` (242) | | |
| `-[OOCache setObject:forKey:]` (256) | | |
| `-[OOCache removeObjectForKey:]` (269) | | |
| `-[OOCache setPruneThreshold:]` (279) | | |
| `-[OOCache pruneThreshold]` (289) | | |
| `-[OOCache setAutoPrune:]` (294) | | |
| `-[OOCache autoPrune]` (303) | | |
| `-[OOCache prune]` (308) | | |
| `-[OOCache dirty]` (335) | | |
| `-[OOCache markClean]` (340) | | |
| `-[OOCache name]` (345) | | |
| `-[OOCache setName:]` (350) | | |
| `-[OOCache objectsByAge]` (355) | | |
| `-[OOCache(Private) loadFromArray:]` (364) | | |
| `CacheAllocate()` (414) | | |
| `CacheFree()` (419) | | |
| `CacheInsert()` (429) | | |
| `CacheRemove()` (444) | | |
| `CacheRemoveOldest()` (471) | | |
| `CacheRetrieve()` (481) | | |
| `CacheArrayOfContentsByAge()` (497) | | |
| `CacheArrayOfNodesByAge()` (513) | | |
| `CacheGetName()` (529) | | |
| `CacheSetName()` (534) | | |
| `CacheGetCount()` (540) | | |
| `CacheCheckIntegrity()` (547) | | |
| `CacheNodeAllocate()` (569) | | |
| `CacheNodeFree()` (586) | | |
| `CacheNodeGetValue()` (610) | | |
| `CacheNodeSetValue()` (619) | | |
| `CacheNodeGetDescription()` (631) | | |
| `TreeSplay()` (649) | | |
| `TreeInsert()` (721) | | |
| `TreeCountNodes()` (774) | | |
| `TreeCheckIntegrity()` (782) | | |
| `AgeListMakeYoungest()` (835) | | |
| `AgeListRemove()` (850) | | |
| `AgeListCheckIntegrity()` (877) | | |
| `-[OOCache(DebugGraphViz) appendNodesFromSubTree:toString:]` (959) | | |
| `-[OOCache(DebugGraphViz) generateGraphVizBodyWithRootNamed:]` (973) | | |
| `-[OOCache(DebugGraphViz) generateGraphViz]` (1008) | | |
| `-[OOCache(DebugGraphViz) writeGraphVizToURL:]` (1029) | | |
| `-[OOCache(DebugGraphViz) writeGraphVizToPath:]` (1042) | | |

**Differences:**

- *pending*


## OOCacheManager

<a id="oocachemanager"></a>
Upstream `src/Core/OOCacheManager.m` ↔ `Core/OOCacheManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCacheManager init]` (112) | | |
| `-[OOCacheManager dealloc]` (122) | | |
| `-[OOCacheManager description]` (129) | | |
| `+[OOCacheManager sharedCache]` (134) | | |
| `-[OOCacheManager objectForKey:inCache:]` (144) | | |
| `-[OOCacheManager setObject:forKey:inCache:]` (166) | | |
| `-[OOCacheManager removeObjectForKey:inCache:]` (190) | | |
| `-[OOCacheManager clearCache:]` (210) | | |
| `-[OOCacheManager clearAllCaches]` (223) | | |
| `-[OOCacheManager reloadAllCaches]` (230) | | |
| `-[OOCacheManager flush]` (236) | | |
| `-[OOCacheManager finishOngoingFlush]` (244) | | |
| `-[OOCacheManager setAllowCacheWrites:]` (251) | | |
| `-[OOCacheManager cacheDirectoryPathCreatingIfNecessary:]` (256) | | |
| `-[OOCacheManager(Private) loadCache]` (285) | | |
| `-[OOCacheManager(Private) write]` (348) | | |
| `-[OOCacheManager(Private) clear]` (417) | | |
| `-[OOCacheManager(Private) dirty]` (423) | | |
| `-[OOCacheManager(Private) markClean]` (428) | | |
| `-[OOCacheManager(Private) loadDict]` (433) | | |
| `-[OOCacheManager(Private) writeDict:]` (472) | | |
| `-[OOCacheManager(Private) buildCachesFromDictionary:]` (513) | | |
| `-[OOCacheManager(Private) dictionaryOfCaches]` (537) | | |
| `-[OOCacheManager(Private) directoryExists:create:]` (542) | | |
| `-[OOCacheManager(Private) cachePathCreatingIfNecessary:]` (567) | | |
| `-[OOCacheManager(Private) cachePathCreatingIfNecessary:]` (575) | | |
| `+[OOCacheManager(Singleton) allocWithZone:]` (594) | | |
| `-[OOCacheManager(Singleton) copyWithZone:]` (603) | | |
| `-[OOCacheManager(Singleton) retain]` (608) | | |
| `-[OOCacheManager(Singleton) retainCount]` (613) | | |
| `-[OOCacheManager(Singleton) release]` (618) | | |
| `-[OOCacheManager(Singleton) autorelease]` (622) | | |
| `-[OOAsyncCacheWriter initWithCacheContents:]` (632) | | |
| `-[OOAsyncCacheWriter dealloc]` (645) | | |
| `-[OOAsyncCacheWriter performAsyncTask]` (652) | | |
| `-[OOAsyncCacheWriter completeAsyncTask]` (662) | | |

**Differences:**

- *pending*


## OOCollectionExtractors

<a id="oocollectionextractors"></a>
Upstream `src/Core/OOCollectionExtractors.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSArray(OOExtractor) oo_charAtIndex:defaultValue:]` (41) | | |
| `-[NSArray(OOExtractor) oo_shortAtIndex:defaultValue:]` (46) | | |
| `-[NSArray(OOExtractor) oo_intAtIndex:defaultValue:]` (51) | | |
| `-[NSArray(OOExtractor) oo_longAtIndex:defaultValue:]` (56) | | |
| `-[NSArray(OOExtractor) oo_longLongAtIndex:defaultValue:]` (61) | | |
| `-[NSArray(OOExtractor) oo_integerAtIndex:defaultValue:]` (66) | | |
| `-[NSArray(OOExtractor) oo_unsignedCharAtIndex:defaultValue:]` (71) | | |
| `-[NSArray(OOExtractor) oo_unsignedShortAtIndex:defaultValue:]` (76) | | |
| `-[NSArray(OOExtractor) oo_unsignedIntAtIndex:defaultValue:]` (81) | | |
| `-[NSArray(OOExtractor) oo_unsignedLongAtIndex:defaultValue:]` (86) | | |
| `-[NSArray(OOExtractor) oo_unsignedLongLongAtIndex:defaultValue:]` (91) | | |
| `-[NSArray(OOExtractor) oo_unsignedIntegerAtIndex:defaultValue:]` (96) | | |
| `-[NSArray(OOExtractor) oo_boolAtIndex:defaultValue:]` (101) | | |
| `-[NSArray(OOExtractor) oo_fuzzyBooleanAtIndex:defaultValue:]` (107) | | |
| `-[NSArray(OOExtractor) oo_floatAtIndex:defaultValue:]` (113) | | |
| `-[NSArray(OOExtractor) oo_doubleAtIndex:defaultValue:]` (118) | | |
| `-[NSArray(OOExtractor) oo_nonNegativeFloatAtIndex:defaultValue:]` (123) | | |
| `-[NSArray(OOExtractor) oo_nonNegativeDoubleAtIndex:defaultValue:]` (128) | | |
| `-[NSArray(OOExtractor) oo_objectAtIndex:defaultValue:]` (133) | | |
| `-[NSArray(OOExtractor) oo_objectOfClass:atIndex:defaultValue:]` (146) | | |
| `-[NSArray(OOExtractor) oo_stringAtIndex:defaultValue:]` (159) | | |
| `-[NSArray(OOExtractor) oo_arrayAtIndex:defaultValue:]` (164) | | |
| `-[NSArray(OOExtractor) oo_setAtIndex:defaultValue:]` (169) | | |
| `-[NSArray(OOExtractor) oo_dictionaryAtIndex:defaultValue:]` (174) | | |
| `-[NSArray(OOExtractor) oo_dataAtIndex:defaultValue:]` (179) | | |
| `-[NSArray(OOExtractor) oo_vectorAtIndex:defaultValue:]` (185) | | |
| `-[NSArray(OOExtractor) oo_quaternionAtIndex:defaultValue:]` (190) | | |
| `-[NSArray(OOExtractor) oo_charAtIndex:]` (196) | | |
| `-[NSArray(OOExtractor) oo_shortAtIndex:]` (201) | | |
| `-[NSArray(OOExtractor) oo_intAtIndex:]` (206) | | |
| `-[NSArray(OOExtractor) oo_longAtIndex:]` (211) | | |
| `-[NSArray(OOExtractor) oo_longLongAtIndex:]` (216) | | |
| `-[NSArray(OOExtractor) oo_integerAtIndex:]` (221) | | |
| `-[NSArray(OOExtractor) oo_unsignedCharAtIndex:]` (226) | | |
| `-[NSArray(OOExtractor) oo_unsignedShortAtIndex:]` (231) | | |
| `-[NSArray(OOExtractor) oo_unsignedIntAtIndex:]` (236) | | |
| `-[NSArray(OOExtractor) oo_unsignedLongAtIndex:]` (241) | | |
| `-[NSArray(OOExtractor) oo_unsignedLongLongAtIndex:]` (246) | | |
| `-[NSArray(OOExtractor) oo_unsignedIntegerAtIndex:]` (251) | | |
| `-[NSArray(OOExtractor) oo_boolAtIndex:]` (256) | | |
| `-[NSArray(OOExtractor) oo_fuzzyBooleanAtIndex:]` (262) | | |
| `-[NSArray(OOExtractor) oo_floatAtIndex:]` (268) | | |
| `-[NSArray(OOExtractor) oo_doubleAtIndex:]` (273) | | |
| `-[NSArray(OOExtractor) oo_nonNegativeFloatAtIndex:]` (278) | | |
| `-[NSArray(OOExtractor) oo_nonNegativeDoubleAtIndex:]` (283) | | |
| `-[NSArray(OOExtractor) oo_objectAtIndex:]` (288) | | |
| `-[NSArray(OOExtractor) oo_objectOfClass:atIndex:]` (296) | | |
| `-[NSArray(OOExtractor) oo_stringAtIndex:]` (301) | | |
| `-[NSArray(OOExtractor) oo_arrayAtIndex:]` (306) | | |
| `-[NSArray(OOExtractor) oo_setAtIndex:]` (311) | | |
| `-[NSArray(OOExtractor) oo_dictionaryAtIndex:]` (316) | | |
| `-[NSArray(OOExtractor) oo_dataAtIndex:]` (321) | | |
| `-[NSArray(OOExtractor) oo_vectorAtIndex:]` (327) | | |
| `-[NSArray(OOExtractor) oo_quaternionAtIndex:]` (332) | | |
| `-[NSDictionary(OOExtractor) oo_charForKey:defaultValue:]` (342) | | |
| `-[NSDictionary(OOExtractor) oo_shortForKey:defaultValue:]` (347) | | |
| `-[NSDictionary(OOExtractor) oo_intForKey:defaultValue:]` (352) | | |
| `-[NSDictionary(OOExtractor) oo_longForKey:defaultValue:]` (357) | | |
| `-[NSDictionary(OOExtractor) oo_longLongForKey:defaultValue:]` (362) | | |
| `-[NSDictionary(OOExtractor) oo_integerForKey:defaultValue:]` (367) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedCharForKey:defaultValue:]` (372) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedShortForKey:defaultValue:]` (377) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedIntForKey:defaultValue:]` (382) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedLongForKey:defaultValue:]` (387) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedLongLongForKey:defaultValue:]` (392) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedIntegerForKey:defaultValue:]` (397) | | |
| `-[NSDictionary(OOExtractor) oo_boolForKey:defaultValue:]` (402) | | |
| `-[NSDictionary(OOExtractor) oo_fuzzyBooleanForKey:defaultValue:]` (408) | | |
| `-[NSDictionary(OOExtractor) oo_floatForKey:defaultValue:]` (414) | | |
| `-[NSDictionary(OOExtractor) oo_doubleForKey:defaultValue:]` (419) | | |
| `-[NSDictionary(OOExtractor) oo_nonNegativeFloatForKey:defaultValue:]` (424) | | |
| `-[NSDictionary(OOExtractor) oo_nonNegativeDoubleForKey:defaultValue:]` (429) | | |
| `-[NSDictionary(OOExtractor) oo_objectForKey:defaultValue:]` (434) | | |
| `-[NSDictionary(OOExtractor) oo_objectOfClass:forKey:defaultValue:]` (447) | | |
| `-[NSDictionary(OOExtractor) oo_stringForKey:defaultValue:]` (460) | | |
| `-[NSDictionary(OOExtractor) oo_arrayForKey:defaultValue:]` (465) | | |
| `-[NSDictionary(OOExtractor) oo_setForKey:defaultValue:]` (470) | | |
| `-[NSDictionary(OOExtractor) oo_dictionaryForKey:defaultValue:]` (475) | | |
| `-[NSDictionary(OOExtractor) oo_mutableDictionaryForKey:defaultValue:]` (480) | | |
| `-[NSDictionary(OOExtractor) oo_dataForKey:defaultValue:]` (485) | | |
| `-[NSDictionary(OOExtractor) oo_vectorForKey:defaultValue:]` (491) | | |
| `-[NSDictionary(OOExtractor) oo_hpvectorForKey:defaultValue:]` (496) | | |
| `-[NSDictionary(OOExtractor) oo_quaternionForKey:defaultValue:]` (501) | | |
| `-[NSDictionary(OOExtractor) oo_charForKey:]` (507) | | |
| `-[NSDictionary(OOExtractor) oo_shortForKey:]` (512) | | |
| `-[NSDictionary(OOExtractor) oo_intForKey:]` (517) | | |
| `-[NSDictionary(OOExtractor) oo_longForKey:]` (522) | | |
| `-[NSDictionary(OOExtractor) oo_longLongForKey:]` (527) | | |
| `-[NSDictionary(OOExtractor) oo_integerForKey:]` (532) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedCharForKey:]` (537) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedShortForKey:]` (542) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedIntForKey:]` (547) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedLongForKey:]` (552) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedIntegerForKey:]` (557) | | |
| `-[NSDictionary(OOExtractor) oo_unsignedLongLongForKey:]` (562) | | |
| `-[NSDictionary(OOExtractor) oo_boolForKey:]` (567) | | |
| `-[NSDictionary(OOExtractor) oo_fuzzyBooleanForKey:]` (573) | | |
| `-[NSDictionary(OOExtractor) oo_floatForKey:]` (579) | | |
| `-[NSDictionary(OOExtractor) oo_doubleForKey:]` (584) | | |
| `-[NSDictionary(OOExtractor) oo_nonNegativeFloatForKey:]` (589) | | |
| `-[NSDictionary(OOExtractor) oo_nonNegativeDoubleForKey:]` (594) | | |
| `-[NSDictionary(OOExtractor) oo_objectOfClass:forKey:]` (599) | | |
| `-[NSDictionary(OOExtractor) oo_stringForKey:]` (604) | | |
| `-[NSDictionary(OOExtractor) oo_arrayForKey:]` (609) | | |
| `-[NSDictionary(OOExtractor) oo_setForKey:]` (614) | | |
| `-[NSDictionary(OOExtractor) oo_dictionaryForKey:]` (619) | | |
| `-[NSDictionary(OOExtractor) oo_mutableDictionaryForKey:]` (624) | | |
| `-[NSDictionary(OOExtractor) oo_dataForKey:]` (629) | | |
| `-[NSDictionary(OOExtractor) oo_vectorForKey:]` (635) | | |
| `-[NSDictionary(OOExtractor) oo_hpvectorForKey:]` (640) | | |
| `-[NSDictionary(OOExtractor) oo_quaternionForKey:]` (645) | | |
| `-[NSUserDefaults(OOExtractor) oo_charForKey:defaultValue:]` (655) | | |
| `-[NSUserDefaults(OOExtractor) oo_shortForKey:defaultValue:]` (660) | | |
| `-[NSUserDefaults(OOExtractor) oo_intForKey:defaultValue:]` (665) | | |
| `-[NSUserDefaults(OOExtractor) oo_longForKey:defaultValue:]` (670) | | |
| `-[NSUserDefaults(OOExtractor) oo_longLongForKey:defaultValue:]` (675) | | |
| `-[NSUserDefaults(OOExtractor) oo_integerForKey:defaultValue:]` (680) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedCharForKey:defaultValue:]` (685) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedShortForKey:defaultValue:]` (690) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedIntForKey:defaultValue:]` (695) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedLongForKey:defaultValue:]` (700) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedLongLongForKey:defaultValue:]` (705) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedIntegerForKey:defaultValue:]` (710) | | |
| `-[NSUserDefaults(OOExtractor) oo_boolForKey:defaultValue:]` (715) | | |
| `-[NSUserDefaults(OOExtractor) oo_fuzzyBooleanForKey:defaultValue:]` (721) | | |
| `-[NSUserDefaults(OOExtractor) oo_floatForKey:defaultValue:]` (727) | | |
| `-[NSUserDefaults(OOExtractor) oo_doubleForKey:defaultValue:]` (732) | | |
| `-[NSUserDefaults(OOExtractor) oo_nonNegativeFloatForKey:defaultValue:]` (737) | | |
| `-[NSUserDefaults(OOExtractor) oo_nonNegativeDoubleForKey:defaultValue:]` (742) | | |
| `-[NSUserDefaults(OOExtractor) oo_objectForKey:defaultValue:]` (747) | | |
| `-[NSUserDefaults(OOExtractor) oo_objectOfClass:forKey:defaultValue:]` (760) | | |
| `-[NSUserDefaults(OOExtractor) oo_stringForKey:defaultValue:]` (773) | | |
| `-[NSUserDefaults(OOExtractor) oo_arrayForKey:defaultValue:]` (778) | | |
| `-[NSUserDefaults(OOExtractor) oo_setForKey:defaultValue:]` (783) | | |
| `-[NSUserDefaults(OOExtractor) oo_dictionaryForKey:defaultValue:]` (788) | | |
| `-[NSUserDefaults(OOExtractor) oo_dataForKey:defaultValue:]` (793) | | |
| `-[NSUserDefaults(OOExtractor) oo_charForKey:]` (798) | | |
| `-[NSUserDefaults(OOExtractor) oo_shortForKey:]` (803) | | |
| `-[NSUserDefaults(OOExtractor) oo_intForKey:]` (808) | | |
| `-[NSUserDefaults(OOExtractor) oo_longForKey:]` (813) | | |
| `-[NSUserDefaults(OOExtractor) oo_longLongForKey:]` (818) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedCharForKey:]` (823) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedShortForKey:]` (828) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedIntForKey:]` (833) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedLongForKey:]` (838) | | |
| `-[NSUserDefaults(OOExtractor) oo_unsignedLongLongForKey:]` (843) | | |
| `-[NSUserDefaults(OOExtractor) oo_fuzzyBooleanForKey:]` (849) | | |
| `-[NSUserDefaults(OOExtractor) oo_doubleForKey:]` (855) | | |
| `-[NSUserDefaults(OOExtractor) oo_nonNegativeFloatForKey:]` (860) | | |
| `-[NSUserDefaults(OOExtractor) oo_nonNegativeDoubleForKey:]` (865) | | |
| `-[NSUserDefaults(OOExtractor) oo_objectOfClass:forKey:]` (870) | | |
| `-[NSUserDefaults(OOExtractor) oo_setForKey:]` (875) | | |
| `-[NSMutableArray(OOInserter) oo_addInteger:]` (884) | | |
| `-[NSMutableArray(OOInserter) oo_addUnsignedInteger:]` (889) | | |
| `-[NSMutableArray(OOInserter) oo_addFloat:]` (894) | | |
| `-[NSMutableArray(OOInserter) oo_addBool:]` (899) | | |
| `-[NSMutableArray(OOInserter) oo_addVector:]` (905) | | |
| `-[NSMutableArray(OOInserter) oo_addQuaternion:]` (910) | | |
| `-[NSMutableArray(OOInserter) oo_insertInteger:atIndex:]` (916) | | |
| `-[NSMutableArray(OOInserter) oo_insertUnsignedInteger:atIndex:]` (921) | | |
| `-[NSMutableArray(OOInserter) oo_insertFloat:atIndex:]` (926) | | |
| `-[NSMutableArray(OOInserter) oo_insertBool:atIndex:]` (931) | | |
| `-[NSMutableArray(OOInserter) oo_insertVector:atIndex:]` (937) | | |
| `-[NSMutableArray(OOInserter) oo_insertQuaternion:atIndex:]` (942) | | |
| `-[NSMutableDictionary(OOInserter) oo_setInteger:forKey:]` (952) | | |
| `-[NSMutableDictionary(OOInserter) oo_setUnsignedInteger:forKey:]` (957) | | |
| `-[NSMutableDictionary(OOInserter) oo_setLongLong:forKey:]` (962) | | |
| `-[NSMutableDictionary(OOInserter) oo_setUnsignedLongLong:forKey:]` (967) | | |
| `-[NSMutableDictionary(OOInserter) oo_setFloat:forKey:]` (972) | | |
| `-[NSMutableDictionary(OOInserter) oo_setBool:forKey:]` (977) | | |
| `-[NSMutableDictionary(OOInserter) oo_setVector:forKey:]` (983) | | |
| `-[NSMutableDictionary(OOInserter) oo_setHPVector:forKey:]` (988) | | |
| `-[NSMutableDictionary(OOInserter) oo_setQuaternion:forKey:]` (993) | | |
| `-[NSMutableSet(OOInserter) oo_addInteger:]` (1003) | | |
| `-[NSMutableSet(OOInserter) oo_addUnsignedInteger:]` (1008) | | |
| `-[NSMutableSet(OOInserter) oo_addFloat:]` (1013) | | |
| `-[NSMutableSet(OOInserter) oo_addBool:]` (1018) | | |
| `-[NSMutableSet(OOInserter) oo_addVector:]` (1024) | | |
| `-[NSMutableSet(OOInserter) oo_addQuaternion:]` (1029) | | |
| `OOLongLongFromObject()` (1037) | | |
| `OOUnsignedLongLongFromObject()` (1053) | | |
| `IsSpaceOrTab()` (1071) | | |
| `IsZeroString()` (1076) | | |
| `BooleanFromString()` (1098) | | |
| `FuzzyBooleanProbabilityFromString()` (1111) | | |
| `OOBooleanFromObject()` (1124) | | |
| `OOFuzzyBooleanFromObject()` (1143) | | |
| `OOFloatFromObject()` (1167) | | |
| `OODoubleFromObject()` (1185) | | |
| `OONonNegativeFloatFromObject()` (1203) | | |
| `OONonNegativeDoubleFromObject()` (1219) | | |
| `OOVectorFromObject()` (1236) | | |
| `OOHPVectorFromObject()` (1266) | | |
| `OOQuaternionFromObject()` (1294) | | |
| `OOPropertyListFromVector()` (1325) | | |
| `OOPropertyListFromHPVector()` (1334) | | |
| `OOPropertyListFromQuaternion()` (1343) | | |
| `SetForObject()` (1354) | | |
| `StringForObject()` (1364) | | |

**Differences:**

- *pending*


## OODeepCopy

<a id="oodeepcopy"></a>
Upstream `src/Core/OODeepCopy.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OODeepCopy()` (30) | | |
| `-[NSObject(OODeepCopy) ooDeepCopyWithSharedObjects:]` (50) | | |
| `-[NSString(OODeepCopy) ooDeepCopyWithSharedObjects:]` (63) | | |
| `-[NSValue(OODeepCopy) ooDeepCopyWithSharedObjects:]` (85) | | |
| `-[NSArray(OODeepCopy) ooDeepCopyWithSharedObjects:]` (101) | | |
| `-[NSSet(OODeepCopy) ooDeepCopyWithSharedObjects:]` (152) | | |
| `-[NSDictionary(OODeepCopy) ooDeepCopyWithSharedObjects:]` (205) | | |

**Differences:**

- *pending*


## OOEncodingConverter

<a id="ooencodingconverter"></a>
Upstream `src/Core/OOEncodingConverter.m` ↔ `Core/OOEncodingConverter.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOEncodingConverter initWithEncoding:substitutions:]` (66) | | |
| `-[OOEncodingConverter initWithFontPList:]` (87) | | |
| `-[OOEncodingConverter dealloc]` (92) | | |
| `-[OOEncodingConverter descriptionComponents]` (108) | | |
| `-[OOEncodingConverter convertString:]` (113) | | |
| `-[OOEncodingConverter encoding]` (141) | | |
| `-[OOEncodingConverter(Private) performConversionForString:]` (150) | | |
| `-[OOEncodingConverter(Private) profileFire:]` (178) | | |
| `StringFromEncoding()` (206) | | |
| `EncodingFromString()` (229) | | |

**Differences:**

- *pending*


## OOExcludeObjectEnumerator

<a id="ooexcludeobjectenumerator"></a>
Upstream `src/Core/OOExcludeObjectEnumerator.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOExcludeObjectEnumerator initWithEnumerator:]` (30) | | |
| `-[OOExcludeObjectEnumerator dealloc]` (41) | | |
| `+[OOExcludeObjectEnumerator enumeratorWithEnumerator:]` (49) | | |
| `-[OOExcludeObjectEnumerator nextObject]` (60) | | |
| `-[NSEnumerator(OOExcludingObject) ooExcludingObject:]` (74) | | |

**Differences:**

- *pending*


## OOFilteringEnumerator

<a id="oofilteringenumerator"></a>
Upstream `src/Core/OOFilteringEnumerator.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOFilteringEnumerator filterEnumerator:withSelector:]` (36) | | |
| `+[OOFilteringEnumerator filterEnumerator:withSelector:andArgument:]` (48) | | |
| `-[OOFilteringEnumerator initWithUnderlyingEnumerator:]` (60) | | |
| `-[OOFilteringEnumerator dealloc]` (77) | | |
| `-[OOFilteringEnumerator descriptionComponents]` (85) | | |
| `-[OOFilteringEnumerator shortDescriptionComponents]` (95) | | |
| `-[OOFilteringEnumerator nextObject]` (100) | | |
| `-[NSEnumerator(OOFilteringEnumerator) filteredWithSelector:]` (141) | | |
| `-[NSEnumerator(OOFilteringEnumerator) filteredWithSelector:andArgument:]` (146) | | |
| `-[NSArray(OOFilteringEnumerator) objectEnumeratorFilteredWithSelector:]` (155) | | |
| `-[NSArray(OOFilteringEnumerator) objectEnumeratorFilteredWithSelector:andArgument:]` (160) | | |
| `-[NSSet(OOFilteringEnumerator) objectEnumeratorFilteredWithSelector:]` (169) | | |
| `-[NSSet(OOFilteringEnumerator) objectEnumeratorFilteredWithSelector:andArgument:]` (174) | | |
| `-[NSDictionary(OOFilteringEnumerator) objectEnumeratorFilteredWithSelector:]` (183) | | |
| `-[NSDictionary(OOFilteringEnumerator) objectEnumeratorFilteredWithSelector:andArgument:]` (188) | | |
| `-[NSDictionary(OOFilteringEnumerator) keyEnumeratorFilteredWithSelector:]` (193) | | |
| `-[NSDictionary(OOFilteringEnumerator) keyEnumeratorFilteredWithSelector:andArgument:]` (198) | | |
| `-[NSEnumerator(OOMakeObjectsPerformSelector) makeObjectsPerformSelector:]` (207) | | |
| `-[NSEnumerator(OOMakeObjectsPerformSelector) makeObjectsPerformSelector:withObject:]` (217) | | |

**Differences:**

- *pending*


## OOFoundation

<a id="oofoundation"></a>
Upstream `src/Core/OOFoundation.m` ↔ `Core/OOFoundation.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSObject(OODescriptionComponents) descriptionComponents]` (35) | | |
| `-[NSObject(OODescriptionComponents) description]` (44) | | |
| `-[NSObject(OODescriptionComponents) shortDescription]` (59) | | |
| `-[NSObject(OODescriptionComponents) shortDescriptionComponents]` (71) | | |
| `-[NSEnumerator(OOForEachSupport) objectEnumerator]` (80) | | |
| `OOConsumeReference()` (88) | | |

**Differences:**

- *pending*


## OOIsNumberLiteral

<a id="ooisnumberliteral"></a>
Upstream `src/Core/OOIsNumberLiteral.m` ↔ `Core/OOIsNumberLiteral.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOIsNumberLiteral()` (40) | | |

**Differences:**

- *pending*


## OOLogHeader

<a id="oologheader"></a>
Upstream `src/Core/OOLogHeader.m` ↔ `Core/OOLogHeader.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOPrintLogHeader()` (57) | | |
| `OOPlatformDescription()` (183) | | |
| `AdditionalLogHeaderInfo()` (202) | | |
| `GetCPUDescription()` (229) | | |
| `GetSysCtlString()` (309) | | |
| `GetSysCtlInt()` (325) | | |
| `AdditionalLogHeaderInfo()` (347) | | |

**Differences:**

- *pending*


## OOLogOutputHandler

<a id="oologoutputhandler"></a>
Upstream `src/Core/OOLogOutputHandler.m` ↔ `Core/OOLogOutputHandler.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOLogOutputHandlerInit()` (111) | | |
| `OOLogOutputHandlerClose()` (147) | | |
| `OOLogOutputHandlerStartLoggingToStdout()` (170) | | |
| `OOLogOutputHandlerStopLoggingToStdout()` (174) | | |
| `OOLogOutputHandlerPrint()` (179) | | |
| `OOLogHandlerGetLogPath()` (203) | | |
| `OOLogOutputHandlerChangeLogFile()` (208) | | |
| `-[OOAsyncLogger init]` (223) | | |
| `-[OOAsyncLogger dealloc]` (260) | | |
| `-[OOAsyncLogger startLogging]` (270) | | |
| `-[OOAsyncLogger endLogging]` (330) | | |
| `-[OOAsyncLogger changeFile]` (346) | | |
| `-[OOAsyncLogger asyncLogMessage:]` (353) | | |
| `-[OOAsyncLogger flushLog]` (377) | | |
| `-[OOAsyncLogger loggerThread]` (383) | | |
| `LoadLogCStringFunctions()` (463) | | |
| `OONSLogCStringFunction()` (481) | | |
| `OONSLogPrintfHandler()` (490) | | |
| `DirectoryExistCreatingIfNecessary()` (499) | | |
| `ExcludeFromTimeMachine()` (521) | | |
| `GetAppName()` (533) | | |
| `OOLogHandlerGetLogBasePath()` (552) | | |
| `InitCrashReporterInfo()` (606) | | |
| `SetCrashReporterInfo()` (619) | | |

**Differences:**

- *pending*


## OOLogging

<a id="oologging"></a>
Upstream `src/Core/OOLogging.m` ↔ `Core/OOLogging.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `CacheValue()` (120) | | |
| `Inited()` (128) | | |
| `OOLogWillDisplayMessagesInClass()` (136) | | |
| `OOLogSetDisplayMessagesInClass()` (172) | | |
| `OOLogGetParentMessageClass()` (194) | | |
| `OOLogIndentIf()` (210) | | |
| `OOLogOutdentIf()` (216) | | |
| `GetIndentLevel()` (238) | | |
| `SetIndentLevel()` (243) | | |
| `OOLogPushIndent()` (248) | | |
| `OOLogPopIndent()` (264) | | |
| `GetIndentLevel()` (289) | | |
| `SetIndentLevel()` (295) | | |
| `OOLogPushIndent()` (301) | | |
| `OOLogPopIndent()` (323) | | |
| `OOLogIndent()` (345) | | |
| `OOLogOutdent()` (354) | | |
| `OOLogWithPrefix()` (365) | | |
| `OOLogWithFunctionFileAndLine()` (375) | | |
| `OOLogWithFunctionFileAndLineAndArguments()` (384) | | |
| `OOLogGenericParameterErrorForFunction()` (459) | | |
| `OOLogGenericSubclassResponsibilityForFunction()` (464) | | |
| `OOLogShowFunction()` (469) | | |
| `OOLogSetShowFunction()` (474) | | |
| `OOLogShowFileAndLine()` (484) | | |
| `OOLogSetShowFileAndLine()` (489) | | |
| `OOLogShowTime()` (499) | | |
| `OOLogSetShowTime()` (504) | | |
| `OOLogShowMessageClass()` (514) | | |
| `OOLogSetShowMessageClass()` (519) | | |
| `OOLogSetShowMessageClassTemporary()` (529) | | |
| `OOLoggingInit()` (534) | | |
| `OOLoggingTerminate()` (562) | | |
| `OOLoggingReloadSettings()` (576) | | |
| `OOLogInsertMarker()` (581) | | |
| `OOLogInternal_()` (610) | | |
| `LoadExplicitSettings()` (650) | | |
| `LoadExplicitSettingsFromDictionary()` (719) | | |
| `OOLogAbbreviatedFileName()` (762) | | |
| `ResolveDisplaySetting()` (783) | | |
| `ResolveMetaClassReference()` (809) | | |

**Differences:**

- *pending*


## OOPListParsing

<a id="ooplistparsing"></a>
Upstream `src/Core/OOPListParsing.m` ↔ `Core/OOPListParsing.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOPropertyListFromData()` (48) | | |
| `OOPropertyListFromFile()` (83) | | |
| `ChangeDTDIfApplicable()` (103) | | |
| `CopyDataFromFile()` (173) | | |
| `OODictionaryFromData()` (201) | | |
| `OODictionaryFromFile()` (207) | | |
| `OOArrayFromData()` (213) | | |
| `OOArrayFromFile()` (219) | | |
| `ValueIfClass()` (226) | | |

**Differences:**

- *pending*


## OOPriorityQueue

<a id="oopriorityqueue"></a>
Upstream `src/Core/OOPriorityQueue.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `PQLeftChild()` (68) | | |
| `PQRightChild()` (73) | | |
| `PQParent()` (78) | | |
| `PQCompare()` (85) | | |
| `+[OOPriorityQueue queueWithComparator:]` (127) | | |
| `-[OOPriorityQueue initWithComparator:]` (132) | | |
| `-[OOPriorityQueue init]` (147) | | |
| `-[OOPriorityQueue dealloc]` (152) | | |
| `-[OOPriorityQueue description]` (160) | | |
| `-[OOPriorityQueue debugDescription]` (166) | | |
| `-[OOPriorityQueue isEqual:]` (184) | | |
| `-[OOPriorityQueue hash]` (215) | | |
| `-[OOPriorityQueue copyWithZone:]` (222) | | |
| `-[OOPriorityQueue addObject:]` (245) | | |
| `-[OOPriorityQueue removeObject:]` (273) | | |
| `-[OOPriorityQueue removeExactObject:]` (292) | | |
| `-[OOPriorityQueue count]` (306) | | |
| `-[OOPriorityQueue nextObject]` (311) | | |
| `-[OOPriorityQueue peekAtNextObject]` (318) | | |
| `-[OOPriorityQueue removeNextObject]` (326) | | |
| `-[OOPriorityQueue addObjects:]` (331) | | |
| `-[OOPriorityQueue sortedObjects]` (344) | | |
| `-[OOPriorityQueue objectEnumerator]` (349) | | |
| `-[OOPriorityQueue(Private) makeObjectsPerformSelector:]` (358) | | |
| `-[OOPriorityQueue(Private) bubbleUpFrom:]` (370) | | |
| `-[OOPriorityQueue(Private) bubbleDownFrom:]` (389) | | |
| `-[OOPriorityQueue(Private) growBuffer]` (417) | | |
| `-[OOPriorityQueue(Private) shrinkBuffer]` (446) | | |
| `-[OOPriorityQueue(Private) removeObjectAtIndex:]` (466) | | |
| `-[OOPriorityQueue(Private) appendDebugDataToString:index:depth:]` (490) | | |
| `-[OOPriorityQueueEnumerator initWithPriorityQueue:]` (512) | | |
| `-[OOPriorityQueueEnumerator dealloc]` (526) | | |
| `-[OOPriorityQueueEnumerator nextObject]` (533) | | |
| `EscapedString()` (549) | | |
| `-[OOPriorityQueue(DebugGraphViz) generateGraphViz]` (581) | | |
| `-[OOPriorityQueue(DebugGraphViz) writeGraphVizToURL:]` (621) | | |
| `-[OOPriorityQueue(DebugGraphViz) writeGraphVizToPath:]` (634) | | |

**Differences:**

- *pending*


## OOProfilingStopwatch

<a id="ooprofilingstopwatch"></a>
Upstream `src/Core/OOProfilingStopwatch.m` ↔ `Core/OOProfilingStopwatch.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOProfilingStopwatch init]` (35) | | |
| `+[OOProfilingStopwatch stopwatch]` (45) | | |
| `-[OOProfilingStopwatch dealloc]` (50) | | |
| `-[OOProfilingStopwatch start]` (58) | | |
| `-[OOProfilingStopwatch stop]` (66) | | |
| `-[OOProfilingStopwatch currentTime]` (74) | | |
| `-[OOProfilingStopwatch reset]` (84) | | |
| `OOHighResTimeDeltaInSeconds()` (102) | | |

**Differences:**

- *pending*


## OORegExpMatcher

<a id="ooregexpmatcher"></a>
Upstream `src/Core/OORegExpMatcher.m` ↔ `Core/OORegExpMatcher.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OORegExpMatcher regExpMatcher]` (37) | | |
| `-[OORegExpMatcher init]` (48) | | |
| `-[OORegExpMatcher dealloc]` (76) | | |
| `-[OORegExpMatcher string:matchesExpression:]` (88) | | |
| `-[OORegExpMatcher string:matchesExpression:flags:]` (93) | | |
| `-[NSString(OORegExpMatcher) oo_matchesRegularExpression:]` (135) | | |

**Differences:**

- *pending*


## OOWeakReference

<a id="ooweakreference"></a>
Upstream `src/Core/OOWeakReference.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOWeakReference weakRefWithObject:]` (24) | | |
| `-[OOWeakReference dealloc]` (35) | | |
| `-[OOWeakReference description]` (42) | | |
| `-[OOWeakReference weakRefUnderlyingObject]` (50) | | |
| `-[OOWeakReference weakRetain]` (55) | | |
| `-[OOWeakReference weakRefDrop]` (60) | | |
| `-[OOWeakReference class]` (67) | | |
| `-[OOWeakReference isProxy]` (72) | | |
| `-[OOWeakReference forwardInvocation:]` (77) | | |
| `-[OOWeakReference methodSignatureForSelector:]` (83) | | |
| `-[OOWeakReference respondsToSelector:]` (102) | | |
| `-[OOWeakReference forwardingTargetForSelector:]` (115) | | |
| `-[NSObject(OOWeakReference) weakRefUnderlyingObject]` (124) | | |
| `-[OOWeakRefObject weakRetain]` (133) | | |
| `-[OOWeakRefObject weakRefDied:]` (140) | | |
| `-[OOWeakRefObject dealloc]` (146) | | |
| `-[OOWeakRefObject weakSelf]` (152) | | |
| `+[OOWeakReferenceTemplates weakRefDrop]` (162) | | |
| `+[OOWeakReferenceTemplates weakRefUnderlyingObject]` (165) | | |
| `+[OOWeakReferenceTemplates nilMethod]` (169) | | |

**Differences:**

- *pending*


## OOWeakSet

<a id="ooweakset"></a>
Upstream `src/Core/OOWeakSet.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOWeakSet init]` (31) | | |
| `-[OOWeakSet initWithCapacity:]` (36) | | |
| `+[OOWeakSet set]` (48) | | |
| `+[OOWeakSet setWithCapacity:]` (53) | | |
| `-[OOWeakSet dealloc]` (58) | | |
| `-[OOWeakSet description]` (65) | | |
| `-[OOWeakSet copyWithZone:]` (92) | | |
| `-[OOWeakSet mutableCopyWithZone:]` (100) | | |
| `-[OOWeakSet isEqual:]` (105) | | |
| `-[OOWeakSet count]` (129) | | |
| `-[OOWeakSet containsObject:]` (135) | | |
| `-[OOWeakSet objectEnumerator]` (144) | | |
| `-[OOWeakSet addObject:]` (149) | | |
| `-[OOWeakSet removeObject:]` (160) | | |
| `-[OOWeakSet addObjectsByEnumerating:]` (167) | | |
| `-[OOWeakSet makeObjectsPerformSelector:]` (176) | | |
| `-[OOWeakSet makeObjectsPerformSelector:withObject:]` (184) | | |
| `-[OOWeakSet allObjects]` (192) | | |
| `-[OOWeakSet removeAllObjects]` (209) | | |
| `-[OOWeakSet compact]` (214) | | |
| `-[OOWeakRefUnpackingEnumerator initWithEnumerator:]` (242) | | |
| `+[OOWeakRefUnpackingEnumerator enumeratorWithCollection:]` (256) | | |
| `-[OOWeakRefUnpackingEnumerator dealloc]` (261) | | |
| `-[OOWeakRefUnpackingEnumerator nextObject]` (268) | | |

**Differences:**

- *pending*


## OOXMLExtensions

<a id="ooxmlextensions"></a>
Upstream `src/Core/OOXMLExtensions.m` ↔ `Core/OOXMLExtensions.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSDictionary(OOXMLExtensions) writeOOXMLToFile:atomically:errorDescription:]` (29) | | |

**Differences:**

- *pending*


## OldSchoolPropertyListWriting

<a id="oldschoolpropertylistwriting"></a>
Upstream `src/Core/OldSchoolPropertyListWriting.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSString(OldSchoolPropertyListWriting) oldSchoolPListFormatWithIndentation:errorDescription:]` (30) | | |
| `-[NSNumber(OldSchoolPropertyListWriting) oldSchoolPListFormatWithIndentation:errorDescription:]` (89) | | |
| `-[NSData(OldSchoolPropertyListWriting) oldSchoolPListFormatWithIndentation:errorDescription:]` (121) | | |
| `-[NSArray(OldSchoolPropertyListWriting) oldSchoolPListFormatWithIndentation:errorDescription:]` (169) | | |
| `-[NSDictionary(OldSchoolPropertyListWriting) oldSchoolPListFormatWithIndentation:errorDescription:]` (211) | | |
| `-[NSObject(OldSchoolPropertyListWriting) oldSchoolPListFormatWithErrorDescription:]` (278) | | |
| `-[NSObject(OldSchoolPropertyListWriting) oldSchoolPListFormatWithIndentation:errorDescription:]` (286) | | |
| `AppendNewLineAndIndent()` (296) | | |

**Differences:**

- *pending*


## ReleaseLockProxy

<a id="releaselockproxy"></a>
Upstream `src/Core/ReleaseLockProxy.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[ReleaseLockProxy proxyWithObject:name:]` (32) | | |
| `+[ReleaseLockProxy proxyWithRetainedObject:name:]` (37) | | |
| `-[ReleaseLockProxy initWithObject:name:]` (42) | | |
| `-[ReleaseLockProxy initWithRetainedObject:name:]` (47) | | |
| `-[ReleaseLockProxy dealloc]` (64) | | |
| `-[ReleaseLockProxy rlpAllowRelease]` (78) | | |
| `-[ReleaseLockProxy rlpObjectDescription]` (83) | | |
| `-[ReleaseLockProxy release]` (90) | | |
| `-[ReleaseLockProxy autorelease]` (109) | | |
| `-[ReleaseLockProxy forwardInvocation:]` (117) | | |
| `-[ReleaseLockProxy methodSignatureForSelector:]` (122) | | |
| `+[ReleaseLockProxy instancesRespondToSelector:]` (135) | | |
| `-[ReleaseLockProxy respondsToSelector:]` (144) | | |
| `+[ReleaseLockProxy_SignatureTemplateClass initWithObject:name:]` (153) | | |
| `+[ReleaseLockProxy_SignatureTemplateClass initWithRetainedObject:name:]` (157) | | |
| `+[ReleaseLockProxy_SignatureTemplateClass rlpAllowRelease]` (161) | | |
| `+[ReleaseLockProxy_SignatureTemplateClass rlpObjectDescription]` (164) | | |

**Differences:**

- *pending*


## Comparison

<a id="comparison"></a>
Upstream `src/SDL/Comparison.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSObject(OLComparison) isEqualTo:]` (50) | | |
| `-[NSObject(OLComparison) isGreaterThan:]` (55) | | |
| `-[NSObject(OLComparison) isGreaterThanOrEqualTo:]` (60) | | |
| `-[NSObject(OLComparison) isLessThan:]` (65) | | |
| `-[NSObject(OLComparison) isLessThanOrEqualTo:]` (70) | | |
| `-[NSObject(OLComparison) isNotEqualTo:]` (75) | | |

**Differences:**

- *pending*

