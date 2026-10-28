# Parity findings: Materials, textures, meshes, drawables, OpenGL

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## OOBasicMaterial

<a id="oobasicmaterial"></a>
Upstream `src/Core/Materials/OOBasicMaterial.m` ↔ `Core/Materials/OOBasicMaterial.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOBasicMaterial initWithName:]` (41) | | |
| `-[OOBasicMaterial initWithName:configuration:]` (57) | | |
| `-[OOBasicMaterial dealloc]` (94) | | |
| `-[OOBasicMaterial name]` (102) | | |
| `-[OOBasicMaterial doApply]` (107) | | |
| `-[OOBasicMaterial unapplyWithNext:]` (121) | | |
| `-[OOBasicMaterial diffuseColor]` (130) | | |
| `-[OOBasicMaterial setDiffuseColor:]` (138) | | |
| `-[OOBasicMaterial setAmbientAndDiffuseColor:]` (148) | | |
| `-[OOBasicMaterial specularColor]` (154) | | |
| `-[OOBasicMaterial setSpecularColor:]` (162) | | |
| `-[OOBasicMaterial ambientColor]` (172) | | |
| `-[OOBasicMaterial setAmbientColor:]` (180) | | |
| `-[OOBasicMaterial emmisionColor]` (190) | | |
| `-[OOBasicMaterial setEmissionColor:]` (198) | | |
| `-[OOBasicMaterial getDiffuseComponents:]` (208) | | |
| `-[OOBasicMaterial setDiffuseComponents:]` (213) | | |
| `-[OOBasicMaterial setAmbientAndDiffuseComponents:]` (218) | | |
| `-[OOBasicMaterial getSpecularComponents:]` (224) | | |
| `-[OOBasicMaterial setSpecularComponents:]` (229) | | |
| `-[OOBasicMaterial getAmbientComponents:]` (234) | | |
| `-[OOBasicMaterial setAmbientComponents:]` (239) | | |
| `-[OOBasicMaterial getEmissionComponents:]` (244) | | |
| `-[OOBasicMaterial setEmissionComponents:]` (249) | | |
| `-[OOBasicMaterial setDiffuseRed:green:blue:alpha:]` (254) | | |
| `-[OOBasicMaterial setAmbientAndDiffuseRed:green:blue:alpha:]` (262) | | |
| `-[OOBasicMaterial setSpecularRed:green:blue:alpha:]` (268) | | |
| `-[OOBasicMaterial setAmbientRed:green:blue:alpha:]` (276) | | |
| `-[OOBasicMaterial setEmissionRed:green:blue:alpha:]` (284) | | |
| `-[OOBasicMaterial shininess]` (292) | | |
| `-[OOBasicMaterial setShininess:]` (297) | | |
| `-[OOBasicMaterial permitSpecular]` (302) | | |
| `-[OOBasicMaterial allTextures]` (308) | | |

**Differences:**

- *pending*


## OOCombinedEmissionMapGenerator

<a id="oocombinedemissionmapgenerator"></a>
Upstream `src/Core/Materials/OOCombinedEmissionMapGenerator.m` ↔ `Core/Materials/OOCombinedEmissionMapGenerator.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCombinedEmissionMapGenerator initWithEmissionMapSpec:]` (59) | | |
| `-[OOCombinedEmissionMapGenerator initWithEmissionAndIlluminationMapSpec:]` (77) | | |
| `-[OOCombinedEmissionMapGenerator initWithEmissionMapSpec:]` (94) | | |
| `-[OOCombinedEmissionMapGenerator constructCacheKey]` (193) | | |
| `-[OOCombinedEmissionMapGenerator dealloc]` (232) | | |
| `-[OOCombinedEmissionMapGenerator descriptionComponents]` (254) | | |
| `-[OOCombinedEmissionMapGenerator textureOptions]` (294) | | |
| `-[OOCombinedEmissionMapGenerator anisotropy]` (299) | | |
| `-[OOCombinedEmissionMapGenerator lodBias]` (304) | | |
| `-[OOCombinedEmissionMapGenerator cacheKey]` (309) | | |
| `-[OOCombinedEmissionMapGenerator loadTexture]` (314) | | |
| `ModulateColor()` (414) | | |
| `ScaleToMatch()` (433) | | |

**Differences:**

- *pending*


## OOConcreteTexture

<a id="ooconcretetexture"></a>
Upstream `src/Core/Materials/OOConcreteTexture.m` ↔ `Core/Materials/OOConcreteTexture.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOConcreteTexture initWithLoader:]` (71) | | |
| `-[OOConcreteTexture initWithPath:]` (109) | | |
| `-[OOConcreteTexture dealloc]` (130) | | |
| `-[OOConcreteTexture descriptionComponents]` (165) | | |
| `-[OOConcreteTexture shortDescriptionComponents]` (182) | | |
| `-[OOConcreteTexture name]` (188) | | |
| `-[OOConcreteTexture apply]` (225) | | |
| `-[OOConcreteTexture ensureFinishedLoading]` (242) | | |
| `-[OOConcreteTexture isFinishedLoading]` (248) | | |
| `-[OOConcreteTexture cacheKey]` (253) | | |
| `-[OOConcreteTexture dimensions]` (258) | | |
| `-[OOConcreteTexture originalDimensions]` (265) | | |
| `-[OOConcreteTexture isMipMapped]` (272) | | |
| `-[OOConcreteTexture copyPixMapRepresentation]` (279) | | |
| `-[OOConcreteTexture isRectangleTexture]` (328) | | |
| `-[OOConcreteTexture isCubeMap]` (337) | | |
| `-[OOConcreteTexture texCoordsScale]` (346) | | |
| `-[OOConcreteTexture glTextureName]` (370) | | |
| `-[OOConcreteTexture(Private) setUpTexture]` (381) | | |
| `-[OOConcreteTexture(Private) uploadTexture]` (418) | | |
| `-[OOConcreteTexture(Private) uploadTextureDataWithMipMap:format:]` (499) | | |
| `-[OOConcreteTexture(Private) uploadTextureCubeMapDataWithMipMap:format:]` (528) | | |
| `-[OOConcreteTexture(Private) glTextureTarget]` (562) | | |
| `-[OOConcreteTexture(Private) forceRebind]` (573) | | |
| `-[OOConcreteTexture(Private) isReloadable]` (600) | | |
| `DecodeFormat()` (609) | | |

**Differences:**

- *pending*


## OODefaultShaderSynthesizer

<a id="oodefaultshadersynthesizer"></a>
Upstream `src/Core/Materials/OODefaultShaderSynthesizer.m` ↔ `Core/Materials/OODefaultShaderSynthesizer.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOSynthesizeMaterialShader()` (264) | | |
| `-[OODefaultShaderSynthesizer initWithMaterialConfiguration:]` (301) | | |
| `-[OODefaultShaderSynthesizer dealloc]` (314) | | |
| `-[OODefaultShaderSynthesizer vertexShader]` (327) | | |
| `-[OODefaultShaderSynthesizer fragmentShader]` (332) | | |
| `-[OODefaultShaderSynthesizer textureSpecifications]` (337) | | |
| `-[OODefaultShaderSynthesizer uniformSpecifications]` (346) | | |
| `-[OODefaultShaderSynthesizer run]` (355) | | |
| `-[OODefaultShaderSynthesizer materialKey]` (379) | | |
| `-[OODefaultShaderSynthesizer entityName]` (384) | | |
| `AppendIfNotEmpty()` (391) | | |
| `GetExtractMode()` (402) | | |
| `-[OODefaultShaderSynthesizer appendVariable:ofType:withPrefix:to:]` (424) | | |
| `-[OODefaultShaderSynthesizer addAttribute:ofType:]` (431) | | |
| `-[OODefaultShaderSynthesizer addVarying:ofType:]` (436) | | |
| `-[OODefaultShaderSynthesizer addVertexUniform:ofType:]` (441) | | |
| `-[OODefaultShaderSynthesizer addFragmentUniform:ofType:]` (446) | | |
| `-[OODefaultShaderSynthesizer defineBindingUniform:ofType:]` (451) | | |
| `-[OODefaultShaderSynthesizer composeVertexShader]` (484) | | |
| `-[OODefaultShaderSynthesizer composeFragmentShader]` (505) | | |
| `KeyFromTextureParameters()` (537) | | |
| `KeyFromTextureSpec()` (549) | | |
| `-[OODefaultShaderSynthesizer assignIDForTexture:]` (562) | | |
| `-[OODefaultShaderSynthesizer textureIDForSpec:]` (613) | | |
| `-[OODefaultShaderSynthesizer setUpOneTexture:]` (618) | | |
| `-[OODefaultShaderSynthesizer getSampleName:andSwizzleOp:forTextureSpec:]` (632) | | |
| `-[OODefaultShaderSynthesizer readRGBForTextureSpec:mapName:]` (643) | | |
| `-[OODefaultShaderSynthesizer readOneChannelForTextureSpec:mapName:]` (664) | | |
| `-[OODefaultShaderSynthesizer performStage:]` (684) | | |
| `-[OODefaultShaderSynthesizer createTemporaries]` (700) | | |
| `-[OODefaultShaderSynthesizer destroyTemporaries]` (725) | | |
| `-[OODefaultShaderSynthesizer writeTextureCoordRead]` (757) | | |
| `-[OODefaultShaderSynthesizer writeDiffuseColorTermIfNeeded]` (806) | | |
| `-[OODefaultShaderSynthesizer writeDiffuseColorTerm]` (843) | | |
| `-[OODefaultShaderSynthesizer writeDiffuseLighting]` (852) | | |
| `-[OODefaultShaderSynthesizer writeLightVector]` (874) | | |
| `-[OODefaultShaderSynthesizer writeEyeVector]` (887) | | |
| `-[OODefaultShaderSynthesizer writeVertexTangentBasis]` (898) | | |
| `-[OODefaultShaderSynthesizer writeNormalIfNeeded]` (910) | | |
| `-[OODefaultShaderSynthesizer writeNormal]` (936) | | |
| `-[OODefaultShaderSynthesizer writeSpecularLighting]` (945) | | |
| `-[OODefaultShaderSynthesizer writeLightMaps]` (1059) | | |
| `-[OODefaultShaderSynthesizer writeVertexPosition]` (1163) | | |
| `-[OODefaultShaderSynthesizer writeTotalColor]` (1170) | | |
| `-[OODefaultShaderSynthesizer writeFinalColorComposite]` (1175) | | |
| `CanonicalizeMaterialSpecifier()` (1198) | | |
| `FormatFloat()` (1443) | | |

**Differences:**

- *pending*


## OOMaterial

<a id="oomaterial"></a>
Upstream `src/Core/Materials/OOMaterial.m` ↔ `Core/Materials/OOMaterial.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOMaterial setUp]` (36) | | |
| `-[OOMaterial dealloc]` (41) | | |
| `-[OOMaterial descriptionComponents]` (49) | | |
| `-[OOMaterial name]` (54) | | |
| `-[OOMaterial apply]` (61) | | |
| `+[OOMaterial applyNone]` (72) | | |
| `+[OOMaterial current]` (79) | | |
| `-[OOMaterial ensureFinishedLoading]` (84) | | |
| `-[OOMaterial isFinishedLoading]` (88) | | |
| `-[OOMaterial setBindingTarget:]` (93) | | |
| `-[OOMaterial wantsNormalsAsTextureCoordinates]` (97) | | |
| `-[OOMaterial countOfTextureUnitsWithBaseCoordinates]` (103) | | |
| `-[OOMaterial allTextures]` (110) | | |
| `-[OOMaterial(OOSubclassInterface) doApply]` (120) | | |
| `-[OOMaterial(OOSubclassInterface) unapplyWithNext:]` (126) | | |
| `-[OOMaterial(OOSubclassInterface) willDealloc]` (131) | | |

**Differences:**

- *pending*


## OOMaterialConvenienceCreators

<a id="oomaterialconveniencecreators"></a>
Upstream `src/Core/Materials/OOMaterialConvenienceCreators.m` ↔ `Core/Materials/OOMaterialConvenienceCreators.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOMaterial(OOConvenienceCreators) synthesizeMaterialDictionaryWithName:]` (88) | | |
| `+[OOMaterial(OOConvenienceCreators) defaultShaderMaterialWithName:]` (142) | | |
| `+[OOMaterial(OOConvenienceCreators) initialize]` (194) | | |
| `+[OOMaterial(OOConvenienceCreators) defaultShaderMaterialWithName:]` (200) | | |
| `+[OOMaterial(OOConvenienceCreators) materialWithName:]` (254) | | |
| `+[OOMaterial(OOConvenienceCreators) materialWithName:]` (309) | | |
| `SetUniform()` (352) | | |
| `SetUniformFloat()` (357) | | |
| `AddTexture()` (362) | | |
| `AddColorIfAppropriate()` (377) | | |
| `AddMacroColorIfAppropriate()` (388) | | |
| `SynthDiffuse()` (402) | | |
| `SynthEmissionAndIllumination()` (421) | | |
| `SynthNormalMap()` (467) | | |
| `SynthSpecular()` (489) | | |

**Differences:**

- *pending*


## OOMaterialSpecifier

<a id="oomaterialspecifier"></a>
Upstream `src/Core/Materials/OOMaterialSpecifier.m` ↔ `Core/Materials/OOMaterialSpecifier.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[NSDictionary(OOMateralProperties) oo_rawSpecularExponentValue]` (73) | | |
| `-[NSDictionary(OOMateralProperties) oo_diffuseColor]` (81) | | |
| `-[NSDictionary(OOMateralProperties) oo_ambientColor]` (92) | | |
| `-[NSDictionary(OOMateralProperties) oo_specularColor]` (100) | | |
| `-[NSDictionary(OOMateralProperties) oo_specularModulateColor]` (111) | | |
| `-[NSDictionary(OOMateralProperties) oo_emissionColor]` (120) | | |
| `-[NSDictionary(OOMateralProperties) oo_emissionModulateColor]` (131) | | |
| `-[NSDictionary(OOMateralProperties) oo_illuminationModulateColor]` (140) | | |
| `-[NSDictionary(OOMateralProperties) oo_diffuseMapSpecifierWithDefaultName:]` (149) | | |
| `-[NSDictionary(OOMateralProperties) oo_combinedSpecularMapSpecifier]` (154) | | |
| `-[NSDictionary(OOMateralProperties) oo_specularColorMapSpecifier]` (161) | | |
| `-[NSDictionary(OOMateralProperties) oo_specularExponentMapSpecifier]` (171) | | |
| `-[NSDictionary(OOMateralProperties) oo_normalMapSpecifier]` (181) | | |
| `-[NSDictionary(OOMateralProperties) oo_parallaxMapSpecifier]` (188) | | |
| `-[NSDictionary(OOMateralProperties) oo_normalAndParallaxMapSpecifier]` (200) | | |
| `-[NSDictionary(OOMateralProperties) oo_emissionMapSpecifier]` (205) | | |
| `-[NSDictionary(OOMateralProperties) oo_illuminationMapSpecifier]` (210) | | |
| `-[NSDictionary(OOMateralProperties) oo_emissionAndIlluminationMapSpecifier]` (215) | | |
| `-[NSDictionary(OOMateralProperties) oo_parallaxScale]` (222) | | |
| `-[NSDictionary(OOMateralProperties) oo_parallaxBias]` (227) | | |
| `-[NSDictionary(OOMateralProperties) oo_gammaCorrect]` (232) | | |
| `-[NSDictionary(OOMateralProperties) oo_gloss]` (237) | | |
| `-[NSDictionary(OOMateralProperties) oo_specularExponent]` (242) | | |

**Differences:**

- *pending*


## OOMultiTextureMaterial

<a id="oomultitexturematerial"></a>
Upstream `src/Core/Materials/OOMultiTextureMaterial.m` ↔ `Core/Materials/OOMultiTextureMaterial.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOMultiTextureMaterial initWithName:configuration:]` (40) | | |
| `-[OOMultiTextureMaterial dealloc]` (108) | | |
| `-[OOMultiTextureMaterial descriptionComponents]` (118) | | |
| `-[OOMultiTextureMaterial textureUnitCount]` (132) | | |
| `-[OOMultiTextureMaterial countOfTextureUnitsWithBaseCoordinates]` (137) | | |
| `-[OOMultiTextureMaterial ensureFinishedLoading]` (142) | | |
| `-[OOMultiTextureMaterial apply]` (148) | | |
| `-[OOMultiTextureMaterial unapplyWithNext:]` (177) | | |
| `-[OOMultiTextureMaterial allTextures]` (194) | | |

**Differences:**

- *pending*


## OONullTexture

<a id="oonulltexture"></a>
Upstream `src/Core/Materials/OONullTexture.m` ↔ `Core/Materials/OONullTexture.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OONullTexture sharedNullTexture]` (36) | | |
| `-[OONullTexture apply]` (46) | | |
| `-[OONullTexture dimensions]` (51) | | |
| `-[OONullTexture isMipMapped]` (56) | | |
| `-[OONullTexture forceRebind]` (61) | | |
| `-[OONullTexture name]` (66) | | |
| `+[OONullTexture(Singleton) allocWithZone:]` (83) | | |
| `-[OONullTexture(Singleton) copyWithZone:]` (92) | | |
| `-[OONullTexture(Singleton) retain]` (97) | | |
| `-[OONullTexture(Singleton) retainCount]` (102) | | |
| `-[OONullTexture(Singleton) release]` (107) | | |
| `-[OONullTexture(Singleton) autorelease]` (111) | | |

**Differences:**

- *pending*


## OOPNGTextureLoader

<a id="oopngtextureloader"></a>
Upstream `src/Core/Materials/OOPNGTextureLoader.m` ↔ `Core/Materials/OOPNGTextureLoader.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPNGTextureLoader loadTexture]` (49) | | |
| `-[OOPNGTextureLoader dealloc]` (63) | | |
| `-[OOPNGTextureLoader(OOPrivate) doLoadTexture]` (77) | | |
| `-[OOPNGTextureLoader(OOPrivate) readBytes:count:]` (172) | | |
| `PNGError()` (199) | | |
| `PNGWarning()` (211) | | |
| `PNGRead()` (217) | | |

**Differences:**

- *pending*


## OOPixMap

<a id="oopixmap"></a>
Upstream `src/Core/Materials/OOPixMap.m` ↔ `Core/Materials/OOPixMap.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOIsValidPixMap()` (39) | | |
| `OOMakePixMap()` (44) | | |
| `OOAllocatePixMap()` (66) | | |
| `OOFreePixMap()` (80) | | |
| `OODuplicatePixMap()` (89) | | |
| `OOResizePixMap()` (106) | | |
| `OOExpandPixMap()` (125) | | |
| `OODumpPixMap()` (140) | | |
| `OOIsValidPixMapFormat()` (180) | | |
| `OOPixMapBytesPerPixelForFormat()` (195) | | |
| `OOPixMapFormatName()` (212) | | |
| `OOPixMapFormatHasAlpha()` (228) | | |

**Differences:**

- *pending*


## OOPixMapChannelOperations

<a id="oopixmapchanneloperations"></a>
Upstream `src/Core/Materials/OOPixMapChannelOperations.m` ↔ `Core/Materials/OOPixMapChannelOperations.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOExtractPixMapChannel()` (38) | | |
| `ExtractChannel_4()` (56) | | |
| `OOPixMapToRGBA()` (78) | | |
| `ToRGBA_1()` (118) | | |
| `ToRGBA_2()` (138) | | |
| `OOPixMapModulateUniform()` (166) | | |
| `ModulateUniform_4()` (178) | | |
| `OOPixMapModulatePixMap()` (220) | | |
| `ModulatePixMap_4()` (236) | | |
| `OOPixMapAddPixMap()` (278) | | |
| `AddPixMap_4()` (294) | | |

**Differences:**

- *pending*


## OOPixMapTextureLoader

<a id="oopixmaptextureloader"></a>
Upstream `src/Core/Materials/OOPixMapTextureLoader.m` ↔ `Core/Materials/OOPixMapTextureLoader.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPixMapTextureLoader initWithPixMap:textureOptions:freeWhenDone:]` (33) | | |
| `-[OOPixMapTextureLoader dealloc]` (50) | | |
| `-[OOPixMapTextureLoader loadTexture]` (57) | | |
| `-[OOPixMapTextureLoader textureOptions]` (81) | | |

**Differences:**

- *pending*


## OOPlanetTextureGenerator

<a id="ooplanettexturegenerator"></a>
Upstream `src/Core/Materials/OOPlanetTextureGenerator.m` ↔ `Core/Materials/OOPlanetTextureGenerator.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPlanetTextureGenerator initWithPlanetInfo:]` (143) | | |
| `+[OOPlanetTextureGenerator planetTextureWithInfo:]` (188) | | |
| `+[OOPlanetTextureGenerator generatePlanetTexture:andAtmosphere:withInfo:]` (200) | | |
| `+[OOPlanetTextureGenerator generatePlanetTexture:secondaryTexture:withInfo:]` (221) | | |
| `+[OOPlanetTextureGenerator generatePlanetTexture:secondaryTexture:andAtmosphere:withInfo:]` (247) | | |
| `-[OOPlanetTextureGenerator dealloc]` (287) | | |
| `-[OOPlanetTextureGenerator descriptionComponents]` (295) | | |
| `-[OOPlanetTextureGenerator textureOptions]` (300) | | |
| `-[OOPlanetTextureGenerator cacheKey]` (305) | | |
| `-[OOPlanetTextureGenerator cacheKeyForType:]` (313) | | |
| `-[OOPlanetTextureGenerator normalMapGenerator]` (324) | | |
| `-[OOPlanetTextureGenerator atmosphereGenerator]` (332) | | |
| `-[OOPlanetTextureGenerator getResult:]` (340) | | |
| `-[OOPlanetTextureGenerator loadTexture]` (362) | | |
| `-[OOPlanetTextureGenerator dumpNoiseBuffer:]` (546) | | |
| `Lerp()` (570) | | |
| `Blend()` (576) | | |
| `BlendAlpha()` (585) | | |
| `CloudMix()` (590) | | |
| `PlanetMix()` (636) | | |
| `FloatRGBFromDictColor()` (697) | | |
| `Hermite()` (705) | | |
| `fast_floor()` (718) | | |
| `FillFBMBuffer()` (727) | | |
| `TDot3()` (795) | | |
| `SampleNoise3D()` (806) | | |
| `MakePermutationTable()` (884) | | |
| `GenerateFBMNoise3D()` (900) | | |
| `FillRandomBuffer()` (961) | | |
| `AddNoise()` (969) | | |
| `GenerateFBMNoise()` (1013) | | |
| `QFactor()` (1048) | | |
| `GetQ()` (1059) | | |
| `SetMixConstants()` (1078) | | |
| `-[OOPlanetNormalMapGenerator initWithCacheKey:seed:]` (1088) | | |
| `-[OOPlanetNormalMapGenerator dealloc]` (1099) | | |
| `-[OOPlanetNormalMapGenerator cacheKey]` (1106) | | |
| `-[OOPlanetNormalMapGenerator textureOptions]` (1111) | | |
| `-[OOPlanetNormalMapGenerator enqueue]` (1116) | | |
| `-[OOPlanetNormalMapGenerator enqueued]` (1127) | | |
| `-[OOPlanetNormalMapGenerator loadTexture]` (1132) | | |
| `-[OOPlanetNormalMapGenerator completeWithData:width:height:]` (1137) | | |
| `-[OOPlanetAtmosphereGenerator initWithCacheKey:seed:andParent:]` (1164) | | |
| `-[OOPlanetAtmosphereGenerator dealloc]` (1177) | | |
| `-[OOPlanetAtmosphereGenerator cacheKey]` (1184) | | |
| `-[OOPlanetAtmosphereGenerator textureOptions]` (1189) | | |
| `-[OOPlanetAtmosphereGenerator enqueue]` (1194) | | |
| `-[OOPlanetAtmosphereGenerator enqueued]` (1200) | | |
| `-[OOPlanetAtmosphereGenerator loadTexture]` (1205) | | |
| `-[OOPlanetAtmosphereGenerator getResult:]` (1219) | | |
| `-[OOPlanetAtmosphereGenerator completeWithData:width:height:]` (1234) | | |

**Differences:**

- *pending*


## OOShaderMaterial

<a id="ooshadermaterial"></a>
Upstream `src/Core/Materials/OOShaderMaterial.m` ↔ `Core/Materials/OOShaderMaterial.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOShaderMaterial configurationDictionarySpecifiesShaderMaterial:]` (71) | | |
| `+[OOShaderMaterial shaderMaterialWithName:]` (88) | | |
| `-[OOShaderMaterial initWithName:]` (96) | | |
| `-[OOShaderMaterial dealloc]` (265) | | |
| `-[OOShaderMaterial bindUniform:]` (286) | | |
| `-[OOShaderMaterial bindSafeUniform:]` (313) | | |
| `-[OOShaderMaterial setUniform:intValue:]` (334) | | |
| `-[OOShaderMaterial setUniform:floatValue:]` (354) | | |
| `-[OOShaderMaterial setUniform:vectorValue:]` (374) | | |
| `-[OOShaderMaterial setUniform:vectorObjectValue:]` (394) | | |
| `-[OOShaderMaterial setUniform:quaternionValue:asMatrix:]` (425) | | |
| `-[OOShaderMaterial addUniformsFromDictionary:withBindingTarget:]` (446) | | |
| `-[OOShaderMaterial doApply]` (589) | | |
| `-[OOShaderMaterial ensureFinishedLoading]` (617) | | |
| `-[OOShaderMaterial isFinishedLoading]` (628) | | |
| `-[OOShaderMaterial unapplyWithNext:]` (642) | | |
| `-[OOShaderMaterial setBindingTarget:]` (669) | | |
| `-[OOShaderMaterial permitSpecular]` (676) | | |
| `-[OOShaderMaterial allTextures]` (682) | | |
| `-[OOShaderMaterial(OOPrivate) loadTexturesFromArray:unitCount:]` (692) | | |
| `-[OOShaderMaterial(OOPrivate) addTexturesFromArray:unitCount:]` (708) | | |
| `MacrosToString()` (731) | | |
| `GetShaderSource()` (761) | | |

**Differences:**

- *pending*


## OOShaderProgram

<a id="ooshaderprogram"></a>
Upstream `src/Core/Materials/OOShaderProgram.m` ↔ `Core/Materials/OOShaderProgram.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOShaderProgram shaderProgramWithVertexShader:]` (66) | | |
| `+[OOShaderProgram shaderProgramWithVertexShaderName:]` (105) | | |
| `-[OOShaderProgram dealloc]` (149) | | |
| `-[OOShaderProgram apply]` (174) | | |
| `+[OOShaderProgram applyNone]` (186) | | |
| `-[OOShaderProgram program]` (197) | | |
| `ValidateShaderObject()` (204) | | |
| `-[OOShaderProgram(OOPrivate) initWithVertexShaderSource:]` (269) | | |
| `-[OOShaderProgram(OOPrivate) bindAttributes:]` (361) | | |
| `-[OOShaderProgram(OOPrivate) bindStandardMatrixUniforms]` (373) | | |
| `GetShaderSource()` (403) | | |
| `GetGLSLInfoLog()` (443) | | |

**Differences:**

- *pending*


## OOShaderUniform

<a id="ooshaderuniform"></a>
Upstream `src/Core/Materials/OOShaderUniform.m` ↔ `Core/Materials/OOShaderUniform.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOShaderUniform initWithName:shaderProgram:intValue:]` (50) | | |
| `-[OOShaderUniform initWithName:shaderProgram:floatValue:]` (61) | | |
| `-[OOShaderUniform initWithName:shaderProgram:vectorValue:]` (72) | | |
| `-[OOShaderUniform initWithName:shaderProgram:colorValue:]` (83) | | |
| `-[OOShaderUniform initWithName:shaderProgram:quaternionValue:asMatrix:]` (102) | | |
| `-[OOShaderUniform initWithName:shaderProgram:matrixValue:]` (121) | | |
| `-[OOShaderUniform initWithName:]` (132) | | |
| `-[OOShaderUniform dealloc]` (179) | | |
| `-[OOShaderUniform description]` (188) | | |
| `-[OOShaderUniform apply]` (272) | | |
| `-[OOShaderUniform setBindingTarget:]` (282) | | |
| `-[OOShaderUniform(OOPrivate) initWithName:shaderProgram:]` (364) | | |
| `-[OOShaderUniform(OOPrivate) applySimple]` (394) | | |
| `-[OOShaderUniform(OOPrivate) applyBinding]` (414) | | |

**Differences:**

- *pending*


## OOShaderUniformMethodType

<a id="ooshaderuniformmethodtype"></a>
Upstream `src/Core/Materials/OOShaderUniformMethodType.m` ↔ `Core/Materials/OOShaderUniformMethodType.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOShaderUniformTypeFromMethodSignature()` (59) | | |
| `InitTemplates()` (100) | | |
| `CopyTemplateForSelector()` (127) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedCharMethod]` (143) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedCharMethod]` (148) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedShortMethod]` (153) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedShortMethod]` (158) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedIntMethod]` (163) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedIntMethod]` (168) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedLongMethod]` (173) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedLongMethod]` (178) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass floatMethod]` (183) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass doubleMethod]` (188) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass vectorMethod]` (193) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass hpvectorMethod]` (199) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass quaternionMethod]` (205) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass matrixMethod]` (211) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass pointMethod]` (216) | | |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass idMethod]` (221) | | |
| `OOCallIntegerMethod()` (228) | | |
| `OOCallFloatMethod()` (266) | | |

**Differences:**

- *pending*


## OOSingleTextureMaterial

<a id="oosingletexturematerial"></a>
Upstream `src/Core/Materials/OOSingleTextureMaterial.m` ↔ `Core/Materials/OOSingleTextureMaterial.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSingleTextureMaterial initWithName:configuration:]` (35) | | |
| `-[OOSingleTextureMaterial initWithName:texture:configuration:]` (50) | | |
| `-[OOSingleTextureMaterial dealloc]` (64) | | |
| `-[OOSingleTextureMaterial descriptionComponents]` (72) | | |
| `-[OOSingleTextureMaterial doApply]` (77) | | |
| `-[OOSingleTextureMaterial unapplyWithNext:]` (86) | | |
| `-[OOSingleTextureMaterial ensureFinishedLoading]` (93) | | |
| `-[OOSingleTextureMaterial isFinishedLoading]` (98) | | |
| `-[OOSingleTextureMaterial wantsNormalsAsTextureCoordinates]` (103) | | |
| `-[OOSingleTextureMaterial allTextures]` (109) | | |

**Differences:**

- *pending*


## OOStandaloneAtmosphereGenerator

<a id="oostandaloneatmospheregenerator"></a>
Upstream `src/Core/Materials/OOStandaloneAtmosphereGenerator.m` ↔ `Core/Materials/OOStandaloneAtmosphereGenerator.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOStandaloneAtmosphereGenerator initWithPlanetInfo:]` (95) | | |
| `+[OOStandaloneAtmosphereGenerator planetTextureWithInfo:]` (129) | | |
| `+[OOStandaloneAtmosphereGenerator generateAtmosphereTexture:withInfo:]` (141) | | |
| `-[OOStandaloneAtmosphereGenerator dealloc]` (154) | | |
| `-[OOStandaloneAtmosphereGenerator descriptionComponents]` (159) | | |
| `-[OOStandaloneAtmosphereGenerator textureOptions]` (164) | | |
| `-[OOStandaloneAtmosphereGenerator cacheKey]` (169) | | |
| `-[OOStandaloneAtmosphereGenerator getResult:]` (180) | | |
| `-[OOStandaloneAtmosphereGenerator loadTexture]` (204) | | |
| `-[OOStandaloneAtmosphereGenerator dumpNoiseBuffer:]` (287) | | |
| `Lerp()` (311) | | |
| `Blend()` (317) | | |
| `BlendAlpha()` (326) | | |
| `CloudMix()` (331) | | |
| `FloatRGBFromDictColor()` (377) | | |
| `Hermite()` (385) | | |
| `fast_floor()` (398) | | |
| `FillFBMBuffer()` (407) | | |
| `TDot3()` (475) | | |
| `SampleNoise3D()` (486) | | |
| `MakePermutationTable()` (569) | | |
| `GenerateFBMNoise3D()` (594) | | |
| `FillRandomBuffer()` (655) | | |
| `AddNoise()` (663) | | |
| `GenerateFBMNoise()` (707) | | |
| `QFactor()` (742) | | |

**Differences:**

- *pending*


## OOTexture

<a id="ootexture"></a>
Upstream `src/Core/Materials/OOTexture.m` ↔ `Core/Materials/OOTexture.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOTexture textureWithName:]` (132) | | |
| `+[OOTexture textureWithName:]` (176) | | |
| `+[OOTexture textureWithConfiguration:]` (186) | | |
| `+[OOTexture textureWithConfiguration:extraOptions:]` (191) | | |
| `+[OOTexture nullTexture]` (204) | | |
| `+[OOTexture textureWithGenerator:]` (209) | | |
| `+[OOTexture textureWithGenerator:enqueue:]` (214) | | |
| `-[OOTexture init]` (240) | | |
| `-[OOTexture dealloc]` (251) | | |
| `-[OOTexture apply]` (258) | | |
| `+[OOTexture applyNone]` (263) | | |
| `-[OOTexture ensureFinishedLoading]` (278) | | |
| `-[OOTexture isFinishedLoading]` (282) | | |
| `-[OOTexture cacheKey]` (287) | | |
| `-[OOTexture dimensions]` (292) | | |
| `-[OOTexture originalDimensions]` (298) | | |
| `-[OOTexture isMipMapped]` (303) | | |
| `-[OOTexture copyPixMapRepresentation]` (309) | | |
| `-[OOTexture isRectangleTexture]` (314) | | |
| `-[OOTexture isCubeMap]` (319) | | |
| `-[OOTexture texCoordsScale]` (324) | | |
| `-[OOTexture glTextureName]` (329) | | |
| `+[OOTexture clearCache]` (335) | | |
| `+[OOTexture rebindAllTextures]` (350) | | |
| `-[OOTexture setTrace:]` (364) | | |
| `+[OOTexture cachedTexturesByAge]` (372) | | |
| `+[OOTexture allTextures]` (377) | | |
| `-[OOTexture dataSize]` (388) | | |
| `-[OOTexture name]` (400) | | |
| `-[OOTexture forceRebind]` (407) | | |
| `-[OOTexture addToCaches]` (412) | | |
| `-[OOTexture removeFromCaches]` (441) | | |
| `+[OOTexture existingTextureForKey:]` (469) | | |
| `+[OOTexture checkExtensions]` (481) | | |
| `-[OOTexture retainInContext:]` (530) | | |
| `-[OOTexture releaseInContext:]` (542) | | |
| `-[OOTexture autoreleaseInContext:]` (554) | | |
| `-[OOTexture retain]` (566) | | |
| `-[OOTexture release]` (571) | | |
| `-[OOTexture autorelease]` (576) | | |
| `-[NSDictionary(OOTextureConveniences) oo_textureSpecifierForKey:defaultName:]` (586) | | |
| `-[NSArray(OOTextureConveniences) oo_textureSpecifierAtIndex:defaultName:]` (595) | | |
| `OOTextureSpecFromObject()` (602) | | |
| `OOTextureComponentsForFormat()` (624) | | |
| `OOCubeMapsAvailable()` (643) | | |
| `OOInterpretTextureSpecifier()` (648) | | |
| `OOMakeTextureSpecifier()` (736) | | |
| `OOApplyTextureOptionDefaults()` (819) | | |
| `OOGenerateTextureCacheKey()` (862) | | |
| `OOTextureCacheKeyForSpecifier()` (875) | | |

**Differences:**

- *pending*


## OOTextureGenerator

<a id="ootexturegenerator"></a>
Upstream `src/Core/Materials/OOTextureGenerator.m` ↔ `Core/Materials/OOTextureGenerator.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOTextureGenerator textureOptions]` (33) | | |
| `-[OOTextureGenerator anisotropy]` (38) | | |
| `-[OOTextureGenerator lodBias]` (43) | | |
| `-[OOTextureGenerator cacheKey]` (48) | | |
| `-[OOTextureGenerator enqueue]` (53) | | |

**Differences:**

- *pending*


## OOTextureLoader

<a id="ootextureloader"></a>
Upstream `src/Core/Materials/OOTextureLoader.m` ↔ `Core/Materials/OOTextureLoader.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOTextureLoader loaderWithPath:options:]` (70) | | |
| `+[OOTextureLoader loaderWithTextureSpecifier:extraOptions:folder:]` (99) | | |
| `-[OOTextureLoader initWithPath:options:]` (120) | | |
| `-[OOTextureLoader dealloc]` (179) | | |
| `-[OOTextureLoader descriptionComponents]` (189) | | |
| `-[OOTextureLoader shortDescriptionComponents]` (209) | | |
| `-[OOTextureLoader path]` (214) | | |
| `-[OOTextureLoader isReady]` (219) | | |
| `-[OOTextureLoader getResult:]` (224) | | |
| `-[OOTextureLoader cacheKey]` (258) | | |
| `-[OOTextureLoader loadTexture]` (263) | | |
| `+[OOTextureLoader setUp]` (268) | | |
| `-[OOTextureLoader performAsyncTask]` (288) | | |
| `-[OOTextureLoader generateMipMapsForCubeMap]` (316) | | |
| `-[OOTextureLoader applySettings]` (347) | | |
| `-[OOTextureLoader getDesiredWidth:andHeight:]` (445) | | |
| `-[OOTextureLoader completeAsyncTask]` (500) | | |

**Differences:**

- *pending*


## OOConvertCubeMapToLatLong

<a id="ooconvertcubemaptolatlong"></a>
Upstream `src/Core/OOConvertCubeMapToLatLong.m` ↔ `Core/OOConvertCubeMapToLatLong.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOConvertCubeMapToLatLong()` (35) | | |

**Differences:**

- *pending*


## OODebugGLDrawing

<a id="oodebuggldrawing"></a>
Upstream `src/Core/OODebugGLDrawing.m` ↔ `Core/OODebugGLDrawing.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `ApplyColor()` (34) | | |
| `OODebugDrawColoredBoundingBoxBetween()` (46) | | |
| `OODebugDrawColoredLine()` (76) | | |
| `OODebugDrawBasis()` (91) | | |
| `OODebugDrawPoint()` (113) | | |
| `OODebugBeginWireframe()` (128) | | |
| `OODebugEndWireframe()` (153) | | |

**Differences:**

- *pending*


## OODrawable

<a id="oodrawable"></a>
Upstream `src/Core/OODrawable.m` ↔ `Core/OODrawable.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OODrawable renderOpaqueParts]` (33) | | |
| `-[OODrawable renderTranslucentParts]` (37) | | |
| `-[OODrawable hasOpaqueParts]` (41) | | |
| `-[OODrawable hasTranslucentParts]` (46) | | |
| `-[OODrawable collisionRadius]` (51) | | |
| `-[OODrawable maxDrawDistance]` (56) | | |
| `-[OODrawable boundingBox]` (61) | | |
| `-[OODrawable setBindingTarget:]` (66) | | |
| `-[OODrawable dumpSelfState]` (70) | | |
| `-[OODrawable allTextures]` (75) | | |
| `-[OODrawable totalSize]` (80) | | |

**Differences:**

- *pending*


## OOEnvironmentCubeMap

<a id="ooenvironmentcubemap"></a>
Upstream `src/Core/OOEnvironmentCubeMap.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOEnvironmentCubeMap initWithSideLength:]` (55) | | |
| `-[OOEnvironmentCubeMap dealloc]` (69) | | |
| `-[OOEnvironmentCubeMap render]` (76) | | |
| `-[OOEnvironmentCubeMap renderOnePassWithSky:sun:planets:]` (130) | | |
| `-[OOEnvironmentCubeMap setUp]` (166) | | |
| `-[OOEnvironmentCubeMap apply]` (216) | | |
| `-[OOEnvironmentCubeMap dimensions]` (222) | | |
| `-[OOEnvironmentCubeMap forceRebind]` (227) | | |
| `-[OOEnvironmentCubeMap isCubeMap]` (240) | | |
| `-[OOEnvironmentCubeMap copyPixMapRepresentation]` (245) | | |

**Differences:**

- *pending*


## OOGraphicsResetManager

<a id="oographicsresetmanager"></a>
Upstream `src/Core/OOGraphicsResetManager.m` ↔ `Core/OOGraphicsResetManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOGraphicsResetManager dealloc]` (36) | | |
| `+[OOGraphicsResetManager sharedManager]` (45) | | |
| `-[OOGraphicsResetManager registerClient:]` (52) | | |
| `-[OOGraphicsResetManager unregisterClient:]` (61) | | |
| `-[OOGraphicsResetManager resetGraphicsState]` (66) | | |
| `+[OOGraphicsResetManager(Singleton) allocWithZone:]` (103) | | |
| `-[OOGraphicsResetManager(Singleton) copyWithZone:]` (112) | | |
| `-[OOGraphicsResetManager(Singleton) retain]` (117) | | |
| `-[OOGraphicsResetManager(Singleton) retainCount]` (122) | | |
| `-[OOGraphicsResetManager(Singleton) release]` (127) | | |
| `-[OOGraphicsResetManager(Singleton) autorelease]` (131) | | |

**Differences:**

- *pending*


## OOMesh

<a id="oomesh"></a>
Upstream `src/Core/OOMesh.m` ↔ `Core/OOMesh.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `Profile()` (87) | | |
| `IsLegacyNormalMode()` (180) | | |
| `IsPerVertexNormalMode()` (201) | | |
| `+[OOMesh meshWithName:]` (223) | | |
| `+[OOMesh meshWithName:]` (242) | | |
| `+[OOMesh placeholderMaterial]` (263) | | |
| `-[OOMesh init]` (274) | | |
| `-[OOMesh dealloc]` (294) | | |
| `NormalModeDescription()` (326) | | |
| `-[OOMesh descriptionComponents]` (340) | | |
| `-[OOMesh copyWithZone:]` (345) | | |
| `-[OOMesh modelName]` (353) | | |
| `-[OOMesh vertexCount]` (358) | | |
| `-[OOMesh faceCount]` (363) | | |
| `-[OOMesh renderOpaqueParts]` (368) | | |
| `-[OOMesh rebindMaterials]` (530) | | |
| `-[OOMesh materials]` (565) | | |
| `-[OOMesh shaders]` (570) | | |
| `-[OOMesh hasOpaqueParts]` (575) | | |
| `-[OOMesh collisionRadius]` (580) | | |
| `-[OOMesh maxDrawDistance]` (585) | | |
| `-[OOMesh octreeDepth]` (591) | | |
| `-[OOMesh octreeDepth]` (625) | | |
| `-[OOMesh octree]` (631) | | |
| `-[OOMesh findBoundingBoxRelativeToPosition:]` (664) | | |
| `-[OOMesh findSubentityBoundingBoxWithPosition:rotMatrix:]` (709) | | |
| `-[OOMesh meshRescaledBy:]` (739) | | |
| `-[OOMesh setBindingTarget:]` (746) | | |
| `-[OOMesh dumpSelfState]` (756) | | |
| `-[OOMesh allTextures]` (768) | | |
| `-[OOMesh totalSize]` (779) | | |
| `-[OOMesh suppressClangStuff]` (808) | | |
| `-[OOMesh(Private) initWithName:]` (817) | | |
| `-[OOMesh(Private) mutableCopyWithZone:]` (881) | | |
| `-[OOMesh(Private) deleteDisplayLists]` (913) | | |
| `-[OOMesh(Private) resetGraphicsState]` (923) | | |
| `-[OOMesh(Private) modelData]` (930) | | |
| `-[OOMesh(Private) setModelFromModelData:name:]` (992) | | |
| `-[OOMesh(Private) loadData:scaleFactor:]` (1080) | | |
| `-[OOMesh(Private) checkNormalsAndAdjustWinding]` (1488) | | |
| `-[OOMesh(Private) generateFaceTangents]` (1534) | | |
| `FaceArea()` (1571) | | |
| `-[OOMesh(Private) calculateVertexNormalsAndTangentsWithFaceRefs:]` (1580) | | |
| `FaceAreaCorrect()` (1619) | | |
| `-[OOMesh(Private) calculateVertexTangentsWithFaceRefs:]` (1634) | | |
| `-[OOMesh(Private) getNormal:andTangent:forVertex:inSmoothGroup:]` (1681) | | |
| `-[OOMesh(Private) setUpVertexArrays]` (1706) | | |
| `-[OOMesh(Private) calculateBoundingVolumes]` (1786) | | |
| `-[OOMesh(Private) rescaleByFactor:]` (1827) | | |
| `-[OOMesh(Private) boundingBox]` (1850) | | |
| `-[OOMesh(Private) debugDrawNormals]` (1856) | | |
| `-[OOMesh(Private) setRetainedObject:forKey:]` (1907) | | |
| `Scribble()` (1919) | | |
| `-[OOMesh(Private) allocateBytesWithSize:count:key:]` (1941) | | |
| `-[OOMesh(Private) allocateVertexBuffersWithCount:]` (1956) | | |
| `-[OOMesh(Private) allocateNormalBuffersWithCount:]` (1962) | | |
| `-[OOMesh(Private) allocateFaceBuffersWithCount:]` (1969) | | |
| `-[OOMesh(Private) allocateVertexArrayBuffersWithCount:]` (1975) | | |
| `-[OOMesh(Private) renameTexturesFrom:to:]` (1986) | | |
| `+[OOCacheManager(OOMesh) meshDataForName:]` (2006) | | |
| `+[OOCacheManager(OOMesh) setMeshData:forName:]` (2011) | | |
| `+[OOCacheManager(Octree) octreeForModel:]` (2024) | | |
| `+[OOCacheManager(Octree) setOctree:forModel:]` (2041) | | |
| `VFRAddFace()` (2048) | | |
| `VFRGetCount()` (2061) | | |
| `VFRGetFaceAtIndex()` (2068) | | |

**Differences:**

- *pending*


## OOOpenGL

<a id="ooopengl"></a>
Upstream `src/Core/OOOpenGL.m` ↔ `Core/OOOpenGL.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOCheckOpenGLErrors()` (38) | | |
| `OOGLWireframeModeOn()` (85) | | |
| `OOGLWireframeModeOff()` (100) | | |
| `GLDrawBallBillboard()` (107) | | |
| `GLDrawOvalPoints()` (132) | | |
| `GLDrawOval()` (149) | | |
| `GLDrawFilledOval()` (158) | | |
| `GLDrawPoints()` (167) | | |
| `GLDrawFilledPoints()` (180) | | |
| `GLDrawQuadStrip()` (193) | | |
| `GLScaledLineWidth()` (206) | | |
| `GLScaledPointSize()` (212) | | |
| `GLGetDisplayScaleFactor()` (218) | | |
| `GLSetDisplayScaleFactor()` (223) | | |
| `LogOpenGLState()` (239) | | |
| `OOGLColorToString()` (270) | | |
| `GLDumpLightState()` (292) | | |
| `GLDumpMaterialState()` (317) | | |
| `GLDumpCullingState()` (380) | | |
| `GLDumpFogState()` (402) | | |
| `GLDumpStateFlags()` (439) | | |
| `OOGLEnumToString()` (471) | | |
| `OOGLFlagToString()` (541) | | |
| `LogOpenGLState()` (548) | | |

**Differences:**

- *pending*


## OOOpenGLExtensionManager

<a id="ooopenglextensionmanager"></a>
Upstream `src/Core/OOOpenGLExtensionManager.m` ↔ `Core/OOOpenGLExtensionManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `ArrayOfExtensions()` (173) | | |
| `-[OOOpenGLExtensionManager init]` (187) | | |
| `-[OOOpenGLExtensionManager reset]` (202) | | |
| `-[OOOpenGLExtensionManager dealloc]` (300) | | |
| `+[OOOpenGLExtensionManager sharedManager]` (315) | | |
| `-[OOOpenGLExtensionManager haveExtension:]` (323) | | |
| `-[OOOpenGLExtensionManager shadersSupported]` (339) | | |
| `-[OOOpenGLExtensionManager shadersForceDisabled]` (348) | | |
| `-[OOOpenGLExtensionManager defaultDetailLevel]` (357) | | |
| `-[OOOpenGLExtensionManager maximumDetailLevel]` (370) | | |
| `-[OOOpenGLExtensionManager textureImageUnitCount]` (383) | | |
| `-[OOOpenGLExtensionManager vboSupported]` (392) | | |
| `-[OOOpenGLExtensionManager fboSupported]` (401) | | |
| `-[OOOpenGLExtensionManager textureCombinersSupported]` (410) | | |
| `-[OOOpenGLExtensionManager textureUnitCount]` (419) | | |
| `-[OOOpenGLExtensionManager majorVersionNumber]` (428) | | |
| `-[OOOpenGLExtensionManager minorVersionNumber]` (433) | | |
| `-[OOOpenGLExtensionManager releaseVersionNumber]` (438) | | |
| `-[OOOpenGLExtensionManager getVersionMajor:minor:release:]` (443) | | |
| `-[OOOpenGLExtensionManager versionIsAtLeastMajor:minor:]` (453) | | |
| `-[OOOpenGLExtensionManager vendorString]` (458) | | |
| `-[OOOpenGLExtensionManager rendererString]` (463) | | |
| `-[OOOpenGLExtensionManager usePointSmoothing]` (468) | | |
| `-[OOOpenGLExtensionManager useLineSmoothing]` (473) | | |
| `-[OOOpenGLExtensionManager useDustShader]` (478) | | |
| `IntegerFromString()` (485) | | |
| `-[OOOpenGLExtensionManager(OOPrivate) checkShadersSupported]` (509) | | |
| `-[OOOpenGLExtensionManager(OOPrivate) checkVBOSupported]` (580) | | |
| `-[OOOpenGLExtensionManager(OOPrivate) checkFBOSupported]` (600) | | |
| `-[OOOpenGLExtensionManager(OOPrivate) checkTextureCombinersSupported]` (657) | | |
| `CheckRegExps()` (677) | | |
| `CompareGPUSettingsByPriority()` (704) | | |
| `-[OOOpenGLExtensionManager(OOPrivate) lookUpPerGPUSettingsWithVersionString:extensionsString:]` (722) | | |
| `+[OOOpenGLExtensionManager(Singleton) allocWithZone:]` (775) | | |
| `-[OOOpenGLExtensionManager(Singleton) copyWithZone:]` (784) | | |
| `-[OOOpenGLExtensionManager(Singleton) retain]` (789) | | |
| `-[OOOpenGLExtensionManager(Singleton) retainCount]` (794) | | |
| `-[OOOpenGLExtensionManager(Singleton) release]` (799) | | |
| `-[OOOpenGLExtensionManager(Singleton) autorelease]` (803) | | |
| `OOBadOpenGLExtensionUsed()` (812) | | |

**Differences:**

- *pending*


## OOOpenGLMatrixManager

<a id="ooopenglmatrixmanager"></a>
Upstream `src/Core/OOOpenGLMatrixManager.m` ↔ `Core/OOOpenGLMatrixManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOOpenGLMatrixStack init]` (51) | | |
| `-[OOOpenGLMatrixStack dealloc]` (59) | | |
| `-[OOOpenGLMatrixStack push:]` (65) | | |
| `-[OOOpenGLMatrixStack pop]` (70) | | |
| `-[OOOpenGLMatrixStack stackCount]` (81) | | |
| `-[OOOpenGLMatrixManager(Private) updateModelView]` (97) | | |
| `-[OOOpenGLMatrixManager(Private) updateProjection]` (112) | | |
| `-[OOOpenGLMatrixManager init]` (127) | | |
| `-[OOOpenGLMatrixManager dealloc]` (150) | | |
| `-[OOOpenGLMatrixManager loadModelView:]` (157) | | |
| `-[OOOpenGLMatrixManager resetModelView]` (164) | | |
| `-[OOOpenGLMatrixManager multModelView:]` (170) | | |
| `-[OOOpenGLMatrixManager translateModelView:]` (176) | | |
| `-[OOOpenGLMatrixManager rotateModelView:axis:]` (185) | | |
| `-[OOOpenGLMatrixManager scaleModelView:]` (190) | | |
| `-[OOOpenGLMatrixManager lookAtWithEye:center:up:]` (195) | | |
| `-[OOOpenGLMatrixManager pushModelView]` (209) | | |
| `-[OOOpenGLMatrixManager popModelView]` (214) | | |
| `-[OOOpenGLMatrixManager getModelView]` (221) | | |
| `-[OOOpenGLMatrixManager countModelView]` (226) | | |
| `-[OOOpenGLMatrixManager syncModelView]` (231) | | |
| `-[OOOpenGLMatrixManager loadProjection:]` (239) | | |
| `-[OOOpenGLMatrixManager multProjection:]` (246) | | |
| `-[OOOpenGLMatrixManager translateProjection:]` (252) | | |
| `-[OOOpenGLMatrixManager rotateProjection:axis:]` (261) | | |
| `-[OOOpenGLMatrixManager scaleProjection:]` (266) | | |
| `-[OOOpenGLMatrixManager frustumLeft:right:bottom:top:near:far:]` (271) | | |
| `-[OOOpenGLMatrixManager orthoLeft:right:bottom:top:near:far:]` (282) | | |
| `-[OOOpenGLMatrixManager perspectiveFovy:aspect:zNear:zFar:]` (293) | | |
| `-[OOOpenGLMatrixManager resetProjection]` (305) | | |
| `-[OOOpenGLMatrixManager pushProjection]` (311) | | |
| `-[OOOpenGLMatrixManager popProjection]` (316) | | |
| `-[OOOpenGLMatrixManager getProjection]` (323) | | |
| `-[OOOpenGLMatrixManager syncProjection]` (328) | | |
| `-[OOOpenGLMatrixManager getMatrix:]` (336) | | |
| `-[OOOpenGLMatrixManager standardMatrixUniformLocations:]` (398) | | |
| `OOGLPushModelView()` (431) | | |
| `OOGLPopModelView()` (438) | | |
| `OOGLGetModelView()` (446) | | |
| `OOGLResetModelView()` (453) | | |
| `OOGLLoadModelView()` (460) | | |
| `OOGLMultModelView()` (467) | | |
| `OOGLTranslateModelView()` (474) | | |
| `OOGLRotateModelView()` (481) | | |
| `OOGLScaleModelView()` (488) | | |
| `OOGLLookAt()` (495) | | |
| `OOGLResetProjection()` (502) | | |
| `OOGLPushProjection()` (509) | | |
| `OOGLPopProjection()` (516) | | |
| `OOGLGetProjection()` (524) | | |
| `OOGLLoadProjection()` (531) | | |
| `OOGLMultProjection()` (538) | | |
| `OOGLTranslateProjection()` (545) | | |
| `OOGLRotateProjection()` (552) | | |
| `OOGLScaleProjection()` (559) | | |
| `OOGLFrustum()` (566) | | |
| `OOGLOrtho()` (573) | | |
| `OOGLPerspective()` (580) | | |
| `OOGLGetModelViewProjection()` (587) | | |

**Differences:**

- *pending*


## OOOpenGLStateManager

<a id="ooopenglstatemanager"></a>
Upstream `src/Core/OOOpenGLStateManager.m` ↔ `Core/OOOpenGLStateManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `GetState_DEPTH_WRITEMASK()` (97) | | |
| `GetState_TEXTURE_ENV_MODE()` (112) | | |
| `GetState_ACTIVE_TEXTURE()` (126) | | |
| `GetState_CLIENT_ACTIVE_TEXTURE()` (139) | | |
| `SetState_CULL_FACE_MODE()` (158) | | |
| `OOGLNoteCurrentFunction()` (209) | | |
| `SetFunctionTracking()` (226) | | |
| `OOSetOpenGLState_()` (238) | | |
| `OOVerifyOpenGLState_()` (255) | | |
| `GetCurrentOpenGLState()` (265) | | |
| `StatesEqual()` (290) | | |
| `VerifyOpenGLStateInternal()` (325) | | |
| `OOSetOpenGLState()` (385) | | |
| `SwitchOpenGLStateInternal()` (397) | | |
| `OOResetGLStateVerifier()` (443) | | |

**Differences:**

- *pending*


## OOPlanetDrawable

<a id="ooplanetdrawable"></a>
Upstream `src/Core/OOPlanetDrawable.m` ↔ `Core/OOPlanetDrawable.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOPlanetDrawable planetWithTextureName:radius:]` (57) | | |
| `+[OOPlanetDrawable atmosphereWithRadius:]` (66) | | |
| `-[OOPlanetDrawable init]` (74) | | |
| `-[OOPlanetDrawable initAsAtmosphere]` (85) | | |
| `-[OOPlanetDrawable dealloc]` (94) | | |
| `-[OOPlanetDrawable copyWithZone:]` (101) | | |
| `-[OOPlanetDrawable material]` (113) | | |
| `-[OOPlanetDrawable setMaterial:]` (118) | | |
| `-[OOPlanetDrawable textureName]` (124) | | |
| `-[OOPlanetDrawable setTextureName:]` (129) | | |
| `-[OOPlanetDrawable radius]` (138) | | |
| `-[OOPlanetDrawable setRadius:]` (143) | | |
| `-[OOPlanetDrawable levelOfDetail]` (149) | | |
| `-[OOPlanetDrawable setLevelOfDetail:]` (154) | | |
| `-[OOPlanetDrawable calculateLevelOfDetailForViewDistance:]` (159) | | |
| `-[OOPlanetDrawable renderOpaqueParts]` (173) | | |
| `-[OOPlanetDrawable renderTranslucentParts]` (184) | | |
| `-[OOPlanetDrawable renderTranslucentPartsOnOpaquePass]` (196) | | |
| `-[OOPlanetDrawable renderCommonParts]` (212) | | |
| `-[OOPlanetDrawable hasOpaqueParts]` (286) | | |
| `-[OOPlanetDrawable hasTranslucentParts]` (291) | | |
| `-[OOPlanetDrawable collisionRadius]` (296) | | |
| `-[OOPlanetDrawable maxDrawDistance]` (301) | | |
| `-[OOPlanetDrawable boundingBox]` (307) | | |
| `-[OOPlanetDrawable setBindingTarget:]` (312) | | |
| `-[OOPlanetDrawable dumpSelfState]` (317) | | |
| `-[OOPlanetDrawable recalculateTransform]` (324) | | |
| `-[OOPlanetDrawable debugDrawNormals]` (331) | | |
| `-[OOPlanetDrawable allTextures]` (380) | | |

**Differences:**

- *pending*


## OOPolygonSprite

<a id="oopolygonsprite"></a>
Upstream `src/Core/OOPolygonSprite.m` ↔ `Core/OOPolygonSprite.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPolygonSprite initWithDataArray:outlineWidth:name:]` (131) | | |
| `-[OOPolygonSprite dealloc]` (158) | | |
| `-[OOPolygonSprite descriptionComponents]` (172) | | |
| `-[OOPolygonSprite drawWithData:count:VBO:]` (178) | | |
| `-[OOPolygonSprite drawFilled]` (219) | | |
| `-[OOPolygonSprite drawOutline]` (230) | | |
| `-[OOPolygonSprite resetGraphicsState]` (241) | | |
| `-[OOPolygonSprite loadPolygons:outlineWidth:]` (257) | | |
| `SubmitVertices()` (408) | | |
| `DataArrayToPoints()` (426) | | |
| `BuildOutlineContour()` (491) | | |
| `GrowTessPolygonData()` (585) | | |
| `AppendVertex()` (612) | | |
| `TessBeginCallback()` (625) | | |
| `TessVertexCallback()` (636) | | |
| `TessCombineCallback()` (723) | | |
| `TessEndCallback()` (729) | | |
| `ErrorCallback()` (740) | | |
| `SVGDumpBegin()` (760) | | |
| `SVGDumpEnd()` (771) | | |
| `SVGDumpBeginGroup()` (781) | | |
| `SVGDumpEndGroup()` (789) | | |
| `SVGDumpAppendBaseContour()` (796) | | |
| `SVGDumpBeginPrimitive()` (815) | | |
| `SVGDumpEndPrimitive()` (849) | | |
| `SVGDumpAppendTriangle()` (856) | | |

**Differences:**

- *pending*


## OOProbabilisticTextureManager

<a id="ooprobabilistictexturemanager"></a>
Upstream `src/Core/OOProbabilisticTextureManager.m` ↔ `Core/OOProbabilisticTextureManager.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOProbabilisticTextureManager initWithPListName:]` (36) | | |
| `-[OOProbabilisticTextureManager initWithPListName:]` (48) | | |
| `-[OOProbabilisticTextureManager dealloc]` (147) | | |
| `-[OOProbabilisticTextureManager description]` (168) | | |
| `-[OOProbabilisticTextureManager selectTexture]` (173) | | |
| `-[OOProbabilisticTextureManager textureCount]` (205) | | |
| `-[OOProbabilisticTextureManager ensureTexturesLoaded]` (210) | | |
| `-[OOProbabilisticTextureManager seed]` (219) | | |
| `-[OOProbabilisticTextureManager setSeed:]` (224) | | |

**Differences:**

- *pending*


## OOSkyDrawable

<a id="ooskydrawable"></a>
Upstream `src/Core/OOSkyDrawable.m` ↔ `Core/OOSkyDrawable.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSkyDrawable initWithColor1:]` (120) | | |
| `-[OOSkyDrawable dealloc]` (166) | | |
| `-[OOSkyDrawable renderOpaqueParts]` (178) | | |
| `-[OOSkyDrawable hasOpaqueParts]` (223) | | |
| `-[OOSkyDrawable maxDrawDistance]` (228) | | |
| `-[OOSkyDrawable allTextures]` (234) | | |
| `-[OOSkyDrawable totalSize]` (246) | | |
| `DebugColor()` (262) | | |
| `-[OOSkyDrawable(OOPrivate) setUpStarsWithColor1:color2:]` (271) | | |
| `-[OOSkyDrawable(OOPrivate) setUpNebulaeWithColor1:]` (322) | | |
| `-[OOSkyDrawable(OOPrivate) addQuads:count:]` (412) | | |
| `-[OOSkyDrawable(OOPrivate) loadStarTextures]` (420) | | |
| `-[OOSkyDrawable(OOPrivate) loadNebulaTextures]` (436) | | |
| `-[OOSkyDrawable(OOPrivate) ensureTexturesLoaded]` (452) | | |
| `-[OOSkyDrawable(OOPrivate) resetGraphicsState]` (458) | | |
| `+[OOSkyQuadSet addQuads:count:toArray:]` (472) | | |
| `-[OOSkyQuadSet initWithQuadsWithTexture:inArray:count:]` (500) | | |
| `-[OOSkyQuadSet dealloc]` (616) | | |
| `-[OOSkyQuadSet description]` (630) | | |
| `-[OOSkyQuadSet render]` (635) | | |
| `-[OOSkyQuadSet totalSize]` (649) | | |
| `-[OOSkyQuadSet texture]` (654) | | |
| `SaturatedColorInRange()` (662) | | |

**Differences:**

- *pending*


## OOTextureScaling

<a id="ootexturescaling"></a>
Upstream `src/Core/OOTextureScaling.m` ↔ `Core/OOTextureScaling.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `StretchVertically()` (91) | | |
| `StretchVertically()` (104) | | |
| `SqueezeVertically()` (115) | | |
| `StretchHorizontally()` (142) | | |
| `SqueezeHorizontally()` (171) | | |
| `OOScalePixMap()` (244) | | |
| `OOGenerateMipMaps()` (336) | | |
| `GenerateMipMaps1()` (365) | | |
| `ScaleToHalf_1_x1()` (414) | | |
| `ScaleToHalf_1_x4()` (453) | | |
| `ScaleToHalf_1_x8()` (502) | | |
| `GenerateMipMaps2()` (551) | | |
| `ScaleToHalf_2_x1()` (605) | | |
| `GenerateMipMaps4()` (643) | | |
| `ScaleToHalf_4_x1()` (690) | | |
| `ScaleToHalf_4_x2()` (743) | | |
| `DumpMipMap()` (818) | | |
| `StretchVerticallyN_x1()` (825) | | |
| `StretchVerticallyN_x4()` (869) | | |
| `StretchVerticallyN_x8()` (917) | | |
| `StretchHorizontally1()` (964) | | |
| `StretchHorizontally2()` (1019) | | |
| `StretchHorizontally4()` (1081) | | |
| `SqueezeHorizontally1()` (1144) | | |
| `SqueezeVertically1()` (1202) | | |
| `SqueezeHorizontally2()` (1283) | | |
| `SqueezeVertically2()` (1340) | | |
| `SqueezeHorizontally4()` (1433) | | |
| `SqueezeVertically4()` (1490) | | |
| `EnsureCorrectDataSize()` (1548) | | |

**Differences:**

- *pending*


## OOTextureSprite

<a id="ootexturesprite"></a>
Upstream `src/Core/OOTextureSprite.m` ↔ `Core/OOTextureSprite.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOTextureSprite initWithTexture:]` (32) | | |
| `-[OOTextureSprite initWithTexture:size:]` (37) | | |
| `-[OOTextureSprite dealloc]` (52) | | |
| `-[OOTextureSprite size]` (59) | | |
| `-[OOTextureSprite blitToX:Y:Z:alpha:]` (64) | | |
| `-[OOTextureSprite blitCentredToX:Y:Z:alpha:]` (95) | | |
| `-[OOTextureSprite blitBackgroundCentredToX:Y:Z:alpha:]` (102) | | |

**Differences:**

- *pending*


## TextureStore

<a id="texturestore"></a>
Upstream `src/Core/TextureStore.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `FloatRGBFromDictColor()` (46) | | |
| `Blend()` (59) | | |
| `PlanetTextureColor()` (68) | | |
| `+[TextureStore getPlanetTextureNameFor:intoData:width:height:]` (100) | | |
| `+[TextureStore getCloudTextureNameFor:impress:bias:intoData:width:height:]` (139) | | |
| `fillRanNoiseBuffer()` (167) | | |
| `addNoise()` (176) | | |
| `q_factor()` (201) | | |
| `fillSquareImageDataWithCloudTexture()` (236) | | |

**Differences:**

- *pending*

