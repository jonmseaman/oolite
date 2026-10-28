# Parity findings: HUD, GUI, crosshairs

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## GuiDisplayGen

<a id="guidisplaygen"></a>
Upstream `src/Core/GuiDisplayGen.m` ↔ `Core/GuiDisplayGen.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `RowInRange()` (42) | | |
| `-[GuiDisplayGen init]` (65) | | |
| `-[GuiDisplayGen initWithPixelSize:]` (115) | | |
| `-[GuiDisplayGen dealloc]` (160) | | |
| `-[GuiDisplayGen resizeWithPixelSize:]` (175) | | |
| `-[GuiDisplayGen resizeTo:]` (200) | | |
| `-[GuiDisplayGen size]` (244) | | |
| `-[GuiDisplayGen columns]` (249) | | |
| `-[GuiDisplayGen rows]` (254) | | |
| `-[GuiDisplayGen rowHeight]` (259) | | |
| `-[GuiDisplayGen rowStart]` (264) | | |
| `-[GuiDisplayGen title]` (269) | | |
| `-[GuiDisplayGen setTitle:]` (274) | | |
| `-[GuiDisplayGen setDrawPosition:]` (284) | | |
| `-[GuiDisplayGen drawPosition]` (289) | | |
| `-[GuiDisplayGen userSettings]` (294) | | |
| `-[GuiDisplayGen fadeOutFromTime:overDuration:]` (299) | | |
| `-[GuiDisplayGen stopFadeOuts]` (310) | | |
| `-[GuiDisplayGen alpha]` (315) | | |
| `-[GuiDisplayGen setAlpha:]` (320) | | |
| `-[GuiDisplayGen setMaxAlpha:]` (325) | | |
| `-[GuiDisplayGen setBackgroundColor:]` (330) | | |
| `-[GuiDisplayGen textColor]` (336) | | |
| `-[GuiDisplayGen setTextColor:]` (342) | | |
| `-[GuiDisplayGen textCommsColor]` (350) | | |
| `-[GuiDisplayGen setTextCommsColor:]` (355) | | |
| `-[GuiDisplayGen colorFromSetting:defaultValue:]` (363) | | |
| `-[GuiDisplayGen setGLColorFromSetting:defaultValue:alpha:]` (381) | | |
| `-[GuiDisplayGen setGuiColorSettingFromKey:color:]` (390) | | |
| `-[GuiDisplayGen setCharacterSize:]` (403) | | |
| `-[GuiDisplayGen setShowAdvancedNavArray:]` (408) | | |
| `-[GuiDisplayGen setColor:forRow:]` (413) | | |
| `-[GuiDisplayGen objectForRow:]` (419) | | |
| `-[GuiDisplayGen rowForKey:]` (427) | | |
| `-[GuiDisplayGen keyForRow:]` (437) | | |
| `-[GuiDisplayGen selectedRow]` (445) | | |
| `-[GuiDisplayGen setSelectedRow:]` (453) | | |
| `-[GuiDisplayGen setNextRow:]` (468) | | |
| `-[GuiDisplayGen setFirstSelectableRow]` (482) | | |
| `-[GuiDisplayGen setLastSelectableRow]` (497) | | |
| `-[GuiDisplayGen reportSelectedRow:]` (512) | | |
| `-[GuiDisplayGen setNoSelectedRow]` (517) | | |
| `-[GuiDisplayGen selectedRowText]` (522) | | |
| `-[GuiDisplayGen selectedRowKey]` (531) | | |
| `-[GuiDisplayGen setShowTextCursor:]` (539) | | |
| `-[GuiDisplayGen setCurrentRow:]` (544) | | |
| `-[GuiDisplayGen selectableRange]` (554) | | |
| `-[GuiDisplayGen setSelectableRange:]` (559) | | |
| `-[GuiDisplayGen setTabStops:]` (564) | | |
| `-[GuiDisplayGen overrideTabs:from:length:]` (570) | | |
| `-[GuiDisplayGen clear]` (584) | | |
| `-[GuiDisplayGen clearAndKeepBackground:]` (589) | | |
| `-[GuiDisplayGen setKey:forRow:]` (607) | | |
| `-[GuiDisplayGen setText:forRow:]` (613) | | |
| `-[GuiDisplayGen setText:forRow:align:]` (620) | | |
| `-[GuiDisplayGen addLongText:]` (628) | | |
| `-[GuiDisplayGen reflowTextForMFD:]` (666) | | |
| `-[GuiDisplayGen leaveLastLine]` (697) | | |
| `-[GuiDisplayGen getLastLines]` (710) | | |
| `-[GuiDisplayGen printLongText:]` (736) | | |
| `-[GuiDisplayGen printLineNoScroll:]` (797) | | |
| `-[GuiDisplayGen setArray:forRow:]` (814) | | |
| `-[GuiDisplayGen insertItemsFromArray:]` (820) | | |
| `-[GuiDisplayGen scrollUp:]` (863) | | |
| `-[GuiDisplayGen clearBackground]` (882) | | |
| `TextureForGUITexture()` (888) | | |
| `NewTextureSpriteWithDescriptor()` (912) | | |
| `-[GuiDisplayGen setBackgroundTextureSpecial:withBackground:]` (952) | | |
| `-[GuiDisplayGen setBackgroundTextureDescriptor:]` (1003) | | |
| `-[GuiDisplayGen setForegroundTextureDescriptor:]` (1011) | | |
| `-[GuiDisplayGen setBackgroundTextureKey:]` (1020) | | |
| `-[GuiDisplayGen setForegroundTextureKey:]` (1025) | | |
| `-[GuiDisplayGen preloadGUITexture:]` (1030) | | |
| `-[GuiDisplayGen textureDescriptorFromJSValue:]` (1035) | | |
| `-[GuiDisplayGen setStatusPage:]` (1076) | | |
| `-[GuiDisplayGen statusPage]` (1085) | | |
| `-[GuiDisplayGen drawEquipmentList:z:]` (1090) | | |
| `-[GuiDisplayGen drawGUIBackground]` (1209) | | |
| `-[GuiDisplayGen refreshStarChart]` (1220) | | |
| `-[GuiDisplayGen drawGUI:drawCursor:]` (1225) | | |
| `-[GuiDisplayGen drawGLDisplay:y:z:alpha:]` (1308) | | |
| `-[GuiDisplayGen drawCrossHairsWithSize:x:y:z:]` (1517) | | |
| `-[GuiDisplayGen setStarChartTitle]` (1531) | | |
| `-[GuiDisplayGen drawStarChart:y:z:alpha:compact:]` (1550) | | |
| `-[GuiDisplayGen drawSystemMarkers:atX:andY:andZ:withAlpha:andScale:]` (2060) | | |
| `-[GuiDisplayGen drawSystemMarker:atX:andY:andZ:withAlpha:andScale:]` (2068) | | |
| `-[GuiDisplayGen targetNextFoundSystem:]` (2119) | | |
| `-[GuiDisplayGen drawAdvancedNavArrayAtX:y:z:alpha:usingRoute:optimizedBy:zoom:]` (2164) | | |

**Differences:**

- *pending*


## HeadUpDisplay

<a id="headupdisplay"></a>
Upstream `src/Core/HeadUpDisplay.m` ↔ `Core/HeadUpDisplay.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `useDefined()` (80) | | |
| `GLColorWithOverallAlpha()` (203) | | |
| `-[HeadUpDisplay initWithDictionary:]` (209) | | |
| `-[HeadUpDisplay initWithDictionary:inFile:]` (214) | | |
| `-[HeadUpDisplay dealloc]` (323) | | |
| `-[HeadUpDisplay resetGui:withInfo:]` (342) | | |
| `-[HeadUpDisplay resetGuiPosition:withInfo:]` (366) | | |
| `-[HeadUpDisplay resetGuis:]` (383) | | |
| `-[HeadUpDisplay hudName]` (505) | | |
| `-[HeadUpDisplay setHudName:]` (510) | | |
| `-[HeadUpDisplay reticleColorForIndex:]` (518) | | |
| `-[HeadUpDisplay setReticleColorForIndex:toColor:]` (526) | | |
| `-[HeadUpDisplay scannerZoom]` (535) | | |
| `-[HeadUpDisplay setScannerZoom:]` (540) | | |
| `-[HeadUpDisplay overallAlpha]` (545) | | |
| `-[HeadUpDisplay setOverallAlpha:]` (550) | | |
| `-[HeadUpDisplay reticleTargetSensitive]` (555) | | |
| `-[HeadUpDisplay setReticleTargetSensitive:]` (560) | | |
| `-[HeadUpDisplay propertiesReticleTargetSensitive]` (565) | | |
| `-[HeadUpDisplay isHidden]` (570) | | |
| `-[HeadUpDisplay setHidden:]` (575) | | |
| `-[HeadUpDisplay allowBigGui]` (580) | | |
| `-[HeadUpDisplay hasHidden:]` (585) | | |
| `-[HeadUpDisplay setHiddenSelector:hidden:]` (593) | | |
| `-[HeadUpDisplay clearHiddenSelectors]` (602) | | |
| `-[HeadUpDisplay isCompassActive]` (607) | | |
| `-[HeadUpDisplay setCompassActive:]` (612) | | |
| `-[HeadUpDisplay isUpdating]` (617) | | |
| `-[HeadUpDisplay setDeferredHudName:]` (622) | | |
| `-[HeadUpDisplay deferredHudName]` (628) | | |
| `-[HeadUpDisplay addLegend:]` (633) | | |
| `-[HeadUpDisplay addDial:]` (675) | | |
| `-[HeadUpDisplay addMFD:]` (712) | | |
| `-[HeadUpDisplay mfdCount]` (719) | | |
| `-[HeadUpDisplay renderHUD]` (751) | | |
| `-[HeadUpDisplay drawLegends]` (776) | | |
| `-[HeadUpDisplay drawDials]` (789) | | |
| `-[HeadUpDisplay drawMFDs]` (810) | | |
| `-[HeadUpDisplay drawCrosshairs]` (823) | | |
| `-[HeadUpDisplay crosshairDefinition]` (858) | | |
| `-[HeadUpDisplay setCrosshairDefinition:]` (863) | | |
| `-[HeadUpDisplay crosshairDefinitionForWeaponType:]` (885) | | |
| `-[HeadUpDisplay drawLegend:]` (926) | | |
| `-[HeadUpDisplay drawHUDItem:]` (989) | | |
| `-[HeadUpDisplay checkPlayerInFlight]` (1025) | | |
| `-[HeadUpDisplay checkPlayerInSystemFlight]` (1030) | | |
| `prefetchData()` (1040) | | |
| `-[HeadUpDisplay drawScanner:]` (1053) | | |
| `-[HeadUpDisplay minimalisticScanner]` (1358) | | |
| `-[HeadUpDisplay setMinimalisticScanner:]` (1363) | | |
| `+[HeadUpDisplay nonlinearScannerScale:Zoom:Scale:]` (1368) | | |
| `-[HeadUpDisplay nonlinearScanner]` (1375) | | |
| `-[HeadUpDisplay setNonlinearScanner:]` (1380) | | |
| `-[HeadUpDisplay scannerUltraZoom]` (1385) | | |
| `-[HeadUpDisplay setScannerUltraZoom:]` (1390) | | |
| `-[HeadUpDisplay refreshLastTransmitter]` (1395) | | |
| `-[HeadUpDisplay drawScannerZoomIndicator:]` (1405) | | |
| `-[HeadUpDisplay drawCompass:]` (1452) | | |
| `SetCompassBlipColor()` (1552) | | |
| `-[HeadUpDisplay drawCompassPlanetBlipAt:Size:Alpha:]` (1561) | | |
| `-[HeadUpDisplay drawCompassStationBlipAt:Size:Alpha:]` (1574) | | |
| `-[HeadUpDisplay drawCompassSunBlipAt:Size:Alpha:]` (1586) | | |
| `-[HeadUpDisplay drawCompassTargetBlipAt:Size:Alpha:]` (1596) | | |
| `-[HeadUpDisplay drawCompassBeaconBlipAt:Size:Alpha:]` (1610) | | |
| `-[HeadUpDisplay drawAegis:]` (1638) | | |
| `-[HeadUpDisplay drawCustomBar:]` (1688) | | |
| `-[HeadUpDisplay drawCustomText:]` (1724) | | |
| `-[HeadUpDisplay drawCustomIndicator:]` (1750) | | |
| `-[HeadUpDisplay drawCustomLight:]` (1779) | | |
| `-[HeadUpDisplay drawCustomImage:]` (1813) | | |
| `-[HeadUpDisplay drawSpeedBar:]` (1856) | | |
| `-[HeadUpDisplay drawRollBar:]` (1892) | | |
| `-[HeadUpDisplay drawPitchBar:]` (1919) | | |
| `-[HeadUpDisplay drawYawBar:]` (1946) | | |
| `-[HeadUpDisplay drawEnergyGauge:]` (1974) | | |
| `-[HeadUpDisplay drawForwardShieldBar:]` (2045) | | |
| `-[HeadUpDisplay drawAftShieldBar:]` (2079) | | |
| `-[HeadUpDisplay drawFuelBar:]` (2113) | | |
| `-[HeadUpDisplay drawWitchspaceDestination:]` (2154) | | |
| `-[HeadUpDisplay drawCabinTempBar:]` (2188) | | |
| `-[HeadUpDisplay drawWeaponTempBar:]` (2229) | | |
| `-[HeadUpDisplay drawAltitudeBar:]` (2262) | | |
| `IconForMissileRole()` (2308) | | |
| `-[HeadUpDisplay drawIconForMissile:]` (2348) | | |
| `-[HeadUpDisplay drawIconForEmptyPylonAtX:y:]` (2401) | | |
| `-[HeadUpDisplay drawMissileDisplay:]` (2417) | | |
| `-[HeadUpDisplay drawTargetReticle:]` (2486) | | |
| `-[HeadUpDisplay drawSecondaryTargetReticle:]` (2500) | | |
| `-[HeadUpDisplay drawWaypoints:]` (2529) | | |
| `-[HeadUpDisplay drawStatusLight:]` (2542) | | |
| `-[HeadUpDisplay drawDirectionCue:]` (2596) | | |
| `-[HeadUpDisplay drawClock:]` (2682) | | |
| `-[HeadUpDisplay drawPrimedEquipment:]` (2703) | | |
| `-[HeadUpDisplay drawASCTarget:]` (2754) | | |
| `-[HeadUpDisplay drawWeaponsOfflineText:]` (2786) | | |
| `-[HeadUpDisplay drawFPSInfoCounter:]` (2818) | | |
| `-[HeadUpDisplay drawScoopStatus:]` (2857) | | |
| `-[HeadUpDisplay drawStickSensitivityIndicator:]` (2958) | | |
| `-[HeadUpDisplay drawSurroundInternal:color:]` (3005) | | |
| `-[HeadUpDisplay drawSurround:]` (3029) | | |
| `-[HeadUpDisplay drawGreenSurround:]` (3045) | | |
| `-[HeadUpDisplay drawYellowSurround:]` (3050) | | |
| `-[HeadUpDisplay drawTrumbles:]` (3055) | | |
| `-[HeadUpDisplay drawMultiFunctionDisplay:withText:asIndex:]` (3065) | | |
| `hudDrawIndicatorAt()` (3132) | | |
| `hudDrawMarkerAt()` (3155) | | |
| `hudDrawBarAt()` (3178) | | |
| `hudDrawSurroundAt()` (3203) | | |
| `hudDrawStatusIconAt()` (3216) | | |
| `hudDrawWaypoint()` (3427) | | |
| `hudRotateViewpointForVirtualDepth()` (3492) | | |
| `InitTextEngine()` (3561) | | |
| `OOHUDResetTextEngine()` (3593) | | |
| `drawCharacterQuad()` (3599) | | |
| `OORectFromString()` (3621) | | |
| `OOStringWidthInEm()` (3639) | | |
| `drawHighlight()` (3644) | | |
| `OODrawString()` (3673) | | |
| `OODrawStringAligned()` (3678) | | |
| `OOStartDrawingStrings()` (3685) | | |
| `OODrawStringQuadsAligned()` (3694) | | |
| `OOStopDrawingStrings()` (3714) | | |
| `OODrawHilightedString()` (3724) | | |
| `OODrawPlanetInfo()` (3746) | | |
| `OODrawHilightedPlanetInfo()` (3799) | | |
| `GLDrawNonlinearCascadeWeapon()` (3831) | | |
| `nonlinearScannerFunc()` (3898) | | |
| `drawScannerGrid()` (3911) | | |
| `DrawSpecialOval()` (4047) | | |
| `-[HeadUpDisplay setLineWidth:]` (4066) | | |
| `-[HeadUpDisplay lineWidth]` (4071) | | |
| `-[NSString(OODisplayEncoding) cStringUsingOoliteEncoding]` (4080) | | |
| `-[NSString(OODisplayEncoding) cStringUsingOoliteEncodingAndRemapping]` (4089) | | |
| `-[OOPolygonSprite(OOHUDBeaconIcon) oo_drawHUDBeaconIconAt:size:alpha:z:]` (4102) | | |
| `-[NSString(OOHUDBeaconIcon) oo_drawHUDBeaconIconAt:size:alpha:z:]` (4125) | | |
| `SetGLColourFromInfo()` (4132) | | |
| `GetRGBAArrayFromInfo()` (4149) | | |

**Differences:**

- *pending*


## OOCrosshairs

<a id="oocrosshairs"></a>
Upstream `src/Core/OOCrosshairs.m` ↔ `Core/OOCrosshairs.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCrosshairs initWithPoints:]` (53) | | |
| `-[OOCrosshairs dealloc]` (67) | | |
| `-[OOCrosshairs render]` (74) | | |
| `-[OOCrosshairs setUpDataWithPoints:]` (105) | | |
| `-[OOCrosshairs setUpDataForOnePoint:]` (133) | | |

**Differences:**

- *pending*


## OOMouseInteractionMode

<a id="oomouseinteractionmode"></a>
Upstream `src/Core/OOMouseInteractionMode.m` ↔ `Core/OOMouseInteractionMode.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOStringFromMouseInteractionMode()` (28) | | |
| `OOMouseInteractionModeIsUIScreen()` (44) | | |
| `OOMouseInteractionModeIsFlightMode()` (60) | | |

**Differences:**

- *pending*

