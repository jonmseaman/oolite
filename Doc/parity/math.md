# Parity findings: Maths, geometry, collision

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## CollisionRegion

<a id="collisionregion"></a>
Upstream `src/Core/CollisionRegion.m` ↔ `Core/CollisionRegion.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[CollisionRegion init]` (46) | | |
| `-[CollisionRegion initAsUniverse]` (61) | | |
| `-[CollisionRegion initAtLocation:withRadius:withinRegion:]` (69) | | |
| `-[CollisionRegion dealloc]` (80) | | |
| `-[CollisionRegion description]` (88) | | |
| `-[CollisionRegion clearSubregions]` (93) | | |
| `-[CollisionRegion addSubregionAtPosition:withRadius:]` (99) | | |
| `positionIsWithinRegion()` (126) | | |
| `sphereIsWithinRegion()` (143) | | |
| `positionIsWithinBorders()` (160) | | |
| `-[CollisionRegion clearEntityList]` (179) | | |
| `-[CollisionRegion addEntity:]` (186) | | |
| `-[CollisionRegion checkEntity:]` (205) | | |
| `-[CollisionRegion findCollisions]` (226) | | |
| `entityByEntityOcclusionToValue()` (377) | | |
| `shadowAtPointOcclusionToValue()` (387) | | |
| `testEntityOccludedByEntity()` (496) | | |
| `-[CollisionRegion findShadowedEntities]` (502) | | |
| `-[CollisionRegion collisionDescription]` (618) | | |
| `-[CollisionRegion debugOut]` (623) | | |

**Differences:**

- *pending*


## OOHPVector

<a id="oohpvector"></a>
Upstream `src/Core/OOHPVector.m` ↔ `Core/OOHPVector.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `HPVectorDescription()` (37) | | |
| `ArrayFromHPVector()` (42) | | |
| `OORandomUnitHPVector()` (62) | | |
| `OOHPVectorRandomSpatial()` (75) | | |
| `OOHPVectorRandomRadial()` (88) | | |
| `OOHPRandomPositionInBoundingBox()` (93) | | |
| `OORandomPositionInCylinder()` (102) | | |
| `OORandomPositionInShell()` (116) | | |
| `OOProjectHPVectorToPlane()` (126) | | |

**Differences:**

- *pending*


## OOMatrix

<a id="oomatrix"></a>
Upstream `src/Core/OOMatrix.m` ↔ `Core/OOMatrix.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOMatrixForRotation()` (40) | | |
| `OOMatrixForQuaternionRotation()` (56) | | |
| `OOMatrixEqual()` (91) | | |
| `OOMatrixMultiply()` (105) | | |
| `OOVectorMultiplyMatrix()` (121) | | |
| `OOHPVectorMultiplyMatrix()` (137) | | |
| `OOMatrixOrthogonalize()` (150) | | |
| `OOMatrixDescription()` (176) | | |
| `OOMatrixForBillboard()` (186) | | |
| `OOMatrixRowSwap()` (207) | | |
| `OOMatrixRowScale()` (222) | | |
| `OOMatrixRowOperation()` (234) | | |
| `OOMatrixColumnSwap()` (246) | | |
| `OOMatrixColumnScale()` (261) | | |
| `OOMatrixColumnOperation()` (273) | | |
| `OOMatrixLeftTransform()` (285) | | |
| `OOMatrixRightTransform()` (320) | | |
| `OOMatrixInverse()` (359) | | |
| `OOMatrixInverseWithDeterminant()` (364) | | |
| `GLUniformMatrix3()` (411) | | |

**Differences:**

- *pending*


## OOMeshToOctreeConverter

<a id="oomeshtooctreeconverter"></a>
Upstream `src/Core/OOMeshToOctreeConverter.m` ↔ `Core/OOMeshToOctreeConverter.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `InitGeometryData()` (181) | | |
| `DestroyGeometryData()` (191) | | |
| `AddTriangle()` (210) | | |
| `-[OOMeshToOctreeConverter initWithCapacity:]` (223) | | |
| `-[OOMeshToOctreeConverter dealloc]` (236) | | |
| `+[OOMeshToOctreeConverter converterWithCapacity:]` (243) | | |
| `-[OOMeshToOctreeConverter descriptionComponents]` (248) | | |
| `-[OOMeshToOctreeConverter addTriangle:]` (253) | | |
| `-[OOMeshToOctreeConverter findOctreeToDepth:]` (260) | | |
| `MaxDimensionFromOrigin()` (272) | | |
| `BuildSubOctree()` (289) | | |
| `TranslateGeometryX()` (414) | | |
| `TranslateGeometryY()` (428) | | |
| `TranslateGeometryZ()` (442) | | |
| `SplitGeometryX()` (456) | | |
| `SplitGeometryY()` (565) | | |
| `SplitGeometryZ()` (676) | | |
| `AddTriangle_slow()` (802) | | |

**Differences:**

- *pending*


## OOQuaternion

<a id="ooquaternion"></a>
Upstream `src/Core/OOQuaternion.m` ↔ `Core/OOQuaternion.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `quaternion_multiply()` (30) | | |
| `quaternion_set_random()` (41) | | |
| `vector_forward_from_quaternion()` (50) | | |
| `HPvector_forward_from_quaternion()` (83) | | |
| `vector_up_from_quaternion()` (89) | | |
| `vector_right_from_quaternion()` (122) | | |
| `basis_vectors_from_quaternion()` (154) | | |
| `quaternion_rotation_between()` (213) | | |
| `quaternion_rotation_betweenHP()` (237) | | |
| `quaternion_limited_rotation_between()` (261) | | |
| `quaternion_rotate_about_x()` (291) | | |
| `quaternion_rotate_about_y()` (309) | | |
| `quaternion_rotate_about_z()` (327) | | |
| `quaternion_rotate_about_axis()` (345) | | |
| `QuaternionDescription()` (361) | | |
| `quaternion_rotate_vector()` (378) | | |
| `quaternion_rotate_HPvector()` (394) | | |

**Differences:**

- *pending*


## OOSpatialReference

<a id="oospatialreference"></a>
Upstream `src/Core/OOSpatialReference.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|

**Differences:**

- *pending*


## OOVector

<a id="oovector"></a>
Upstream `src/Core/OOVector.m` ↔ `Core/OOVector.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `VectorDescription()` (39) | | |
| `-[OONativeVector initWithVector:]` (46) | | |
| `-[OONativeVector getVector]` (57) | | |
| `OORandomUnitVector()` (76) | | |
| `OOVectorRandomSpatial()` (89) | | |
| `OOVectorRandomRadial()` (102) | | |
| `OORandomPositionInBoundingBox()` (107) | | |
| `OORandomPositionInShell()` (130) | | |

**Differences:**

- *pending*


## OOVoxel

<a id="oovoxel"></a>
Upstream `src/Core/OOVoxel.m` ↔ `Core/OOVoxel.c`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `checkFace()` (29) | | |
| `checkBevel()` (47) | | |
| `checkCorner()` (78) | | |
| `lineIntersectionWithFace()` (101) | | |
| `checkPoint()` (135) | | |
| `checkLine()` (144) | | |
| `lineCubeIntersection()` (164) | | |

**Differences:**

- *pending*


## Octree

<a id="octree"></a>
Upstream `src/Core/Octree.m` ↔ `Core/Octree.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[Octree init]` (80) | | |
| `-[Octree initWithData:]` (89) | | |
| `-[Octree initWithDictionary:]` (112) | | |
| `-[Octree dealloc]` (124) | | |
| `-[Octree hasCollision]` (132) | | |
| `-[Octree setHasCollision:]` (137) | | |
| `-[Octree octreeDetails]` (142) | | |
| `-[Octree octreeScaledBy:]` (152) | | |
| `offsetForOctant()` (158) | | |
| `-[Octree drawOctree]` (165) | | |
| `-[Octree drawOctreeFromLocation:scale:offset:]` (191) | | |
| `-[Octree drawOctreeCollisions]` (254) | | |
| `-[Octree drawOctreeCollisionFromLocation:scale:offset:]` (269) | | |
| `isHitByLineSub()` (336) | | |
| `isHitByLine()` (347) | | |
| `-[Octree isHitByLine:v1:]` (457) | | |
| `-[Octree isHitByOctree:withOrigin:andIJK:]` (597) | | |
| `-[Octree isHitByOctree:withOrigin:andIJK:s1:s2:]` (610) | | |
| `-[Octree dictionaryRepresentation]` (626) | | |
| `volumeOfOctree()` (634) | | |
| `-[Octree volume]` (669) | | |
| `randomFullNodeFrom()` (683) | | |
| `-[Octree randomPoint]` (716) | | |
| `-[Octree totalSize]` (722) | | |
| `State()` (743) | | |
| `SetNode()` (750) | | |
| `InsertNode()` (760) | | |
| `-[OOOctreeBuilder init]` (768) | | |
| `-[OOOctreeBuilder dealloc]` (788) | | |
| `-[OOOctreeBuilder buildOctreeWithRadius:]` (795) | | |
| `-[OOOctreeBuilder writeSolid]` (818) | | |
| `-[OOOctreeBuilder writeEmpty]` (823) | | |
| `-[OOOctreeBuilder beginInnerNode]` (828) | | |
| `-[OOOctreeBuilder endInnerNode]` (851) | | |
| `SetNode_slow()` (881) | | |
| `-[OOOctreeBuilder suppressClangStuff]` (900) | | |

**Differences:**

- *pending*

