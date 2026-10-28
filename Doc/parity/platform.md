# Parity findings: SDL platform, input devices, debug support

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## OODebugMonitor

<a id="oodebugmonitor"></a>
Upstream `src/Core/Debug/OODebugMonitor.m` ↔ `Core/Debug/OODebugMonitor.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OODebugMonitor init]` (75) | | |
| `-[OODebugMonitor dealloc]` (121) | | |
| `+[OODebugMonitor sharedDebugMonitor]` (139) | | |
| `-[OODebugMonitor setDebugger:]` (149) | | |
| `-[OODebugMonitor performJSConsoleCommand:]` (181) | | |
| `-[OODebugMonitor appendJSConsoleLine:]` (191) | | |
| `-[OODebugMonitor appendJSConsoleLine:]` (210) | | |
| `-[OODebugMonitor clearJSConsole]` (218) | | |
| `-[OODebugMonitor showJSConsole]` (230) | | |
| `-[OODebugMonitor configurationValueForKey:]` (242) | | |
| `-[OODebugMonitor configurationValueForKey:class:defaultValue:]` (247) | | |
| `-[OODebugMonitor configurationIntValueForKey:defaultValue:]` (265) | | |
| `-[OODebugMonitor setConfigurationValue:forKey:]` (281) | | |
| `-[OODebugMonitor configurationKeys]` (311) | | |
| `-[OODebugMonitor debuggerConnected]` (322) | | |
| `-[OODebugMonitor writeMemStat:]` (327) | | |
| `SizeString()` (340) | | |
| `-[OODebugMonitor dumpEntity:withState:parentVisible:]` (380) | | |
| `-[OODebugMonitor dumpMemoryStatistics]` (454) | | |
| `-[OODebugMonitor dumpJSMemoryStatistics]` (565) | | |
| `-[OODebugMonitor setTCPIgnoresDroppedPackets:]` (580) | | |
| `-[OODebugMonitor TCPIgnoresDroppedPackets]` (589) | | |
| `-[OODebugMonitor setUsingPlugInController:]` (594) | | |
| `-[OODebugMonitor usingPlugInController]` (599) | | |
| `-[OODebugMonitor sourceCodeForFile:line:]` (604) | | |
| `-[OODebugMonitor disconnectDebugger:]` (626) | | |
| `-[OODebugMonitor applicationWillTerminate]` (640) | | |
| `-[OODebugMonitor applicationWillTerminate:]` (646) | | |
| `-[OODebugMonitor(Private) setUpDebugConsoleScript]` (659) | | |
| `-[OODebugMonitor(Private) javaScriptEngineWillReset:]` (688) | | |
| `-[OODebugMonitor(Private) disconnectDebuggerWithMessage:]` (696) | | |
| `-[OODebugMonitor(Private) mergedConfiguration]` (710) | | |
| `-[OODebugMonitor(Private) loadSourceFile:]` (723) | | |
| `-[OODebugMonitor(Private) normalizeConfigDictionary:]` (742) | | |
| `-[OODebugMonitor(Private) normalizeConfigValue:forKey:]` (761) | | |
| `-[OODebugMonitor(Private) jsEngine:]` (779) | | |
| `-[OODebugMonitor(Private) jsEngine:]` (858) | | |
| `-[OODebugMonitor(Private) oo_jsValueInContext:]` (869) | | |
| `+[OODebugMonitor(Singleton) allocWithZone:]` (897) | | |
| `-[OODebugMonitor(Singleton) copyWithZone:]` (906) | | |
| `-[OODebugMonitor(Singleton) retain]` (911) | | |
| `-[OODebugMonitor(Singleton) retainCount]` (916) | | |
| `-[OODebugMonitor(Singleton) release]` (921) | | |
| `-[OODebugMonitor(Singleton) autorelease]` (925) | | |

**Differences:**

- *pending*


## OODebugStandards

<a id="oodebugstandards"></a>
Upstream `src/Core/Debug/OODebugStandards.m` ↔ `Core/Debug/OODebugStandards.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOStandardsSetup()` (69) | | |
| `OOStandardsInternal()` (85) | | |
| `OOStandardsDeprecated()` (100) | | |
| `OOStandardsError()` (105) | | |
| `OOEnforceStandards()` (110) | | |
| `OOSetStandardsForOXPVerifierMode()` (116) | | |

**Differences:**

- *pending*


## OODebugSupport

<a id="oodebugsupport"></a>
Upstream `src/Core/Debug/OODebugSupport.m` ↔ `Core/Debug/OODebugSupport.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOInitDebugSupport()` (52) | | |
| `LoadDebugPlugIn()` (106) | | |

**Differences:**

- *pending*


## OODebugTCPConsoleClient

<a id="oodebugtcpconsoleclient"></a>
Upstream `src/Core/Debug/OODebugTCPConsoleClient.m` ↔ `Core/Debug/OODebugTCPConsoleClient.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `StatusIsSendable()` (60) | | |
| `-[OODebugTCPConsoleClient init]` (99) | | |
| `-[OODebugTCPConsoleClient initWithAddress:port:]` (104) | | |
| `-[OODebugTCPConsoleClient dealloc]` (175) | | |
| `-[OODebugTCPConsoleClient connectDebugMonitor:]` (192) | | |
| `-[OODebugTCPConsoleClient disconnectDebugMonitor:]` (211) | | |
| `-[OODebugTCPConsoleClient debugMonitor:]` (218) | | |
| `-[OODebugTCPConsoleClient debugMonitorClearConsole:]` (241) | | |
| `-[OODebugTCPConsoleClient debugMonitorShowConsole:]` (247) | | |
| `-[OODebugTCPConsoleClient debugMonitor:]` (253) | | |
| `-[OODebugTCPConsoleClient debugMonitor:]` (261) | | |
| `-[OODebugTCPConsoleClient stream:handleEvent:]` (276) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) closeConnection]` (295) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) sendBytes:count:]` (311) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) sendDictionary:]` (330) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) sendPacket:]` (398) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) sendPacket:]` (415) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) readData]` (430) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) dispatchPacket:ofType:]` (446) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handleApproveConnectionPacket:]` (472) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handleRejectConnectionPacket:]` (499) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handleCloseConnectionPacket:]` (515) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handleNoteConfigurationChangePacket:]` (528) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handlePerformCommandPacket:]` (554) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handleRequestConfigurationValuePacket:]` (563) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handlePingPacket:]` (577) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) handlePongPacket:]` (587) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) disconnectFromServerWithMessage:]` (592) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) breakConnectionWithMessage:]` (604) | | |
| `-[OODebugTCPConsoleClient(OOPrivate) breakConnectionWithBadStream:]` (624) | | |
| `DecoderPacket()` (642) | | |
| `DecoderError()` (647) | | |
| `LogOOTCPStreamDecoderPacket()` (653) | | |
| `LogSendPacket()` (663) | | |

**Differences:**

- *pending*


## OOTCPStreamDecoderAbstractionLayer

<a id="ootcpstreamdecoderabstractionlayer"></a>
Upstream `src/Core/Debug/OOTCPStreamDecoderAbstractionLayer.m` ↔ `Core/Debug/OOTCPStreamDecoderAbstractionLayer.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOALGetConstantString()` (15) | | |
| `OOALRelease()` (37) | | |
| `OOTypeDescription()` (42) | | |
| `OOALIsString()` (47) | | |
| `OOALStringCreateWithFormatAndArguments()` (52) | | |
| `OOALIsDictionary()` (57) | | |
| `OOALDictionaryGetValue()` (62) | | |
| `OOALIsData()` (67) | | |
| `OOALDataCreateMutable()` (72) | | |
| `OOALMutableDataAppendBytes()` (77) | | |
| `OOALDataGetBytePtr()` (82) | | |
| `OOALDataGetLength()` (87) | | |
| `OOALCreateAutoreleasePool()` (92) | | |
| `OOALPropertyListFromData()` (97) | | |

**Differences:**

- *pending*


## OOFullScreenController

<a id="oofullscreencontroller"></a>
Upstream `src/Core/OOFullScreenController.m` ↔ `Core/OOFullScreenController.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOFullScreenController initWithGameView:]` (31) | | |
| `-[OOFullScreenController dealloc]` (39) | | |
| `-[OOFullScreenController gameView]` (46) | | |
| `-[OOFullScreenController inFullScreenMode]` (51) | | |
| `-[OOFullScreenController setFullScreenMode:]` (57) | | |
| `-[OOFullScreenController displayModes]` (62) | | |
| `-[OOFullScreenController currentDisplayMode]` (68) | | |
| `-[OOFullScreenController indexOfCurrentDisplayMode]` (73) | | |
| `-[OOFullScreenController setDisplayWidth:height:refreshRate:]` (79) | | |
| `-[OOFullScreenController findDisplayModeForWidth:height:refreshRate:]` (85) | | |
| `-[OOFullScreenController noteMouseInteractionModeChangedFrom:to:]` (91) | | |

**Differences:**

- *pending*


## OOJoystickManager

<a id="oojoystickmanager"></a>
Upstream `src/Core/OOJoystickManager.m` ↔ `Core/OOJoystickManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOJoystickManager sharedStickHandler]` (49) | | |
| `+[OOJoystickManager setStickHandlerClass:]` (59) | | |
| `-[OOJoystickManager init]` (68) | | |
| `-[OOJoystickManager rollPitchAxis]` (88) | | |
| `-[OOJoystickManager viewAxis]` (93) | | |
| `-[OOJoystickManager getButtonState:]` (98) | | |
| `-[OOJoystickManager getAllButtonStates]` (103) | | |
| `-[OOJoystickManager isButtonDown:stick:]` (108) | | |
| `-[OOJoystickManager getAxisState:]` (113) | | |
| `-[OOJoystickManager getSensitivity]` (142) | | |
| `-[OOJoystickManager setProfile:forAxis:]` (147) | | |
| `-[OOJoystickManager getProfileForAxis:]` (168) | | |
| `-[OOJoystickManager saveProfileForAxis:]` (181) | | |
| `-[OOJoystickManager loadProfileForAxis:]` (228) | | |
| `-[OOJoystickManager listSticks]` (272) | | |
| `-[OOJoystickManager axisFunctions]` (283) | | |
| `-[OOJoystickManager buttonFunctions]` (305) | | |
| `-[OOJoystickManager setFunction:withDict:]` (327) | | |
| `-[OOJoystickManager setFunctionForAxis:]` (344) | | |
| `-[OOJoystickManager setFunctionForButton:]` (364) | | |
| `-[OOJoystickManager unsetAxisFunction:]` (382) | | |
| `-[OOJoystickManager unsetButtonFunction:]` (396) | | |
| `-[OOJoystickManager setDefaultMapping]` (409) | | |
| `-[OOJoystickManager clearMappings]` (420) | | |
| `-[OOJoystickManager clearStickStates]` (426) | | |
| `-[OOJoystickManager clearStickButtonState:]` (442) | | |
| `-[OOJoystickManager setCallback:]` (449) | | |
| `-[OOJoystickManager clearCallback]` (458) | | |
| `-[OOJoystickManager saveStickSettings]` (464) | | |
| `-[OOJoystickManager loadStickSettings]` (478) | | |
| `-[OOJoystickManager joystickCount]` (510) | | |
| `-[OOJoystickManager nameOfJoystick:]` (515) | | |
| `-[OOJoystickManager getAxisWithStick:axis:]` (520) | | |

**Differences:**

- *pending*


## OOJoystickProfile

<a id="oojoystickprofile"></a>
Upstream `src/Core/OOJoystickProfile.m` ↔ `Core/OOJoystickProfile.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOJoystickAxisProfile init]` (84) | | |
| `-[OOJoystickAxisProfile copyWithZone:]` (92) | | |
| `-[OOJoystickAxisProfile rawValue:]` (98) | | |
| `-[OOJoystickAxisProfile value:]` (103) | | |
| `-[OOJoystickAxisProfile deadzone]` (111) | | |
| `-[OOJoystickAxisProfile setDeadzone:]` (116) | | |
| `-[OOJoystickStandardAxisProfile init]` (125) | | |
| `-[OOJoystickStandardAxisProfile copyWithZone:]` (134) | | |
| `-[OOJoystickStandardAxisProfile setPower:]` (142) | | |
| `-[OOJoystickStandardAxisProfile power]` (154) | | |
| `-[OOJoystickStandardAxisProfile setParameter:]` (159) | | |
| `-[OOJoystickStandardAxisProfile parameter]` (165) | | |
| `-[OOJoystickStandardAxisProfile rawValue:]` (170) | | |
| `-[OOJoystickSplineSegment init]` (182) | | |
| `-[OOJoystickSplineSegment copyWithZone:]` (195) | | |
| `-[OOJoystickSplineSegment initWithData:right:]` (207) | | |
| `-[OOJoystickSplineSegment initWithData:right:gradientleft:]` (224) | | |
| `-[OOJoystickSplineSegment initWithData:right:gradientright:]` (240) | | |
| `-[OOJoystickSplineSegment initWithData:right:gradientleft:gradientright:]` (256) | | |
| `+[OOJoystickSplineSegment segmentWithData:right:]` (273) | | |
| `+[OOJoystickSplineSegment segmentWithData:right:gradientleft:]` (279) | | |
| `+[OOJoystickSplineSegment segmentWithData:right:gradientright:]` (285) | | |
| `+[OOJoystickSplineSegment segmentWithData:right:gradientleft:gradientright:]` (291) | | |
| `-[OOJoystickSplineSegment start]` (297) | | |
| `-[OOJoystickSplineSegment end]` (302) | | |
| `-[OOJoystickSplineSegment value:]` (307) | | |
| `-[OOJoystickSplineSegment gradient:]` (312) | | |
| `-[OOJoystickSplineAxisProfile init]` (321) | | |
| `-[OOJoystickSplineAxisProfile dealloc]` (331) | | |
| `-[OOJoystickSplineAxisProfile copyWithZone:]` (339) | | |
| `-[OOJoystickSplineAxisProfile addControl:]` (347) | | |
| `-[OOJoystickSplineAxisProfile pointAtIndex:]` (382) | | |
| `-[OOJoystickSplineAxisProfile countPoints]` (397) | | |
| `-[OOJoystickSplineAxisProfile controlPoints]` (402) | | |
| `-[OOJoystickSplineAxisProfile makeSegments]` (408) | | |
| `-[OOJoystickSplineAxisProfile removeControl:]` (461) | | |
| `-[OOJoystickSplineAxisProfile clearControlPoints]` (470) | | |
| `-[OOJoystickSplineAxisProfile moveControl:point:]` (476) | | |
| `-[OOJoystickSplineAxisProfile rawValue:]` (515) | | |
| `-[OOJoystickSplineAxisProfile gradient:]` (536) | | |

**Differences:**

- *pending*


## GameController+SDLFullScreen

<a id="gamecontrollersdlfullscreen"></a>
Upstream `src/SDL/GameController+SDLFullScreen.m` ↔ `SDL/GameController+SDLFullScreen.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[GameController(FullScreen) setUpDisplayModes]` (38) | | |
| `-[GameController(FullScreen) setFullScreenMode:]` (69) | | |
| `-[GameController(FullScreen) exitFullScreenMode]` (74) | | |
| `-[GameController(FullScreen) inFullScreenMode]` (80) | | |
| `-[GameController(FullScreen) setDisplayWidth:Height:Refresh:]` (85) | | |
| `-[GameController(FullScreen) findDisplayModeForWidth:Height:Refresh:]` (108) | | |
| `-[GameController(FullScreen) displayModes]` (128) | | |
| `-[GameController(FullScreen) indexOfCurrentDisplayMode]` (133) | | |
| `-[GameController(FullScreen) pauseFullScreenModeToPerform:onTarget:]` (146) | | |

**Differences:**

- *pending*


## MyOpenGLView+Input

<a id="myopenglviewinput"></a>
Upstream `src/SDL/MyOpenGLView+Input.m` ↔ `SDL/MyOpenGLView+Input.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[MyOpenGLView(Input) initKeyMappingData]` (30) | | |
| `-[MyOpenGLView(Input) autoShowMouse]` (48) | | |
| `-[MyOpenGLView(Input) setStringInput:]` (59) | | |
| `-[MyOpenGLView(Input) allowStringInput:]` (63) | | |
| `-[MyOpenGLView(Input) allowingStringInput]` (70) | | |
| `-[MyOpenGLView(Input) typedString]` (74) | | |
| `-[MyOpenGLView(Input) resetTypedString]` (78) | | |
| `-[MyOpenGLView(Input) setTypedString:]` (82) | | |
| `-[MyOpenGLView(Input) noteMouseInteractionModeChangedFrom:to:]` (86) | | |
| `-[MyOpenGLView(Input) setVirtualJoystick:vmy:]` (91) | | |
| `-[MyOpenGLView(Input) virtualJoystickPosition]` (96) | | |
| `-[MyOpenGLView(Input) clearKeys]` (102) | | |
| `-[MyOpenGLView(Input) clearMouse]` (109) | | |
| `-[MyOpenGLView(Input) clearKey:]` (115) | | |
| `-[MyOpenGLView(Input) resetMouse]` (121) | | |
| `-[MyOpenGLView(Input) isAlphabetKeyDown]` (129) | | |
| `-[MyOpenGLView(Input) suppressKeysUntilKeyUp]` (139) | | |
| `-[MyOpenGLView(Input) isDown:]` (148) | | |
| `-[MyOpenGLView(Input) isOptDown]` (158) | | |
| `-[MyOpenGLView(Input) isCtrlDown]` (162) | | |
| `-[MyOpenGLView(Input) isCommandDown]` (166) | | |
| `-[MyOpenGLView(Input) isShiftDown]` (170) | | |
| `-[MyOpenGLView(Input) isCapsLockOn]` (174) | | |
| `-[MyOpenGLView(Input) lastKeyWasShifted]` (184) | | |
| `-[MyOpenGLView(Input) numKeys]` (188) | | |
| `-[MyOpenGLView(Input) mouseWheelState]` (192) | | |
| `-[MyOpenGLView(Input) mouseWheelDelta]` (201) | | |
| `-[MyOpenGLView(Input) setMouseWheelDelta:]` (205) | | |
| `-[MyOpenGLView(Input) isCommandQDown]` (209) | | |
| `-[MyOpenGLView(Input) isCommandFDown]` (213) | | |
| `-[MyOpenGLView(Input) clearCommandF]` (217) | | |
| `-[MyOpenGLView(Input) pollControls]` (221) | | |
| `-[MyOpenGLView(Input) handleStringInput:keyID:]` (850) | | |
| `-[MyOpenGLView(Input) setMouseInDeltaMode:]` (885) | | |

**Differences:**

- *pending*


## MyOpenGLView

<a id="myopenglview"></a>
Upstream `src/SDL/MyOpenGLView.m` ↔ `SDL/MyOpenGLView.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[MyOpenGLView getDisplayId]` (59) | | |
| `-[MyOpenGLView getNativeSize]` (77) | | |
| `-[MyOpenGLView getWindowCaption]` (100) | | |
| `-[MyOpenGLView createWindowWithSize:]` (106) | | |
| `-[MyOpenGLView init]` (275) | | |
| `-[MyOpenGLView endSplashScreen]` (393) | | |
| `-[MyOpenGLView updateGLSize:]` (445) | | |
| `-[MyOpenGLView setUpBasicOpenGLStateWithSize]` (468) | | |
| `-[MyOpenGLView initialiseGLWithSize:]` (505) | | |
| `-[MyOpenGLView dealloc]` (553) | | |
| `-[MyOpenGLView bounds]` (584) | | |
| `-[MyOpenGLView viewSize]` (589) | | |
| `-[MyOpenGLView backingViewSize]` (594) | | |
| `-[MyOpenGLView display_z]` (599) | | |
| `-[MyOpenGLView x_offset]` (604) | | |
| `-[MyOpenGLView y_offset]` (609) | | |
| `-[MyOpenGLView gameController]` (614) | | |
| `-[MyOpenGLView setGameController:]` (619) | | |
| `-[MyOpenGLView inFullScreenMode]` (624) | | |
| `-[MyOpenGLView setFullScreenMode:]` (630) | | |
| `-[MyOpenGLView toggleScreenMode]` (641) | | |
| `-[MyOpenGLView setDisplayMode:fullScreen:]` (656) | | |
| `-[MyOpenGLView setScreenSize:]` (664) | | |
| `-[MyOpenGLView getScreenSizeArray]` (671) | | |
| `-[MyOpenGLView modeAsSize:]` (676) | | |
| `-[MyOpenGLView updateScreen]` (685) | | |
| `-[MyOpenGLView initSplashScreen]` (697) | | |
| `-[MyOpenGLView getDisplayDimensions:height:]` (810) | | |
| `-[MyOpenGLView isRunningOnPrimaryDisplayDevice]` (823) | | |
| `-[MyOpenGLView grabMouseInsideGameWindow:]` (835) | | |
| `-[MyOpenGLView stringToClipboard:]` (841) | | |
| `-[MyOpenGLView atDesktopResolution]` (853) | | |
| `-[MyOpenGLView hdrOutput]` (858) | | |
| `-[MyOpenGLView isOutputDisplayHDREnabled]` (863) | | |
| `-[MyOpenGLView hdrMaxBrightness]` (881) | | |
| `-[MyOpenGLView setHDRMaxBrightness:]` (886) | | |
| `-[MyOpenGLView hdrPaperWhiteBrightness]` (897) | | |
| `-[MyOpenGLView setHDRPaperWhiteBrightness:]` (902) | | |
| `-[MyOpenGLView hdrToneMapper]` (913) | | |
| `-[MyOpenGLView setHDRToneMapper:]` (918) | | |
| `-[MyOpenGLView hdrOutput]` (929) | | |
| `-[MyOpenGLView isOutputDisplayHDREnabled]` (934) | | |
| `-[MyOpenGLView sdrToneMapper]` (941) | | |
| `-[MyOpenGLView setSDRToneMapper:]` (946) | | |
| `-[MyOpenGLView colorSaturation]` (955) | | |
| `-[MyOpenGLView snapShot:]` (966) | | |
| `-[MyOpenGLView populateFullScreenModelist]` (1077) | | |
| `-[MyOpenGLView saveWindowSize:]` (1115) | | |
| `-[MyOpenGLView loadWindowSize]` (1123) | | |
| `-[MyOpenGLView loadFullscreenSettings]` (1137) | | |
| `-[MyOpenGLView findDisplayModeForWidth:Height:Refresh:]` (1172) | | |
| `-[MyOpenGLView currentScreenSize]` (1196) | | |
| `-[MyOpenGLView currentScreenMode]` (1208) | | |
| `-[MyOpenGLView setFov:fromFraction:]` (1214) | | |
| `-[MyOpenGLView fov:]` (1219) | | |
| `-[MyOpenGLView msaa]` (1224) | | |
| `-[MyOpenGLView setMsaa:]` (1229) | | |
| `-[MyOpenGLView getOpenGLMatrixManager]` (1234) | | |
| `+[MyOpenGLView pollShiftKey]` (1239) | | |
| `-[MyOpenGLView dumpRGBAToFileNamed:]` (1251) | | |
| `-[MyOpenGLView dumpRGBToFileNamed:]` (1269) | | |
| `-[MyOpenGLView dumpGrayToFileNamed:]` (1287) | | |
| `-[MyOpenGLView dumpGrayAlphaToFileNamed:]` (1315) | | |
| `-[MyOpenGLView dumpRGBAToRGBFileNamed:]` (1343) | | |

**Differences:**

- *pending*


## OOSDLJoystickManager

<a id="oosdljoystickmanager"></a>
Upstream `src/SDL/OOSDLJoystickManager.m` ↔ `SDL/OOSDLJoystickManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSDLJoystickManager init]` (33) | | |
| `-[OOSDLJoystickManager dealloc]` (67) | | |
| `-[OOSDLJoystickManager getJoystickIndexFromId:]` (73) | | |
| `-[OOSDLJoystickManager joystickCount]` (82) | | |
| `-[OOSDLJoystickManager nameOfJoystick:]` (87) | | |
| `-[OOSDLJoystickManager getAxisWithStick:axis:]` (94) | | |
| `-[OOSDLJoystickManager makeJoyAxisEvent:]` (99) | | |
| `-[OOSDLJoystickManager makeJoyButtonEvent:]` (109) | | |
| `-[OOSDLJoystickManager makeJoyHatEvent:]` (119) | | |
| `-[OOSDLJoystickManager decodeAxisEvent:]` (129) | | |
| `-[OOSDLJoystickManager decodeButtonEvent:]` (191) | | |
| `-[OOSDLJoystickManager decodeHatEvent:]` (229) | | |
| `-[OOSDLJoystickManager handleSDLEvent:]` (249) | | |

**Differences:**

- *pending*


## main

<a id="main"></a>
Upstream `src/SDL/main.m` ↔ `SDL/main.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `main()` (68) | | |

**Differences:**

- *pending*

