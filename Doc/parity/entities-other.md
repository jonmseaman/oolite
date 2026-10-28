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

**Status:** done — 10 rows: 8 ✅ 0 ⚠️ 0 ❌ 2 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOBreakPatternEntity initWithPolygonSides:startAngle:aspectRatio:]` (41) | `OOBreakPatternEntity::initWithPolygonSides` `Core/Entities/OOBreakPatternEntity.cpp:34` | ✅ |
| `+[OOBreakPatternEntity breakPatternWithPolygonSides:startAngle:aspectRatio:]` (78) | `OOBreakPatternEntity::breakPatternWithPolygonSides` `Core/Entities/OOBreakPatternEntity.cpp:70` | ✅ |
| `-[OOBreakPatternEntity setInnerColor:outerColor:]` (83) | `OOBreakPatternEntity::setInnerColor` `Core/Entities/OOBreakPatternEntity.cpp:77` | ✅ |
| `-[OOBreakPatternEntity setInnerColorComponents:outerColorComponents:]` (91) | `OOBreakPatternEntity::setInnerColorComponents` `Core/Entities/OOBreakPatternEntity.cpp:88` | ✅ |
| `-[OOBreakPatternEntity setLifetime:]` (101) | `OOBreakPatternEntity::setLifetime` `Core/Entities/OOBreakPatternEntity.cpp:98` | ✅ |
| `-[OOBreakPatternEntity update:]` (106) | `OOBreakPatternEntity::update` `Core/Entities/OOBreakPatternEntity.cpp:103` | ✅ |
| `-[OOBreakPatternEntity drawImmediate:translucent:]` (116) | `OOBreakPatternEntity::drawImmediate` `Core/Entities/OOBreakPatternEntity.cpp:114` | ✅ |
| `-[OOBreakPatternEntity canCollide]` (150) | `OOBreakPatternEntity::canCollide` `Core/Entities/OOBreakPatternEntity.cpp:148` | ✅ |
| `-[OOBreakPatternEntity isBreakPattern]` (155) | none; `Universe::removeEntity` uses `dynamic_cast<OOBreakPatternEntity*>` `Core/Universe.cpp:10114` | ➖ |
| `-[Entity(OOBreakPatternEntity) isBreakPattern]` (164) | none; `OOVisualEffectEntity::isBreakPattern` `Core/Entities/OOVisualEffectEntity.cpp:651` keeps the VE flag | ➖ |

**Differences:**

- ➖ **low** — `-isBreakPattern` on the ring and on the Entity category are gone. Their only real caller, `Universe -removeEntity:`'s `[entity isBreakPattern] && ![entity isVisualEffect]` breakPatternCounter test, is now `dynamic_cast<OOBreakPatternEntity*>`, which is equivalent. The by-name table answers `isBreakPattern` only for visual effects (`Core/Entities/OOEntityCalledByNameTable.h:2026`), so a by-name query on a core ring now answers NO where it answered YES. Scripts cannot reach a core ring — upstream `Core/Entities/OOBreakPatternEntity.m:155` ↔ port `Core/Universe.cpp:10114`
- Constants are unchanged: 3..128 sides, vertex radii 50/40, z −40/0, inner colour (1,0,0,0.5), outer colour (0,0,1,0.25), `BREAK_PATTERN_RING_SPEED` 200.

## OOECMBlastEntity

<a id="ooecmblastentity"></a>
Upstream `src/Core/Entities/OOECMBlastEntity.m` ↔ `Core/Entities/OOECMBlastEntity.cpp`

**Status:** done — 5 rows: 4 ✅ 0 ⚠️ 0 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOECMBlastEntity initFromShip:]` (45) | `OOECMBlastEntity::initFromShip` `Core/Entities/OOECMBlastEntity.cpp:60` + ctor `:48` | ✅ |
| `-[OOECMBlastEntity update:]` (63) | `OOECMBlastEntity::update` `Core/Entities/OOECMBlastEntity.cpp:68` | ✅ |
| `-[OOECMBlastEntity drawImmediate:translucent:]` (101) | `OOECMBlastEntity::drawImmediate` `Core/Entities/OOECMBlastEntity.cpp:107` | ✅ |
| `-[OOECMBlastEntity isECMBlast]` (109) | `OOECMBlastEntity::isECMBlast` `Core/Entities/OOECMBlastEntity.cpp:115` | ✅ |
| `-[Entity(OOECMBlastEntity) isECMBlast]` (118) | none (header says nothing sent it; bead oo-9ht.75) | ➖ |

**Differences:**

- ➖ **low** — the Entity category `-isECMBlast` was removed. Nothing upstream sent it — upstream `Core/Entities/OOECMBlastEntity.m:118` ↔ port `Core/Entities/OOECMBlastEntity.h:11`
- Gameplay check: `ECM_EFFECT_DURATION` 2.0, `ECM_PULSE_COUNT` 4, interval 0.5 s and `_blastsRemaining` `uint8_t` are all unchanged. Pulse radius is `clamp01((4−remaining+1)/4) × SCANNER_MAX_RANGE`, decremented before the event. The order is the same: `shipHitByECM(remaining, whom)`, then the `ECM` AI message, then `noticeECM`. Removal happens on `remaining==0 || !validShip`. All ✅.

## OOEntityWithDrawable

<a id="ooentitywithdrawable"></a>
Upstream `src/Core/Entities/OOEntityWithDrawable.m` ↔ `Core/Entities/OOEntityWithDrawable.cpp`

**Status:** done — 6 rows: 5 ✅ 0 ⚠️ 0 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOEntityWithDrawable dealloc]` (33) | `oo::Ref<OODrawable> drawable` member `Core/Entities/OOEntityWithDrawable.h:52` | ➖ |
| `-[OOEntityWithDrawable drawable]` (41) | `OOEntityWithDrawable::getDrawable` `Core/Entities/OOEntityWithDrawable.cpp:35` | ✅ |
| `-[OOEntityWithDrawable setDrawable:]` (46) | `OOEntityWithDrawable::setDrawable` `Core/Entities/OOEntityWithDrawable.cpp:40` | ✅ |
| `-[OOEntityWithDrawable findCollisionRadius]` (59) | `OOEntityWithDrawable::findCollisionRadius` `Core/Entities/OOEntityWithDrawable.cpp:56` | ✅ |
| `-[OOEntityWithDrawable drawImmediate:translucent:]` (64) | `OOEntityWithDrawable::drawImmediate` `Core/Entities/OOEntityWithDrawable.cpp:62` | ✅ |
| `-[OOEntityWithDrawable allTextures]` (116) | `OOEntityWithDrawable::allTextures` `Core/Entities/OOEntityWithDrawable.cpp:115` | ✅ |

**Differences:**

- ➖ **low** — `-dealloc` release is now RAII. The old drawable is still deferred via `OODrawableAutorelease`, as `[drawable autorelease]` was — upstream `Core/Entities/OOEntityWithDrawable.m:33` ↔ port `Core/Entities/OOEntityWithDrawable.cpp:43`
- Subtle (✅): the frustum clip radius calls the virtual `frustumRadius()` only for ships and visual effects, which are its only overrides (`ShipEntity.cpp:1186`, `OOVisualEffectEntity.cpp:245`). Other entities use `collision_radius`, as upstream does.

## OOExhaustPlumeEntity

<a id="ooexhaustplumeentity"></a>
Upstream `src/Core/Entities/OOExhaustPlumeEntity.m` ↔ `Core/Entities/OOExhaustPlumeEntity.cpp`

**Status:** done — 18 rows: 16 ✅ 0 ⚠️ 1 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOExhaustPlumeEntity exhaustForShip:withDefinition:andScale:]` (57) | `OOExhaustPlumeEntity::exhaustForShip` `Core/Entities/OOExhaustPlumeEntity.cpp:61` | ✅ |
| `-[OOExhaustPlumeEntity initForShip:withDefinition:andScale:]` (62) | `OOExhaustPlumeEntity::initForShip` `Core/Entities/OOExhaustPlumeEntity.cpp:69` | ✅ |
| `-[OOExhaustPlumeEntity scale]` (81) | `OOExhaustPlumeEntity::scale` `Core/Entities/OOExhaustPlumeEntity.cpp:90` | ✅ |
| `-[OOExhaustPlumeEntity setScale:]` (86) | `OOExhaustPlumeEntity::setScale` `Core/Entities/OOExhaustPlumeEntity.cpp:95` | ✅ |
| `-[OOExhaustPlumeEntity isExhaust]` (94) | `OOExhaustPlumeEntity::isExhaust` `Core/Entities/OOExhaustPlumeEntity.cpp:103` | ✅ |
| `-[OOExhaustPlumeEntity findCollisionRadius]` (99) | `OOExhaustPlumeEntity::findCollisionRadius` `Core/Entities/OOExhaustPlumeEntity.cpp:108` | ✅ |
| `-[OOExhaustPlumeEntity update:]` (104) | `OOExhaustPlumeEntity::update` `Core/Entities/OOExhaustPlumeEntity.cpp:113` | ✅ |
| `-[OOExhaustPlumeEntity drawSubEntityImmediate:translucent:]` (359) | `OOExhaustPlumeEntity::drawSubEntityImmediate` `Core/Entities/OOExhaustPlumeEntity.cpp:370` | ✅ |
| `-[OOExhaustPlumeEntity saveToLastFrame]` (542) | `OOExhaustPlumeEntity::saveToLastFrame` `Core/Entities/OOExhaustPlumeEntity.cpp:554` | ✅ |
| `-[OOExhaustPlumeEntity frameAtTime:fromFrame:]` (558) | `OOExhaustPlumeEntity::frameAtTime` `Core/Entities/OOExhaustPlumeEntity.cpp:570` | ✅ |
| `-[OOExhaustPlumeEntity resetPlume]` (610) | `OOExhaustPlumeEntity::resetPlume` `Core/Entities/OOExhaustPlumeEntity.cpp:622` | ✅ |
| `-[OOExhaustPlumeEntity rescaleBy:]` (631) | `OOExhaustPlumeEntity::rescaleBy(GLfloat)` `Core/Entities/OOExhaustPlumeEntity.cpp:643` | ✅ |
| `-[OOExhaustPlumeEntity rescaleBy:writeToCache:]` (636) | `OOExhaustPlumeEntity::rescaleBy(GLfloat,bool)` `Core/Entities/OOExhaustPlumeEntity.cpp:648` | ✅ |
| `-[OOExhaustPlumeEntity texture]` (642) | `OOExhaustPlumeEntity::texture` `Core/Entities/OOExhaustPlumeEntity.cpp:654` | ✅ |
| `+[OOExhaustPlumeEntity setUpTexture]` (647) | `OOExhaustPlumeEntity::setUpTexture` `Core/Entities/OOExhaustPlumeEntity.cpp:659` | ❌ |
| `+[OOExhaustPlumeEntity plumeTexture]` (659) | `OOExhaustPlumeEntity::plumeTexture` `Core/Entities/OOExhaustPlumeEntity.cpp:667` | ✅ |
| `+[OOExhaustPlumeEntity resetGraphicsState]` (666) | `OOExhaustPlumeEntity::resetGraphicsState` `Core/Entities/OOExhaustPlumeEntity.cpp:674` | ✅ |
| `-[Entity(OOExhaustPlume) isExhaust]` (676) | none (category removed) | ➖ |

**Differences:**

- ❌ **low** — `setUpTexture` registers a fresh `new PlumeTextureResetClient` each time the texture is recreated. Upstream registered the class pointer into a set, which is idempotent. The manager's `unordered_set` is keyed by pointer, so every graphics reset followed by a redraw adds one more client. The result is a small leak and N redundant (harmless) `resetGraphicsState` calls — upstream `Core/Entities/OOExhaustPlumeEntity.m:655` ↔ port `Core/Entities/OOExhaustPlumeEntity.cpp:663`
- ➖ **low** — the `Entity(OOExhaustPlume) -isExhaust` category was removed and the subclass keeps `isExhaust()` — upstream `Core/Entities/OOExhaustPlumeEntity.m:676`
- Subtle (✅): the definition is now `std::vector<std::string>` tokens read through `oo::PList::at<float>`. Out-of-range indices give 0, as `oo_floatAtIndex:` did (`oofnd/PList.hpp:258`). Geometry is unchanged: 16 frames, `kTimeStep` 0.05, i03/06/08/10 = −0.12/−0.25/−0.32/−0.40 × scale.z, fade levels 0.4/0.2/0.02, scale levels 1.0/0.8/0.6 with extra 0.8/0.5, z-scale clamp [0.5,2.0]→1. The RNG order is unchanged: one `ranrot_rand()%11`, then %50/%40/%25, then 24 `randf()`.

## OOExplosionCloudEntity

<a id="ooexplosioncloudentity"></a>
Upstream `src/Core/Entities/OOExplosionCloudEntity.m` ↔ `Core/Entities/OOExplosionCloudEntity.cpp`

**Status:** done — 6 rows: 5 ✅ 0 ⚠️ 0 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOExplosionCloudEntity initExplosionCloudWithEntity:size:andSettings:]` (57) | `OOExplosionCloudEntity::initExplosionCloudWithEntity` `Core/Entities/OOExplosionCloudEntity.cpp:57` | ✅ |
| `-[OOExplosionCloudEntity dealloc]` (160) | `oo::Ref` members (comment `:156`) | ➖ |
| `+[OOExplosionCloudEntity explosionCloudFromEntity:withSettings:]` (167) | `OOExplosionCloudEntity::explosionCloudFromEntity(Entity*, const PList&)` `Core/Entities/OOExplosionCloudEntity.cpp:158` | ✅ |
| `+[OOExplosionCloudEntity explosionCloudFromEntity:withSize:andSettings:]` (172) | `OOExplosionCloudEntity::explosionCloudFromEntity(Entity*, float, const PList&)` `Core/Entities/OOExplosionCloudEntity.cpp:166` | ✅ |
| `-[OOExplosionCloudEntity update:]` (177) | `OOExplosionCloudEntity::update` `Core/Entities/OOExplosionCloudEntity.cpp:174` | ✅ |
| `-[OOExplosionCloudEntity texture]` (247) | `OOExplosionCloudEntity::texture` `Core/Entities/OOExplosionCloudEntity.cpp:244` | ✅ |

**Differences:**

- ➖ **low** — `-dealloc` releases are now members — upstream `Core/Entities/OOExplosionCloudEntity.m:160` ↔ port `Core/Entities/OOExplosionCloudEntity.cpp:156`
- Subtle (✅): keys and defaults are identical: count 25 capped at 10 (detail ≤ SHADERS) or 25, size 2.5, growth_rate 1.5, alpha 0.85, brightness ≥ 1.0, duration 0.9, spread 1.0, texture `oolite-particle-cloud2.png`, color_order `rgb`. The velocity clamp is 1000 above 10⁶ m²/s². Speeds are 0.8–1.2 × size × spread. Per-particle draws happen in the base init, then 3 (or 1 for `white`) `randf()` per particle. A missing texture still yields no entity.

## OOFlashEffectEntity

<a id="ooflasheffectentity"></a>
Upstream `src/Core/Entities/OOFlashEffectEntity.m` ↔ `Core/Entities/OOFlashEffectEntity.cpp`

**Status:** done — 9 rows: 8 ✅ 0 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOFlashEffectEntity initExplosionFlashWithPosition:velocity:size:]` (55) | `OOFlashEffectEntity::initExplosionFlashWithPosition` `Core/Entities/OOFlashEffectEntity.cpp:54` | ✅ |
| `-[OOFlashEffectEntity initLaserFlashWithPosition:velocity:color:]` (65) | `OOFlashEffectEntity::initLaserFlashWithPosition` `Core/Entities/OOFlashEffectEntity.cpp:65` | ✅ |
| `+[OOFlashEffectEntity explosionFlashFromEntity:]` (74) | `OOFlashEffectEntity::explosionFlashFromEntity` `Core/Entities/OOFlashEffectEntity.cpp:77` | ✅ |
| `+[OOFlashEffectEntity laserFlashWithPosition:velocity:color:]` (79) | `OOFlashEffectEntity::laserFlashWithPosition` `Core/Entities/OOFlashEffectEntity.cpp:84` | ✅ |
| `-[OOFlashEffectEntity initWithPosition:size:color:duration:]` (84) | `OOFlashEffectEntity::initWithPosition` `Core/Entities/OOFlashEffectEntity.cpp:91` | ✅ |
| `-[OOFlashEffectEntity update:]` (96) | `OOFlashEffectEntity::update` `Core/Entities/OOFlashEffectEntity.cpp:104` | ✅ |
| `-[OOFlashEffectEntity texture]` (115) | `OOFlashEffectEntity::texture` `Core/Entities/OOFlashEffectEntity.cpp:125` | ✅ |
| `+[OOFlashEffectEntity setUpTexture]` (122) | `OOFlashEffectEntity::setUpTexture` `Core/Entities/OOFlashEffectEntity.cpp:132` | ❌ |
| `+[OOFlashEffectEntity resetGraphicsState]` (134) | `OOFlashEffectEntity::resetGraphicsState` `Core/Entities/OOFlashEffectEntity.cpp:140` | ✅ |

**Differences:**

- ❌ **low** — each texture re-creation after a graphics reset registers a new `FlashTextureResetClient`. Upstream's class registration was idempotent. The result is accumulating clients and a leak — upstream `Core/Entities/OOFlashEffectEntity.m:130` ↔ port `Core/Entities/OOFlashEffectEntity.cpp:136`
- Subtle (✅): constants are identical: laser flash 0.3 s, size 1, alpha 1; explosion flash 0.4 s, alpha 0.5, growth `max(150×size, 600)`. The fade is in over 0.667 × duration and out over the rest.

## OOFlasherEntity

<a id="ooflasherentity"></a>
Upstream `src/Core/Entities/OOFlasherEntity.m` ↔ `Core/Entities/OOFlasherEntity.cpp`

**Status:** done — 22 rows: 19 ✅ 1 ⚠️ 0 ❌ 2 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOFlasherEntity flasherWithDictionary:]` (42) | `OOFlasherEntity::flasherWithDictionary` `Core/Entities/OOFlasherEntity.cpp:34` | ✅ |
| `-[OOFlasherEntity initWithDictionary:]` (47) | `OOFlasherEntity::initWithDictionary` `Core/Entities/OOFlasherEntity.cpp:41` | ✅ |
| `-[OOFlasherEntity dealloc]` (64) | `std::vector<oo::Ref<OOColor>> _colors` member | ➖ |
| `-[OOFlasherEntity setUpColors:]` (71) | `OOFlasherEntity::setUpColors` `Core/Entities/OOFlasherEntity.cpp:59` | ✅ |
| `-[OOFlasherEntity getCurrentColorComponents]` (82) | `OOFlasherEntity::getCurrentColorComponents` `Core/Entities/OOFlasherEntity.cpp:77` (+ `flasherColorAtIndex` `:72`) | ⚠️ |
| `-[OOFlasherEntity isActive]` (87) | `OOFlasherEntity::isActive` `Core/Entities/OOFlasherEntity.cpp:82` | ✅ |
| `-[OOFlasherEntity setActive:]` (92) | `OOFlasherEntity::setActive` `Core/Entities/OOFlasherEntity.cpp:87` | ✅ |
| `-[OOFlasherEntity color]` (97) | `OOFlasherEntity::color` `Core/Entities/OOFlasherEntity.cpp:92` | ✅ |
| `-[OOFlasherEntity frequency]` (105) | `OOFlasherEntity::frequency` `Core/Entities/OOFlasherEntity.cpp:100` | ✅ |
| `-[OOFlasherEntity setFrequency:]` (110) | `OOFlasherEntity::setFrequency` `Core/Entities/OOFlasherEntity.cpp:105` | ✅ |
| `-[OOFlasherEntity phase]` (115) | `OOFlasherEntity::phase` `Core/Entities/OOFlasherEntity.cpp:110` | ✅ |
| `-[OOFlasherEntity setPhase:]` (120) | `OOFlasherEntity::setPhase` `Core/Entities/OOFlasherEntity.cpp:115` | ✅ |
| `-[OOFlasherEntity fraction]` (125) | `OOFlasherEntity::fraction` `Core/Entities/OOFlasherEntity.cpp:120` | ✅ |
| `-[OOFlasherEntity setFraction:]` (130) | `OOFlasherEntity::setFraction` `Core/Entities/OOFlasherEntity.cpp:125` | ✅ |
| `-[OOFlasherEntity update:]` (135) | `OOFlasherEntity::update` `Core/Entities/OOFlasherEntity.cpp:130` | ✅ |
| `-[OOFlasherEntity drawImmediate:translucent:]` (173) | `OOFlasherEntity::drawImmediate` `Core/Entities/OOFlasherEntity.cpp:168` | ✅ |
| `-[OOFlasherEntity drawSubEntityImmediate:translucent:]` (180) | `OOFlasherEntity::drawSubEntityImmediate` `Core/Entities/OOFlasherEntity.cpp:175` | ✅ |
| `-[OOFlasherEntity isFlasher]` (187) | `OOFlasherEntity::isFlasher` `Core/Entities/OOFlasherEntity.cpp:182` | ✅ |
| `-[OOFlasherEntity findCollisionRadius]` (192) | `OOFlasherEntity::findCollisionRadius` `Core/Entities/OOFlasherEntity.cpp:187` | ✅ |
| `-[OOFlasherEntity rescaleBy:]` (197) | `OOFlasherEntity::rescaleBy(GLfloat)` `Core/Entities/OOFlasherEntity.cpp:192` | ✅ |
| `-[OOFlasherEntity rescaleBy:writeToCache:]` (202) | `OOFlasherEntity::rescaleBy(GLfloat,bool)` `Core/Entities/OOFlasherEntity.cpp:197` | ✅ |
| `-[Entity(OOFlasherEntityExtensions) isFlasher]` (212) | none (category removed) | ➖ |

**Differences:**

- ⚠️ **low** — with an empty colour list, upstream `[_colors objectAtIndex:0]` raised `NSRangeException`. The port's `flasherColorAtIndex` returns null, so the colour stays white (1,1,1); this is documented in the port comment. It is unreachable from shipdata, because `OOShipRegistry` guarantees at least one valid colour (`OOShipRegistry.m:1332-1348`) — upstream `Core/Entities/OOFlasherEntity.m:84` ↔ port `Core/Entities/OOFlasherEntity.cpp:71-80`
- Subtle (✅, low): `setUpColors` pushes a null `Ref` for a specifier that `colorWithDescription` rejects, where upstream `addObject:nil` raised. The registry pre-validates specifiers — upstream `Core/Entities/OOFlasherEntity.m:76` ↔ port `Core/Entities/OOFlasherEntity.cpp:64`
- ➖ **low** — the `Entity -isFlasher` category was removed — upstream `Core/Entities/OOFlasherEntity.m:212`
- Subtle (✅): frequency and phase are identical: `_frequency = frequency×2`, `wave = sinf(f·π·(t+phase))` (double `_time`), and colours switch on a rising negative half-wave. Threshold `cosf(bf·π)` and the brightness piecewise formula are unchanged. Defaults are size 1, frequency 1, phase 0, bright_fraction 0.5, initially_on YES, saturation 0.75.

## OOLaserShotEntity

<a id="oolasershotentity"></a>
Upstream `src/Core/Entities/OOLaserShotEntity.m` ↔ `Core/Entities/OOLaserShotEntity.cpp`

**Status:** done — 16 rows: 14 ✅ 0 ⚠️ 1 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOLaserShotEntity initLaserFromShip:direction:offset:]` (58) | `OOLaserShotEntity::initLaserFromShip` `Core/Entities/OOLaserShotEntity.cpp:70` | ✅ |
| `+[OOLaserShotEntity laserFromShip:direction:offset:]` (117) | `OOLaserShotEntity::laserFromShip` `Core/Entities/OOLaserShotEntity.cpp:126` | ✅ |
| `-[OOLaserShotEntity dealloc]` (122) | removed (comment `:133`) | ➖ |
| `-[OOLaserShotEntity descriptionComponents]` (129) | `OOLaserShotEntity::descriptionComponents` `Core/Entities/OOLaserShotEntity.cpp:135` | ✅ |
| `-[OOLaserShotEntity setColor:]` (134) | `OOLaserShotEntity::setColor` `Core/Entities/OOLaserShotEntity.cpp:141` | ✅ |
| `-[OOLaserShotEntity setRange:]` (142) | `OOLaserShotEntity::setRange` `Core/Entities/OOLaserShotEntity.cpp:150` | ✅ |
| `-[OOLaserShotEntity update:]` (148) | `OOLaserShotEntity::update` `Core/Entities/OOLaserShotEntity.cpp:156` | ✅ |
| `-[OOLaserShotEntity drawImmediate:translucent:]` (197) | `OOLaserShotEntity::drawImmediate` `Core/Entities/OOLaserShotEntity.cpp:206` | ✅ |
| `-[OOLaserShotEntity isEffect]` (255) | `OOLaserShotEntity::isEffect` `Core/Entities/OOLaserShotEntity.cpp:266` | ✅ |
| `-[OOLaserShotEntity canCollide]` (260) | `OOLaserShotEntity::canCollide` `Core/Entities/OOLaserShotEntity.cpp:271` | ✅ |
| `-[OOLaserShotEntity texture1]` (265) | `OOLaserShotEntity::texture1` `Core/Entities/OOLaserShotEntity.cpp:276` | ✅ |
| `-[OOLaserShotEntity texture2]` (270) | `OOLaserShotEntity::texture2` `Core/Entities/OOLaserShotEntity.cpp:281` | ✅ |
| `+[OOLaserShotEntity setUpTexture]` (275) | `OOLaserShotEntity::setUpTexture` `Core/Entities/OOLaserShotEntity.cpp:286` | ❌ |
| `+[OOLaserShotEntity innerTexture]` (293) | `OOLaserShotEntity::innerTexture` `Core/Entities/OOLaserShotEntity.cpp:296` | ✅ |
| `+[OOLaserShotEntity outerTexture]` (300) | `OOLaserShotEntity::outerTexture` `Core/Entities/OOLaserShotEntity.cpp:303` | ✅ |
| `+[OOLaserShotEntity resetGraphicsState]` (307) | `OOLaserShotEntity::resetGraphicsState` `Core/Entities/OOLaserShotEntity.cpp:310` | ✅ |

**Differences:**

- ❌ **low** — `setUpTexture` registers a new `ShotTextureResetClient` on every re-creation after a reset, where upstream registration was idempotent. This leaks and adds redundant reset calls — upstream `Core/Entities/OOLaserShotEntity.m:283` ↔ port `Core/Entities/OOLaserShotEntity.cpp:290`
- ➖ **low** — `-dealloc`'s `[self setColor:nil]` was dropped, which is unobservable — upstream `Core/Entities/OOLaserShotEntity.m:124` ↔ port `Core/Entities/OOLaserShotEntity.cpp:133`
- Subtle (✅): range and lifetime are identical: `_range = collision_radius = [srcEntity weaponRange]` (`getWeaponRange`), `kLaserDuration` 0.09 s, and removal when `_lifetime < 0`. Player shots are re-anchored each tick. Colour is `brightness 5 × rgb / 3` and alpha `0.45/5`. Widths are 0.4/1.8/3.6. Texture phase uses `/200` and `/500`. The facing rotations (aft π, port π/2, starboard −π/2) are unchanged.

## OOLightParticleEntity

<a id="oolightparticleentity"></a>
Upstream `src/Core/Entities/OOLightParticleEntity.m` ↔ `Core/Entities/OOLightParticleEntity.cpp`

**Status:** done — 14 rows: 13 ✅ 0 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOLightParticleEntity initWithDiameter:]` (50) | `OOLightParticleEntity::initWithDiameter` `Core/Entities/OOLightParticleEntity.cpp:53` | ✅ |
| `-[OOLightParticleEntity diameter]` (69) | `OOLightParticleEntity::diameter` `Core/Entities/OOLightParticleEntity.cpp:71` | ✅ |
| `-[OOLightParticleEntity setDiameter:]` (74) | `OOLightParticleEntity::setDiameter` `Core/Entities/OOLightParticleEntity.cpp:76` | ✅ |
| `-[OOLightParticleEntity setColor:]` (79) | `OOLightParticleEntity::setColor(OOColor*)` `Core/Entities/OOLightParticleEntity.cpp:81` | ✅ |
| `-[OOLightParticleEntity setColor:alpha:]` (84) | `OOLightParticleEntity::setColor(OOColor*, GLfloat)` `Core/Entities/OOLightParticleEntity.cpp:88` | ✅ |
| `-[OOLightParticleEntity drawSubEntityImmediate:translucent:]` (90) | `OOLightParticleEntity::drawSubEntityImmediate` `Core/Entities/OOLightParticleEntity.cpp:94` | ✅ |
| `-[OOLightParticleEntity drawImmediate:translucent:]` (129) | `OOLightParticleEntity::drawImmediate` `Core/Entities/OOLightParticleEntity.cpp:135` | ✅ |
| `-[OOLightParticleEntity texture]` (282) | `OOLightParticleEntity::texture` `Core/Entities/OOLightParticleEntity.cpp:291` | ✅ |
| `+[OOLightParticleEntity setUpTexture]` (287) | `OOLightParticleEntity::setUpTexture` `Core/Entities/OOLightParticleEntity.cpp:296` | ❌ |
| `+[OOLightParticleEntity defaultParticleTexture]` (299) | `OOLightParticleEntity::defaultParticleTexture` `Core/Entities/OOLightParticleEntity.cpp:304` | ✅ |
| `+[OOLightParticleEntity resetGraphicsState]` (306) | `OOLightParticleEntity::resetGraphicsState` `Core/Entities/OOLightParticleEntity.cpp:311` | ✅ |
| `-[OOLightParticleEntity isEffect]` (312) | `OOLightParticleEntity::isEffect` `Core/Entities/OOLightParticleEntity.cpp:317` | ✅ |
| `-[OOLightParticleEntity canCollide]` (317) | `OOLightParticleEntity::canCollide` `Core/Entities/OOLightParticleEntity.cpp:322` | ✅ |
| `-[OOLightParticleEntity allTextures]` (323) | `OOLightParticleEntity::allTextures` `Core/Entities/OOLightParticleEntity.cpp:328` | ✅ |

**Differences:**

- ❌ **low** — `setUpTexture` adds a new `BlobTextureResetClient` each time the blob texture is recreated after a reset, where upstream's class registration was idempotent. This leaks and adds redundant calls — upstream `Core/Entities/OOLightParticleEntity.m:295` ↔ port `Core/Entities/OOLightParticleEntity.cpp:300`
- Subtle (✅): the owner walk drops the `!= NO_TARGET` test (NO_TARGET is 0, which equals null). An ownerless sub-entity under break-pattern hide still returns, because `[nil isImmuneToBreakPatternHide]` was NO. `no_draw_distance = (d/2)^√2 × NDDF² × (12 reduced / 36)` is unchanged.

## OOParticleSystem

<a id="ooparticlesystem"></a>
Upstream `src/Core/Entities/OOParticleSystem.m` ↔ `Core/Entities/OOParticleSystem.cpp`

**Status:** done — 16 rows: 14 ✅ 1 ⚠️ 0 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOParticleSystem init]` (39) | protected default ctor `Core/Entities/OOParticleSystem.h:62` | ➖ |
| `-[OOParticleSystem initWithPosition:]` (48) | `OOParticleSystem::initWithPosition` `Core/Entities/OOParticleSystem.cpp:50` | ⚠️ |
| `-[OOParticleSystem descriptionComponents]` (87) | `OOParticleSystem::descriptionComponents` `Core/Entities/OOParticleSystem.cpp:91` | ✅ |
| `-[OOParticleSystem canCollide]` (92) | `OOParticleSystem::canCollide` `Core/Entities/OOParticleSystem.cpp:96` | ✅ |
| `-[OOParticleSystem checkCloseCollisionWith:]` (97) | `OOParticleSystem::checkCloseCollisionWith` `Core/Entities/OOParticleSystem.cpp:101` | ✅ |
| `-[OOParticleSystem update:]` (104) | `OOParticleSystem::update` `Core/Entities/OOParticleSystem.cpp:108` | ✅ |
| `-[OOParticleSystem drawImmediate:translucent:]` (136) | `OOParticleSystem::drawImmediate` `Core/Entities/OOParticleSystem.cpp:142` | ✅ |
| `-[OOParticleSystem isEffect]` (225) | `OOParticleSystem::isEffect` `Core/Entities/OOParticleSystem.cpp:232` | ✅ |
| `-[OOParticleSystem texture]` (230) | `OOParticleSystem::texture` `Core/Entities/OOParticleSystem.cpp:237` | ✅ |
| `-[OOParticleSystem allTextures]` (236) | `OOParticleSystem::allTextures` `Core/Entities/OOParticleSystem.cpp:243` | ✅ |
| `-[OOSmallFragmentBurstEntity initFragmentBurstFrom:velocity:size:]` (246) | `OOSmallFragmentBurstEntity::initFragmentBurstFrom` `Core/Entities/OOParticleSystem.cpp:251` | ✅ |
| `+[OOSmallFragmentBurstEntity fragmentBurstFromEntity:]` (272) | `OOSmallFragmentBurstEntity::fragmentBurstFromEntity` `Core/Entities/OOParticleSystem.cpp:276` | ✅ |
| `-[OOSmallFragmentBurstEntity update:]` (277) | `OOSmallFragmentBurstEntity::update` `Core/Entities/OOParticleSystem.cpp:283` | ✅ |
| `-[OOBigFragmentBurstEntity initFragmentBurstFrom:velocity:size:]` (300) | `OOBigFragmentBurstEntity::initFragmentBurstFrom` `Core/Entities/OOParticleSystem.cpp:302` | ✅ |
| `+[OOBigFragmentBurstEntity fragmentBurstFromEntity:]` (322) | `OOBigFragmentBurstEntity::fragmentBurstFromEntity` `Core/Entities/OOParticleSystem.cpp:323` | ✅ |
| `-[OOBigFragmentBurstEntity update:]` (327) | `OOBigFragmentBurstEntity::update` `Core/Entities/OOParticleSystem.cpp:330` | ✅ |

**Differences:**

- ⚠️ **low** — the three `randf()` calls inside `make_vector(...)` for each particle's colour are sequenced into locals in Clang's left-to-right order (ADR-0063 item 5). The result is identical under Clang, which is the order the goldens use. A GCC-built upstream would have drawn right to left — upstream `Core/Entities/OOParticleSystem.m:70` ↔ port `Core/Entities/OOParticleSystem.cpp:73-76`
- ➖ **low** — `-init` (which returned nil) is now a protected constructor, so only subclasses construct — upstream `Core/Entities/OOParticleSystem.m:39` ↔ port `Core/Entities/OOParticleSystem.h:62`
- Subtle (✅): particle counts are identical. Small burst is `(unsigned)(0.4·size) | 12` capped at 64, speeds 100–400, 1.5 s, one hue `randf()` before the base init's draws, size `3200/speed`. Big burst is `(0.2·size) | 3` capped at 16, speeds `1+size/2` to ×4, 1.0 s, base colour (1,1,0.5,1), velocity ×0.85. The per-particle RNG order is `randf, randf, OORandomUnitVector, 3×randf`. A null `other` in `checkCloseCollisionWith` answers true, as `![nil isEffect]` did.

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

**Status:** done — 2 rows: 2 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPlasmaBurstEntity initWithPosition:]` (35) | `OOPlasmaBurstEntity::initWithPosition` `Core/Entities/OOPlasmaBurstEntity.cpp:40` (factory `burstWithPosition` `:33`) | ✅ |
| `-[OOPlasmaBurstEntity update:]` (47) | `OOPlasmaBurstEntity::update` `Core/Entities/OOPlasmaBurstEntity.cpp:52` | ✅ |

**Differences:**

- None. Initial size 64, growth 64/s, duration 2 s, red colour, collision radius 2 and linear alpha attenuation are all unchanged.

## OOPlasmaShotEntity

<a id="ooplasmashotentity"></a>
Upstream `src/Core/Entities/OOPlasmaShotEntity.m` ↔ `Core/Entities/OOPlasmaShotEntity.cpp`

**Status:** done — 4 rows: 4 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPlasmaShotEntity initWithPosition:]` (43) | `OOPlasmaShotEntity::initWithPosition` `Core/Entities/OOPlasmaShotEntity.cpp:56` (factory `shotWithPosition` `:45`) | ✅ |
| `-[OOPlasmaShotEntity canCollide]` (64) | `OOPlasmaShotEntity::canCollide` `Core/Entities/OOPlasmaShotEntity.cpp:77` | ✅ |
| `-[OOPlasmaShotEntity checkCloseCollisionWith:]` (69) | `OOPlasmaShotEntity::checkCloseCollisionWith` `Core/Entities/OOPlasmaShotEntity.cpp:83` | ✅ |
| `-[OOPlasmaShotEntity update:]` (74) | `OOPlasmaShotEntity::update` `Core/Entities/OOPlasmaShotEntity.cpp:88` | ✅ |

**Differences:**

- Subtle (✅): `update` iterates a snapshot of `collidingEntities`. Upstream iterated the live array by index with the count taken up front, so the result is identical unless the array shrinks mid-loop, which upstream would have raised on. Every non-owner hit still takes `energy × 1.0` with weapon `EQ_WEAPON_PLASMA_SHOT`, removes the shot and spawns one burst — upstream `Core/Entities/OOPlasmaShotEntity.m:86` ↔ port `Core/Entities/OOPlasmaShotEntity.cpp:100`
- Activation delay 0.05 s, size 12 and collision radius 2 are unchanged.

## OOQuiriumCascadeEntity

<a id="ooquiriumcascadeentity"></a>
Upstream `src/Core/Entities/OOQuiriumCascadeEntity.m` ↔ `Core/Entities/OOQuiriumCascadeEntity.cpp`

**Status:** done — 10 rows: 9 ✅ 0 ⚠️ 0 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOQuiriumCascadeEntity initQuiriumCascadeFromShip:]` (36) | `OOQuiriumCascadeEntity::initQuiriumCascadeFromShip` `Core/Entities/OOQuiriumCascadeEntity.cpp:36` | ✅ |
| `+[OOQuiriumCascadeEntity quiriumCascadeFromShip:]` (59) | `OOQuiriumCascadeEntity::quiriumCascadeFromShip` `Core/Entities/OOQuiriumCascadeEntity.cpp:59` | ✅ |
| `-[OOQuiriumCascadeEntity descriptionComponents]` (64) | `OOQuiriumCascadeEntity::descriptionComponents` `Core/Entities/OOQuiriumCascadeEntity.cpp:67` | ✅ |
| `-[OOQuiriumCascadeEntity update:]` (69) | `OOQuiriumCascadeEntity::update` `Core/Entities/OOQuiriumCascadeEntity.cpp:72` | ✅ |
| `-[OOQuiriumCascadeEntity drawImmediate:translucent:]` (110) | `OOQuiriumCascadeEntity::drawImmediate` `Core/Entities/OOQuiriumCascadeEntity.cpp:117` | ✅ |
| `-[OOQuiriumCascadeEntity isEffect]` (136) | `OOQuiriumCascadeEntity::isEffect` `Core/Entities/OOQuiriumCascadeEntity.cpp:143` | ✅ |
| `-[OOQuiriumCascadeEntity isCascadeWeapon]` (141) | `OOQuiriumCascadeEntity::isCascadeWeapon` `Core/Entities/OOQuiriumCascadeEntity.cpp:148` | ✅ |
| `-[OOQuiriumCascadeEntity canCollide]` (146) | `OOQuiriumCascadeEntity::canCollide` `Core/Entities/OOQuiriumCascadeEntity.cpp:153` | ✅ |
| `-[OOQuiriumCascadeEntity checkCloseCollisionWith:]` (151) | `OOQuiriumCascadeEntity::checkCloseCollisionWith` `Core/Entities/OOQuiriumCascadeEntity.cpp:158` | ✅ |
| `-[Entity(OOQuiriumCascadeExtensions) isCascadeWeapon]` (160) | `OOEntityIsCascadeWeapon()` `Core/Entities/OOQuiriumCascadeEntity.h:61`; `Facade_isCascadeWeapon` `Core/Entities/OOEntityCalledByName.cpp:301` | ➖ |

**Differences:**

- ➖ **low** — the category `-isCascadeWeapon` is replaced by the free function `OOEntityIsCascadeWeapon` (a dynamic_cast, false for null). All five upstream callers use it (PlayerEntity ×2, ShipEntity, StationEntity, HeadUpDisplay) — upstream `Core/Entities/OOQuiriumCascadeEntity.m:160` ↔ port `Core/Entities/OOQuiriumCascadeEntity.h:61`
- Subtle (✅): `update` damages a snapshot of `collidingEntities`. Upstream fast enumeration would have raised on mutation. Damage is identical: per-tick `energy = Δt·(100000 − 90000·tf)` to every colliding entity with weapon `EQ_QC_MINE`, and the damage source is the weak owner (`ownerEntity()`, null if gone, as `-owner` was). The radius grows by `Δt·min(240 + 10/tf², 1000)`. Duration is 20 s, collision delay 0.05 s, and the colour/alpha formulas and the single `randf()` are unchanged — upstream `Core/Entities/OOQuiriumCascadeEntity.m:84-102` ↔ port `Core/Entities/OOQuiriumCascadeEntity.cpp:88-108`

## OORingEffectEntity

<a id="ooringeffectentity"></a>
Upstream `src/Core/Entities/OORingEffectEntity.m` ↔ `Core/Entities/OORingEffectEntity.cpp`

**Status:** done — 9 rows: 9 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OORingEffectEntity initialize]` (52) | `OORingEffectEntity::initialize` `Core/Entities/OORingEffectEntity.cpp:56` (called from `ringFromEntity`) | ✅ |
| `-[OORingEffectEntity initRingFromEntity:]` (61) | `OORingEffectEntity::initRingFromEntity` `Core/Entities/OORingEffectEntity.cpp:71` | ✅ |
| `+[OORingEffectEntity ringFromEntity:]` (88) | `OORingEffectEntity::ringFromEntity` `Core/Entities/OORingEffectEntity.cpp:98` | ✅ |
| `+[OORingEffectEntity shrinkingRingFromEntity:]` (93) | `OORingEffectEntity::shrinkingRingFromEntity` `Core/Entities/OORingEffectEntity.cpp:107` | ✅ |
| `-[OORingEffectEntity descriptionComponents]` (103) | `OORingEffectEntity::descriptionComponents` `Core/Entities/OORingEffectEntity.cpp:117` | ✅ |
| `-[OORingEffectEntity update:]` (108) | `OORingEffectEntity::update` `Core/Entities/OORingEffectEntity.cpp:122` | ✅ |
| `-[OORingEffectEntity drawImmediate:translucent:]` (121) | `OORingEffectEntity::drawImmediate` `Core/Entities/OORingEffectEntity.cpp:136` | ✅ |
| `-[OORingEffectEntity isEffect]` (147) | `OORingEffectEntity::isEffect` `Core/Entities/OORingEffectEntity.cpp:162` | ✅ |
| `-[OORingEffectEntity canCollide]` (152) | `OORingEffectEntity::canCollide` `Core/Entities/OORingEffectEntity.cpp:167` | ✅ |

**Differences:**

- Subtle (✅): `+initialize` ran lazily on the first message to the class; the port runs it on the first `ringFromEntity` call, which is the only way to construct a ring. Constants are identical: 2 s duration, 0.4 s attack, inner/outer sizes 0.5/0.625 × radius, growth 0.55/0.625 × radius, shrink factors −2.5/−2.0, and 65 segments. The GL error label still says "OOQuiriumCascadeEntity", as upstream does.

## OOSparkEntity

<a id="oosparkentity"></a>
Upstream `src/Core/Entities/OOSparkEntity.m` ↔ `Core/Entities/OOSparkEntity.cpp`

**Status:** done — 3 rows: 3 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSparkEntity initWithPosition:]` (39) | `OOSparkEntity::initWithPosition` `Core/Entities/OOSparkEntity.cpp:42` (factory `sparkWithPosition` `:31`) | ✅ |
| `-[OOSparkEntity update:]` (58) | `OOSparkEntity::update` `Core/Entities/OOSparkEntity.cpp:63` | ✅ |
| `-[OOSparkEntity performUpdate:]` (64) | `OOSparkEntity::performUpdate` `Core/Entities/OOSparkEntity.cpp:69` | ✅ |

**Differences:**

- None. The fade-to-red mix, removal at `mix == 0`, and `GLfloat` duration and time-remaining are all unchanged.

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

**Status:** done — 22 rows: 20 ✅ 0 ⚠️ 1 ❌ 1 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOWaypointEntity waypointWithDictionary:]` (43) | `OOWaypointEntity::waypointWithDictionary` `Core/Entities/OOWaypointEntity.cpp:46` | ✅ |
| `-[OOWaypointEntity initWithDictionary:]` (48) | `OOWaypointEntity::initWithDictionary` `Core/Entities/OOWaypointEntity.cpp:53` | ✅ |
| `-[OOWaypointEntity dealloc]` (68) | members (`std::optional`, `WeakRef`, `Ref`) `Core/Entities/OOWaypointEntity.h` | ➖ |
| `-[OOWaypointEntity setOrientation:]` (80) | `OOWaypointEntity::setOrientation` `Core/Entities/OOWaypointEntity.cpp:71` | ✅ |
| `-[OOWaypointEntity oriented]` (91) | `OOWaypointEntity::getOriented` `Core/Entities/OOWaypointEntity.cpp:82` | ✅ |
| `-[OOWaypointEntity size]` (96) | `OOWaypointEntity::size` `Core/Entities/OOWaypointEntity.cpp:87` | ✅ |
| `-[OOWaypointEntity setSize:]` (101) | `OOWaypointEntity::setSize` `Core/Entities/OOWaypointEntity.cpp:92` | ✅ |
| `-[OOWaypointEntity isEffect]` (109) | `OOWaypointEntity::isEffect` `Core/Entities/OOWaypointEntity.cpp:100` | ✅ |
| `-[OOWaypointEntity isWaypoint]` (114) | `OOWaypointEntity::isWaypoint` `Core/Entities/OOWaypointEntity.cpp:105` | ✅ |
| `-[OOWaypointEntity drawImmediate:translucent:]` (119) | `OOWaypointEntity::drawImmediate` `Core/Entities/OOWaypointEntity.cpp:110` | ✅ |
| `-[OOWaypointEntity compareBeaconCodeWith:]` (187) | `OOWaypointEntity::compareBeaconCodeWith` `Core/Entities/OOWaypointEntity.cpp:178` | ❌ |
| `-[OOWaypointEntity beaconCode]` (192) | `OOWaypointEntity::beaconCode` `Core/Entities/OOWaypointEntity.cpp:183` | ✅ |
| `-[OOWaypointEntity setBeaconCode:]` (197) | `OOWaypointEntity::setBeaconCode` `Core/Entities/OOWaypointEntity.cpp:190` | ✅ |
| `-[OOWaypointEntity beaconLabel]` (214) | `OOWaypointEntity::beaconLabel` `Core/Entities/OOWaypointEntity.cpp:207` | ✅ |
| `-[OOWaypointEntity setBeaconLabel:]` (219) | `OOWaypointEntity::setBeaconLabel` `Core/Entities/OOWaypointEntity.cpp:212` | ✅ |
| `-[OOWaypointEntity isBeacon]` (230) | `OOWaypointEntity::isBeacon` `Core/Entities/OOWaypointEntity.cpp:223` | ✅ |
| `-[OOWaypointEntity beaconDrawable]` (235) | `OOWaypointEntity::beaconDrawable` `Core/Entities/OOWaypointEntity.cpp:228` | ✅ |
| `-[OOWaypointEntity prevBeacon]` (258) | `OOWaypointEntity::prevBeacon` `Core/Entities/OOWaypointEntity.cpp:252` | ✅ |
| `-[OOWaypointEntity nextBeacon]` (263) | `OOWaypointEntity::nextBeacon` `Core/Entities/OOWaypointEntity.cpp:257` | ✅ |
| `-[OOWaypointEntity setPrevBeacon:]` (268) | `OOWaypointEntity::setPrevBeacon` `Core/Entities/OOWaypointEntity.cpp:262` | ✅ |
| `-[OOWaypointEntity setNextBeacon:]` (276) | `OOWaypointEntity::setNextBeacon` `Core/Entities/OOWaypointEntity.cpp:269` | ✅ |
| `-[OOWaypointEntity isJammingScanning]` (284) | `OOWaypointEntity::isJammingScanning` `Core/Entities/OOWaypointEntity.cpp:276` | ✅ |

**Differences:**

- ❌ **low** — `compareBeaconCodeWith` substitutes `""` for a missing code on either side. Upstream `[nil compare:...]` answered `NSOrderedSame` for a nil receiver, and `[code compare:nil ...]` took Foundation's nil-argument path. The beacon list holds only coded beacons, so this is unlikely to be observable — upstream `Core/Entities/OOWaypointEntity.m:189` ↔ port `Core/Entities/OOWaypointEntity.cpp:180`
- ➖ **low** — `-dealloc` releases are now RAII members — upstream `Core/Entities/OOWaypointEntity.m:68`
- Subtle (✅): `setBeaconCode` and `setBeaconLabel` replace whenever either side is non-nil. Upstream compared string pointers, so `[wp setBeaconCode:[wp beaconCode]]` kept the cached drawable, which the port rebuilds; the visible result is the same (port comment `:188`). A nil dictionary reproduces messaging nil: zero position, zero quaternion (identity, `oriented=NO`), size 0 ignored, nil code and label (`:57-64`). Defaults are size 1000 (non-negative), code `W`, label `Waypoint`. Orientation defaults to identity, as `oo_quaternionForKey:` does (`OOCollectionExtractors.m:645`). `beaconDrawable` measures length in UTF-16 units, as `-length` did.

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

