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

**Status:** done — 13 rows: 12 ✅ 0 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOCombinedEmissionMapGenerator initWithEmissionMapSpec:]` (59) | `generatorWithEmissionMapSpec()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:92` | ✅ |
| `-[OOCombinedEmissionMapGenerator initWithEmissionAndIlluminationMapSpec:]` (77) | `generatorWithEmissionAndIlluminationMapSpec()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:113` | ✅ |
| `-[OOCombinedEmissionMapGenerator initWithEmissionMapSpec:]` (94) (private designated init) | `initWithEmissionMapSpec()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:134` (+ `SpecWithExtraShrink()` :45) | ✅ |
| `-[OOCombinedEmissionMapGenerator constructCacheKey]` (193) | `constructCacheKey()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:231` | ❌ |
| `-[OOCombinedEmissionMapGenerator dealloc]` (232) | `~OOCombinedEmissionMapGenerator()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:275` | ✅ |
| `-[OOCombinedEmissionMapGenerator descriptionComponents]` (254) | `descriptionComponents()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:283` | ✅ |
| `-[OOCombinedEmissionMapGenerator textureOptions]` (294) | `textureOptions()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:325` | ✅ |
| `-[OOCombinedEmissionMapGenerator anisotropy]` (299) | `anisotropy()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:330` | ✅ |
| `-[OOCombinedEmissionMapGenerator lodBias]` (304) | `lodBias()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:335` | ✅ |
| `-[OOCombinedEmissionMapGenerator cacheKey]` (309) | `cacheKey()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:340` | ✅ |
| `-[OOCombinedEmissionMapGenerator loadTexture]` (314) | `loadTexture()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:345` | ✅ |
| `ModulateColor()` (414) | `ModulateColor()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:56` | ✅ |
| `ScaleToMatch()` (433) | `ScaleToMatch()` `Core/Materials/OOCombinedEmissionMapGenerator.cpp:75` | ✅ |

**Differences:**

- ❌ **low** — When `_diffuseMap` is nil (or has no cache key), upstream formats `[_diffuseMap cacheKey]` with `%@` as `(null)`, giving `illumination:{…}*{(null)}`; the port substitutes `""` (`*{}`). Only the cache-key string changes, so cache hits are unaffected, but key text differs in any log. upstream `Core/Materials/OOCombinedEmissionMapGenerator.m:223` ↔ port `Core/Materials/OOCombinedEmissionMapGenerator.cpp:263`
- ✅ **low** (subtle) — `SpecWithExtraShrink()` adds `extra_shrink` only when the spec is a dictionary. On a non-dictionary spec, upstream's `mutableCopy`+`oo_setBool:forKey:` would have thrown. Callers pass dictionaries from `OOTextureSpecFromObject`, so in practice the behaviour is the same. upstream `Core/Materials/OOCombinedEmissionMapGenerator.m:130-136` ↔ port `Core/Materials/OOCombinedEmissionMapGenerator.cpp:45-54`
- ✅ (subtle) — The `illuminationDesc != nil` test became `!illuminationDesc.empty()`. This is the same because a generated texture cache key is never empty. A nil loader from `loaderWithTextureSpecifier` is now guarded explicitly, where upstream relied on a message to nil doing nothing. The pixmap modulate/add/scale sequence is verbatim. upstream `:222`, `:170-188` ↔ port `:262`, `:202-222`

## OOConcreteTexture

<a id="ooconcretetexture"></a>
Upstream `src/Core/Materials/OOConcreteTexture.m` ↔ `Core/Materials/OOConcreteTexture.cpp`

**Status:** done — 26 rows: 26 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOConcreteTexture initWithLoader:]` (71) | `OOConcreteTexture()` ctor `Core/Materials/OOConcreteTexture.cpp:59` + `initWithLoader()` :86 | ✅ |
| `-[OOConcreteTexture initWithPath:]` (109) | `initWithPath()` `Core/Materials/OOConcreteTexture.cpp:103` | ✅ |
| `-[OOConcreteTexture dealloc]` (130) | `~OOConcreteTexture()` `Core/Materials/OOConcreteTexture.cpp:124` | ✅ |
| `-[OOConcreteTexture descriptionComponents]` (165) | `descriptionComponents()` `Core/Materials/OOConcreteTexture.cpp:157` | ✅ |
| `-[OOConcreteTexture shortDescriptionComponents]` (182) | `shortDescriptionComponents()` `Core/Materials/OOConcreteTexture.cpp:174` | ✅ |
| `-[OOConcreteTexture name]` (188) | `name()` `Core/Materials/OOConcreteTexture.cpp:180` | ✅ |
| `-[OOConcreteTexture apply]` (225) | `apply()` `Core/Materials/OOConcreteTexture.cpp:225` | ✅ |
| `-[OOConcreteTexture ensureFinishedLoading]` (242) | `ensureFinishedLoading()` `Core/Materials/OOConcreteTexture.cpp:242` | ✅ |
| `-[OOConcreteTexture isFinishedLoading]` (248) | `isFinishedLoading()` `Core/Materials/OOConcreteTexture.cpp:248` | ✅ |
| `-[OOConcreteTexture cacheKey]` (253) | `cacheKey()` `Core/Materials/OOConcreteTexture.cpp:253` | ✅ |
| `-[OOConcreteTexture dimensions]` (258) | `dimensions()` `Core/Materials/OOConcreteTexture.cpp:258` | ✅ |
| `-[OOConcreteTexture originalDimensions]` (265) | `originalDimensions()` `Core/Materials/OOConcreteTexture.cpp:265` | ✅ |
| `-[OOConcreteTexture isMipMapped]` (272) | `isMipMapped()` `Core/Materials/OOConcreteTexture.cpp:272` | ✅ |
| `-[OOConcreteTexture copyPixMapRepresentation]` (279) | `copyPixMapRepresentation()` `Core/Materials/OOConcreteTexture.cpp:279` | ✅ |
| `-[OOConcreteTexture isRectangleTexture]` (328) | `isRectangleTexture()` `Core/Materials/OOConcreteTexture.cpp:328` | ✅ |
| `-[OOConcreteTexture isCubeMap]` (337) | `isCubeMap()` `Core/Materials/OOConcreteTexture.cpp:337` | ✅ |
| `-[OOConcreteTexture texCoordsScale]` (346) | `texCoordsScale()` `Core/Materials/OOConcreteTexture.cpp:346` | ✅ |
| `-[OOConcreteTexture glTextureName]` (370) | `glTextureName()` `Core/Materials/OOConcreteTexture.cpp:370` | ✅ |
| `-[OOConcreteTexture(Private) setUpTexture]` (381) | `setUpTexture()` `Core/Materials/OOConcreteTexture.cpp:379` | ✅ |
| `-[OOConcreteTexture(Private) uploadTexture]` (418) | `uploadTexture()` `Core/Materials/OOConcreteTexture.cpp:417` | ✅ |
| `-[OOConcreteTexture(Private) uploadTextureDataWithMipMap:format:]` (499) | `uploadTextureDataWithMipMap()` `Core/Materials/OOConcreteTexture.cpp:498` | ✅ |
| `-[OOConcreteTexture(Private) uploadTextureCubeMapDataWithMipMap:format:]` (528) | `uploadTextureCubeMapDataWithMipMap()` `Core/Materials/OOConcreteTexture.cpp:527` | ✅ |
| `-[OOConcreteTexture(Private) glTextureTarget]` (562) | `glTextureTarget()` `Core/Materials/OOConcreteTexture.cpp:561` | ✅ |
| `-[OOConcreteTexture(Private) forceRebind]` (573) | `forceRebind()` `Core/Materials/OOConcreteTexture.cpp:572` | ✅ |
| `-[OOConcreteTexture(Private) isReloadable]` (600) | `isReloadable()` `Core/Materials/OOConcreteTexture.cpp:599` | ✅ |
| `DecodeFormat()` (609) | `DecodeFormat()` `Core/Materials/OOConcreteTexture.cpp:606` | ✅ |

**Differences:**

- ✅ (subtle) — The GL upload, mip-level, anisotropy (`OOClamp_0_1_f(anisotropy) * anisotropyScale`), LOD-bias and `DecodeFormat` bodies are textually identical. Only the message syntax changed. upstream `Core/Materials/OOConcreteTexture.m:418-640` ↔ port `Core/Materials/OOConcreteTexture.cpp:417-637`
- ✅ **low** (subtle) — Under `OO_DEBUG_MONITOR_ENABLED`, the generator `_name` is `"<" + className() + ">"`. `className()` is the demangled C++ class name, which matches the ObjC class name for these global-namespace classes. The nil key/name cases print `(null)` as `%@` did. upstream `:96-99`, `:179` ↔ port `:76-80`, `:171`

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

**Status:** done — 12 rows: 6 ✅ 0 ⚠️ 0 ❌ 6 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OONullTexture sharedNullTexture]` (36) | `sharedNullTexture()` `Core/Materials/OONullTexture.cpp:38` | ✅ |
| `-[OONullTexture apply]` (46) | `apply()` `Core/Materials/OONullTexture.cpp:48` | ✅ |
| `-[OONullTexture dimensions]` (51) | `dimensions()` `Core/Materials/OONullTexture.cpp:53` | ✅ |
| `-[OONullTexture isMipMapped]` (56) | `isMipMapped()` `Core/Materials/OONullTexture.cpp:58` | ✅ |
| `-[OONullTexture forceRebind]` (61) | `forceRebind()` `Core/Materials/OONullTexture.cpp:63` | ✅ |
| `-[OONullTexture name]` (66) | `name()` `Core/Materials/OONullTexture.cpp:68` | ✅ |
| `+[OONullTexture(Singleton) allocWithZone:]` (83) | — (comment `Core/Materials/OONullTexture.cpp:74-78`) | ➖ |
| `-[OONullTexture(Singleton) copyWithZone:]` (92) | — | ➖ |
| `-[OONullTexture(Singleton) retain]` (97) | — | ➖ |
| `-[OONullTexture(Singleton) retainCount]` (102) | — | ➖ |
| `-[OONullTexture(Singleton) release]` (107) | — | ➖ |
| `-[OONullTexture(Singleton) autorelease]` (111) | — | ➖ |

**Differences:**

- ➖ **low** — The singleton retain/release boilerplate is not translated. The single instance is created by `sharedNullTexture()` with `leakRef()`, so it is never freed, which matches the UINT_MAX retain count (amendment oo-r7m0 item 1). Nothing else constructs one. upstream `Core/Materials/OONullTexture.m:83-114` ↔ port `Core/Materials/OONullTexture.cpp:42,74-78`

## OOPNGTextureLoader

<a id="oopngtextureloader"></a>
Upstream `src/Core/Materials/OOPNGTextureLoader.m` ↔ `Core/Materials/OOPNGTextureLoader.cpp`

**Status:** done — 7 rows: 7 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPNGTextureLoader loadTexture]` (49) | `loadTexture()` `Core/Materials/OOPNGTextureLoader.cpp:48` | ✅ |
| `-[OOPNGTextureLoader dealloc]` (63) | `~OOPNGTextureLoader()` `Core/Materials/OOPNGTextureLoader.cpp:61` | ✅ |
| `-[OOPNGTextureLoader(OOPrivate) doLoadTexture]` (77) | `doLoadTexture()` `Core/Materials/OOPNGTextureLoader.cpp:69` | ✅ |
| `-[OOPNGTextureLoader(OOPrivate) readBytes:count:]` (172) | `readBytes()` `Core/Materials/OOPNGTextureLoader.cpp:164` | ✅ |
| `PNGError()` (199) | `PNGError()` `Core/Materials/OOPNGTextureLoader.cpp:200` (+ `LoaderPath()` :195) | ✅ |
| `PNGWarning()` (211) | `PNGWarning()` `Core/Materials/OOPNGTextureLoader.cpp:212` | ✅ |
| `PNGRead()` (217) | `PNGRead()` `Core/Materials/OOPNGTextureLoader.cpp:218` | ✅ |

**Differences:**

- ✅ (subtle) — The libpng setup, transforms (`strip_16`, `expand`, `filler`/`bgr`/`swap_alpha` by endianness), format mapping and row reading are verbatim. The `setjmp` frame holds no C++ objects with destructors. The truncated-file message moved into a `thread_local` string so that `png_error`'s `longjmp` skips no destructor. upstream `Core/Materials/OOPNGTextureLoader.m:77-170,176-177` ↔ port `Core/Materials/OOPNGTextureLoader.cpp:69-162,168-171`
- ✅ (subtle) — Before `png_set_read_fn`, the io pointer is NULL and upstream printed `[nil path]` as `(null)`. The port's `LoaderPath()` prints `(null)` too. `PNGRead` with a NULL loader is a no-op, matching a message to nil. port `:195-198,220-222`

## OOPixMap

<a id="oopixmap"></a>
Upstream `src/Core/Materials/OOPixMap.m` ↔ `Core/Materials/OOPixMap.cpp`

**Status:** done — 12 rows: 12 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOIsValidPixMap()` (39) | `OOIsValidPixMap()` `Core/Materials/OOPixMap.cpp:42` | ✅ |
| `OOMakePixMap()` (44) | `OOMakePixMap()` `Core/Materials/OOPixMap.cpp:47` | ✅ |
| `OOAllocatePixMap()` (66) | `OOAllocatePixMap()` `Core/Materials/OOPixMap.cpp:69` | ✅ |
| `OOFreePixMap()` (80) | `OOFreePixMap()` `Core/Materials/OOPixMap.cpp:83` | ✅ |
| `OODuplicatePixMap()` (89) | `OODuplicatePixMap()` `Core/Materials/OOPixMap.cpp:92` | ✅ |
| `OOResizePixMap()` (106) | `OOResizePixMap()` `Core/Materials/OOPixMap.cpp:109` | ✅ |
| `OOExpandPixMap()` (125) | `OOExpandPixMap()` `Core/Materials/OOPixMap.cpp:128` | ✅ |
| `OODumpPixMap()` (140) | `OODumpPixMap()` `Core/Materials/OOPixMap.cpp:143` | ✅ |
| `OOIsValidPixMapFormat()` (180) | `OOIsValidPixMapFormat()` `Core/Materials/OOPixMap.cpp:173` | ✅ |
| `OOPixMapBytesPerPixelForFormat()` (195) | `OOPixMapBytesPerPixelForFormat()` `Core/Materials/OOPixMap.cpp:188` | ✅ |
| `OOPixMapFormatName()` (212) | `OOPixMapFormatName()` `Core/Materials/OOPixMap.cpp:205` | ✅ |
| `OOPixMapFormatHasAlpha()` (228) | `OOPixMapFormatHasAlpha()` `Core/Materials/OOPixMap.cpp:221` | ✅ |

**Differences:**

- ✅ (subtle) — `diff -w` shows only `BOOL`→`bool`, `NSString*`→`std::string`, and the `OODumpPixMap` game-view calls. Those calls now null-check `UNIVERSE` and `gameView` where upstream relied on messages to nil, with the same no-op result. All size, rowBytes and realloc arithmetic is verbatim. upstream `Core/Materials/OOPixMap.m:140-178` ↔ port `Core/Materials/OOPixMap.cpp:143-171`

## OOPixMapChannelOperations

<a id="oopixmapchanneloperations"></a>
Upstream `src/Core/Materials/OOPixMapChannelOperations.m` ↔ `Core/Materials/OOPixMapChannelOperations.cpp`

**Status:** done — 11 rows: 11 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOExtractPixMapChannel()` (38) | `OOExtractPixMapChannel()` `Core/Materials/OOPixMapChannelOperations.cpp:48` | ✅ |
| `ExtractChannel_4()` (56) | `ExtractChannel_4()` `Core/Materials/OOPixMapChannelOperations.cpp:67` | ✅ |
| `OOPixMapToRGBA()` (78) | `OOPixMapToRGBA()` `Core/Materials/OOPixMapChannelOperations.cpp:90` | ✅ |
| `ToRGBA_1()` (118) | `ToRGBA_1()` `Core/Materials/OOPixMapChannelOperations.cpp:131` | ✅ |
| `ToRGBA_2()` (138) | `ToRGBA_2()` `Core/Materials/OOPixMapChannelOperations.cpp:153` | ✅ |
| `OOPixMapModulateUniform()` (166) | `OOPixMapModulateUniform()` `Core/Materials/OOPixMapChannelOperations.cpp:182` | ✅ |
| `ModulateUniform_4()` (178) | `ModulateUniform_4()` `Core/Materials/OOPixMapChannelOperations.cpp:195` | ✅ |
| `OOPixMapModulatePixMap()` (220) | `OOPixMapModulatePixMap()` `Core/Materials/OOPixMapChannelOperations.cpp:238` | ✅ |
| `ModulatePixMap_4()` (236) | `ModulatePixMap_4()` `Core/Materials/OOPixMapChannelOperations.cpp:255` | ✅ |
| `OOPixMapAddPixMap()` (278) | `OOPixMapAddPixMap()` `Core/Materials/OOPixMapChannelOperations.cpp:298` | ✅ |
| `AddPixMap_4()` (294) | `AddPixMap_4()` `Core/Materials/OOPixMapChannelOperations.cpp:315` | ✅ |

**Differences:**

- ✅ (subtle) — `diff -w` shows only `static`→anonymous namespace, `BOOL`→`bool`, pointer casts and `(OOPixMapFormat)4` (= `kOOPixMapRGBA`). The per-channel fixed-point maths and the reversed `f3..f0` parameter order of `ModulateUniform_4` (an upstream quirk) are kept. Licence text line 20 got a stray `NO`→`false` rewrite ("IN false EVENT"), which is cosmetic. port `Core/Materials/OOPixMapChannelOperations.cpp:20,195`

## OOPixMapTextureLoader

<a id="oopixmaptextureloader"></a>
Upstream `src/Core/Materials/OOPixMapTextureLoader.m` ↔ `Core/Materials/OOPixMapTextureLoader.cpp`

**Status:** done — 4 rows: 4 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOPixMapTextureLoader initWithPixMap:textureOptions:freeWhenDone:]` (33) | `loaderWithPixMap()` `Core/Materials/OOPixMapTextureLoader.cpp:33` + `initWithPixMap()` :41 | ✅ |
| `-[OOPixMapTextureLoader dealloc]` (50) | `~OOPixMapTextureLoader()` `Core/Materials/OOPixMapTextureLoader.cpp:59` | ✅ |
| `-[OOPixMapTextureLoader loadTexture]` (57) | `loadTexture()` `Core/Materials/OOPixMapTextureLoader.cpp:64` | ✅ |
| `-[OOPixMapTextureLoader textureOptions]` (81) | `textureOptions()` `Core/Materials/OOPixMapTextureLoader.cpp:88` | ✅ |

**Differences:**

- ✅ (subtle, upstream bugs preserved) — `OODuplicatePixMap(_pixMap, 0)` duplicates the still-empty ivar instead of the `pixMap` argument, so `freeWhenDone == NO` always fails. Separately, when `OOExpandPixMap` fails, the mip fallback sets `kOOTextureMinFilterMipMap` again rather than Linear. Both quirks are faithfully kept. upstream `Core/Materials/OOPixMapTextureLoader.m:39,66` ↔ port `Core/Materials/OOPixMapTextureLoader.cpp:49,73`

## OOPlanetTextureGenerator

<a id="ooplanettexturegenerator"></a>
Upstream `src/Core/Materials/OOPlanetTextureGenerator.m` ↔ `Core/Materials/OOPlanetTextureGenerator.cpp`

**Status:** done — 52 rows: 50 ✅ 1 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| constants `ALBEDO_FACTOR`, `PLANET_TEXTURE_OPTIONS`, `kRandomBufferSize`, `kPermutationCount` (35, 61, 64, 747) | same `Core/Materials/OOPlanetTextureGenerator.cpp:33, 65, 68, 767` | ✅ |
| `-[OOPlanetTextureGenerator initWithPlanetInfo:]` (143) | `generatorWithPlanetInfo()` `Core/Materials/OOPlanetTextureGenerator.cpp:146` + `initWithPlanetInfo()` :154 | ⚠️ |
| `+[OOPlanetTextureGenerator planetTextureWithInfo:]` (188) | `planetTextureWithInfo()` `Core/Materials/OOPlanetTextureGenerator.cpp:200` | ✅ |
| `+[OOPlanetTextureGenerator generatePlanetTexture:andAtmosphere:withInfo:]` (200) | `generatePlanetTextureAndAtmosphere()` `Core/Materials/OOPlanetTextureGenerator.cpp:211` | ✅ |
| `+[OOPlanetTextureGenerator generatePlanetTexture:secondaryTexture:withInfo:]` (221) | `generatePlanetTexture(tex, secondary, …)` `Core/Materials/OOPlanetTextureGenerator.cpp:232` | ✅ |
| `+[OOPlanetTextureGenerator generatePlanetTexture:secondaryTexture:andAtmosphere:withInfo:]` (247) | `generatePlanetTexture(tex, secondary, atmo, …)` `Core/Materials/OOPlanetTextureGenerator.cpp:258` | ✅ |
| `-[OOPlanetTextureGenerator dealloc]` (287) | `~OOPlanetTextureGenerator()` `Core/Materials/OOPlanetTextureGenerator.cpp:299` | ✅ |
| `-[OOPlanetTextureGenerator descriptionComponents]` (295) | `descriptionComponents()` `Core/Materials/OOPlanetTextureGenerator.cpp:303` | ✅ |
| `-[OOPlanetTextureGenerator textureOptions]` (300) | `textureOptions()` `Core/Materials/OOPlanetTextureGenerator.cpp:308` | ✅ |
| `-[OOPlanetTextureGenerator cacheKey]` (305) | `cacheKey()` `Core/Materials/OOPlanetTextureGenerator.cpp:313` | ✅ |
| `-[OOPlanetTextureGenerator cacheKeyForType:]` (313) | `cacheKeyForType()` `Core/Materials/OOPlanetTextureGenerator.cpp:321` | ✅ |
| `-[OOPlanetTextureGenerator normalMapGenerator]` (324) | `normalMapGenerator()` `Core/Materials/OOPlanetTextureGenerator.cpp:332` | ✅ |
| `-[OOPlanetTextureGenerator atmosphereGenerator]` (332) | `atmosphereGenerator()` `Core/Materials/OOPlanetTextureGenerator.cpp:342` | ✅ |
| `-[OOPlanetTextureGenerator getResult:]` (340) (`getResult:format:width:height:`) | `getResultFormatWidthHeight()` `Core/Materials/OOPlanetTextureGenerator.cpp:352` | ✅ |
| `-[OOPlanetTextureGenerator loadTexture]` (362) | `loadTexture()` `Core/Materials/OOPlanetTextureGenerator.cpp:371` | ✅ |
| `-[OOPlanetTextureGenerator dumpNoiseBuffer:]` (546) | `dumpNoiseBuffer()` `Core/Materials/OOPlanetTextureGenerator.cpp:559` | ✅ |
| `Lerp()` (570) | `Lerp()` `Core/Materials/OOPlanetTextureGenerator.cpp:581` | ✅ |
| `Blend()` (576) | `Blend()` `Core/Materials/OOPlanetTextureGenerator.cpp:587` | ✅ |
| `BlendAlpha()` (585) | `BlendAlpha()` `Core/Materials/OOPlanetTextureGenerator.cpp:596` | ✅ |
| `CloudMix()` (590) | `CloudMix()` `Core/Materials/OOPlanetTextureGenerator.cpp:601` | ✅ |
| `PlanetMix()` (636) | `PlanetMix()` `Core/Materials/OOPlanetTextureGenerator.cpp:647` | ✅ |
| `FloatRGBFromDictColor()` (697) | `FloatRGBFromDictColor()` `Core/Materials/OOPlanetTextureGenerator.cpp:710` | ❌ |
| `Hermite()` (705) | `Hermite()` `Core/Materials/OOPlanetTextureGenerator.cpp:723` | ✅ |
| `fast_floor()` (718) | `fast_floor()` `Core/Materials/OOPlanetTextureGenerator.cpp:736` | ✅ |
| `FillFBMBuffer()` (727) | `FillFBMBuffer()` `Core/Materials/OOPlanetTextureGenerator.cpp:747` | ✅ |
| `TDot3()` (795) | `TDot3()` `Core/Materials/OOPlanetTextureGenerator.cpp:815` | ✅ |
| `SampleNoise3D()` (806) | `SampleNoise3D()` `Core/Materials/OOPlanetTextureGenerator.cpp:826` | ✅ |
| `MakePermutationTable()` (884) | `MakePermutationTable()` `Core/Materials/OOPlanetTextureGenerator.cpp:904` | ✅ |
| `GenerateFBMNoise3D()` (900) | `GenerateFBMNoise3D()` `Core/Materials/OOPlanetTextureGenerator.cpp:920` | ✅ |
| `FillRandomBuffer()` (961) | `FillRandomBuffer()` `Core/Materials/OOPlanetTextureGenerator.cpp:983` | ✅ |
| `AddNoise()` (969) | `AddNoise()` `Core/Materials/OOPlanetTextureGenerator.cpp:991` | ✅ |
| `GenerateFBMNoise()` (1013) | `GenerateFBMNoise()` `Core/Materials/OOPlanetTextureGenerator.cpp:1035` | ✅ |
| `QFactor()` (1048) | `QFactor()` `Core/Materials/OOPlanetTextureGenerator.cpp:1070` | ✅ |
| `GetQ()` (1059) | `GetQ()` `Core/Materials/OOPlanetTextureGenerator.cpp:1081` | ✅ |
| `SetMixConstants()` (1078) | `SetMixConstants()` `Core/Materials/OOPlanetTextureGenerator.cpp:1100` | ✅ |
| `-[OOPlanetNormalMapGenerator initWithCacheKey:seed:]` (1088) | `OOPlanetNormalMapGenerator::initWithCacheKey()` `Core/Materials/OOPlanetTextureGenerator.cpp:1108` | ✅ |
| `-[OOPlanetNormalMapGenerator dealloc]` (1099) | implicit destructor (`std::string` member) | ✅ |
| `-[OOPlanetNormalMapGenerator cacheKey]` (1106) | `cacheKey()` `Core/Materials/OOPlanetTextureGenerator.cpp:1120` | ✅ |
| `-[OOPlanetNormalMapGenerator textureOptions]` (1111) | `textureOptions()` `Core/Materials/OOPlanetTextureGenerator.cpp:1125` | ✅ |
| `-[OOPlanetNormalMapGenerator enqueue]` (1116) | `enqueue()` `Core/Materials/OOPlanetTextureGenerator.cpp:1130` | ✅ |
| `-[OOPlanetNormalMapGenerator enqueued]` (1127) | `enqueued()` `Core/Materials/OOPlanetTextureGenerator.cpp:1141` | ✅ |
| `-[OOPlanetNormalMapGenerator loadTexture]` (1132) | `loadTexture()` `Core/Materials/OOPlanetTextureGenerator.cpp:1146` | ✅ |
| `-[OOPlanetNormalMapGenerator completeWithData:width:height:]` (1137) | `completeWithData()` `Core/Materials/OOPlanetTextureGenerator.cpp:1151` | ✅ |
| `-[OOPlanetAtmosphereGenerator initWithCacheKey:seed:andParent:]` (1164) | `OOPlanetAtmosphereGenerator::initWithCacheKey()` `Core/Materials/OOPlanetTextureGenerator.cpp:1174` | ✅ |
| `-[OOPlanetAtmosphereGenerator dealloc]` (1177) | implicit destructor (`std::string`, `oo::Ref` members) | ✅ |
| `-[OOPlanetAtmosphereGenerator cacheKey]` (1184) | `cacheKey()` `Core/Materials/OOPlanetTextureGenerator.cpp:1188` | ✅ |
| `-[OOPlanetAtmosphereGenerator textureOptions]` (1189) | `textureOptions()` `Core/Materials/OOPlanetTextureGenerator.cpp:1193` | ✅ |
| `-[OOPlanetAtmosphereGenerator enqueue]` (1194) | `enqueue()` `Core/Materials/OOPlanetTextureGenerator.cpp:1198` | ✅ |
| `-[OOPlanetAtmosphereGenerator enqueued]` (1200) | `enqueued()` `Core/Materials/OOPlanetTextureGenerator.cpp:1204` | ✅ |
| `-[OOPlanetAtmosphereGenerator loadTexture]` (1205) | `loadTexture()` `Core/Materials/OOPlanetTextureGenerator.cpp:1209` | ✅ |
| `-[OOPlanetAtmosphereGenerator getResult:]` (1219) | `getResult()` override `Core/Materials/OOPlanetTextureGenerator.cpp:1223` | ✅ |
| `-[OOPlanetAtmosphereGenerator completeWithData:width:height:]` (1234) | `completeWithData()` `Core/Materials/OOPlanetTextureGenerator.cpp:1235` | ✅ |

**Differences:**

- ⚠️ **low** — The noise seed is now a `RANROTSeed` parameter instead of an `NSValue` under `noise_map_seed` in `planetInfo` (bead oo-3rb.48). The one caller passes `_noiseMapSeed` (`Core/Entities/OOPlanetEntity.cpp:1035-1038`). Upstream, a missing key left the seed at 0,0. That path no longer exists. upstream `Core/Materials/OOPlanetTextureGenerator.m:157` ↔ port `Core/Materials/OOPlanetTextureGenerator.cpp:168`
- ❌ **low** — `FloatRGBFromDictColor`: a value that is not an `OOColor` now yields black `{0,0,0}`. Upstream had only an `NSCAssert`, so in a release build it sent `-redComponent` to the wrong class and threw. A missing key gives zeros in both. upstream `Core/Materials/OOPlanetTextureGenerator.m:699-702` ↔ port `Core/Materials/OOPlanetTextureGenerator.cpp:712-718`
- ✅ (emphasis: noise/RNG/colour) — `diff -w` over upstream 570-1085 (Lerp/Blend/CloudMix/PlanetMix/Hermite/FBM/SampleNoise3D/permutations/AddNoise/QFactor/GetQ/SetMixConstants) shows only `malloc`/`calloc` casts, scope braces and `fast_floor`'s type-pun replaced by a `memcpy` of the same two 32-bit words (bead oo-1q4). The `RANROTSetFullSeed`/`randf` call order, octave loop, constants and float expression order are unchanged. Outputs therefore depend only on `legacy_random`/`OOMaths`, which are outside this audit. upstream `:718-722` ↔ port `:736-742`
- ✅ (emphasis: loadTexture colour maths) — The `loadTexture` main loop is verbatim apart from a scope block and casts. The `p3dnsf` normal scale (debug builds only) now comes from `oo::Defaults` with default 1.0. upstream `Core/Materials/OOPlanetTextureGenerator.m:405-512` ↔ port `Core/Materials/OOPlanetTextureGenerator.cpp:415-520`
- ✅ (emphasis: cache key) — The `cacheKeyForType` format string `OOPlanetTextureGenerator-%@@%u\n%u,%u/%g/%u,%u/%f…` is unchanged; only `%@` became `%s`. The `diffuse-baked`/`diffuse-raw`/`-atmo` suffix logic is the same. Options remain `PLANET_TEXTURE_OPTIONS`. upstream `:305-322` ↔ port `:313-330`
- ✅ (subtle) — Upstream `-getResult:format:width:height:` never overrode the root `-getResult:format:originalWidth:originalHeight:`, so it was dead code. The port keeps it as the non-overriding `getResultFormatWidthHeight` (amendment oo-y3dd). `OOPlanetAtmosphereGenerator::getResult` is a true override and still waits on the parent through `OOAsyncWorkManager::waitForTaskToComplete`. upstream `:340-360,1219-1232` ↔ port `:352-369,1223-1233`; header `Core/Materials/OOPlanetTextureGenerator.h:111-115`
- ✅ **low** (subtle) — `#if NEW_PLANETS` was dropped. Upstream always compiled the file because `OOStellarBody.h:35` defines it to 1. The `DEBUG_DUMP` blocks (compiled out: `0 && OOLITE_DEBUG`) still contain ObjC message syntax and would not compile if enabled. port `:29-30,542-580`

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

**Status:** done — 26 rows: 24 ✅ 1 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOStandaloneAtmosphereGenerator initWithPlanetInfo:]` (95) | `generatorWithPlanetInfo()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:91` + `initWithPlanetInfo()` :99 | ⚠️ |
| `+[OOStandaloneAtmosphereGenerator planetTextureWithInfo:]` (129) | `planetTextureWithInfo()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:134` | ✅ |
| `+[OOStandaloneAtmosphereGenerator generateAtmosphereTexture:withInfo:]` (141) | `generateAtmosphereTexture()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:145` | ✅ |
| `-[OOStandaloneAtmosphereGenerator dealloc]` (154) | implicit destructor (upstream only called super) | ✅ |
| `-[OOStandaloneAtmosphereGenerator descriptionComponents]` (159) | `descriptionComponents()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:158` | ✅ |
| `-[OOStandaloneAtmosphereGenerator textureOptions]` (164) | `textureOptions()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:163` | ✅ |
| `-[OOStandaloneAtmosphereGenerator cacheKey]` (169) | `cacheKey()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:168` | ✅ |
| `-[OOStandaloneAtmosphereGenerator getResult:]` (180) (`getResult:format:width:height:`) | `getResultFormatWidthHeight()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:179` | ✅ |
| `-[OOStandaloneAtmosphereGenerator loadTexture]` (204) | `loadTexture()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:200` | ✅ |
| `-[OOStandaloneAtmosphereGenerator dumpNoiseBuffer:]` (287) | `dumpNoiseBuffer()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:285` | ✅ |
| `Lerp()` (311) | `Lerp()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:307` | ✅ |
| `Blend()` (317) | `Blend()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:313` | ✅ |
| `BlendAlpha()` (326) | `BlendAlpha()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:322` | ✅ |
| `CloudMix()` (331) | `CloudMix()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:327` | ✅ |
| `FloatRGBFromDictColor()` (377) | `FloatRGBFromDictColor()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:375` | ❌ |
| `Hermite()` (385) | `Hermite()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:388` | ✅ |
| `fast_floor()` (398) | `fast_floor()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:401` | ✅ |
| `FillFBMBuffer()` (407) | `FillFBMBuffer()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:412` | ✅ |
| `TDot3()` (475) | `TDot3()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:480` | ✅ |
| `SampleNoise3D()` (486) | `SampleNoise3D()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:491` | ✅ |
| `MakePermutationTable()` (569) | `MakePermutationTable()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:574` | ✅ |
| `GenerateFBMNoise3D()` (594) | `GenerateFBMNoise3D()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:599` | ✅ |
| `FillRandomBuffer()` (655) | `FillRandomBuffer()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:662` | ✅ |
| `AddNoise()` (663) | `AddNoise()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:670` | ✅ |
| `GenerateFBMNoise()` (707) | `GenerateFBMNoise()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:714` | ✅ |
| `QFactor()` (742) | `QFactor()` `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:749` | ✅ |

**Differences:**

- ⚠️ **low** — The seed is a parameter rather than `noise_map_seed` in `planetInfo` (bead oo-3rb.48). The caller passes `_noiseMapSeed` (`Core/Entities/OOPlanetEntity.cpp:1054-1056`). upstream `Core/Materials/OOStandaloneAtmosphereGenerator.m:102` ↔ port `Core/Materials/OOStandaloneAtmosphereGenerator.cpp:106`
- ❌ **low** — `FloatRGBFromDictColor`: a non-`OOColor` value now gives black where upstream would have thrown. This is the same as in OOPlanetTextureGenerator. upstream `:379-382` ↔ port `:377-383`
- ✅ (emphasis) — The noise, RNG, cloud mix and cache-key format (`OOStandaloneAtmosphereGenerator-@%u\n…`) are verbatim per `diff -w`. The only code change is `fast_floor`'s `memcpy` (oo-1q4). `PLANET_TEXTURE_OPTIONS` is the same. upstream `:169-178,398-402` ↔ port `:168-177,401-407`

## OOTexture

<a id="ootexture"></a>
Upstream `src/Core/Materials/OOTexture.m` ↔ `Core/Materials/OOTexture.cpp`

**Status:** done — 52 rows: 41 ✅ 1 ⚠️ 2 ❌ 8 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| specifier key constants `kOOTextureSpecifier*Key`, `_oo_internal_flags` (44-63) | `cxx_kOOTextureSpecifier*Key` `Core/Materials/OOTexture.h:316-333`, `kOOTextureSpecifierFlagValueInternalKey` `Core/Materials/OOTexture.cpp:61` | ✅ |
| `+[OOTexture textureWithName:]` (132) (`…inFolder:options:anisotropy:lodBias:`) | `textureWithName()` `Core/Materials/OOTexture.cpp:117` → `loadTextureWithName()` :126 | ✅ |
| `+[OOTexture textureWithName:]` (176) (`…inFolder:`) | `textureWithName(name, dir)` `Core/Materials/OOTexture.cpp:170` | ✅ |
| `+[OOTexture textureWithConfiguration:]` (186) | `textureWithConfiguration()` `Core/Materials/OOTexture.cpp:180` → `loadTextureWithConfiguration()` :185 | ✅ |
| `+[OOTexture textureWithConfiguration:extraOptions:]` (191) | `textureWithConfiguration(cfg, extra)` `Core/Materials/OOTexture.cpp:190` | ✅ |
| `+[OOTexture nullTexture]` (204) | `nullTexture()` `Core/Materials/OOTexture.cpp:203` | ✅ |
| `+[OOTexture textureWithGenerator:]` (209) | `textureWithGenerator()` `Core/Materials/OOTexture.cpp:210` | ✅ |
| `+[OOTexture textureWithGenerator:enqueue:]` (214) | `textureWithGenerator(gen, enqueue)` `Core/Materials/OOTexture.cpp:215` | ✅ |
| `-[OOTexture init]` (240) | `OOTexture()` `Core/Materials/OOTexture.cpp:242` | ✅ |
| `-[OOTexture dealloc]` (251) | `~OOTexture()` `Core/Materials/OOTexture.cpp:249` | ✅ |
| `-[OOTexture apply]` (258) | `apply()` `Core/Materials/OOTexture.cpp:255` | ✅ |
| `+[OOTexture applyNone]` (263) | `applyNone()` `Core/Materials/OOTexture.cpp:260` | ✅ |
| `-[OOTexture ensureFinishedLoading]` (278) | `ensureFinishedLoading()` `Core/Materials/OOTexture.cpp:275` | ✅ |
| `-[OOTexture isFinishedLoading]` (282) | `isFinishedLoading()` `Core/Materials/OOTexture.cpp:279` | ✅ |
| `-[OOTexture cacheKey]` (287) | `cacheKey()` `Core/Materials/OOTexture.cpp:284` | ✅ |
| `-[OOTexture dimensions]` (292) | `dimensions()` `Core/Materials/OOTexture.cpp:289` | ✅ |
| `-[OOTexture originalDimensions]` (298) | `originalDimensions()` `Core/Materials/OOTexture.cpp:295` | ✅ |
| `-[OOTexture isMipMapped]` (303) | `isMipMapped()` `Core/Materials/OOTexture.cpp:300` | ✅ |
| `-[OOTexture copyPixMapRepresentation]` (309) | `copyPixMapRepresentation()` `Core/Materials/OOTexture.cpp:306` | ✅ |
| `-[OOTexture isRectangleTexture]` (314) | `isRectangleTexture()` `Core/Materials/OOTexture.cpp:311` | ✅ |
| `-[OOTexture isCubeMap]` (319) | `isCubeMap()` `Core/Materials/OOTexture.cpp:316` | ✅ |
| `-[OOTexture texCoordsScale]` (324) | `texCoordsScale()` `Core/Materials/OOTexture.cpp:321` | ✅ |
| `-[OOTexture glTextureName]` (329) | `glTextureName()` `Core/Materials/OOTexture.cpp:326` | ✅ |
| `+[OOTexture clearCache]` (335) | `clearCache()` `Core/Materials/OOTexture.cpp:332` | ✅ |
| `+[OOTexture rebindAllTextures]` (350) | `rebindAllTextures()` `Core/Materials/OOTexture.cpp:350` | ✅ |
| `-[OOTexture setTrace:]` (364) | `setTrace()` `Core/Materials/OOTexture.cpp:406` | ✅ |
| `+[OOTexture cachedTexturesByAge]` (372) | `cachedTexturesByAge()` `Core/Materials/OOTexture.cpp:367` | ✅ |
| `+[OOTexture allTextures]` (377) | `allTextures()` `Core/Materials/OOTexture.cpp:376` | ✅ |
| `-[OOTexture dataSize]` (388) | `dataSize()` `Core/Materials/OOTexture.cpp:388` | ✅ |
| `-[OOTexture name]` (400) | `name()` `Core/Materials/OOTexture.cpp:400` | ✅ |
| `-[OOTexture forceRebind]` (407) | `forceRebind()` `Core/Materials/OOTexture.cpp:460` | ✅ |
| `-[OOTexture addToCaches]` (412) | `addToCaches()` `Core/Materials/OOTexture.cpp:465` | ✅ |
| `-[OOTexture removeFromCaches]` (441) | `removeFromCaches()` `Core/Materials/OOTexture.cpp:494` | ✅ |
| `+[OOTexture existingTextureForKey:]` (469) | `existingTextureForKey()` `Core/Materials/OOTexture.cpp:525` | ✅ |
| `+[OOTexture checkExtensions]` (481) | `checkExtensions()` `Core/Materials/OOTexture.cpp:540` | ✅ |
| `-[OOTexture retainInContext:]` (530) | — | ➖ |
| `-[OOTexture releaseInContext:]` (542) | — | ➖ |
| `-[OOTexture autoreleaseInContext:]` (554) | — | ➖ |
| `-[OOTexture retain]` (566) | — | ➖ |
| `-[OOTexture release]` (571) | — | ➖ |
| `-[OOTexture autorelease]` (576) | — | ➖ |
| `-[NSDictionary(OOTextureConveniences) oo_textureSpecifierForKey:defaultName:]` (586) | helper in `Core/Materials/OOMaterialSpecifier.cpp:48-52` (`cxx_OOTextureSpecFromObject(ValueFor(…))`) | ➖ |
| `-[NSArray(OOTextureConveniences) oo_textureSpecifierAtIndex:defaultName:]` (595) | — (callers call `cxx_OOTextureSpecFromObject` directly) | ➖ |
| `OOTextureSpecFromObject()` (602) | `cxx_OOTextureSpecFromObject()` `Core/Materials/OOTexture.cpp:594` | ✅ |
| `OOTextureComponentsForFormat()` (624) | `OOTextureComponentsForFormat()` `Core/Materials/OOTexture.cpp:621` | ✅ |
| `OOCubeMapsAvailable()` (643) | `OOCubeMapsAvailable()` `Core/Materials/OOTexture.cpp:640` | ✅ |
| `OOInterpretTextureSpecifier()` (648) | `cxx_OOInterpretTextureSpecifier()` `Core/Materials/OOTexture.cpp:645` | ❌ |
| `OOMakeTextureSpecifier()` (736) | `cxx_OOMakeTextureSpecifier()` `Core/Materials/OOTexture.cpp:739` | ✅ |
| `OOApplyTextureOptionDefaults()` (819) | `OOApplyTextureOptionDefaults()` `Core/Materials/OOTexture.cpp:823` | ✅ |
| `OOGenerateTextureCacheKey()` (862) | `OOGenerateTextureCacheKey()` `Core/Materials/OOTexture.cpp:866` | ✅ |
| `OOTextureCacheKeyForSpecifier()` (875) | `cxx_OOTextureCacheKeyForSpecifier()` `Core/Materials/OOTexture.cpp:879` | ⚠️ |
| `-[NSObject(OOExtensions) shortDescription]` (inherited, `Core/OOFoundation.m:59`) | `OOTexture::shortDescription()` `Core/Materials/OOTexture.cpp:447` | ❌ |

**Differences:**

- ❌ **low** — For a bad specifier type, the `OOInterpretTextureSpecifier` log text differs. `__PRETTY_FUNCTION__` in C++ expands to the full signature, not the bare C name. The "got %@" class (e.g. `GSInlineArray`) became the plist type name, or `(null)` for Object nodes, which a port comment acknowledges. The log class `kOOLogParameterError` is unchanged. upstream `Core/Materials/OOTexture.m:717` ↔ port `Core/Materials/OOTexture.cpp:720`
- ❌ **low** — `OOTexture::shortDescription()` hard-codes `<OOTexture 0x…>`. Upstream used `[self class]`, giving `<OOConcreteTexture …>`. This feeds `OOCombinedEmissionMapGenerator`'s debug `_diffuseDesc` text. upstream `Core/OOFoundation.m:65` ↔ port `Core/Materials/OOTexture.cpp:449`
- ⚠️ **low** — `OOTextureCacheKeyForSpecifier` now zero-initialises its outputs (empty name, options 0) when the specifier is not understood (port comment, :881). Upstream left them uninitialised, which was UB. upstream `:877-883` ↔ port `:882-888`
- ➖ **low** — The debug-monitor retain/release/autorelease tracing (`texture.allocTrace.retain/release/autoreleased`) is gone because there are no ObjC refcount hooks. `setTrace` and the `allocTrace.begin`/`dealloc` messages remain. upstream `:530-579` ↔ port —
- ✅ (emphasis: flags) — Every `kOOTexture*` flag value, mask, `kOOTextureDefinedFlags`, the rectangle and cube-map masks, and the default anisotropy (0.5) and LOD bias (-0.25) are identical. upstream `Core/Materials/OOTexture.h:45-105` ↔ port `Core/Materials/OOTexture.h:53-113`
- ✅ (emphasis: cache keys) — `OOGenerateTextureCacheKey` uses the same `%@%@%@:0x%.4X/%g/%g` format (as `%s`). It zeroes anisotropy and LOD bias for non-mipmap filtering the same way and masks `kOOTextureNoFNFMessage` before `OOApplyTextureOptionDefaults`. `OOApplyTextureOptionDefaults` is verbatim (`[UNIVERSE reducedDetail]` with nil → NO). `sLiveTextureCache` (`unordered_map`) is only looked up, never iterated. upstream `:819-873` ↔ port `:823-877`
- ✅ (subtle, upstream bug preserved) — `OOMakeTextureSpecifier` still stores the min-filter name under the `no_shrink` key. upstream `:768` ↔ port `:772`
- ✅ (subtle) — The 2-arg `textureWithName` and `textureWithConfiguration:extraOptions:` call `loadTextureWithName` directly. This bypasses the test-only `nameFactory` hook (assigned only in `tests/unit/core/*`), so production behaviour is identical. port `:170-178,190-201`

## OOTextureGenerator

<a id="ootexturegenerator"></a>
Upstream `src/Core/Materials/OOTextureGenerator.m` ↔ `Core/Materials/OOTextureGenerator.cpp`

**Status:** done — 5 rows: 5 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOTextureGenerator textureOptions]` (33) | `textureOptions()` `Core/Materials/OOTextureGenerator.cpp:32` | ✅ |
| `-[OOTextureGenerator anisotropy]` (38) | `anisotropy()` `Core/Materials/OOTextureGenerator.cpp:37` | ✅ |
| `-[OOTextureGenerator lodBias]` (43) | `lodBias()` `Core/Materials/OOTextureGenerator.cpp:42` | ✅ |
| `-[OOTextureGenerator cacheKey]` (48) | `cacheKey()` `Core/Materials/OOTextureGenerator.cpp:47` | ✅ |
| `-[OOTextureGenerator enqueue]` (53) | `enqueue()` `Core/Materials/OOTextureGenerator.cpp:52` | ✅ |

**Differences:**

- ✅ (emphasis: async) — `enqueue` adds the task to `OOAsyncWorkManager::sharedAsyncWorkManager()` at `kOOAsyncPriorityMedium`, as upstream did. `nil` → `std::nullopt` for the default cache key. upstream `Core/Materials/OOTextureGenerator.m:48-56` ↔ port `Core/Materials/OOTextureGenerator.cpp:47-55`

## OOTextureLoader

<a id="ootextureloader"></a>
Upstream `src/Core/Materials/OOTextureLoader.m` ↔ `Core/Materials/OOTextureLoader.cpp`

**Status:** done — 19 rows: 18 ✅ 0 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| shrink thresholds enum + file statics (43-56) | same `Core/Materials/OOTextureLoader.cpp:58-71` | ✅ |
| `+[OOTextureLoader loaderWithPath:options:]` (70) | `loaderWithPath()` `Core/Materials/OOTextureLoader.cpp:73` | ✅ |
| `+[OOTextureLoader loaderWithTextureSpecifier:extraOptions:folder:]` (99) | `loaderWithTextureSpecifier()` `Core/Materials/OOTextureLoader.cpp:105` | ✅ |
| `-[OOTextureLoader initWithPath:options:]` (120) | `initWithPath()` `Core/Materials/OOTextureLoader.cpp:133` | ✅ |
| `-[OOTextureLoader dealloc]` (179) | `~OOTextureLoader()` `Core/Materials/OOTextureLoader.cpp:187` | ✅ |
| `-[OOTextureLoader descriptionComponents]` (189) | `descriptionComponents()` `Core/Materials/OOTextureLoader.cpp:193` | ✅ |
| `-[OOTextureLoader shortDescriptionComponents]` (209) | `shortDescriptionComponents()` `Core/Materials/OOTextureLoader.cpp:213` | ✅ |
| `-[OOTextureLoader path]` (214) | `path()` `Core/Materials/OOTextureLoader.cpp:253` | ✅ |
| `-[OOTextureLoader isReady]` (219) | `isReady()` `Core/Materials/OOTextureLoader.cpp:258` | ✅ |
| `-[OOTextureLoader getResult:]` (224) | `getResult()` `Core/Materials/OOTextureLoader.cpp:263` | ✅ |
| `-[OOTextureLoader cacheKey]` (258) | `cacheKey()` `Core/Materials/OOTextureLoader.cpp:298` | ✅ |
| `-[OOTextureLoader loadTexture]` (263) | `loadTexture()` `Core/Materials/OOTextureLoader.cpp:303` | ✅ |
| `+[OOTextureLoader setUp]` (268) | `setUp()` `Core/Materials/OOTextureLoader.cpp:308` | ✅ |
| `-[OOTextureLoader performAsyncTask]` (288) | `performAsyncTask()` `Core/Materials/OOTextureLoader.cpp:331` | ✅ |
| `-[OOTextureLoader generateMipMapsForCubeMap]` (316) | `generateMipMapsForCubeMap()` `Core/Materials/OOTextureLoader.cpp:361` | ✅ |
| `-[OOTextureLoader applySettings]` (347) | `applySettings()` `Core/Materials/OOTextureLoader.cpp:392` | ✅ |
| `-[OOTextureLoader getDesiredWidth:andHeight:]` (445) | `getDesiredWidth()` `Core/Materials/OOTextureLoader.cpp:496` | ✅ |
| `-[OOTextureLoader completeAsyncTask]` (500) | `completeAsyncTask()` `Core/Materials/OOTextureLoader.cpp:551` (+ `hasCompleteAsyncTask()` true, `OOTextureLoader.h:135`) | ✅ |
| `-[NSObject(OOExtensions) shortDescription]` (inherited, `Core/OOFoundation.m:59`) | `OOTextureLoader::shortDescription()` `Core/Materials/OOTextureLoader.cpp:244` | ❌ |

**Differences:**

- ❌ **low** — `shortDescription()` prints `<OOTextureLoader …>` or `<OOTextureGenerator …>` (the former facade names) instead of the concrete class (`[self class]`, e.g. `<OOPNGTextureLoader …>`). This shows up in `OOCombinedEmissionMapGenerator`'s debug `_emissionDesc`/`_illuminationDesc`. `description()` does use the real class name. upstream `Core/OOFoundation.m:65` ↔ port `Core/Materials/OOTextureLoader.cpp:244-250`
- ✅ (emphasis: mipmap/scaling arithmetic) — The `getDesiredWidth` power-of-two rounding (`OORoundUpToPowerOf2_PixMap((2*w)/3)`), `sGLMaxSize`, `/8` and `sUserMaxSize/4` cube limits, and the reduced-detail halving at `_shrinkThreshold` are verbatim. So are the extra-shrink cap of 256, the cube→lat/long conversion size (`MIN(w*2,512)`, halved over 256), the mip buffer size `w*c*h*4/3 + 1` and the cube-face 16-byte rounding. upstream `Core/Materials/OOTextureLoader.m:316-498` ↔ port `Core/Materials/OOTextureLoader.cpp:361-549`
- ✅ (emphasis: async) — `loaderWithPath` queues the PNG loader with `addTask(…, kOOAsyncPriorityMedium)` and drops it if queueing fails. `getResult` waits with `waitForTaskToComplete(this)` when not ready. `completeAsyncTask` is reached because `hasCompleteAsyncTask()` returns true, which replaces `respondsToSelector:` (bead oo-9ht.14). `performAsyncTask` catches `OOException` as `@catch (NSException*)` did and rethrows other C++ exceptions. upstream `:91-94,233-235,288-313,500-503` ↔ port `:97-100,273-275,331-358,551-554`
- ✅ (subtle) — `max-texture-size` is read through `oo::Defaults` with `PListGet<unsigned int>` and default `0x80000000`, then `OORoundUpToPowerOf2_32` with a minimum of 64, as upstream. upstream `:275-280` ↔ port `:315-324`

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

**Status:** done — 30 rows: 23 ✅ 6 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `StretchVertically()` (91) | `StretchVertically()` `Core/OOTextureScaling.cpp:95` | ✅ |
| `StretchVertically()` (104) | `StretchVertically()` `Core/OOTextureScaling.cpp:108` | ✅ |
| `SqueezeVertically()` (115) | `SqueezeVertically()` `Core/OOTextureScaling.cpp:119` | ✅ |
| `StretchHorizontally()` (142) | `StretchHorizontally()` `Core/OOTextureScaling.cpp:145` | ✅ |
| `SqueezeHorizontally()` (171) | `SqueezeHorizontally()` `Core/OOTextureScaling.cpp:173` | ✅ |
| `OOScalePixMap()` (244) | `OOScalePixMap()` `Core/OOTextureScaling.cpp:245` | ✅ |
| `OOGenerateMipMaps()` (336) | `OOGenerateMipMaps()` `Core/OOTextureScaling.cpp:337` | ❌ |
| `GenerateMipMaps1()` (365) | `GenerateMipMaps1()` `Core/OOTextureScaling.cpp:366` | ✅ |
| `ScaleToHalf_1_x1()` (414) | `ScaleToHalf_1_x1()` `Core/OOTextureScaling.cpp:415` | ✅ |
| `ScaleToHalf_1_x4()` (453) | `ScaleToHalf_1_x4()` `Core/OOTextureScaling.cpp:454` | ✅ |
| `ScaleToHalf_1_x8()` (502) | `ScaleToHalf_1_x8()` `Core/OOTextureScaling.cpp:503` | ✅ |
| `GenerateMipMaps2()` (551) | `GenerateMipMaps2()` `Core/OOTextureScaling.cpp:552` | ✅ |
| `ScaleToHalf_2_x1()` (605) | `ScaleToHalf_2_x1()` `Core/OOTextureScaling.cpp:606` | ✅ |
| `GenerateMipMaps4()` (643) | `GenerateMipMaps4()` `Core/OOTextureScaling.cpp:644` | ✅ |
| `ScaleToHalf_4_x1()` (690) | `ScaleToHalf_4_x1()` `Core/OOTextureScaling.cpp:691` | ✅ |
| `ScaleToHalf_4_x2()` (743) | `ScaleToHalf_4_x2()` `Core/OOTextureScaling.cpp:744` | ✅ |
| `DumpMipMap()` (818) | `DumpMipMap()` `Core/OOTextureScaling.cpp:819` | ✅ |
| `StretchVerticallyN_x1()` (825) | `StretchVerticallyN_x1()` `Core/OOTextureScaling.cpp:826` | ⚠️ |
| `StretchVerticallyN_x4()` (869) | `StretchVerticallyN_x4()` `Core/OOTextureScaling.cpp:874` | ⚠️ |
| `StretchVerticallyN_x8()` (917) | `StretchVerticallyN_x8()` `Core/OOTextureScaling.cpp:926` | ⚠️ |
| `StretchHorizontally1()` (964) | `StretchHorizontally1()` `Core/OOTextureScaling.cpp:977` | ⚠️ |
| `StretchHorizontally2()` (1019) | `StretchHorizontally2()` `Core/OOTextureScaling.cpp:1036` | ⚠️ |
| `StretchHorizontally4()` (1081) | `StretchHorizontally4()` `Core/OOTextureScaling.cpp:1102` | ✅ |
| `SqueezeHorizontally1()` (1144) | `SqueezeHorizontally1()` `Core/OOTextureScaling.cpp:1165` | ✅ |
| `SqueezeVertically1()` (1202) | `SqueezeVertically1()` `Core/OOTextureScaling.cpp:1223` | ⚠️ |
| `SqueezeHorizontally2()` (1283) | `SqueezeHorizontally2()` `Core/OOTextureScaling.cpp:1310` | ✅ |
| `SqueezeVertically2()` (1340) | `SqueezeVertically2()` `Core/OOTextureScaling.cpp:1367` | ✅ |
| `SqueezeHorizontally4()` (1433) | `SqueezeHorizontally4()` `Core/OOTextureScaling.cpp:1460` | ✅ |
| `SqueezeVertically4()` (1490) | `SqueezeVertically4()` `Core/OOTextureScaling.cpp:1517` | ✅ |
| `EnsureCorrectDataSize()` (1548) | `EnsureCorrectDataSize()` `Core/OOTextureScaling.cpp:1575` | ✅ |

**Differences:**

- ⚠️ **med** — `SqueezeVertically1` changes `endY < lastRow` to `endY <= lastRow` to match the 2- and 4-channel versions (bead oo-9ht.115). Upstream counted the last row's `endWeight` in `weight` but not in `accum`, which darkened the last output row of a 1-channel (grayscale) non-integer vertical squeeze (port comment: 90 → 33 for 8 → 5). The port now produces different pixels there, which a golden showing such a texture could capture. upstream `Core/OOTextureScaling.m:1245` ↔ port `Core/OOTextureScaling.cpp:1272`
- ⚠️ **low** — `StretchVerticallyN_x1/x4/x8`: the copied last row is re-pointed to source row `height-1` (bead oo-snnh6, an ASan OOB read). Upstream copied from "row after the last blended row". That is the same row when `dstH < 2·srcH` and rows are unpadded. Otherwise upstream read past the buffer (UB) or into row padding, and the port reads the true last row. upstream `:861,909,957` ↔ port `Core/OOTextureScaling.cpp:865,917,969`
- ⚠️ **low** — `StretchHorizontally1/2`: the last-row look-ahead is clamped to `width-1` (oo-snnh6). The clamp fires only when `fractX>>12 == width`, where `weight1 = (srcW<<8)&0xFF = 0`, and it is the row's last pixel. Output is unchanged; only the OOB read is removed. upstream `:1012,1071` ↔ port `:1026-1029,1089-1092`
- ❌ **low** — The `OOGenerateMipMaps` parameter-error logs use `__PRETTY_FUNCTION__`, which in C++ is the full signature (`bool OOGenerateMipMaps(void*, …)`) rather than the C name, so the `kOOLogParameterError` text differs. upstream `:339,361` ↔ port `:340,362`
- ✅ (emphasis: mipmap/scaling arithmetic) — Apart from the items above, `diff -w` shows only casts, `BOOL`→`bool`, `NSCParameterAssert`→`OOCParameterAssert` and `NSException`→`OORaiseException` (same `NSInternalInconsistencyException`-equivalent name and message). All fixed-point (24.8, 20.12) maths, masks and the `ScaleToHalf` averaging are verbatim. upstream `Core/OOTextureScaling.m` ↔ port `Core/OOTextureScaling.cpp`

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

**Status:** done — 9 rows: 0 ✅ 0 ⚠️ 0 ❌ 9 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `FloatRGBFromDictColor()` (46) | — (compiled out upstream) | ➖ |
| `Blend()` (59) | — | ➖ |
| `PlanetTextureColor()` (68) | — | ➖ |
| `+[TextureStore getPlanetTextureNameFor:intoData:width:height:]` (100) | — | ➖ |
| `+[TextureStore getCloudTextureNameFor:impress:bias:intoData:width:height:]` (139) | — | ➖ |
| `fillRanNoiseBuffer()` (167) | — | ➖ |
| `addNoise()` (176) | — | ➖ |
| `q_factor()` (201) | — | ➖ |
| `fillSquareImageDataWithCloudTexture()` (236) | — | ➖ |

**Differences:**

- ➖ **low** — The whole file is wrapped in `#if !NEW_PLANETS`, and `NEW_PLANETS` is defined as 1 at `Core/Entities/OOStellarBody.h:35`. Upstream therefore never compiled `TextureStore`, and its only caller, the old `PlanetEntity`, is excluded the same way. The port has no counterpart (no `TextureStore` symbol under `src/`). Planet textures come from `OOPlanetTextureGenerator`/`OOStandaloneAtmosphereGenerator`, so no shipped behaviour is lost. upstream `Core/TextureStore.m:27,338`, `Core/TextureStore.h:33` ↔ port —

