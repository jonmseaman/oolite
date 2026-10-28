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

**Status:** done — 24 rows: 15 ✅ 4 ⚠️ 0 ❌ 5 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitAsyncWorkManager()` (95) | `InitAsyncWorkManager` `Core/OOAsyncWorkManager.cpp:228` | ✅ |
| `+[OOAsyncWorkManager initialize]` (121) | `sInitLock` (std::mutex, Windows only) `Core/OOAsyncWorkManager.cpp:222` | ✅ |
| `+[OOAsyncWorkManager sharedAsyncWorkManager]` (130) | `OOAsyncWorkManager::sharedAsyncWorkManager` `Core/OOAsyncWorkManager.cpp:250` | ✅ |
| `+[OOAsyncWorkManager allocWithZone:]` (148) | — (singleton made once and leaked, `Core/OOAsyncWorkManager.cpp:223-226`) | ➖ |
| `-[OOAsyncWorkManager dealloc]` (157) | `~OOAsyncWorkManager` (abort) `Core/OOAsyncWorkManager.cpp:268` | ✅ |
| `-[OOAsyncWorkManager release]` (163) | — (immortal singleton) | ➖ |
| `-[OOAsyncWorkManager retain]` (167) | — (immortal singleton) | ➖ |
| `-[OOAsyncWorkManager retainCount]` (172) | — (immortal singleton) | ➖ |
| `-[OOAsyncWorkManager addTask:priority:]` (177) | `OOAsyncWorkManager::addTask` `Core/OOAsyncWorkManager.cpp:273` | ✅ |
| `-[OOAsyncWorkManager completePendingTasks]` (183) | `OOAsyncWorkManager::completePendingTasks` `Core/OOAsyncWorkManager.cpp:279` | ✅ |
| `-[OOAsyncWorkManager waitForTaskToComplete:]` (188) | `OOAsyncWorkManager::waitForTaskToComplete` `Core/OOAsyncWorkManager.cpp:284` | ✅ |
| `-[OOAsyncWorkManagerInternal init]` (198) | `OOAsyncWorkManagerInternal()` `Core/OOAsyncWorkManager.cpp:292` | ✅ |
| `-[OOAsyncWorkManagerInternal completePendingTasks]` (220) | `OOAsyncWorkManagerInternal::completePendingTasks` `Core/OOAsyncWorkManager.cpp:297` | ✅ |
| `-[OOAsyncWorkManagerInternal waitForTaskToComplete:]` (236) | `OOAsyncWorkManagerInternal::waitForTaskToComplete` `Core/OOAsyncWorkManager.cpp:313` | ✅ |
| `-[OOAsyncWorkManagerInternal queueResult:]` (268) | `OOAsyncWorkManagerInternal::queueResult` (`hasCompleteAsyncTask()` for `respondsToSelector:`) `Core/OOAsyncWorkManager.cpp:345` | ✅ |
| `-[OOAsyncWorkManagerInternal noteTaskQueued:]` (275) | `OOAsyncWorkManagerInternal::noteTaskQueued` (identity set) `Core/OOAsyncWorkManager.cpp:352` | ✅ |
| `-[OOManualDispatchAsyncWorkManager init]` (293) | `OOManualDispatchAsyncWorkManager()` + `WorkThreadCount` `:172` + `StartWorkThreads` `:187` `Core/OOAsyncWorkManager.cpp:362` | ✅ |
| `-[OOManualDispatchAsyncWorkManager addTask:priority:]` (318) | `OOManualDispatchAsyncWorkManager::addTask` `Core/OOAsyncWorkManager.cpp:370` | ✅ |
| `-[OOManualDispatchAsyncWorkManager queueTask:]` (329) | `OOManualDispatchAsyncWorkManager::queueTask` + `SetUpWorkThread` `:199` `Core/OOAsyncWorkManager.cpp:381` | ⚠️ |
| `+[OOOperationQueueAsyncWorkManager canBeUsed]` (363) | `OOOperationQueueAsyncWorkManager::canBeUsed` `Core/OOAsyncWorkManager.cpp:409` | ⚠️ |
| `-[OOOperationQueueAsyncWorkManager init]` (371) | `OOOperationQueueAsyncWorkManager()` `Core/OOAsyncWorkManager.cpp:414` | ⚠️ |
| `-[OOOperationQueueAsyncWorkManager dealloc]` (385) | — (immortal singleton; queue never freed) | ➖ |
| `-[OOOperationQueueAsyncWorkManager addTask:priority:]` (392) | `OOOperationQueueAsyncWorkManager::addTask` `Core/OOAsyncWorkManager.cpp:420` | ✅ |
| `-[OOOperationQueueAsyncWorkManager dispatchTask:]` (413) | `OOOperationQueueAsyncWorkManager::dispatchTask` + `workThread` `:441` `Core/OOAsyncWorkManager.cpp:470` | ⚠️ |

**Differences:**

- ⚠️ **low** — task exceptions: upstream `@catch (id)` swallowed everything; port catches only game exceptions (`oo::CurrentOOException()`) and rethrows the rest on a detached thread (→ `std::terminate`). Cited bead oo-rskgl.1 — upstream `Core/OOAsyncWorkManager.m:342-346,415-419` ↔ port `Core/OOAsyncWorkManager.cpp:391-396,472-477`
- ⚠️ **low** — NSOperationQueue replaced by a fixed pool of `WorkThreadCount()` = min(OOCPUCount(), 8) (8 in OO_DEBUG) threads named "OOAsyncWorkManager thread N" at priority 0.5, high→low then FIFO (bead oo-3rb.6). Concurrency and completion order of async tasks (texture loads, cache write) can differ; no log text changes — upstream `Core/OOAsyncWorkManager.m:371-411` ↔ port `Core/OOAsyncWorkManager.cpp:409-468`
- ⚠️ **low** — `canBeUsed` no longer checks that `NSInvocationOperation` exists, only `disable-operation-queue-work-manager`. On GNUstep 1.31 (OO_HAVE_NSOPERATION=0, class present) both select the operation-queue manager — upstream `Core/OOAsyncWorkManager.m:363-368` ↔ port `Core/OOAsyncWorkManager.cpp:409-412`
- ✅ subtle — the `asyncWorkManager.dispatchMethod` line prints the class name as a literal ("OOOperationQueueAsyncWorkManager"), matching `%@` of `[sSingleton class]` — upstream `Core/OOAsyncWorkManager.m:115` ↔ port `Core/OOAsyncWorkManager.cpp:247`
- ✅ subtle — the base `waitForTaskToComplete` exception text uses C++ `__PRETTY_FUNCTION__` (not "-[OOAsyncWorkManager waitForTaskToComplete:]"); reached only on a subclass-responsibility bug — upstream `Core/OOAsyncWorkManager.m:191` ↔ port `Core/OOAsyncWorkManager.cpp:287`


## OOCPUInfo

<a id="oocpuinfo"></a>
Upstream `src/Core/OOCPUInfo.m` ↔ `Core/OOCPUInfo.cpp`

**Status:** done — 7 rows: 7 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOCPUInfoInit()` (60) | `OOCPUInfoInit` `Core/OOCPUInfo.cpp:63` | ✅ |
| `OOCPUCount()` (99) | `OOCPUCount` `Core/OOCPUInfo.cpp:102` | ✅ |
| `OOCPUID()` (115) | `OOCPUID` `Core/OOCPUInfo.cpp:114` | ✅ |
| `OOCPUDescription()` (132) | `OOCPUDescription` `Core/OOCPUInfo.cpp:131` | ✅ |
| `OOSystemMemoryStatus()` (154) | `OOSystemMemoryStatus` `Core/OOCPUInfo.cpp:153` | ✅ |
| `operatingSystemFullVersion()` (181) | `operatingSystemFullVersion` `Core/OOCPUInfo.cpp:180` | ✅ |
| `is64BitSystem()` (210) | `is64BitSystem` `Core/OOCPUInfo.cpp:212` | ✅ |

**Differences:**

- ✅ subtle — `operatingSystemFullVersion`'s `%S` service-pack string is converted UTF-16→UTF-8 explicitly and printed with `%s`, as `-stringWithFormat:` did; feeds the log header's "Windows <ver> 64-bit" line — upstream `Core/OOCPUInfo.m:202-203` ↔ port `Core/OOCPUInfo.cpp:201-205`
- ✅ subtle — `OOCPUDescription` keeps the original's uninitialised `CPUBrandString` when the CPU reports fewer than 0x80000004 extended leaves (same UB both sides); the endian-test log lines keep class `cpuInfo.endianTest.failed` — upstream `Core/OOCPUInfo.m:132-151` ↔ port `Core/OOCPUInfo.cpp:131-150`


## OOCache

<a id="oocache"></a>
Upstream `src/Core/OOCache.m` ↔ `Core/OOCache.cpp`

**Status:** done — 48 rows: 44 ✅ 3 ⚠️ 0 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCache dealloc]` (185) | `~OOCache` `Core/OOCache.cpp:180` | ✅ |
| `-[OOCache description]` (193) | `OOCache::descriptionComponents` `Core/OOCache.cpp:188` | ✅ |
| `-[OOCache init]` (198) | `OOCache::cacheWithPList(oo::PList())` `Core/OOCache.cpp:193` | ✅ |
| `-[OOCache initWithPList:]` (203) | `OOCache::initWithPList` `Core/OOCache.cpp:201` | ✅ |
| `-[OOCache pListRepresentation]` (235) | `OOCache::pListRepresentation` `Core/OOCache.cpp:225` | ✅ |
| `-[OOCache objectForKey:]` (242) | `OOCache::pListForKey` `Core/OOCache.cpp:230` | ✅ |
| `-[OOCache setObject:forKey:]` (256) | `OOCache::setPList` `Core/OOCache.cpp:245` | ✅ |
| `-[OOCache removeObjectForKey:]` (269) | `OOCache::removePListForKey` `Core/OOCache.cpp:258` | ✅ |
| `-[OOCache setPruneThreshold:]` (279) | `OOCache::setPruneThreshold` `Core/OOCache.cpp:268` | ✅ |
| `-[OOCache pruneThreshold]` (289) | `OOCache::pruneThreshold` `Core/OOCache.cpp:278` | ✅ |
| `-[OOCache setAutoPrune:]` (294) | `OOCache::setAutoPrune` `Core/OOCache.cpp:283` | ✅ |
| `-[OOCache autoPrune]` (303) | `OOCache::autoPrune` `Core/OOCache.cpp:292` | ✅ |
| `-[OOCache prune]` (308) | `OOCache::prune` `Core/OOCache.cpp:297` | ✅ |
| `-[OOCache dirty]` (335) | `OOCache::dirty` `Core/OOCache.cpp:327` | ✅ |
| `-[OOCache markClean]` (340) | `OOCache::markClean` `Core/OOCache.cpp:332` | ✅ |
| `-[OOCache name]` (345) | `OOCache::name` `Core/OOCache.cpp:337` | ✅ |
| `-[OOCache setName:]` (350) | `OOCache::setName` `Core/OOCache.cpp:342` | ✅ |
| `-[OOCache objectsByAge]` (355) | `OOCache::pListsByAge` `Core/OOCache.cpp:347` | ⚠️ |
| `-[OOCache(Private) loadFromArray:]` (364) | `OOCache::loadFromArray` `Core/OOCache.cpp:354` | ✅ |
| `CacheAllocate()` (414) | `CacheAllocate` `Core/OOCache.cpp:399` | ✅ |
| `CacheFree()` (419) | `CacheFree` `Core/OOCache.cpp:404` | ✅ |
| `CacheInsert()` (429) | `CacheInsert` `Core/OOCache.cpp:413` | ✅ |
| `CacheRemove()` (444) | `CacheRemove` `Core/OOCache.cpp:428` | ✅ |
| `CacheRemoveOldest()` (471) | `CacheRemoveOldest` `Core/OOCache.cpp:456` | ✅ |
| `CacheRetrieve()` (481) | `CacheRetrieve` `Core/OOCache.cpp:468` | ✅ |
| `CacheArrayOfContentsByAge()` (497) | `CacheArrayOfContentsByAge` `Core/OOCache.cpp:487` | ⚠️ |
| `CacheArrayOfNodesByAge()` (513) | `CacheArrayOfNodesByAge` `Core/OOCache.cpp:504` | ✅ |
| `CacheGetName()` (529) | `CacheGetName` `Core/OOCache.cpp:523` | ✅ |
| `CacheSetName()` (534) | `CacheSetName` `Core/OOCache.cpp:528` | ✅ |
| `CacheGetCount()` (540) | `CacheGetCount` `Core/OOCache.cpp:535` | ✅ |
| `CacheCheckIntegrity()` (547) | `CacheCheckIntegrity` (compiled out) `Core/OOCache.cpp:542` | ✅ |
| `CacheNodeAllocate()` (569) | `CacheNodeAllocate` `Core/OOCache.cpp:564` | ✅ |
| `CacheNodeFree()` (586) | `CacheNodeFree` `Core/OOCache.cpp:574` | ✅ |
| `CacheNodeGetValue()` (610) | `CacheNodeGetValue` `Core/OOCache.cpp:588` | ✅ |
| `CacheNodeSetValue()` (619) | `CacheNodeSetValue` `Core/OOCache.cpp:597` | ✅ |
| `CacheNodeGetDescription()` (631) | `CacheNodeGetDescription` (compiled out) `Core/OOCache.cpp:607` | ✅ |
| `TreeSplay()` (649) | `TreeSplay` `Core/OOCache.cpp:625` | ⚠️ |
| `TreeInsert()` (721) | `TreeInsert` `Core/OOCache.cpp:695` | ✅ |
| `TreeCountNodes()` (774) | `TreeCountNodes` (compiled out) `Core/OOCache.cpp:748` | ✅ |
| `TreeCheckIntegrity()` (782) | `TreeCheckIntegrity` (compiled out) `Core/OOCache.cpp:756` | ✅ |
| `AgeListMakeYoungest()` (835) | `AgeListMakeYoungest` `Core/OOCache.cpp:804` | ✅ |
| `AgeListRemove()` (850) | `AgeListRemove` `Core/OOCache.cpp:819` | ✅ |
| `AgeListCheckIntegrity()` (877) | `AgeListCheckIntegrity` (compiled out) `Core/OOCache.cpp:846` | ✅ |
| `-[OOCache(DebugGraphViz) appendNodesFromSubTree:toString:]` (959) | `OOCache::appendNodesFromSubTree` `Core/OOCache.cpp:922` | ✅ |
| `-[OOCache(DebugGraphViz) generateGraphVizBodyWithRootNamed:]` (973) | `OOCache::generateGraphVizBodyWithRootNamed` `Core/OOCache.cpp:936` | ✅ |
| `-[OOCache(DebugGraphViz) generateGraphViz]` (1008) | `OOCache::generateGraphViz` `Core/OOCache.cpp:969` | ✅ |
| `-[OOCache(DebugGraphViz) writeGraphVizToURL:]` (1029) | folded into `writeGraphVizToPath` (bead oo-3rb.264) `Core/OOCache.cpp:988` | ➖ |
| `-[OOCache(DebugGraphViz) writeGraphVizToPath:]` (1042) | `OOCache::writeGraphVizToPath` `Core/OOCache.cpp:989` | ✅ |

**Differences:**

- ⚠️ **low** — keys are `std::string` ordered by `std::string::compare` (UTF-8 bytes) instead of `-[NSString compare:]` (UTF-16 units, non-literal). Only the splay-tree shape changes for keys mixing supplementary and U+E000–U+FFFF characters; lookups, age order and `pListRepresentation` (age order) are unaffected. Documented `Core/OOCache.cpp:139` — upstream `Core/OOCache.m:669-691` ↔ port `Core/OOCache.cpp:643-665`
- ⚠️ **low** — `objectsByAge` of an empty cache returns an empty vector, not nil (documented `Core/OOCache.cpp:486`) — upstream `Core/OOCache.m:502-503` ↔ port `Core/OOCache.cpp:492-493`
- ✅ subtle — prune logging unchanged: class `dataCache.prune.<name>` (nil name → "(null)"), "Pruning cache \"%@\" - removing %u entries", per-entry "Pruning cache \"%@\": removing %@", indented under `indentIf`/`outdentIf` — upstream `Core/OOCache.m:325-332,477` ↔ port `Core/OOCache.cpp:317-324,462`
- ✅ subtle — loading from a plist leaves the cache dirty and unpruned (prune threshold 0 / autoPrune off until after `loadFromArray`) in both — upstream `Core/OOCache.m:216-225` ↔ port `Core/OOCache.cpp:211-220`, `Core/OOCache.h:109-112`
- ✅ subtle — `pListForKey` returns a copy of the stored `oo::PList` where `-objectForKey:` returned the same retained object; values are immutable plist data, so no observable effect — upstream `Core/OOCache.m:253` ↔ port `Core/OOCache.cpp:236-237`
- ✅ subtle — the debug-only `node->key is NULL` check in `TreeSplay` and the nil-key check in `TreeCheckIntegrity` are gone (a `std::string` key cannot be nil) — upstream `Core/OOCache.m:665-667,790-793` ↔ port `Core/OOCache.cpp:638-642,764`


## OOCacheManager

<a id="oocachemanager"></a>
Upstream `src/Core/OOCacheManager.m` ↔ `Core/OOCacheManager.cpp`

**Status:** done — 36 rows: 28 ✅ 2 ⚠️ 0 ❌ 6 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCacheManager init]` (112) | `OOCacheManager::init` `Core/OOCacheManager.cpp:107` | ✅ |
| `-[OOCacheManager dealloc]` (122) | `~OOCacheManager` `Core/OOCacheManager.cpp:113` | ✅ |
| `-[OOCacheManager description]` (129) | `OOCacheManager::descriptionComponents` `Core/OOCacheManager.cpp:120` | ✅ |
| `+[OOCacheManager sharedCache]` (134) | `OOCacheManager::sharedCache` `Core/OOCacheManager.cpp:125` | ✅ |
| `-[OOCacheManager objectForKey:inCache:]` (144) | `OOCacheManager::pListForKey` `Core/OOCacheManager.cpp:137` | ✅ |
| `-[OOCacheManager setObject:forKey:inCache:]` (166) | `OOCacheManager::setPList` `Core/OOCacheManager.cpp:164` | ✅ |
| `-[OOCacheManager removeObjectForKey:inCache:]` (190) | `OOCacheManager::removeObjectForKey` `Core/OOCacheManager.cpp:179` | ✅ |
| `-[OOCacheManager clearCache:]` (210) | `OOCacheManager::clearCache` `Core/OOCacheManager.cpp:200` | ✅ |
| `-[OOCacheManager clearAllCaches]` (223) | `OOCacheManager::clearAllCaches` `Core/OOCacheManager.cpp:215` | ✅ |
| `-[OOCacheManager reloadAllCaches]` (230) | `OOCacheManager::reloadAllCaches` `Core/OOCacheManager.cpp:222` | ✅ |
| `-[OOCacheManager flush]` (236) | `OOCacheManager::flush` `Core/OOCacheManager.cpp:228` | ✅ |
| `-[OOCacheManager finishOngoingFlush]` (244) | `OOCacheManager::finishOngoingFlush` `Core/OOCacheManager.cpp:236` | ✅ |
| `-[OOCacheManager setAllowCacheWrites:]` (251) | `OOCacheManager::setAllowCacheWrites` `Core/OOCacheManager.cpp:243` | ✅ |
| `-[OOCacheManager cacheDirectoryPathCreatingIfNecessary:]` (256) | `OOCacheManager::cacheDirectoryPathCreatingIfNecessary` (`ResourcePaths::cachesDirectory()`, `oofnd/ResourcePaths.hpp:188`) `Core/OOCacheManager.cpp:248` | ✅ |
| `-[OOCacheManager(Private) loadCache]` (285) | `OOCacheManager::loadCache` (+ `OoliteInfoString` `:91`) `Core/OOCacheManager.cpp:273` | ✅ |
| `-[OOCacheManager(Private) write]` (348) | `OOCacheManager::write` `Core/OOCacheManager.cpp:334` | ✅ |
| `-[OOCacheManager(Private) clear]` (417) | `OOCacheManager::clear` `Core/OOCacheManager.cpp:394` | ✅ |
| `-[OOCacheManager(Private) dirty]` (423) | `OOCacheManager::dirty` `Core/OOCacheManager.cpp:399` | ✅ |
| `-[OOCacheManager(Private) markClean]` (428) | `OOCacheManager::markClean` `Core/OOCacheManager.cpp:404` | ✅ |
| `-[OOCacheManager(Private) loadDict]` (433) | `OOCacheManager::loadDict` `Core/OOCacheManager.cpp:409` | ⚠️ |
| `-[OOCacheManager(Private) writeDict:]` (472) | `OOCacheManager::writeDict` `Core/OOCacheManager.cpp:430` | ⚠️ |
| `-[OOCacheManager(Private) buildCachesFromDictionary:]` (513) | `OOCacheManager::buildCachesFromDictionary` `Core/OOCacheManager.cpp:464` | ✅ |
| `-[OOCacheManager(Private) dictionaryOfCaches]` (537) | `OOCacheManager::dictionaryOfCaches` `Core/OOCacheManager.cpp:485` | ✅ |
| `-[OOCacheManager(Private) directoryExists:create:]` (542) | `OOCacheManager::directoryExists` `Core/OOCacheManager.cpp:497` | ✅ |
| `-[OOCacheManager(Private) cachePathCreatingIfNecessary:]` (567) | `OOCacheManager::cachePathCreatingIfNecessary` (Mac) `Core/OOCacheManager.cpp:521` | ✅ |
| `-[OOCacheManager(Private) cachePathCreatingIfNecessary:]` (575) | `OOCacheManager::cachePathCreatingIfNecessary` `Core/OOCacheManager.cpp:531` | ✅ |
| `+[OOCacheManager(Singleton) allocWithZone:]` (594) | — (singleton leaked once, `Core/OOCacheManager.cpp:129-131`) | ➖ |
| `-[OOCacheManager(Singleton) copyWithZone:]` (603) | — | ➖ |
| `-[OOCacheManager(Singleton) retain]` (608) | — | ➖ |
| `-[OOCacheManager(Singleton) retainCount]` (613) | — | ➖ |
| `-[OOCacheManager(Singleton) release]` (618) | — | ➖ |
| `-[OOCacheManager(Singleton) autorelease]` (622) | — | ➖ |
| `-[OOAsyncCacheWriter initWithCacheContents:]` (632) | `OOAsyncCacheWriter::writerWithCacheContents` `Core/OOCacheManager.cpp:541` | ✅ |
| `-[OOAsyncCacheWriter dealloc]` (645) | implicit `~OOAsyncCacheWriter` `Core/OOCacheManager.h:106` | ✅ |
| `-[OOAsyncCacheWriter performAsyncTask]` (652) | `OOAsyncCacheWriter::performAsyncTask` `Core/OOCacheManager.cpp:548` | ✅ |
| `-[OOAsyncCacheWriter completeAsyncTask]` (662) | `OOAsyncCacheWriter::completeAsyncTask` (+ `hasCompleteAsyncTask` → true, `Core/OOCacheManager.h:112`) `Core/OOCacheManager.cpp:558` | ✅ |

**Differences:**

- ⚠️ **low** — the cache file is written as XML (`oo::writeXMLPList`) instead of GNUstep binary; an existing GNUstep-binary `Oolite-cache.plist` is unreadable, so the first run logs `dataCache.badData` "Could not read data cache: binary property lists are not supported by oofnd" and rebuilds, where upstream logged `dataCache.found` "Found data cache." (ADR-0027 item 3, comment `Core/OOCacheManager.cpp:43-45`) — upstream `Core/OOCacheManager.m:449-464,486` ↔ port `Core/OOCacheManager.cpp:419-423,440`
- ⚠️ **low** — `dataCache.badData` / `dataCache.write.serialize.failed` carry oofnd's parser/writer error text instead of GNUstep's `errorDescription` — upstream `Core/OOCacheManager.m:460,491` ↔ port `Core/OOCacheManager.cpp:421,442`
- ✅ subtle — the Oolite version compared against the cache is `CFBundleVersion` read from `Info-gnustep.plist` in the built-in resources instead of `[[NSBundle mainBundle] infoDictionary]`; a missing version still mismatches and prints "(null)" — upstream `Core/OOCacheManager.m:295,306-309` ↔ port `Core/OOCacheManager.cpp:278,291-294`
- ✅ subtle — `writeDict` failing at `cachePathCreatingIfNecessary` returns before clearing `_scheduledWrite` in both, so later flushes are skipped for the session (faithful) — upstream `Core/OOCacheManager.m:478-480,508` ↔ port `Core/OOCacheManager.cpp:432-434,459`
- ✅ subtle — the `dataCache.retrieve/set/remove/clear.*` lines are OO_DEBUG-only in both (`OODebugLog`) — upstream `Core/OOCacheManager.m:155-217` ↔ port `Core/OOCacheManager.cpp:147-211`


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

**Status:** done — 10 rows: 8 ✅ 2 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOEncodingConverter initWithEncoding:substitutions:]` (66) | `OOEncodingConverter(encoding, substitutions)` `Core/OOEncodingConverter.cpp:74` | ⚠️ |
| `-[OOEncodingConverter initWithFontPList:]` (87) | `OOEncodingConverter(fontPList)` + `FontSubstitutions` `:66` `Core/OOEncodingConverter.cpp:101` | ✅ |
| `-[OOEncodingConverter dealloc]` (92) | `~OOEncodingConverter` `Core/OOEncodingConverter.cpp:106` | ✅ |
| `-[OOEncodingConverter descriptionComponents]` (108) | `OOEncodingConverter::descriptionComponents` `Core/OOEncodingConverter.cpp:118` | ✅ |
| `-[OOEncodingConverter convertString:]` (113) | `OOEncodingConverter::convertString` `Core/OOEncodingConverter.cpp:124` | ✅ |
| `-[OOEncodingConverter encoding]` (141) | `OOEncodingConverter::encoding` (nullopt = NSNotFound) `Core/OOEncodingConverter.cpp:167` | ✅ |
| `-[OOEncodingConverter(Private) performConversionForString:]` (150) | `OOEncodingConverter::performConversionForString` → `oo::str::convertForFont` (`oofnd/Encoding.hpp:2086`) `Core/OOEncodingConverter.cpp:172` | ✅ |
| `-[OOEncodingConverter(Private) profileFire:]` (178) | `OOEncodingConverter::profileFire` (compiled out) `Core/OOEncodingConverter.cpp:189` | ⚠️ |
| `StringFromEncoding()` (206) | `StringFromEncoding` → `oo::str::encodingName` (`oofnd/Encoding.hpp:90`) `Core/OOEncodingConverter.cpp:211` | ✅ |
| `EncodingFromString()` (229) | `oo::str::encodingFromName` `oofnd/Encoding.hpp:108` | ✅ |

**Differences:**

- ⚠️ **low** — substitutions are applied in the dictionary's key (byte) order instead of NSDictionary hash order; results differ only when one key occurs in another's key or replacement (not the case for the stock `oolite-font.plist`). A non-string replacement substitutes "" where upstream sent `-length` to a non-string (bead oo-qps.51, comment `:78-84`) — upstream `Core/OOEncodingConverter.m:158-163` ↔ port `Core/OOEncodingConverter.cpp:77-87`
- ⚠️ **low** — PROFILE_ENCODING_CONVERTER's repeating NSTimer became a deadline checked on each conversion (proposed ADR-0033); compiled out (`PROFILE_ENCODING_CONVERTER 0`) — upstream `Core/OOEncodingConverter.m:78-81` ↔ port `Core/OOEncodingConverter.cpp:94-98,148-158`
- ✅ subtle — an unknown encoding (NSNotFound) gave a nil conversion that was returned and not cached; the port returns empty `oo::Data` and caches nothing — same zero-length result for the HUD; `descriptionComponents` prints 4294967295 as `%u` of NSNotFound did — upstream `Core/OOEncodingConverter.m:110,127-130,166` ↔ port `Core/OOEncodingConverter.cpp:121,130-136,175-176`
- ✅ subtle — encoding numeric values are kept (CP1251=11 … CP1250=15, `oofnd/Encoding.hpp:81-86`) and the lossy conversion reproduces GNUstep/iconv transliteration per the captured tables in `oofnd/Encoding.hpp` (banner :18-28); the converter's OOCache is named "Text encoding" with threshold 200, so its prune log class is still `dataCache.prune.Text encoding` — upstream `Core/OOEncodingConverter.m:70-72` ↔ port `Core/OOEncodingConverter.cpp:76-78`


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

**Status:** done — 6 rows: 0 ✅ 0 ⚠️ 0 ❌ 6 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSObject(OODescriptionComponents) descriptionComponents]` (35) | per-class `descriptionComponents()` returning `std::optional<std::string>` (e.g. `Core/OOCache.h:92`); no root default (`Core/OODescription.h:82-87`) | ➖ |
| `-[NSObject(OODescriptionComponents) description]` (44) | no central wrapper; "<Class 0x…>{…}" is assembled per call site with `oo::str::pointerDescription` (`Core/OODescription.h:17-18`); plists via `oo::DescriptionOf(const oo::PList&)` `Core/OODescription.h:90` | ➖ |
| `-[NSObject(OODescriptionComponents) shortDescription]` (59) | per-class/per-site (e.g. `Core/Entities/EntityHelpers.h:49`) | ➖ |
| `-[NSObject(OODescriptionComponents) shortDescriptionComponents]` (71) | per-class (no root default) | ➖ |
| `-[NSEnumerator(OOForEachSupport) objectEnumerator]` (80) | — (no NSEnumerator; range-for) | ➖ |
| `OOConsumeReference()` (88) | — (went with libobjc2, bead oo-dbz; `Core/OOFoundation.cpp:35`) | ➖ |

**Differences:**

- ➖ **low** — the NSObject description category is gone: upstream printed "<Class %p>{components}" or, when components was nil, "<Class %p>" (no braces) for every object; the port has no shared wrapper, so each call site must reproduce the nil→no-braces rule and the GNUstep `%p` (low 32 bits) itself. Not a defect in this file, but a drift risk for any log line that prints an object with `%@` — upstream `Core/OOFoundation.m:44-56` ↔ port `Core/OODescription.h:9-18,82-87`


## OOIsNumberLiteral

<a id="ooisnumberliteral"></a>
Upstream `src/Core/OOIsNumberLiteral.m` ↔ `Core/OOIsNumberLiteral.cpp`

**Status:** done — 1 rows: 1 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOIsNumberLiteral()` (40) | `OOIsNumberLiteral(const std::string&, BOOL)` `Core/OOIsNumberLiteral.cpp:42` | ✅ |

**Differences:**

- ✅ subtle — scans UTF-8 bytes instead of UTF-16 units; every non-ASCII unit/byte hits `default:` → NO in both, so results match. The nil check is gone (the argument is a reference); the debug FAIL variant (NSLog → `OO_LOG("unclassified")`) is compiled out in both — upstream `Core/OOIsNumberLiteral.m:27-37,52-56` ↔ port `Core/OOIsNumberLiteral.cpp:28-40,53-55`


## OOLogHeader

<a id="oologheader"></a>
Upstream `src/Core/OOLogHeader.m` ↔ `Core/OOLogHeader.cpp`

**Status:** done — 7 rows: 7 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOPrintLogHeader()` (57) | `OOPrintLogHeader` `Core/OOLogHeader.cpp:61` | ✅ |
| `OOPlatformDescription()` (183) | `OOPlatformDescription` (std::string) `Core/OOLogHeader.cpp:192` | ✅ |
| `AdditionalLogHeaderInfo()` (202) | `AdditionalLogHeaderInfo` (Mac) `Core/OOLogHeader.cpp:212` | ✅ |
| `GetCPUDescription()` (229) | `GetCPUDescription` (Mac) `Core/OOLogHeader.cpp:241` | ✅ |
| `GetSysCtlString()` (309) | `GetSysCtlString` (Mac) `Core/OOLogHeader.cpp:322` | ✅ |
| `GetSysCtlInt()` (325) | `GetSysCtlInt` (Mac) `Core/OOLogHeader.cpp:338` | ✅ |
| `AdditionalLogHeaderInfo()` (347) | `AdditionalLogHeaderInfo` `Core/OOLogHeader.cpp:361` | ✅ |

**Differences:**

- ✅ subtle — header text is literal-for-literal: "Opening log for Oolite %@ by %@ (<cpu><variant>) under %@ at %@.\n", the "<cpu> N processor(s) detected. System RAM: … MB (free: … MB)." line, "\nBuild options: %@.\n", the logcontrol note, and the whole logged once as `log.header` with format "%@\n" (trailing blank line kept) — upstream `Core/OOLogHeader.m:169-180,353` ↔ port `Core/OOLogHeader.cpp:171-187,367`
- ✅ subtle — "new planets" is now unconditional; upstream gated it on NEW_PLANETS, which `OOStellarBody.h:35` always defines to 1 — upstream `Core/OOLogHeader.m:108-110` ↔ port `Core/OOLogHeader.cpp:112`
- ✅ subtle — the date is `oo::date::description()` (NSDate `-description` "YYYY-MM-DD HH:MM:SS +hhmm", local zone, `oofnd/Date.hpp:85-139`) and the non-Windows OS string is `oo::process::operatingSystemVersionString()` (uname release, `oofnd/Process.hpp:125-135`) — upstream `Core/OOLogHeader.m:153,169` ↔ port `Core/OOLogHeader.cpp:155,171`
- ✅ subtle — the header is printed before `LoadExplicitSettings`, so it carries no time/class prefix and every class shows (Options default all-false, `_default` YES), as upstream (sShowTime/sShowClass start NO) — upstream `Core/OOLogging.m:83-87,555-557` ↔ port `oofnd/Log.hpp:78-83,448`, `Core/OOLogging.cpp:202-204`


## OOLogOutputHandler

<a id="oologoutputhandler"></a>
Upstream `src/Core/OOLogOutputHandler.m` ↔ `Core/OOLogOutputHandler.cpp`

**Status:** done — 24 rows: 17 ✅ 2 ⚠️ 4 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOLogOutputHandlerInit()` (111) | `OOLogOutputHandlerInit` `Core/OOLogOutputHandler.cpp:132` | ✅ |
| `OOLogOutputHandlerClose()` (147) | `OOLogOutputHandlerClose` `Core/OOLogOutputHandler.cpp:181` | ✅ |
| `OOLogOutputHandlerStartLoggingToStdout()` (170) | `OOLogOutputHandlerStartLoggingToStdout` `Core/OOLogOutputHandler.cpp:200` | ✅ |
| `OOLogOutputHandlerStopLoggingToStdout()` (174) | `OOLogOutputHandlerStopLoggingToStdout` `Core/OOLogOutputHandler.cpp:204` | ✅ |
| `OOLogOutputHandlerPrint()` (179) | `OOLogOutputHandlerPrintLine` (+ `oo::log::consoleBytes`, `oofnd/LogFile.hpp:111`) `Core/OOLogOutputHandler.cpp:220` | ✅ |
| `OOLogHandlerGetLogPath()` (203) | `cxx_OOLogHandlerGetLogPath` / `LogPath` `:512` `Core/OOLogOutputHandler.cpp:244` | ✅ |
| `OOLogOutputHandlerChangeLogFile()` (208) | `cxx_OOLogOutputHandlerChangeLogFile` `Core/OOLogOutputHandler.cpp:249` | ❌ |
| `-[OOAsyncLogger init]` (223) | `StartLogger` + `oo::log::rotateToPrevious` (`oofnd/LogFile.hpp:133`) `Core/OOLogOutputHandler.cpp:271` | ❌ |
| `-[OOAsyncLogger dealloc]` (260) | `FileWriter::~FileWriter` `oofnd/LogFile.hpp:157` | ✅ |
| `-[OOAsyncLogger startLogging]` (270) | `FileWriter::start` `oofnd/LogFile.hpp:161` | ✅ |
| `-[OOAsyncLogger endLogging]` (330) | `FileWriter::end` (`oofnd/LogFile.hpp:194`) + `Postamble` `Core/OOLogOutputHandler.cpp:167` | ✅ |
| `-[OOAsyncLogger changeFile]` (346) | folded into `cxx_OOLogOutputHandlerChangeLogFile` `Core/OOLogOutputHandler.cpp:253-261` | ✅ |
| `-[OOAsyncLogger asyncLogMessage:]` (353) | `AsyncLogMessage` (+ `FileWriter::write`, `logFileBytes` `oofnd/LogFile.hpp:82,180`) `Core/OOLogOutputHandler.cpp:297` | ⚠️ |
| `-[OOAsyncLogger flushLog]` (377) | `OOLogOutputHandlerFlushIfDue` (+ `FileWriter::flush` `oofnd/LogFile.hpp:191`) `Core/OOLogOutputHandler.cpp:209` | ⚠️ |
| `-[OOAsyncLogger loggerThread]` (383) | `FileWriter::run` `oofnd/LogFile.hpp:240` | ✅ |
| `LoadLogCStringFunctions()` (463) | `LoadLogCStringFunctions` (Mac) `Core/OOLogOutputHandler.cpp:339` | ✅ |
| `OONSLogCStringFunction()` (481) | `OONSLogCStringFunction` (Mac) `Core/OOLogOutputHandler.cpp:357` | ✅ |
| `OONSLogPrintfHandler()` (490) | — (no gnustep-base: nothing to capture) | ➖ |
| `DirectoryExistCreatingIfNecessary()` (499) | `DirectoryExistCreatingIfNecessary` `Core/OOLogOutputHandler.cpp:368` | ❌ |
| `ExcludeFromTimeMachine()` (521) | `ExcludeFromTimeMachine` (Mac) `Core/OOLogOutputHandler.cpp:389` | ✅ |
| `GetAppName()` (533) | `GetAppName` (Mac) `Core/OOLogOutputHandler.cpp:401` | ❌ |
| `OOLogHandlerGetLogBasePath()` (552) | `LogBasePath` / `cxx_OOLogHandlerGetLogBasePath` `:505` (+ `AppendPathComponent` `:434`) `Core/OOLogOutputHandler.cpp:458` | ✅ |
| `InitCrashReporterInfo()` (606) | `InitCrashReporterInfo` (Mac debug) `Core/OOLogOutputHandler.cpp:529` | ✅ |
| `SetCrashReporterInfo()` (619) | `SetCrashReporterInfo` (Mac debug) `Core/OOLogOutputHandler.cpp:537` | ✅ |

**Differences:**

- ⚠️ **low** — Latest.log flushing: the one-shot 2 s NSTimer on the run loop became a steady-clock deadline that only fires when the frame loop calls `OOLogOutputHandlerFlushIfDue` (proposed ADR-0033, comment `:117-122`). The file is unbuffered (`setvbuf _IONBF`, `oofnd/LogFile.hpp:168`), so a flush is only fsync/_commit: readers see every line as soon as the writer thread writes it; only durability across an OS crash depends on the call site. A first request off the main thread still disables flushing forever, as upstream did — upstream `Core/OOLogOutputHandler.m:370-381` ↔ port `Core/OOLogOutputHandler.cpp:209-218,305-311`
- ❌ **low** — log-setup failures used `NSLog` (stderr with GNUstep's "date process[pid:tid] " banner, or — once the printf handler was installed — a `gnustep`-class line in Latest.log); the port uses `OO_LOG("unclassified", …)`, so text/prefix and message class differ ("could not move or delete existing log", "could not open log", "expected … to be a folder", "could not create folder") — upstream `Core/OOLogOutputHandler.m:244,322,507,512` ↔ port `Core/OOLogOutputHandler.cpp:259,280,286,373,378`
- ❌ **low** — `ChangeLogFile` start failure: upstream logged "could not open log" only when creating the file failed (from `-startLogging`) and as class `gnustep` via the installed NSLog handler; the port logs it as `unclassified` and also when `LogPath()` is fine but `FileWriter::start` fails for another reason — upstream `Core/OOLogOutputHandler.m:316-324,349-350` ↔ port `Core/OOLogOutputHandler.cpp:255-261`
- ❌ **low** (Mac only) — `GetAppName` reads `CFBundleName` from `<built-in resources>/Info-gnustep.plist` instead of the main bundle's Info.plist, so on a Mac bundle without that file the log folder becomes `~/Library/Logs/<unknown application>` — upstream `Core/OOLogOutputHandler.m:539-544` ↔ port `Core/OOLogOutputHandler.cpp:406-420`
- ➖ **low** — GNUstep's own `NSLog` output (class `gnustep`) is no longer captured into Latest.log; with gnustep-base removed there is no such output to capture — upstream `Core/OOLogOutputHandler.m:137-141,490-495` ↔ port (none; `Core/OOLogOutputHandler.cpp:87-89`)
- ✅ subtle — file bytes match: line + "\n", every "\n" → "\r\n" on Windows (except before a combining unit, as GNUstep's non-literal split), lone-surrogate lines dropped, 1 GiB saturation notice text and process-wide flag, postamble "\nClosing log at <NSDate description>.", rotation Latest.log → Previous.log — upstream `Core/OOLogOutputHandler.m:238-248,336-338,359-368,403-416` ↔ port `oofnd/LogFile.hpp:82-141,180-276`, `Core/OOLogOutputHandler.cpp:167-170`
- ✅ subtle — `LogBasePath` retries after a failed mkdir where upstream kept an unretained autoreleased string in its static (dangling on the next call); the default paths (`$OO_LOGSDIR`, `<home>/Logs`, `<home>/.Oolite/Logs`) are unchanged — upstream `Core/OOLogOutputHandler.m:556-594` ↔ port `Core/OOLogOutputHandler.cpp:458-501`


## OOLogging

<a id="oologging"></a>
Upstream `src/Core/OOLogging.m` ↔ `Core/OOLogging.cpp`

**Status:** done — 44 rows: 40 ✅ 2 ⚠️ 2 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `CacheValue()` (120) | `Logger::Value::on()/off()` `oofnd/Log.hpp:372-373` | ✅ |
| `Inited()` (128) | `Logger::checkInitialized` `oofnd/Log.hpp:290` | ✅ |
| `OOLogWillDisplayMessagesInClass()` (136) | `Logger::willDisplay` / `oo::log::willDisplay` `:463` `oofnd/Log.hpp:313` | ✅ |
| `OOLogSetDisplayMessagesInClass()` (172) | `Logger::setDisplay` `oofnd/Log.hpp:299` | ✅ |
| `OOLogGetParentMessageClass()` (194) | `oo::log::parentClass` `oofnd/Log.hpp:124` | ✅ |
| `OOLogIndentIf()` (210) | `oo::log::indentIf` `oofnd/Log.hpp:477` | ✅ |
| `OOLogOutdentIf()` (216) | `oo::log::outdentIf` `oofnd/Log.hpp:483` | ✅ |
| `GetIndentLevel()` (238) | `oo::log::indentLevel` (thread_local) `oofnd/Log.hpp:194` | ✅ |
| `SetIndentLevel()` (243) | `IndentState::level` writes in `indent`/`outdent`/`popIndent` `oofnd/Log.hpp:195-200,472` | ✅ |
| `OOLogPushIndent()` (248) | `OOLogPushIndent` → `oo::log::pushIndent` `Core/OOLogging.cpp:88`, `oofnd/Log.hpp:202` | ✅ |
| `OOLogPopIndent()` (264) | `OOLogPopIndent` → `oo::log::popIndent` `Core/OOLogging.cpp:93`, `oofnd/Log.hpp:465` | ✅ |
| `GetIndentLevel()` (289) | (thread-dictionary variant) same as above `oofnd/Log.hpp:194` | ✅ |
| `SetIndentLevel()` (295) | (thread-dictionary variant) same as above | ✅ |
| `OOLogPushIndent()` (301) | (thread-dictionary variant) `oofnd/Log.hpp:202` | ✅ |
| `OOLogPopIndent()` (323) | (thread-dictionary variant) `oofnd/Log.hpp:465` | ✅ |
| `OOLogIndent()` (345) | `OOLogIndent` → `oo::log::indent` `Core/OOLogging.cpp:98`, `oofnd/Log.hpp:195` | ✅ |
| `OOLogOutdent()` (354) | `OOLogOutdent` → `oo::log::outdent` `Core/OOLogging.cpp:103`, `oofnd/Log.hpp:196` | ✅ |
| `OOLogWithPrefix()` (365) | `oo::log::messageWithPrefix` `oofnd/Log.hpp:499` | ✅ |
| `OOLogWithFunctionFileAndLine()` (375) | `oo::log::message` `oofnd/Log.hpp:492` | ✅ |
| `OOLogWithFunctionFileAndLineAndArguments()` (384) | `Logger::write` + `composeLine` `:164` + `timeStamp` `:147` `oofnd/Log.hpp:337` | ✅ |
| `OOLogGenericParameterErrorForFunction()` (459) | `OOLogGenericParameterErrorForFunction` `Core/OOLogging.cpp:108` | ✅ |
| `OOLogGenericSubclassResponsibilityForFunction()` (464) | `OOLogGenericSubclassResponsibilityForFunction` `Core/OOLogging.cpp:113` | ✅ |
| `OOLogShowFunction()` (469) | `OOLogShowFunction` `Core/OOLogging.cpp:118` | ✅ |
| `OOLogSetShowFunction()` (474) | `OOLogSetShowFunction` `Core/OOLogging.cpp:123` | ✅ |
| `OOLogShowFileAndLine()` (484) | `OOLogShowFileAndLine` `Core/OOLogging.cpp:135` | ✅ |
| `OOLogSetShowFileAndLine()` (489) | `OOLogSetShowFileAndLine` `Core/OOLogging.cpp:140` | ✅ |
| `OOLogShowTime()` (499) | `OOLogShowTime` `Core/OOLogging.cpp:152` | ✅ |
| `OOLogSetShowTime()` (504) | `OOLogSetShowTime` `Core/OOLogging.cpp:157` | ✅ |
| `OOLogShowMessageClass()` (514) | `OOLogShowMessageClass` `Core/OOLogging.cpp:169` | ✅ |
| `OOLogSetShowMessageClass()` (519) | `OOLogSetShowMessageClass` `Core/OOLogging.cpp:174` | ✅ |
| `OOLogSetShowMessageClassTemporary()` (529) | `OOLogSetShowMessageClassTemporary` → `Logger::setShowClassTemporarily` `Core/OOLogging.cpp:186` | ✅ |
| `OOLoggingInit()` (534) | `OOLoggingInit` (+ `LogSink` `:56`) `Core/OOLogging.cpp:191` | ✅ |
| `OOLoggingTerminate()` (562) | `OOLoggingTerminate` `Core/OOLogging.cpp:208` | ✅ |
| `OOLoggingReloadSettings()` (576) | `OOLoggingReloadSettings` `Core/OOLogging.cpp:222` | ✅ |
| `OOLogInsertMarker()` (581) | `OOLogInsertMarker` → `Logger::insertMarker` `Core/OOLogging.cpp:227`, `oofnd/Log.hpp:345` | ✅ |
| `OOLogInternal_()` (610) | `Logger::internal` `oofnd/Log.hpp:352` | ✅ |
| `LoadExplicitSettings()` (650) | `LoadExplicitSettings` + `Logger::replaceSettings` (`oofnd/Log.hpp:246`) `Core/OOLogging.cpp:259` | ❌ |
| `LoadExplicitSettingsFromDictionary()` (719) | `SettingsFromDictionary` (`Core/OOLogging.cpp:63`) + `Logger::replaceSettings` `oofnd/Log.hpp:246` | ⚠️ |
| `OOLogAbbreviatedFileName()` (762) | `oo::log::abbreviatedFileName` (uncached) `oofnd/Log.hpp:134` | ✅ |
| `ResolveDisplaySetting()` (783) | `Logger::resolve` `oofnd/Log.hpp:403` | ✅ |
| `ResolveMetaClassReference()` (809) | `Logger::resolveMeta` `oofnd/Log.hpp:423` | ⚠️ |
| *(added)* `kOOLog*` message-class constants (595-604) | `cxx_kOOLog*` `Core/OOLogging.cpp:232-241` | ✅ |
| *(added)* `OOLOG_ERROR_PREFIX` / `OOLOG_WARNING_PREFIX` (`OOLogging.h:122-123`) | `kErrorPrefix` / `kWarningPrefix` `oofnd/Log.hpp:509-510` | ✅ |
| *(added)* `OOLog()` macro / `OOLOG_FUNCTION_NAME` (`OOLogging.h:39-47,84-89`) | `OO_LOG` (`__FUNCTION__`) `oofnd/Log.hpp:516-520` | ❌ |

**Differences:**

- ❌ **low** — `logging-show-function/-file-and-line/-time/-class` are read with `Defaults::boolForKey` (NSString `-boolValue`: leading Y/y/T/t or non-zero digit) when present, instead of `oo_boolForKey:defaultValue:` (`OOBooleanFromObject`: "yes"/"true"/"on"/non-zero number → YES, "no"/"false"/"off"/zero → NO, anything else → the default). A string "on" now gives NO, and an unrecognised string or non-number gives NO instead of the default (YES for time/class), dropping the time stamp / `[class]` prefix from every line. Only hand-edited or command-line string values are affected; values written by the game are booleans — upstream `Core/OOLogging.m:653-657`, `Core/OOCollectionExtractors.m:1099-1108,1124-1140` ↔ port `Core/OOLogging.cpp:262-267`, `oofnd/Defaults.hpp:90-101,474-480`
- ❌ **low** — the function name in log lines is C++ `__FUNCTION__` (bare "prune", "init") where ObjC's was "-[OOCache prune]"; visible only with `logging-show-function` on (default NO) — upstream `Core/OOLogging.h:41,87`, `Core/OOLogging.m:407,415` ↔ port `oofnd/Log.hpp:171,175,519`
- ⚠️ **low** — a `$metaclass` cycle now falls back to `_default` instead of recursing until the stack overflowed (documented "the one deliberate difference", `oofnd/Log.hpp:421-422`) — upstream `Core/OOLogging.m:809-832` ↔ port `oofnd/Log.hpp:423-442`
- ⚠️ **low** — logcontrol dictionaries are walked in `oo::PList::Dict` key (byte) order instead of NSDictionary hash order (comment `Core/OOLogging.cpp:61-62`); this only reorders the "OOLogging internal - LoadExplicitSettingsFromDictionary: Bad setting value …" diagnostics, never the resulting settings — upstream `Core/OOLogging.m:722-755` ↔ port `Core/OOLogging.cpp:63-82`, `oofnd/Log.hpp:252-273`
- ⚠️ **low** — the built-in logcontrol.plist pre-pass no longer logs `plist.wrongType` when that file's root is not a dictionary (comment `Core/OOLogging.cpp:283`) — upstream `Core/OOLogging.m:667-670` ↔ port `Core/OOLogging.cpp:277-285`
- ✅ subtle — line layout is unchanged: prefix order function/file:line → "[class] " or "[class]: " → time "HH:MM:SS.mmm " (local, ms truncated) → indentation of 2 spaces per level capped at 64 placed before the time; internal diagnostics ("OOLogging internal - <fn>: …") and markers ("\n\n========== [Marker N] ==========") bypass prefixes and indentation; `_default` YES, `_override` sticky once set, settings/display defaults load after the header — upstream `Core/OOLogging.m:85-89,404-448,591,623` ↔ port `oofnd/Log.hpp:164-190,345-355,446-450`
- ✅ subtle — internal diagnostics are collected under the lock and printed after it is released, in the same order and with the same function names (`Inited`, `LoadExplicitSettingsFromDictionary`, `LoadExplicitSettings`, `ResolveMetaClassReference`, `OOLogPopIndent`); the SETTING_SET/RETRIEVE diagnostics stay off (flags 0) — upstream `Core/OOLogging.m:58-65,132,275,692,705,742,748,815,826` ↔ port `oofnd/Log.hpp:265-286,294,330-331,469`
- ✅ subtle — the `@catch` around formatting is gone (std::format strings are checked at compile time); before `OOLoggingInit` lines go to a stderr sink as `OOLogOutputHandlerPrint` did before its init — upstream `Core/OOLogging.m:452-454` ↔ port `oofnd/Log.hpp:28-29,215-217,377-381`


## OOPListParsing

<a id="ooplistparsing"></a>
Upstream `src/Core/OOPListParsing.m` ↔ `Core/OOPListParsing.cpp`

**Status:** done — 9 rows: 3 ✅ 1 ⚠️ 0 ❌ 5 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOPropertyListFromData()` (48) | `cxx_OOPropertyListFromData` (+ `oo::parsePropertyList`, `oofnd/PListParsing.hpp:70`) `Core/OOPListParsing.cpp:34` | ⚠️ |
| `OOPropertyListFromFile()` (83) | `cxx_OOPropertyListFromFile` `Core/OOPListParsing.cpp:61` | ✅ |
| `ChangeDTDIfApplicable()` (103) | `oo::changeDTDIfApplicable` `oofnd/PListXML.hpp:64` | ✅ |
| `CopyDataFromFile()` (173) | `OODataFromOXZFile` (`oofnd/FileSystem.hpp:24-25`) called at `Core/OOPListParsing.cpp:66` | ✅ |
| `OODictionaryFromData()` (201) | — (callers test `isDict()` themselves) | ➖ |
| `OODictionaryFromFile()` (207) | — (e.g. `Core/OOLogging.cpp:281-283`, `Core/ResourceManager.cpp:58-59`) | ➖ |
| `OOArrayFromData()` (213) | — | ➖ |
| `OOArrayFromFile()` (219) | — (e.g. `Core/Scripting/OOScript.cpp:46`) | ➖ |
| `ValueIfClass()` (226) | — (`plist.wrongType` line dropped) | ➖ |

**Differences:**

- ⚠️ **low** — binary property lists (Apple `bplist00`, GNUstep binary) now always fail with "binary property lists are not supported by oofnd", logged as `plist.parse.failed` "Failed to parse <path> as a property list.\n<error>" (ADR-0027 item 3, `oofnd/PListParsing.hpp:7`); for XML/old-style input the `<error>` text is oofnd's `PListError::description()`, so any golden that captures a parse failure depends on oofnd matching GNUstep's error wording — upstream `Core/OOPListParsing.m:66-79` ↔ port `Core/OOPListParsing.cpp:44-55`
- ➖ **low** — `plist.wrongType` ("Property list is wrong type - expected NSDictionary, got …") is no longer logged anywhere; every former `OODictionaryFromFile`/`OOArrayFromFile` caller silently treats a wrong-typed root as missing (comments at `Core/OOLogging.cpp:283`, `Core/ResourceManager.cpp:58-59`, `Core/Scripting/OOJSScript.cpp:663`) — upstream `Core/OOPListParsing.m:226-232` ↔ port (none)
- ✅ subtle — `plist.information` "Parsing %@ as a property list." still only when `UNIVERSE` exists; "<data in memory>" default and "<no error message>" for a nil-without-error result kept; a missing file is still silent — upstream `Core/OOPListParsing.m:53-57,73-76,94` ↔ port `Core/OOPListParsing.cpp:38-41,47-53,71`


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

**Status:** done — 8 rows: 7 ✅ 1 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOProfilingStopwatch init]` (35) | `OOProfilingStopwatch()` `Core/OOProfilingStopwatch.cpp:33` | ✅ |
| `+[OOProfilingStopwatch stopwatch]` (45) | `OOProfilingStopwatch::stopwatch` `Core/OOProfilingStopwatch.cpp:40` | ✅ |
| `-[OOProfilingStopwatch dealloc]` (50) | `~OOProfilingStopwatch` `Core/OOProfilingStopwatch.cpp:45` | ✅ |
| `-[OOProfilingStopwatch start]` (58) | `OOProfilingStopwatch::start` `Core/OOProfilingStopwatch.cpp:51` | ✅ |
| `-[OOProfilingStopwatch stop]` (66) | `OOProfilingStopwatch::stop` `Core/OOProfilingStopwatch.cpp:59` | ✅ |
| `-[OOProfilingStopwatch currentTime]` (74) | `OOProfilingStopwatch::currentTime` `Core/OOProfilingStopwatch.cpp:67` | ✅ |
| `-[OOProfilingStopwatch reset]` (84) | `OOProfilingStopwatch::reset` `Core/OOProfilingStopwatch.cpp:77` | ✅ |
| `OOHighResTimeDeltaInSeconds()` (102) | `OOHighResTimeDeltaInSeconds` `Core/OOProfilingStopwatch.cpp:93` | ⚠️ |

**Differences:**

- ⚠️ **low** — where upstream used SpiderMonkey's `JS_Now()` (wall-clock µs) the port uses `std::chrono::steady_clock` µs (header comment, following the JS engine swap); deltas have the same unit and differ only if the wall clock is adjusted mid-measurement. The mach / standalone-Windows `timeGetTime` / gettimeofday paths are unchanged — upstream `Core/OOProfilingStopwatch.h:81-121`, `Core/OOProfilingStopwatch.m:120-121` ↔ port `Core/OOProfilingStopwatch.h:98-142`, `Core/OOProfilingStopwatch.cpp:115-116`
- ✅ subtle — `stop` still disposes `_start` rather than `_end` (upstream quirk kept; values are plain integers so harmless) — upstream `Core/OOProfilingStopwatch.m:66-72` ↔ port `Core/OOProfilingStopwatch.cpp:59-65`


## OORegExpMatcher

<a id="ooregexpmatcher"></a>
Upstream `src/Core/OORegExpMatcher.m` ↔ `Core/OORegExpMatcher.cpp`

**Status:** done — 6 rows: 5 ✅ 0 ⚠️ 0 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OORegExpMatcher regExpMatcher]` (37) | `OORegExpMatcher::regExpMatcher` `Core/OORegExpMatcher.cpp:58` | ✅ |
| `-[OORegExpMatcher init]` (48) | `OORegExpMatcher::init` `Core/OORegExpMatcher.cpp:73` | ✅ |
| `-[OORegExpMatcher dealloc]` (76) | `~OORegExpMatcher` `Core/OORegExpMatcher.cpp:99` | ✅ |
| `-[OORegExpMatcher string:matchesExpression:]` (88) | `OORegExpMatcher::string(string, regExp)` `Core/OORegExpMatcher.cpp:108` | ✅ |
| `-[OORegExpMatcher string:matchesExpression:flags:]` (93) | `OORegExpMatcher::string(string, regExp, flags)` `Core/OORegExpMatcher.cpp:113` | ✅ |
| `-[NSString(OORegExpMatcher) oo_matchesRegularExpression:]` (135) | — (callers call `regExpMatcher()->string(...)`, e.g. `MatchesRegExp` `Core/OOOpenGLExtensionManager.cpp:664`) | ➖ |

**Differences:**

- ✅ subtle — lifetime: the autoreleased pseudo-singleton lived until the pool drained; the port's `Ref` dies with its last holder, and the only caller holds one matcher per pass (ADR-0056 amendment oo-ct7c item 3, `Core/OOOpenGLExtensionManager.cpp:660-673`), so one tester is compiled per pass as before; a null matcher answers NO as `[nil string:…]` did — upstream `Core/OORegExpMatcher.m:37-46` ↔ port `Core/OORegExpMatcher.cpp:58-71`
- ✅ subtle — pattern length is counted in UTF-16 units and the RegExp object is cached per (pattern, flags) as before; the result goes through the same to-boolean conversion as `-evaluatePredicateWithContext:` (`JS_ValueToBoolean`) — upstream `Core/OORegExpMatcher.m:97-131`, `Core/Scripting/OOJSFunction.m:226-239` ↔ port `Core/OORegExpMatcher.cpp:117-159`
- ✅ subtle — the tester's script file name is now "OORegExpMatcher.cpp" (was "OORegExpMatcher.m") with its own `__LINE__`; visible only in a JS error report from `regexp.test(string)` — upstream `Core/OORegExpMatcher.m:52-64` ↔ port `Core/OORegExpMatcher.cpp:76-87`
- ✅ subtle — the malloc-failure early return still skips `OOJSRelinquishContext` (upstream quirk kept) — upstream `Core/OORegExpMatcher.m:111-113` ↔ port `Core/OORegExpMatcher.cpp:128-130`


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

**Status:** done — 1 rows: 0 ✅ 0 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSDictionary(OOXMLExtensions) writeOOXMLToFile:atomically:errorDescription:]` (29) | `OOWriteXMLPListToFile` (+ `oo::writeXMLPList`, `oofnd/PListWriting.hpp`) `Core/OOXMLExtensions.cpp:30` | ❌ |

**Differences:**

- ❌ **low** — serialisation-failure text: upstream passed `outErrorDesc` to the serializer but formatted its never-set local `errorDesc`, so the message was always "could not convert property list to XML: (null)"; the port appends oofnd's real error description. Visible only when a save game fails to serialise (e.g. a lone surrogate) — upstream `Core/OOXMLExtensions.m:31-38` ↔ port `Core/OOXMLExtensions.cpp:32-37`
- ✅ subtle — the `atomically:` argument is ignored in both (always atomic); the XML bytes come from `oo::writeXMLPList`, which claims GNUstep's XML writer byte for byte (indentation, sorted keys, `%.16g` reals, escaping; `oofnd/PListWriting.hpp:26-34`) — this is the save-game writer, so its fidelity is worth a golden — upstream `Core/OOXMLExtensions.m:34,43` ↔ port `Core/OOXMLExtensions.cpp:32,40`


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

