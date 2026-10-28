# Parity findings: Other entities (planets, effects, particles, base Entity)

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## DustEntity

<a id="dustentity"></a>
Upstream `src/Core/Entities/DustEntity.m` ↔ `Core/Entities/DustEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[DustEntity init]` (62) | | |
| `-[DustEntity dealloc]` (113) | | |
| `-[DustEntity setDustColor:]` (127) | | |
| `-[DustEntity dustColor]` (135) | | |
| `-[DustEntity canCollide]` (140) | | |
| `-[DustEntity updateCameraRelativePosition]` (145) | | |
| `-[DustEntity update:]` (151) | | |
| `-[DustEntity shader]` (187) | | |
| `-[DustEntity offsetPlayerPosition]` (227) | | |
| `-[DustEntity checkShaderMode]` (235) | | |
| `-[DustEntity warpVector]` (246) | | |
| `-[DustEntity drawImmediate:translucent:]` (251) | | |
| `-[DustEntity resetGraphicsState]` (395) | | |
| `-[DustEntity descriptionForObjDump]` (411) | | |

**Differences:**

- *pending*


## Entity

<a id="entity"></a>
Upstream `src/Core/Entities/Entity.m` ↔ `Core/Entities/Entity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[Entity init]` (64) | | |
| `-[Entity dealloc]` (98) | | |
| `-[Entity descriptionComponents]` (115) | | |
| `-[Entity sessionID]` (120) | | |
| `-[Entity isShip]` (125) | | |
| `-[Entity isDock]` (130) | | |
| `-[Entity isStation]` (135) | | |
| `-[Entity isSubEntity]` (140) | | |
| `-[Entity isPlayer]` (145) | | |
| `-[Entity isPlanet]` (150) | | |
| `-[Entity isSun]` (155) | | |
| `-[Entity isSunlit]` (160) | | |
| `-[Entity isStellarObject]` (165) | | |
| `-[Entity isSky]` (170) | | |
| `-[Entity isWormhole]` (175) | | |
| `-[Entity isEffect]` (180) | | |
| `-[Entity isVisualEffect]` (185) | | |
| `-[Entity isWaypoint]` (190) | | |
| `-[Entity validForAddToUniverse]` (195) | | |
| `-[Entity addToLinkedLists]` (207) | | |
| `-[Entity removeFromLinkedLists]` (269) | | |
| `-[Entity checkLinkedLists]` (321) | | |
| `-[Entity updateLinkedLists]` (393) | | |
| `-[Entity wasAddedToUniverse]` (489) | | |
| `-[Entity wasRemovedFromUniverse]` (494) | | |
| `-[Entity warnAboutHostiles]` (499) | | |
| `-[Entity collisionRegion]` (505) | | |
| `-[Entity setCollisionRegion:]` (510) | | |
| `-[Entity setUniversalID:]` (517) | | |
| `-[Entity universalID]` (522) | | |
| `-[Entity throwingSparks]` (527) | | |
| `-[Entity setThrowSparks:]` (532) | | |
| `-[Entity throwSparks]` (537) | | |
| `-[Entity setOwner:]` (542) | | |
| `-[Entity owner]` (548) | | |
| `-[Entity parentEntity]` (553) | | |
| `-[Entity superShaderBindingTarget]` (561) | | |
| `-[Entity rootShipEntity]` (566) | | |
| `-[Entity position]` (576) | | |
| `-[Entity cameraRelativePosition]` (581) | | |
| `-[Entity cameraRangeFront]` (586) | | |
| `-[Entity cameraRangeBack]` (591) | | |
| `-[Entity relativePosition]` (598) | | |
| `-[Entity vectorTo:]` (603) | | |
| `-[Entity setPosition:]` (608) | | |
| `-[Entity setPositionX:y:z:]` (614) | | |
| `-[Entity updateCameraRelativePosition]` (622) | | |
| `-[Entity absolutePositionForSubentity]` (627) | | |
| `-[Entity absolutePositionForSubentityOffset:]` (632) | | |
| `-[Entity zeroDistance]` (648) | | |
| `-[Entity camZeroDistance]` (653) | | |
| `-[Entity compareZeroDistance:]` (658) | | |
| `-[Entity boundingBox]` (666) | | |
| `-[Entity mass]` (671) | | |
| `-[Entity setOrientation:]` (676) | | |
| `-[Entity orientation]` (682) | | |
| `-[Entity normalOrientation]` (687) | | |
| `-[Entity setNormalOrientation:]` (692) | | |
| `-[Entity orientationChanged]` (697) | | |
| `-[Entity setVelocity:]` (703) | | |
| `-[Entity velocity]` (708) | | |
| `-[Entity speed]` (713) | | |
| `-[Entity distanceTravelled]` (718) | | |
| `-[Entity setDistanceTravelled:]` (723) | | |
| `-[Entity setStatus:]` (728) | | |
| `-[Entity status]` (733) | | |
| `-[Entity setScanClass:]` (738) | | |
| `-[Entity scanClass]` (743) | | |
| `-[Entity setEnergy:]` (748) | | |
| `-[Entity energy]` (753) | | |
| `-[Entity setMaxEnergy:]` (758) | | |
| `-[Entity maxEnergy]` (763) | | |
| `-[Entity applyRoll:andClimb:]` (768) | | |
| `-[Entity applyRoll:climb:andYaw:]` (781) | | |
| `-[Entity moveForward:]` (796) | | |
| `-[Entity rotationMatrix]` (803) | | |
| `-[Entity drawRotationMatrix]` (808) | | |
| `-[Entity transformationMatrix]` (813) | | |
| `-[Entity drawTransformationMatrix]` (819) | | |
| `-[Entity canCollide]` (825) | | |
| `-[Entity collisionRadius]` (830) | | |
| `-[Entity frustumRadius]` (835) | | |
| `-[Entity setCollisionRadius:]` (840) | | |
| `-[Entity collisionArray]` (845) | | |
| `-[Entity update:]` (850) | | |
| `-[Entity applyVelocity:]` (878) | | |
| `-[Entity checkCloseCollisionWith:]` (883) | | |
| `-[Entity findCollisionRadius]` (888) | | |
| `-[Entity drawImmediate:translucent:]` (894) | | |
| `-[Entity takeEnergyDamage:from:becauseOf:weaponIdentifier:]` (899) | | |
| `-[Entity dumpState]` (903) | | |
| `-[Entity dumpSelfState]` (918) | | |
| `-[Entity subEntityReallyDied:]` (958) | | |
| `-[Entity lastDrawCounter]` (963) | | |
| `-[Entity setLastDrawCounter:]` (968) | | |
| `-[Entity universalTime]` (975) | | |
| `-[Entity spawnTime]` (980) | | |
| `-[Entity timeElapsedSinceSpawn]` (985) | | |
| `-[Entity setAtmosphereFogging:]` (990) | | |
| `-[Entity fogUniform]` (996) | | |
| `-[Entity descriptionForObjDumpBasic]` (1002) | | |
| `-[Entity descriptionForObjDump]` (1013) | | |
| `-[Entity allTextures]` (1022) | | |
| `-[Entity isVisible]` (1028) | | |
| `-[Entity isInSpace]` (1033) | | |
| `-[Entity isImmuneToBreakPatternHide]` (1050) | | |

**Differences:**

- *pending*


## EntityShaderBindings

<a id="entityshaderbindings"></a>
Upstream `src/Core/Entities/EntityShaderBindings.m` ↔ `Core/Entities/EntityShaderBindings.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[Entity(ShaderBindings) clock]` (35) | | |
| `-[Entity(ShaderBindings) pseudoFixedD100]` (41) | | |
| `-[Entity(ShaderBindings) pseudoFixedD256]` (46) | | |
| `-[Entity(ShaderBindings) systemGovernment]` (52) | | |
| `-[Entity(ShaderBindings) systemEconomy]` (57) | | |
| `-[Entity(ShaderBindings) systemTechLevel]` (62) | | |
| `-[Entity(ShaderBindings) systemPopulation]` (67) | | |
| `-[Entity(ShaderBindings) systemProductivity]` (72) | | |

**Differences:**

- *pending*


## OOBreakPatternEntity

<a id="oobreakpatternentity"></a>
Upstream `src/Core/Entities/OOBreakPatternEntity.m` ↔ `Core/Entities/OOBreakPatternEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOBreakPatternEntity initWithPolygonSides:startAngle:aspectRatio:]` (41) | | |
| `+[OOBreakPatternEntity breakPatternWithPolygonSides:startAngle:aspectRatio:]` (78) | | |
| `-[OOBreakPatternEntity setInnerColor:outerColor:]` (83) | | |
| `-[OOBreakPatternEntity setInnerColorComponents:outerColorComponents:]` (91) | | |
| `-[OOBreakPatternEntity setLifetime:]` (101) | | |
| `-[OOBreakPatternEntity update:]` (106) | | |
| `-[OOBreakPatternEntity drawImmediate:translucent:]` (116) | | |
| `-[OOBreakPatternEntity canCollide]` (150) | | |
| `-[OOBreakPatternEntity isBreakPattern]` (155) | | |
| `-[Entity(OOBreakPatternEntity) isBreakPattern]` (164) | | |

**Differences:**

- *pending*


## OOECMBlastEntity

<a id="ooecmblastentity"></a>
Upstream `src/Core/Entities/OOECMBlastEntity.m` ↔ `Core/Entities/OOECMBlastEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOECMBlastEntity initFromShip:]` (45) | | |
| `-[OOECMBlastEntity update:]` (63) | | |
| `-[OOECMBlastEntity drawImmediate:translucent:]` (101) | | |
| `-[OOECMBlastEntity isECMBlast]` (109) | | |
| `-[Entity(OOECMBlastEntity) isECMBlast]` (118) | | |

**Differences:**

- *pending*


## OOEntityWithDrawable

<a id="ooentitywithdrawable"></a>
Upstream `src/Core/Entities/OOEntityWithDrawable.m` ↔ `Core/Entities/OOEntityWithDrawable.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOEntityWithDrawable dealloc]` (33) | | |
| `-[OOEntityWithDrawable drawable]` (41) | | |
| `-[OOEntityWithDrawable setDrawable:]` (46) | | |
| `-[OOEntityWithDrawable findCollisionRadius]` (59) | | |
| `-[OOEntityWithDrawable drawImmediate:translucent:]` (64) | | |
| `-[OOEntityWithDrawable allTextures]` (116) | | |

**Differences:**

- *pending*


## OOExhaustPlumeEntity

<a id="ooexhaustplumeentity"></a>
Upstream `src/Core/Entities/OOExhaustPlumeEntity.m` ↔ `Core/Entities/OOExhaustPlumeEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOExhaustPlumeEntity exhaustForShip:withDefinition:andScale:]` (57) | | |
| `-[OOExhaustPlumeEntity initForShip:withDefinition:andScale:]` (62) | | |
| `-[OOExhaustPlumeEntity scale]` (81) | | |
| `-[OOExhaustPlumeEntity setScale:]` (86) | | |
| `-[OOExhaustPlumeEntity isExhaust]` (94) | | |
| `-[OOExhaustPlumeEntity findCollisionRadius]` (99) | | |
| `-[OOExhaustPlumeEntity update:]` (104) | | |
| `-[OOExhaustPlumeEntity drawSubEntityImmediate:translucent:]` (359) | | |
| `-[OOExhaustPlumeEntity saveToLastFrame]` (542) | | |
| `-[OOExhaustPlumeEntity frameAtTime:fromFrame:]` (558) | | |
| `-[OOExhaustPlumeEntity resetPlume]` (610) | | |
| `-[OOExhaustPlumeEntity rescaleBy:]` (631) | | |
| `-[OOExhaustPlumeEntity rescaleBy:writeToCache:]` (636) | | |
| `-[OOExhaustPlumeEntity texture]` (642) | | |
| `+[OOExhaustPlumeEntity setUpTexture]` (647) | | |
| `+[OOExhaustPlumeEntity plumeTexture]` (659) | | |
| `+[OOExhaustPlumeEntity resetGraphicsState]` (666) | | |
| `-[Entity(OOExhaustPlume) isExhaust]` (676) | | |

**Differences:**

- *pending*


## OOExplosionCloudEntity

<a id="ooexplosioncloudentity"></a>
Upstream `src/Core/Entities/OOExplosionCloudEntity.m` ↔ `Core/Entities/OOExplosionCloudEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOExplosionCloudEntity initExplosionCloudWithEntity:size:andSettings:]` (57) | | |
| `-[OOExplosionCloudEntity dealloc]` (160) | | |
| `+[OOExplosionCloudEntity explosionCloudFromEntity:withSettings:]` (167) | | |
| `+[OOExplosionCloudEntity explosionCloudFromEntity:withSize:andSettings:]` (172) | | |
| `-[OOExplosionCloudEntity update:]` (177) | | |
| `-[OOExplosionCloudEntity texture]` (247) | | |

**Differences:**

- *pending*


## OOFlashEffectEntity

<a id="ooflasheffectentity"></a>
Upstream `src/Core/Entities/OOFlashEffectEntity.m` ↔ `Core/Entities/OOFlashEffectEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOFlashEffectEntity initExplosionFlashWithPosition:velocity:size:]` (55) | | |
| `-[OOFlashEffectEntity initLaserFlashWithPosition:velocity:color:]` (65) | | |
| `+[OOFlashEffectEntity explosionFlashFromEntity:]` (74) | | |
| `+[OOFlashEffectEntity laserFlashWithPosition:velocity:color:]` (79) | | |
| `-[OOFlashEffectEntity initWithPosition:size:color:duration:]` (84) | | |
| `-[OOFlashEffectEntity update:]` (96) | | |
| `-[OOFlashEffectEntity texture]` (115) | | |
| `+[OOFlashEffectEntity setUpTexture]` (122) | | |
| `+[OOFlashEffectEntity resetGraphicsState]` (134) | | |

**Differences:**

- *pending*


## OOFlasherEntity

<a id="ooflasherentity"></a>
Upstream `src/Core/Entities/OOFlasherEntity.m` ↔ `Core/Entities/OOFlasherEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOFlasherEntity flasherWithDictionary:]` (42) | | |
| `-[OOFlasherEntity initWithDictionary:]` (47) | | |
| `-[OOFlasherEntity dealloc]` (64) | | |
| `-[OOFlasherEntity setUpColors:]` (71) | | |
| `-[OOFlasherEntity getCurrentColorComponents]` (82) | | |
| `-[OOFlasherEntity isActive]` (87) | | |
| `-[OOFlasherEntity setActive:]` (92) | | |
| `-[OOFlasherEntity color]` (97) | | |
| `-[OOFlasherEntity frequency]` (105) | | |
| `-[OOFlasherEntity setFrequency:]` (110) | | |
| `-[OOFlasherEntity phase]` (115) | | |
| `-[OOFlasherEntity setPhase:]` (120) | | |
| `-[OOFlasherEntity fraction]` (125) | | |
| `-[OOFlasherEntity setFraction:]` (130) | | |
| `-[OOFlasherEntity update:]` (135) | | |
| `-[OOFlasherEntity drawImmediate:translucent:]` (173) | | |
| `-[OOFlasherEntity drawSubEntityImmediate:translucent:]` (180) | | |
| `-[OOFlasherEntity isFlasher]` (187) | | |
| `-[OOFlasherEntity findCollisionRadius]` (192) | | |
| `-[OOFlasherEntity rescaleBy:]` (197) | | |
| `-[OOFlasherEntity rescaleBy:writeToCache:]` (202) | | |
| `-[Entity(OOFlasherEntityExtensions) isFlasher]` (212) | | |

**Differences:**

- *pending*


## OOLaserShotEntity

<a id="oolasershotentity"></a>
Upstream `src/Core/Entities/OOLaserShotEntity.m` ↔ `Core/Entities/OOLaserShotEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOLaserShotEntity initLaserFromShip:direction:offset:]` (58) | | |
| `+[OOLaserShotEntity laserFromShip:direction:offset:]` (117) | | |
| `-[OOLaserShotEntity dealloc]` (122) | | |
| `-[OOLaserShotEntity descriptionComponents]` (129) | | |
| `-[OOLaserShotEntity setColor:]` (134) | | |
| `-[OOLaserShotEntity setRange:]` (142) | | |
| `-[OOLaserShotEntity update:]` (148) | | |
| `-[OOLaserShotEntity drawImmediate:translucent:]` (197) | | |
| `-[OOLaserShotEntity isEffect]` (255) | | |
| `-[OOLaserShotEntity canCollide]` (260) | | |
| `-[OOLaserShotEntity texture1]` (265) | | |
| `-[OOLaserShotEntity texture2]` (270) | | |
| `+[OOLaserShotEntity setUpTexture]` (275) | | |
| `+[OOLaserShotEntity innerTexture]` (293) | | |
| `+[OOLaserShotEntity outerTexture]` (300) | | |
| `+[OOLaserShotEntity resetGraphicsState]` (307) | | |

**Differences:**

- *pending*


## OOLightParticleEntity

<a id="oolightparticleentity"></a>
Upstream `src/Core/Entities/OOLightParticleEntity.m` ↔ `Core/Entities/OOLightParticleEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOLightParticleEntity initWithDiameter:]` (50) | | |
| `-[OOLightParticleEntity diameter]` (69) | | |
| `-[OOLightParticleEntity setDiameter:]` (74) | | |
| `-[OOLightParticleEntity setColor:]` (79) | | |
| `-[OOLightParticleEntity setColor:alpha:]` (84) | | |
| `-[OOLightParticleEntity drawSubEntityImmediate:translucent:]` (90) | | |
| `-[OOLightParticleEntity drawImmediate:translucent:]` (129) | | |
| `-[OOLightParticleEntity texture]` (282) | | |
| `+[OOLightParticleEntity setUpTexture]` (287) | | |
| `+[OOLightParticleEntity defaultParticleTexture]` (299) | | |
| `+[OOLightParticleEntity resetGraphicsState]` (306) | | |
| `-[OOLightParticleEntity isEffect]` (312) | | |
| `-[OOLightParticleEntity canCollide]` (317) | | |
| `-[OOLightParticleEntity allTextures]` (323) | | |

**Differences:**

- *pending*


## OOParticleSystem

<a id="ooparticlesystem"></a>
Upstream `src/Core/Entities/OOParticleSystem.m` ↔ `Core/Entities/OOParticleSystem.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOParticleSystem init]` (39) | | |
| `-[OOParticleSystem initWithPosition:]` (48) | | |
| `-[OOParticleSystem descriptionComponents]` (87) | | |
| `-[OOParticleSystem canCollide]` (92) | | |
| `-[OOParticleSystem checkCloseCollisionWith:]` (97) | | |
| `-[OOParticleSystem update:]` (104) | | |
| `-[OOParticleSystem drawImmediate:translucent:]` (136) | | |
| `-[OOParticleSystem isEffect]` (225) | | |
| `-[OOParticleSystem texture]` (230) | | |
| `-[OOParticleSystem allTextures]` (236) | | |
| `-[OOSmallFragmentBurstEntity initFragmentBurstFrom:velocity:size:]` (246) | | |
| `+[OOSmallFragmentBurstEntity fragmentBurstFromEntity:]` (272) | | |
| `-[OOSmallFragmentBurstEntity update:]` (277) | | |
| `-[OOBigFragmentBurstEntity initFragmentBurstFrom:velocity:size:]` (300) | | |
| `+[OOBigFragmentBurstEntity fragmentBurstFromEntity:]` (322) | | |
| `-[OOBigFragmentBurstEntity update:]` (327) | | |

**Differences:**

- *pending*


## OOPlanetEntity

<a id="ooplanetentity"></a>
Upstream `src/Core/Entities/OOPlanetEntity.m` ↔ `Core/Entities/OOPlanetEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPlanetEntity initAsMainPlanetForSystem:]` (70) | | |
| `-[OOPlanetEntity initFromDictionary:withAtmosphere:andSeed:forSystem:]` (84) | | |
| `RandomHSBColor()` (248) | | |
| `LighterHSBColor()` (257) | | |
| `HSBColorWithColor()` (266) | | |
| `ColorWithHSBColor()` (272) | | |
| `-[OOPlanetEntity setUpTypeParametersWithSourceInfo:targetInfo:]` (277) | | |
| `-[OOPlanetEntity setUpTerrainParametersWithSourceInfo:targetInfo:]` (283) | | |
| `-[OOPlanetEntity setUpLandParametersWithSourceInfo:targetInfo:]` (295) | | |
| `-[OOPlanetEntity setUpAtmosphereParametersWithSourceInfo:targetInfo:]` (300) | | |
| `-[OOPlanetEntity setUpColorParametersWithSourceInfo:targetInfo:isAtmosphere:]` (305) | | |
| `-[OOPlanetEntity initAsMiniatureVersionOfPlanet:]` (411) | | |
| `-[OOPlanetEntity dealloc]` (459) | | |
| `-[OOPlanetEntity descriptionComponents]` (477) | | |
| `-[OOPlanetEntity setOrientation:]` (482) | | |
| `-[OOPlanetEntity radius]` (488) | | |
| `-[OOPlanetEntity planetType]` (493) | | |
| `-[OOPlanetEntity miniatureVersion]` (502) | | |
| `-[OOPlanetEntity update:]` (507) | | |
| `-[OOPlanetEntity isFinishedLoading]` (605) | | |
| `-[OOPlanetEntity drawImmediate:translucent:]` (619) | | |
| `-[OOPlanetEntity checkCloseCollisionWith:]` (700) | | |
| `-[OOPlanetEntity planetHasStation]` (714) | | |
| `-[OOPlanetEntity launchShuttle]` (729) | | |
| `-[OOPlanetEntity welcomeShuttle:]` (769) | | |
| `-[OOPlanetEntity isPlanet]` (774) | | |
| `-[OOPlanetEntity isVisible]` (779) | | |
| `-[OOPlanetEntity rotationalVelocity]` (784) | | |
| `-[OOPlanetEntity setRotationalVelocity:]` (789) | | |
| `-[OOPlanetEntity airColorAsVector]` (798) | | |
| `-[OOPlanetEntity airColor]` (805) | | |
| `-[OOPlanetEntity setAirColor:]` (810) | | |
| `-[OOPlanetEntity illuminationColorAsVector]` (819) | | |
| `-[OOPlanetEntity illuminationColor]` (826) | | |
| `-[OOPlanetEntity setIlluminationColor:]` (831) | | |
| `-[OOPlanetEntity airColorMixRatio]` (840) | | |
| `-[OOPlanetEntity setAirColorMixRatio:]` (845) | | |
| `-[OOPlanetEntity airDensity]` (851) | | |
| `-[OOPlanetEntity setAirDensity:]` (856) | | |
| `-[OOPlanetEntity setTerminatorThresholdVector:]` (861) | | |
| `-[OOPlanetEntity terminatorThresholdVector]` (868) | | |
| `-[OOPlanetEntity hasAtmosphere]` (873) | | |
| `-[OOPlanetEntity textureFileName]` (879) | | |
| `-[OOPlanetEntity resetGraphicsState]` (884) | | |
| `-[OOPlanetEntity setTextureFileName:]` (890) | | |
| `-[OOPlanetEntity setUpPlanetFromTexture:]` (1016) | | |
| `-[OOPlanetEntity material]` (1022) | | |
| `-[OOPlanetEntity atmosphereMaterial]` (1027) | | |
| `-[OOPlanetEntity atmosphereShaderMaterial]` (1032) | | |
| `-[OOPlanetEntity name]` (1039) | | |
| `-[OOPlanetEntity setName:]` (1044) | | |

**Differences:**

- *pending*


## OOPlasmaBurstEntity

<a id="ooplasmaburstentity"></a>
Upstream `src/Core/Entities/OOPlasmaBurstEntity.m` ↔ `Core/Entities/OOPlasmaBurstEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPlasmaBurstEntity initWithPosition:]` (35) | | |
| `-[OOPlasmaBurstEntity update:]` (47) | | |

**Differences:**

- *pending*


## OOPlasmaShotEntity

<a id="ooplasmashotentity"></a>
Upstream `src/Core/Entities/OOPlasmaShotEntity.m` ↔ `Core/Entities/OOPlasmaShotEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPlasmaShotEntity initWithPosition:]` (43) | | |
| `-[OOPlasmaShotEntity canCollide]` (64) | | |
| `-[OOPlasmaShotEntity checkCloseCollisionWith:]` (69) | | |
| `-[OOPlasmaShotEntity update:]` (74) | | |

**Differences:**

- *pending*


## OOQuiriumCascadeEntity

<a id="ooquiriumcascadeentity"></a>
Upstream `src/Core/Entities/OOQuiriumCascadeEntity.m` ↔ `Core/Entities/OOQuiriumCascadeEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOQuiriumCascadeEntity initQuiriumCascadeFromShip:]` (36) | | |
| `+[OOQuiriumCascadeEntity quiriumCascadeFromShip:]` (59) | | |
| `-[OOQuiriumCascadeEntity descriptionComponents]` (64) | | |
| `-[OOQuiriumCascadeEntity update:]` (69) | | |
| `-[OOQuiriumCascadeEntity drawImmediate:translucent:]` (110) | | |
| `-[OOQuiriumCascadeEntity isEffect]` (136) | | |
| `-[OOQuiriumCascadeEntity isCascadeWeapon]` (141) | | |
| `-[OOQuiriumCascadeEntity canCollide]` (146) | | |
| `-[OOQuiriumCascadeEntity checkCloseCollisionWith:]` (151) | | |
| `-[Entity(OOQuiriumCascadeExtensions) isCascadeWeapon]` (160) | | |

**Differences:**

- *pending*


## OORingEffectEntity

<a id="ooringeffectentity"></a>
Upstream `src/Core/Entities/OORingEffectEntity.m` ↔ `Core/Entities/OORingEffectEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OORingEffectEntity initialize]` (52) | | |
| `-[OORingEffectEntity initRingFromEntity:]` (61) | | |
| `+[OORingEffectEntity ringFromEntity:]` (88) | | |
| `+[OORingEffectEntity shrinkingRingFromEntity:]` (93) | | |
| `-[OORingEffectEntity descriptionComponents]` (103) | | |
| `-[OORingEffectEntity update:]` (108) | | |
| `-[OORingEffectEntity drawImmediate:translucent:]` (121) | | |
| `-[OORingEffectEntity isEffect]` (147) | | |
| `-[OORingEffectEntity canCollide]` (152) | | |

**Differences:**

- *pending*


## OOSparkEntity

<a id="oosparkentity"></a>
Upstream `src/Core/Entities/OOSparkEntity.m` ↔ `Core/Entities/OOSparkEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSparkEntity initWithPosition:]` (39) | | |
| `-[OOSparkEntity update:]` (58) | | |
| `-[OOSparkEntity performUpdate:]` (64) | | |

**Differences:**

- *pending*


## OOSunEntity

<a id="oosunentity"></a>
Upstream `src/Core/Entities/OOSunEntity.m` ↔ `Core/Entities/OOSunEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSunEntity init]` (52) | | |
| `-[OOSunEntity setSunColor:]` (59) | | |
| `-[OOSunEntity initSunWithColor:andDictionary:]` (123) | | |
| `-[OOSunEntity dealloc]` (203) | | |
| `-[OOSunEntity descriptionComponents]` (209) | | |
| `-[OOSunEntity canCollide]` (221) | | |
| `-[OOSunEntity checkCloseCollisionWith:]` (227) | | |
| `-[OOSunEntity update:]` (237) | | |
| `-[OOSunEntity drawImmediate:translucent:]` (307) | | |
| `-[OOSunEntity updateCameraRelativePosition]` (327) | | |
| `-[OOSunEntity drawOpaqueParts]` (339) | | |
| `-[OOSunEntity drawTranslucentParts]` (419) | | |
| `-[OOSunEntity calculateGLArrays:width:zDistance:]` (438) | | |
| `-[OOSunEntity drawDirectVisionSunGlare]` (550) | | |
| `-[OOSunEntity drawStarGlare]` (584) | | |
| `-[OOSunEntity changeSunProperty:withDictionary:]` (616) | | |
| `-[OOSunEntity planetType]` (647) | | |
| `-[OOSunEntity getDiffuseComponents:]` (652) | | |
| `-[OOSunEntity getSpecularComponents:]` (658) | | |
| `-[OOSunEntity radius]` (664) | | |
| `-[OOSunEntity setRadius:andCorona:]` (669) | | |
| `-[OOSunEntity setPosition:]` (682) | | |
| `-[OOSunEntity willGoNova]` (688) | | |
| `-[OOSunEntity goneNova]` (693) | | |
| `-[OOSunEntity setGoingNova:inTime:]` (698) | | |
| `-[OOSunEntity resetNova]` (710) | | |
| `-[OOSunEntity isSun]` (717) | | |
| `-[OOSunEntity isVisible]` (722) | | |
| `-[OOSunEntity name]` (727) | | |
| `-[OOSunEntity setName:]` (732) | | |

**Differences:**

- *pending*


## OOVisualEffectEntity

<a id="oovisualeffectentity"></a>
Upstream `src/Core/Entities/OOVisualEffectEntity.m` ↔ `Core/Entities/OOVisualEffectEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOVisualEffectEntity init]` (69) | | |
| `-[OOVisualEffectEntity initWithKey:definition:]` (74) | | |
| `-[OOVisualEffectEntity setUpVisualEffectFromDictionary:]` (98) | | |
| `-[OOVisualEffectEntity dealloc]` (159) | | |
| `-[OOVisualEffectEntity isEffect]` (175) | | |
| `-[OOVisualEffectEntity isVisualEffect]` (180) | | |
| `-[OOVisualEffectEntity canCollide]` (185) | | |
| `-[OOVisualEffectEntity mesh]` (190) | | |
| `-[OOVisualEffectEntity setMesh:]` (195) | | |
| `-[OOVisualEffectEntity effectKey]` (202) | | |
| `-[OOVisualEffectEntity frustumRadius]` (207) | | |
| `-[OOVisualEffectEntity clearSubEntities]` (212) | | |
| `-[OOVisualEffectEntity setUpSubEntities]` (227) | | |
| `-[OOVisualEffectEntity removeSubEntity:]` (242) | | |
| `-[OOVisualEffectEntity setNoDrawDistance]` (248) | | |
| `-[OOVisualEffectEntity setUpOneSubentity:]` (254) | | |
| `-[OOVisualEffectEntity setUpOneFlasher:]` (264) | | |
| `-[OOVisualEffectEntity setUpOneStandardSubentity:]` (272) | | |
| `-[OOVisualEffectEntity addSubEntity:]` (304) | | |
| `-[OOVisualEffectEntity subEntities]` (322) | | |
| `-[OOVisualEffectEntity subEntityCount]` (327) | | |
| `-[OOVisualEffectEntity visualEffectSubEntityEnumerator]` (332) | | |
| `-[OOVisualEffectEntity hasSubEntity:]` (337) | | |
| `-[OOVisualEffectEntity subEntityEnumerator]` (342) | | |
| `-[OOVisualEffectEntity effectSubEntityEnumerator]` (347) | | |
| `-[OOVisualEffectEntity flasherEnumerator]` (352) | | |
| `-[OOVisualEffectEntity drawSubEntityImmediate:translucent:]` (357) | | |
| `-[OOVisualEffectEntity rescaleBy:]` (372) | | |
| `-[OOVisualEffectEntity rescaleBy:writeToCache:]` (389) | | |
| `-[OOVisualEffectEntity scaleMax]` (395) | | |
| `-[OOVisualEffectEntity scaleX]` (412) | | |
| `-[OOVisualEffectEntity setScaleX:]` (417) | | |
| `-[OOVisualEffectEntity scaleY]` (437) | | |
| `-[OOVisualEffectEntity setScaleY:]` (442) | | |
| `-[OOVisualEffectEntity scaleZ]` (462) | | |
| `-[OOVisualEffectEntity setScaleZ:]` (467) | | |
| `-[OOVisualEffectEntity collisionRadius]` (487) | | |
| `-[OOVisualEffectEntity orientationChanged]` (492) | | |
| `-[OOVisualEffectEntity forwardVector]` (502) | | |
| `-[OOVisualEffectEntity upVector]` (508) | | |
| `-[OOVisualEffectEntity rightVector]` (514) | | |
| `-[OOVisualEffectEntity scannerDisplayColor1]` (519) | | |
| `-[OOVisualEffectEntity scannerDisplayColor2]` (524) | | |
| `-[OOVisualEffectEntity setScannerDisplayColor1:]` (529) | | |
| `-[OOVisualEffectEntity setScannerDisplayColor2:]` (538) | | |
| `-[OOVisualEffectEntity scannerDisplayColorForShip:scannerDisplayColor1:scannerDisplayColor2:]` (550) | | |
| `-[OOVisualEffectEntity drawImmediate:translucent:]` (575) | | |
| `-[OOVisualEffectEntity update:]` (598) | | |
| `-[OOVisualEffectEntity isBreakPattern]` (613) | | |
| `-[OOVisualEffectEntity setIsBreakPattern:]` (618) | | |
| `-[OOVisualEffectEntity effectInfoDictionary]` (623) | | |
| `-[OOVisualEffectEntity setScript:]` (630) | | |
| `-[OOVisualEffectEntity script]` (646) | | |
| `-[OOVisualEffectEntity scriptInfo]` (651) | | |
| `-[OOVisualEffectEntity doScriptEvent:]` (657) | | |
| `-[OOVisualEffectEntity remove]` (664) | | |
| `-[OOVisualEffectEntity compareBeaconCodeWith:]` (672) | | |
| `-[OOVisualEffectEntity beaconCode]` (677) | | |
| `-[OOVisualEffectEntity setBeaconCode:]` (682) | | |
| `-[OOVisualEffectEntity beaconLabel]` (699) | | |
| `-[OOVisualEffectEntity setBeaconLabel:]` (704) | | |
| `-[OOVisualEffectEntity isBeacon]` (715) | | |
| `-[OOVisualEffectEntity beaconDrawable]` (720) | | |
| `-[OOVisualEffectEntity prevBeacon]` (743) | | |
| `-[OOVisualEffectEntity nextBeacon]` (748) | | |
| `-[OOVisualEffectEntity setPrevBeacon:]` (753) | | |
| `-[OOVisualEffectEntity setNextBeacon:]` (761) | | |
| `-[OOVisualEffectEntity isJammingScanning]` (769) | | |
| `-[OOVisualEffectEntity hullHeatLevel]` (777) | | |
| `-[OOVisualEffectEntity setHullHeatLevel:]` (782) | | |
| `-[OOVisualEffectEntity shaderFloat1]` (787) | | |
| `-[OOVisualEffectEntity setShaderFloat1:]` (792) | | |
| `-[OOVisualEffectEntity shaderFloat2]` (797) | | |
| `-[OOVisualEffectEntity setShaderFloat2:]` (802) | | |
| `-[OOVisualEffectEntity shaderInt1]` (807) | | |
| `-[OOVisualEffectEntity setShaderInt1:]` (812) | | |
| `-[OOVisualEffectEntity shaderInt2]` (817) | | |
| `-[OOVisualEffectEntity setShaderInt2:]` (822) | | |
| `-[OOVisualEffectEntity shaderVector1]` (827) | | |
| `-[OOVisualEffectEntity setShaderVector1:]` (832) | | |
| `-[OOVisualEffectEntity shaderVector2]` (837) | | |
| `-[OOVisualEffectEntity setShaderVector2:]` (842) | | |
| `-[OOVisualEffectEntity(SubEntityRelationship) isShipWithSubEntityShip:]` (852) | | |

**Differences:**

- *pending*


## OOWaypointEntity

<a id="oowaypointentity"></a>
Upstream `src/Core/Entities/OOWaypointEntity.m` ↔ `Core/Entities/OOWaypointEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOWaypointEntity waypointWithDictionary:]` (43) | | |
| `-[OOWaypointEntity initWithDictionary:]` (48) | | |
| `-[OOWaypointEntity dealloc]` (68) | | |
| `-[OOWaypointEntity setOrientation:]` (80) | | |
| `-[OOWaypointEntity oriented]` (91) | | |
| `-[OOWaypointEntity size]` (96) | | |
| `-[OOWaypointEntity setSize:]` (101) | | |
| `-[OOWaypointEntity isEffect]` (109) | | |
| `-[OOWaypointEntity isWaypoint]` (114) | | |
| `-[OOWaypointEntity drawImmediate:translucent:]` (119) | | |
| `-[OOWaypointEntity compareBeaconCodeWith:]` (187) | | |
| `-[OOWaypointEntity beaconCode]` (192) | | |
| `-[OOWaypointEntity setBeaconCode:]` (197) | | |
| `-[OOWaypointEntity beaconLabel]` (214) | | |
| `-[OOWaypointEntity setBeaconLabel:]` (219) | | |
| `-[OOWaypointEntity isBeacon]` (230) | | |
| `-[OOWaypointEntity beaconDrawable]` (235) | | |
| `-[OOWaypointEntity prevBeacon]` (258) | | |
| `-[OOWaypointEntity nextBeacon]` (263) | | |
| `-[OOWaypointEntity setPrevBeacon:]` (268) | | |
| `-[OOWaypointEntity setNextBeacon:]` (276) | | |
| `-[OOWaypointEntity isJammingScanning]` (284) | | |

**Differences:**

- *pending*


## PlanetEntity

<a id="planetentity"></a>
Upstream `src/Core/Entities/PlanetEntity.m` ↔ *(no counterpart; check whether upstream builds it)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[PlanetEntity init]` (180) | | |
| `-[PlanetEntity initAsAtmosphereForPlanet:dictionary:]` (187) | | |
| `-[PlanetEntity miniaturize]` (334) | | |
| `-[PlanetEntity initFromDictionary:withAtmosphere:andSeed:]` (351) | | |
| `-[PlanetEntity dealloc]` (554) | | |
| `-[PlanetEntity descriptionComponents]` (567) | | |
| `-[PlanetEntity canCollide]` (590) | | |
| `-[PlanetEntity checkCloseCollisionWith:]` (606) | | |
| `-[PlanetEntity update:]` (631) | | |
| `-[PlanetEntity setPosition:]` (695) | | |
| `-[PlanetEntity setOrientation:]` (701) | | |
| `-[PlanetEntity setUseTexturedModel:]` (709) | | |
| `-[PlanetEntity drawImmediate:translucent:]` (721) | | |
| `-[PlanetEntity drawUnconditionally]` (739) | | |
| `-[PlanetEntity atmosphere]` (921) | | |
| `-[PlanetEntity planet_seed]` (927) | | |
| `-[PlanetEntity isTextured]` (932) | | |
| `-[PlanetEntity textureFileName]` (937) | | |
| `-[PlanetEntity setTextureColorForPlanet:inSystem:]` (942) | | |
| `-[PlanetEntity setUpPlanetFromTexture:]` (971) | | |
| `-[PlanetEntity polar_color_factor]` (1008) | | |
| `-[PlanetEntity amb_land]` (1013) | | |
| `-[PlanetEntity amb_polar_land]` (1018) | | |
| `-[PlanetEntity amb_sea]` (1023) | | |
| `-[PlanetEntity amb_polar_sea]` (1028) | | |
| `-[PlanetEntity planetType]` (1033) | | |
| `-[PlanetEntity setPlanetType:]` (1038) | | |
| `-[PlanetEntity radius]` (1043) | | |
| `-[PlanetEntity setRadius:]` (1048) | | |
| `-[PlanetEntity rotationalVelocity]` (1053) | | |
| `-[PlanetEntity setRotationalVelocity:]` (1058) | | |
| `-[PlanetEntity sqrtZeroDistance]` (1067) | | |
| `-[PlanetEntity hasAtmosphere]` (1072) | | |
| `-[PlanetEntity drawModelWithVertexArraysAndSubdivision:]` (1077) | | |
| `-[PlanetEntity launchShuttle]` (1082) | | |
| `-[PlanetEntity welcomeShuttle:]` (1115) | | |
| `-[PlanetEntity initialiseBaseVertexArray]` (1120) | | |
| `baseVertexIndexForEdge()` (1212) | | |
| `-[PlanetEntity initialiseBaseTerrainArray:]` (1255) | | |
| `-[PlanetEntity paintVertex:seed:]` (1320) | | |
| `-[PlanetEntity scaleVertices]` (1369) | | |
| `-[PlanetEntity deleteDisplayLists]` (1382) | | |
| `-[PlanetEntity resetGraphicsState]` (1393) | | |
| `-[PlanetEntity isExplicitlyTextured]` (1398) | | |
| `-[PlanetEntity texture]` (1403) | | |
| `-[PlanetEntity loadTexture:]` (1408) | | |
| `-[PlanetEntity planetTextureWithInfo:]` (1430) | | |
| `-[PlanetEntity cloudTextureWithCloudColor:cloudImpress:cloudBias:]` (1452) | | |
| `-[PlanetEntity allTextures]` (1477) | | |
| `-[PlanetEntity isPlanet]` (1486) | | |
| `-[PlanetEntity isVisible]` (1491) | | |

**Differences:**

- *pending*


## SkyEntity

<a id="skyentity"></a>
Upstream `src/Core/Entities/SkyEntity.m` ↔ `Core/Entities/SkyEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[SkyEntity initWithColors:col2:andSystemInfo:]` (51) | | |
| `-[SkyEntity dealloc]` (138) | | |
| `-[SkyEntity skyColor]` (145) | | |
| `-[SkyEntity changeProperty:withDictionary:]` (150) | | |
| `-[SkyEntity update:]` (170) | | |
| `-[SkyEntity isSky]` (182) | | |
| `-[SkyEntity isVisible]` (187) | | |
| `-[SkyEntity canCollide]` (192) | | |
| `-[SkyEntity cameraRangeFront]` (197) | | |
| `-[SkyEntity cameraRangeBack]` (202) | | |
| `-[SkyEntity drawImmediate:translucent:]` (207) | | |
| `-[SkyEntity descriptionForObjDump]` (218) | | |
| `-[SkyEntity(OOPrivate) readColor1:andColor2:andColor3:andColor4:fromDictionary:]` (229) | | |

**Differences:**

- *pending*


## WormholeEntity

<a id="wormholeentity"></a>
Upstream `src/Core/Entities/WormholeEntity.m` ↔ `Core/Entities/WormholeEntity.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[WormholeEntity(Private) init]` (58) | | |
| `-[WormholeEntity initWithDict:]` (84) | | |
| `-[WormholeEntity initWormholeTo:fromShip:]` (144) | | |
| `-[WormholeEntity setMisjump]` (189) | | |
| `-[WormholeEntity setMisjumpWithRange:]` (204) | | |
| `-[WormholeEntity withMisjump]` (226) | | |
| `-[WormholeEntity misjumpRange]` (231) | | |
| `-[WormholeEntity suckInShip:]` (236) | | |
| `-[WormholeEntity disgorgeShips]` (317) | | |
| `-[WormholeEntity setContainsPlayer:]` (448) | | |
| `-[WormholeEntity setExitPosition:]` (453) | | |
| `-[WormholeEntity origin]` (459) | | |
| `-[WormholeEntity destination]` (464) | | |
| `-[WormholeEntity originCoordinates]` (469) | | |
| `-[WormholeEntity destinationCoordinates]` (474) | | |
| `-[WormholeEntity exitSpeed]` (479) | | |
| `-[WormholeEntity setExitSpeed:]` (484) | | |
| `-[WormholeEntity expiryTime]` (489) | | |
| `-[WormholeEntity arrivalTime]` (494) | | |
| `-[WormholeEntity estimatedArrivalTime]` (499) | | |
| `-[WormholeEntity travelTime]` (504) | | |
| `-[WormholeEntity scanTime]` (509) | | |
| `-[WormholeEntity isScanned]` (514) | | |
| `-[WormholeEntity setScannedAt:]` (519) | | |
| `-[WormholeEntity scanInfo]` (528) | | |
| `-[WormholeEntity setScanInfo:]` (533) | | |
| `-[WormholeEntity shipsInTransit]` (538) | | |
| `-[WormholeEntity dealloc]` (543) | | |
| `-[WormholeEntity descriptionComponents]` (550) | | |
| `-[WormholeEntity identFromShip:]` (559) | | |
| `-[WormholeEntity canCollide]` (578) | | |
| `-[WormholeEntity checkCloseCollisionWith:]` (593) | | |
| `-[WormholeEntity update:]` (598) | | |
| `-[WormholeEntity drawImmediate:translucent:]` (626) | | |
| `DrawWormholeCorona()` (669) | | |
| `-[WormholeEntity getDict]` (729) | | |
| `-[WormholeEntity scanInfoString]` (761) | | |
| `-[WormholeEntity dumpSelfState]` (780) | | |

**Differences:**

- *pending*

