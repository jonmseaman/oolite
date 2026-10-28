# Parity findings: Materials, textures, meshes, drawables, OpenGL

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## OOBasicMaterial

<a id="oobasicmaterial"></a>
Upstream `src/Core/Materials/OOBasicMaterial.m` ↔ `Core/Materials/OOBasicMaterial.cpp`

**Status:** done — 33 rows: 32 ✅ 1 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOBasicMaterial initWithName:]` (41) | `OOBasicMaterial::initWithName(name)` `Core/Materials/OOBasicMaterial.cpp:61` | ✅ |
| `-[OOBasicMaterial initWithName:configuration:]` (57) | `OOBasicMaterial::initWithName(name, configuration)` `Core/Materials/OOBasicMaterial.cpp:71` | ✅ |
| `-[OOBasicMaterial dealloc]` (94) | none (implicit destructor; `willDealloc` not called) | ⚠️ |
| `-[OOBasicMaterial name]` (102) | `OOBasicMaterial::name` `Core/Materials/OOBasicMaterial.cpp:108` | ✅ |
| `-[OOBasicMaterial doApply]` (107) | `OOBasicMaterial::doApply` `Core/Materials/OOBasicMaterial.cpp:113` | ✅ |
| `-[OOBasicMaterial unapplyWithNext:]` (121) | `OOBasicMaterial::unapplyWithNext` `Core/Materials/OOBasicMaterial.cpp:128` | ✅ |
| `-[OOBasicMaterial diffuseColor]` (130) | `OOBasicMaterial::diffuseColor` `Core/Materials/OOBasicMaterial.cpp:138` | ✅ |
| `-[OOBasicMaterial setDiffuseColor:]` (138) | `OOBasicMaterial::setDiffuseColor` `Core/Materials/OOBasicMaterial.cpp:146` | ✅ |
| `-[OOBasicMaterial setAmbientAndDiffuseColor:]` (148) | `OOBasicMaterial::setAmbientAndDiffuseColor` `Core/Materials/OOBasicMaterial.cpp:156` | ✅ |
| `-[OOBasicMaterial specularColor]` (154) | `OOBasicMaterial::specularColor` `Core/Materials/OOBasicMaterial.cpp:162` | ✅ |
| `-[OOBasicMaterial setSpecularColor:]` (162) | `OOBasicMaterial::setSpecularColor` `Core/Materials/OOBasicMaterial.cpp:170` | ✅ |
| `-[OOBasicMaterial ambientColor]` (172) | `OOBasicMaterial::ambientColor` `Core/Materials/OOBasicMaterial.cpp:180` | ✅ |
| `-[OOBasicMaterial setAmbientColor:]` (180) | `OOBasicMaterial::setAmbientColor` `Core/Materials/OOBasicMaterial.cpp:188` | ✅ |
| `-[OOBasicMaterial emmisionColor]` (190) | `OOBasicMaterial::emmisionColor` `Core/Materials/OOBasicMaterial.cpp:198` | ✅ |
| `-[OOBasicMaterial setEmissionColor:]` (198) | `OOBasicMaterial::setEmissionColor` `Core/Materials/OOBasicMaterial.cpp:206` | ✅ |
| `-[OOBasicMaterial getDiffuseComponents:]` (208) | `OOBasicMaterial::getDiffuseComponents` `Core/Materials/OOBasicMaterial.cpp:216` | ✅ |
| `-[OOBasicMaterial setDiffuseComponents:]` (213) | `OOBasicMaterial::setDiffuseComponents` `Core/Materials/OOBasicMaterial.cpp:221` | ✅ |
| `-[OOBasicMaterial setAmbientAndDiffuseComponents:]` (218) | `OOBasicMaterial::setAmbientAndDiffuseComponents` `Core/Materials/OOBasicMaterial.cpp:226` | ✅ |
| `-[OOBasicMaterial getSpecularComponents:]` (224) | `OOBasicMaterial::getSpecularComponents` `Core/Materials/OOBasicMaterial.cpp:232` | ✅ |
| `-[OOBasicMaterial setSpecularComponents:]` (229) | `OOBasicMaterial::setSpecularComponents` `Core/Materials/OOBasicMaterial.cpp:237` | ✅ |
| `-[OOBasicMaterial getAmbientComponents:]` (234) | `OOBasicMaterial::getAmbientComponents` `Core/Materials/OOBasicMaterial.cpp:242` | ✅ |
| `-[OOBasicMaterial setAmbientComponents:]` (239) | `OOBasicMaterial::setAmbientComponents` `Core/Materials/OOBasicMaterial.cpp:247` | ✅ |
| `-[OOBasicMaterial getEmissionComponents:]` (244) | `OOBasicMaterial::getEmissionComponents` `Core/Materials/OOBasicMaterial.cpp:252` | ✅ |
| `-[OOBasicMaterial setEmissionComponents:]` (249) | `OOBasicMaterial::setEmissionComponents` `Core/Materials/OOBasicMaterial.cpp:257` | ✅ |
| `-[OOBasicMaterial setDiffuseRed:green:blue:alpha:]` (254) | `OOBasicMaterial::setDiffuseRed` `Core/Materials/OOBasicMaterial.cpp:262` | ✅ |
| `-[OOBasicMaterial setAmbientAndDiffuseRed:green:blue:alpha:]` (262) | `OOBasicMaterial::setAmbientAndDiffuseRed` `Core/Materials/OOBasicMaterial.cpp:270` | ✅ |
| `-[OOBasicMaterial setSpecularRed:green:blue:alpha:]` (268) | `OOBasicMaterial::setSpecularRed` `Core/Materials/OOBasicMaterial.cpp:276` | ✅ |
| `-[OOBasicMaterial setAmbientRed:green:blue:alpha:]` (276) | `OOBasicMaterial::setAmbientRed` `Core/Materials/OOBasicMaterial.cpp:284` | ✅ |
| `-[OOBasicMaterial setEmissionRed:green:blue:alpha:]` (284) | `OOBasicMaterial::setEmissionRed` `Core/Materials/OOBasicMaterial.cpp:292` | ✅ |
| `-[OOBasicMaterial shininess]` (292) | `OOBasicMaterial::shininess` `Core/Materials/OOBasicMaterial.cpp:300` | ✅ |
| `-[OOBasicMaterial setShininess:]` (297) | `OOBasicMaterial::setShininess` `Core/Materials/OOBasicMaterial.cpp:305` | ✅ |
| `-[OOBasicMaterial permitSpecular]` (302) | `OOBasicMaterial::permitSpecular` `Core/Materials/OOBasicMaterial.cpp:310` | ✅ |
| `-[OOBasicMaterial allTextures]` (308) | `OOBasicMaterial::allTextures` `Core/Materials/OOBasicMaterial.cpp:316` | ✅ |

**Differences:**

- ⚠️ **low** — `-dealloc`'s `[super willDealloc]` has no counterpart. The current-material slot holds an `oo::Ref`, so a material cannot be destroyed while it is current, and nothing calls `willDealloc()`. This is documented at `Core/Materials/OOMaterial.h:111-116` (proposed ADR-0056, amendments oo-smy and oo-9ht.8) — upstream `Core/Materials/OOBasicMaterial.m:94-100` ↔ port `Core/Materials/OOMaterial.h:116`
- Subtle (✅): the configuration defaults are the same. Diffuse and ambient default to white; specular and emission default to (0,0,0,1); shininess defaults to 0. With no exponent key the specular exponent defaults to 10, `setShininess` clamps to 128, and an exponent above 255 wraps through `uint8_t` exactly as upstream did. The specular colour defaults to 0.2 grey. Ambient falls back to the diffuse colour. A white `diffuse_color` and a black `emission_color` are dropped by the specifier. — upstream `Core/Materials/OOBasicMaterial.m:57-92` ↔ port `Core/Materials/OOBasicMaterial.cpp:71-106`
- Subtle (✅): `-isMemberOfClass:` became `typeid(*this) == typeid(OOBasicMaterial)` and `-isKindOfClass:` became `dynamic_cast`. For a null `next`, `dynamic_cast` gives null, so the default material is applied as it was for nil — port `Core/Materials/OOBasicMaterial.cpp:121,131`

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

**Status:** done — 47 rows: 46 ✅ 1 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOSynthesizeMaterialShader()` (264) | `OOSynthesizeMaterialShader` `Core/Materials/OODefaultShaderSynthesizer.cpp:81` | ✅ |
| `-[OODefaultShaderSynthesizer initWithMaterialConfiguration:]` (301) | `OODefaultShaderSynthesizer::OODefaultShaderSynthesizer` `Core/Materials/OODefaultShaderSynthesizer.cpp:105` | ✅ |
| `-[OODefaultShaderSynthesizer dealloc]` (314) | implicit destructor (comment `Core/Materials/OODefaultShaderSynthesizer.cpp:112`) | ✅ |
| `-[OODefaultShaderSynthesizer vertexShader]` (327) | `vertexShader` `Core/Materials/OODefaultShaderSynthesizer.cpp:116` | ✅ |
| `-[OODefaultShaderSynthesizer fragmentShader]` (332) | `fragmentShader` `Core/Materials/OODefaultShaderSynthesizer.cpp:121` | ✅ |
| `-[OODefaultShaderSynthesizer textureSpecifications]` (337) | `textureSpecifications` `Core/Materials/OODefaultShaderSynthesizer.cpp:126` | ✅ |
| `-[OODefaultShaderSynthesizer uniformSpecifications]` (346) | `uniformSpecifications` `Core/Materials/OODefaultShaderSynthesizer.cpp:131` | ✅ |
| `-[OODefaultShaderSynthesizer run]` (355) | `run` `Core/Materials/OODefaultShaderSynthesizer.cpp:136` | ✅ |
| `-[OODefaultShaderSynthesizer materialKey]` (379) | `materialKey` `Core/Materials/OODefaultShaderSynthesizer.cpp:160` | ✅ |
| `-[OODefaultShaderSynthesizer entityName]` (384) | `entityName` `Core/Materials/OODefaultShaderSynthesizer.cpp:165` | ✅ |
| `AppendIfNotEmpty()` (391) | `AppendIfNotEmpty` `Core/Materials/OODefaultShaderSynthesizer.cpp:174` | ✅ |
| `GetExtractMode()` (402) | `GetExtractMode` `Core/Materials/OODefaultShaderSynthesizer.cpp:196` | ✅ |
| `-[OODefaultShaderSynthesizer appendVariable:ofType:withPrefix:to:]` (424) | `appendVariable` `Core/Materials/OODefaultShaderSynthesizer.cpp:223` | ✅ |
| `-[OODefaultShaderSynthesizer addAttribute:ofType:]` (431) | `addAttribute` `Core/Materials/OODefaultShaderSynthesizer.cpp:230` | ✅ |
| `-[OODefaultShaderSynthesizer addVarying:ofType:]` (436) | `addVarying` `Core/Materials/OODefaultShaderSynthesizer.cpp:235` | ✅ |
| `-[OODefaultShaderSynthesizer addVertexUniform:ofType:]` (441) | `addVertexUniform` `Core/Materials/OODefaultShaderSynthesizer.cpp:240` | ✅ |
| `-[OODefaultShaderSynthesizer addFragmentUniform:ofType:]` (446) | `addFragmentUniform` `Core/Materials/OODefaultShaderSynthesizer.cpp:245` | ✅ |
| `-[OODefaultShaderSynthesizer defineBindingUniform:ofType:]` (451) | `defineBindingUniform` `Core/Materials/OODefaultShaderSynthesizer.cpp:250` | ✅ |
| `-[OODefaultShaderSynthesizer composeVertexShader]` (484) | `composeVertexShader` `Core/Materials/OODefaultShaderSynthesizer.cpp:287` | ✅ |
| `-[OODefaultShaderSynthesizer composeFragmentShader]` (505) | `composeFragmentShader` `Core/Materials/OODefaultShaderSynthesizer.cpp:304` | ✅ |
| `KeyFromTextureParameters()` (537) | `KeyFromTextureParameters` `Core/Materials/OODefaultShaderSynthesizer.cpp:334` | ✅ |
| `KeyFromTextureSpec()` (549) | `KeyFromTextureSpec` `Core/Materials/OODefaultShaderSynthesizer.cpp:346` | ✅ |
| `-[OODefaultShaderSynthesizer assignIDForTexture:]` (562) | `assignIDForTexture` `Core/Materials/OODefaultShaderSynthesizer.cpp:361` | ✅ |
| `-[OODefaultShaderSynthesizer textureIDForSpec:]` (613) | `textureIDForSpec` `Core/Materials/OODefaultShaderSynthesizer.cpp:413` | ✅ |
| `-[OODefaultShaderSynthesizer setUpOneTexture:]` (618) | `setUpOneTexture` `Core/Materials/OODefaultShaderSynthesizer.cpp:419` | ✅ |
| `-[OODefaultShaderSynthesizer getSampleName:andSwizzleOp:forTextureSpec:]` (632) | `getSampleName` `Core/Materials/OODefaultShaderSynthesizer.cpp:432` | ✅ |
| `-[OODefaultShaderSynthesizer readRGBForTextureSpec:mapName:]` (643) | `readRGBForTextureSpec` `Core/Materials/OODefaultShaderSynthesizer.cpp:443` | ✅ |
| `-[OODefaultShaderSynthesizer readOneChannelForTextureSpec:mapName:]` (664) | `readOneChannelForTextureSpec` `Core/Materials/OODefaultShaderSynthesizer.cpp:464` | ✅ |
| `-[OODefaultShaderSynthesizer performStage:]` (684) | `performStage` `Core/Materials/OODefaultShaderSynthesizer.cpp:484` | ✅ |
| `-[OODefaultShaderSynthesizer createTemporaries]` (700) | `createTemporaries` `Core/Materials/OODefaultShaderSynthesizer.cpp:500` | ✅ |
| `-[OODefaultShaderSynthesizer destroyTemporaries]` (725) | `destroyTemporaries` `Core/Materials/OODefaultShaderSynthesizer.cpp:525` | ✅ |
| `-[OODefaultShaderSynthesizer writeTextureCoordRead]` (757) | `writeTextureCoordRead` `Core/Materials/OODefaultShaderSynthesizer.cpp:553` | ✅ |
| `-[OODefaultShaderSynthesizer writeDiffuseColorTermIfNeeded]` (806) | `writeDiffuseColorTermIfNeeded` `Core/Materials/OODefaultShaderSynthesizer.cpp:602` | ✅ |
| `-[OODefaultShaderSynthesizer writeDiffuseColorTerm]` (843) | `writeDiffuseColorTerm` `Core/Materials/OODefaultShaderSynthesizer.cpp:639` | ✅ |
| `-[OODefaultShaderSynthesizer writeDiffuseLighting]` (852) | `writeDiffuseLighting` `Core/Materials/OODefaultShaderSynthesizer.cpp:648` | ✅ |
| `-[OODefaultShaderSynthesizer writeLightVector]` (874) | `writeLightVector` `Core/Materials/OODefaultShaderSynthesizer.cpp:670` | ✅ |
| `-[OODefaultShaderSynthesizer writeEyeVector]` (887) | `writeEyeVector` `Core/Materials/OODefaultShaderSynthesizer.cpp:682` | ✅ |
| `-[OODefaultShaderSynthesizer writeVertexTangentBasis]` (898) | `writeVertexTangentBasis` `Core/Materials/OODefaultShaderSynthesizer.cpp:693` | ✅ |
| `-[OODefaultShaderSynthesizer writeNormalIfNeeded]` (910) | `writeNormalIfNeeded` `Core/Materials/OODefaultShaderSynthesizer.cpp:704` | ✅ |
| `-[OODefaultShaderSynthesizer writeNormal]` (936) | `writeNormal` `Core/Materials/OODefaultShaderSynthesizer.cpp:730` | ✅ |
| `-[OODefaultShaderSynthesizer writeSpecularLighting]` (945) | `writeSpecularLighting` `Core/Materials/OODefaultShaderSynthesizer.cpp:739` | ✅ |
| `-[OODefaultShaderSynthesizer writeLightMaps]` (1059) | `writeLightMaps` `Core/Materials/OODefaultShaderSynthesizer.cpp:850` | ✅ |
| `-[OODefaultShaderSynthesizer writeVertexPosition]` (1163) | `writeVertexPosition` `Core/Materials/OODefaultShaderSynthesizer.cpp:957` | ✅ |
| `-[OODefaultShaderSynthesizer writeTotalColor]` (1170) | `writeTotalColor` `Core/Materials/OODefaultShaderSynthesizer.cpp:963` | ✅ |
| `-[OODefaultShaderSynthesizer writeFinalColorComposite]` (1175) | `writeFinalColorComposite` `Core/Materials/OODefaultShaderSynthesizer.cpp:968` | ✅ |
| `CanonicalizeMaterialSpecifier()` (1198) | `CanonicalizeMaterialSpecifier` `Core/Materials/OODefaultShaderSynthesizer.cpp:1033` | ⚠️ |
| `FormatFloat()` (1443) | `FormatFloat` `Core/Materials/OODefaultShaderSynthesizer.cpp:1255` | ✅ |

**Differences:**

- ⚠️ **low** — `light_map` entry whose `color` is a non-array description that is not a colour: upstream did `setObject:nil` (throws `NSInvalidArgumentException` outside `-run`'s `@try`); port erases the key and continues. Also nil `materialKey` / non-string diffuse `name` no longer throw (port uses `""` / keeps the dict). Commented at `:1181,1086` — upstream `Core/Materials/OODefaultShaderSynthesizer.m:1236-1252,1368-1372` ↔ port `Core/Materials/OODefaultShaderSynthesizer.cpp:1069-1088,1177-1186`
- Subtle (✅) — **emitted GLSL**: every format string, the indentation and tabs (`appendVariable` padding `(23-len)/4`), the section headers (`// Attributes`, `// Uniforms`, …), the trimming of trailing `"\t\n"`, `FormatFloat` (`%lli.0`, else `%g`), the `%.1f` specular exponent, the `uTexture%zu`/`tex%zuSample` naming and the stage order all match line for line. No change in the emitted shader text was found.
- Subtle (✅): upstream bugs are kept. The second texture with an existing key looks up its ID by `texName` and so gets 0 (commented at `:405`). `_entityName` stays nil (`:109`). `defineBindingUniform` declares the uniform with the *binding* type (`float`/`vector`/`color`), not the GLSL type, so the emitted `uniform vector uX;` is invalid GLSL. `OOSynthesizeMaterialShader` returns YES even when synthesis fails. — upstream `Core/Materials/OODefaultShaderSynthesizer.m:296,308,607,1143` ↔ port `Core/Materials/OODefaultShaderSynthesizer.cpp:102,109,406,937`
- Subtle (✅): `run` catches only game exceptions (`oo::CurrentOOException`) and rethrows the rest; upstream `@catch (NSException*)`. Every raise inside the stages is `OORaiseException`, so all of them are caught — port `Core/Materials/OODefaultShaderSynthesizer.cpp:149-155`
- Note: the synthesizer is not reached in either tree. `USE_NEW_SHADER_SYNTHESIZER` is 0 (`OOMaterialConvenienceCreators.m:29` ↔ `.cpp:29`), so none of the above is live today.
- Subtle (✅ low): the `material.canonicalForm` log prints `oo::DescriptionOf()` of the dictionaries, not `-[NSDictionary description]`. The text can differ in formatting and key order — port `Core/Materials/OODefaultShaderSynthesizer.cpp:1250`

## OOMaterial

<a id="oomaterial"></a>
Upstream `src/Core/Materials/OOMaterial.m` ↔ `Core/Materials/OOMaterial.cpp`

**Status:** done — 16 rows: 15 ✅ 1 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOMaterial setUp]` (36) | `OOMaterial::setUp` `Core/Materials/OOMaterial.cpp:53` | ✅ |
| `-[OOMaterial dealloc]` (41) | none (no destructor calls `willDealloc`) | ⚠️ |
| `-[OOMaterial descriptionComponents]` (49) | `OOMaterial::descriptionComponents` `Core/Materials/OOMaterial.cpp:59` | ✅ |
| `-[OOMaterial name]` (54) | `OOMaterial::name` `Core/Materials/OOMaterial.cpp:76` | ✅ |
| `-[OOMaterial apply]` (61) | `OOMaterial::apply` `Core/Materials/OOMaterial.cpp:83` | ✅ |
| `+[OOMaterial applyNone]` (72) | `OOMaterial::applyNone` `Core/Materials/OOMaterial.cpp:94` | ✅ |
| `+[OOMaterial current]` (79) | `OOMaterial::current` `Core/Materials/OOMaterial.cpp:101` | ✅ |
| `-[OOMaterial ensureFinishedLoading]` (84) | `OOMaterial::ensureFinishedLoading` `Core/Materials/OOMaterial.cpp:124` | ✅ |
| `-[OOMaterial isFinishedLoading]` (88) | `OOMaterial::isFinishedLoading` `Core/Materials/OOMaterial.cpp:128` | ✅ |
| `-[OOMaterial setBindingTarget:]` (93) | `OOMaterial::setBindingTarget` `Core/Materials/OOMaterial.cpp:133` | ✅ |
| `-[OOMaterial wantsNormalsAsTextureCoordinates]` (97) | `OOMaterial::wantsNormalsAsTextureCoordinates` `Core/Materials/OOMaterial.cpp:137` | ✅ |
| `-[OOMaterial countOfTextureUnitsWithBaseCoordinates]` (103) | `OOMaterial::countOfTextureUnitsWithBaseCoordinates` `Core/Materials/OOMaterial.cpp:143` | ✅ |
| `-[OOMaterial allTextures]` (110) | `OOMaterial::allTextures` `Core/Materials/OOMaterial.cpp:150` | ✅ |
| `-[OOMaterial(OOSubclassInterface) doApply]` (120) | `OOMaterial::doApply` `Core/Materials/OOMaterial.cpp:156` | ✅ |
| `-[OOMaterial(OOSubclassInterface) unapplyWithNext:]` (126) | `OOMaterial::unapplyWithNext` `Core/Materials/OOMaterial.cpp:162` | ✅ |
| `-[OOMaterial(OOSubclassInterface) willDealloc]` (131) | `OOMaterial::willDealloc` `Core/Materials/OOMaterial.cpp:167` | ✅ |

**Differences:**

- ⚠️ **low** — `-dealloc`'s `[self willDealloc]` is gone. `willDealloc()` has the same body (it logs `shader.dealloc.imbalance`, unapplies, and detaches the slot without releasing it) but nothing calls it, because the `oo::Ref` slot makes the imbalance impossible. This is documented at `Core/Materials/OOMaterial.h:111-116` (proposed ADR-0056, amendments oo-smy and oo-9ht.8) — upstream `Core/Materials/OOMaterial.m:41-47` ↔ port `Core/Materials/OOMaterial.cpp:167-174`
- Subtle (✅): `allTextures` answers an empty vector where upstream answered nil (debug monitor only). `descriptionComponents` prints `"(null)"` in quotes for a nil name, which is what `%@` printed — port `Core/Materials/OOMaterial.cpp:61,150-153`

## OOMaterialConvenienceCreators

<a id="oomaterialconveniencecreators"></a>
Upstream `src/Core/Materials/OOMaterialConvenienceCreators.m` ↔ `Core/Materials/OOMaterialConvenienceCreators.cpp`

**Status:** done — 15 rows: 14 ✅ 0 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOMaterial(OOConvenienceCreators) synthesizeMaterialDictionaryWithName:]` (88) | `OOMaterial::synthesizeMaterialDictionaryWithName` `Core/Materials/OOMaterialConvenienceCreators.cpp:93` | ✅ |
| `+[OOMaterial(OOConvenienceCreators) defaultShaderMaterialWithName:]` (142) | `OOMaterial::defaultShaderMaterialWithName` `Core/Materials/OOMaterialConvenienceCreators.cpp:151` | ✅ |
| `+[OOMaterial(OOConvenienceCreators) initialize]` (194) | `DumpShaderSource` `Core/Materials/OOMaterialConvenienceCreators.cpp:212` | ✅ |
| `+[OOMaterial(OOConvenienceCreators) defaultShaderMaterialWithName:]` (200) | `OOMaterial::defaultShaderMaterialWithName` (new-synthesizer branch) `Core/Materials/OOMaterialConvenienceCreators.cpp:221` | ❌ |
| `+[OOMaterial(OOConvenienceCreators) materialWithName:]` (254) | `OOMaterial::materialWithName(…configuration…)` `Core/Materials/OOMaterialConvenienceCreators.cpp:282` | ✅ |
| `+[OOMaterial(OOConvenienceCreators) materialWithName:]` (309) | `OOMaterial::materialWithName(…materialDict, shadersDict…)` `Core/Materials/OOMaterialConvenienceCreators.cpp:334` | ✅ |
| `SetUniform()` (352) | `SetUniform` `Core/Materials/OOMaterialConvenienceCreators.cpp:380` | ✅ |
| `SetUniformFloat()` (357) | `SetUniformFloat` `Core/Materials/OOMaterialConvenienceCreators.cpp:387` | ✅ |
| `AddTexture()` (362) | `AddTexture` `Core/Materials/OOMaterialConvenienceCreators.cpp:392` | ✅ |
| `AddColorIfAppropriate()` (377) | `AddColorIfAppropriate` `Core/Materials/OOMaterialConvenienceCreators.cpp:408` | ✅ |
| `AddMacroColorIfAppropriate()` (388) | `AddMacroColorIfAppropriate` `Core/Materials/OOMaterialConvenienceCreators.cpp:421` | ✅ |
| `SynthDiffuse()` (402) | `SynthDiffuse` `Core/Materials/OOMaterialConvenienceCreators.cpp:433` | ✅ |
| `SynthEmissionAndIllumination()` (421) | `SynthEmissionAndIllumination` `Core/Materials/OOMaterialConvenienceCreators.cpp:452` | ✅ |
| `SynthNormalMap()` (467) | `SynthNormalMap` `Core/Materials/OOMaterialConvenienceCreators.cpp:498` | ✅ |
| `SynthSpecular()` (489) | `SynthSpecular` `Core/Materials/OOMaterialConvenienceCreators.cpp:520` | ✅ |

**Differences:**

- ❌ **low** — the dead `USE_NEW_SHADER_SYNTHESIZER` branch would not compile if it were enabled: `ResourceManager::writeDiagnosticString(synthesized ?, std::string(), …)`. Its intent also writes empty strings where upstream dumped the vertex and fragment sources. The branch is disabled (`#define USE_NEW_SHADER_SYNTHESIZER 0`), so there is no runtime effect today — upstream `Core/Materials/OOMaterialConvenienceCreators.m:227-228` ↔ port `Core/Materials/OOMaterialConvenienceCreators.cpp:255-256`
- Subtle (✅ low) — the synthesized-config cache key is `cacheKey/name/<oo::DescriptionOf(config)>` where upstream used `%@` of the NSDictionary, and a nil name gives `""` where upstream gave `(null)`. Only cache hits are affected; old cache entries keyed upstream's way will not match — upstream `Core/Materials/OOMaterialConvenienceCreators.m:162` ↔ port `Core/Materials/OOMaterialConvenienceCreators.cpp:174-177`
- Subtle (✅): upstream reassigned `cacheKey` to the long key before passing it to `+materialWithName:…`; the port passes the original. The only reader is `defaultShaderMaterialWithName`, which returns early on the loop check (`_oo_is_synthesized_config`) before it uses the key, so there is no observable difference — port `Core/Materials/OOMaterialConvenienceCreators.cpp:195-200`
- Subtle (✅): the synthesized defaults are kept. `diffuse_map = ""` when there is no diffuse map. `AddTexture` with a non-nil `nonShaderKey` always writes `diffuse_map`, so a specular map also overwrites `diffuse_map` (upstream bug, comment `:400`). The texture budget is 3 under reducedDetail. The hull-heat, time and fog uniforms appear only at DETAIL_LEVEL_SHADERS. `uGammaCorrect` is a float 0/1. `shininess` is unsigned. — port `Core/Materials/OOMaterialConvenienceCreators.cpp:113-146,392-406,520-572`

## OOMaterialSpecifier

<a id="oomaterialspecifier"></a>
Upstream `src/Core/Materials/OOMaterialSpecifier.m` ↔ `Core/Materials/OOMaterialSpecifier.cpp`

**Status:** done — 24 rows: 24 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| key constants `kOOMaterial*Name` (35-68) *(added)* | `cxx_kOOMaterial*Name` `Core/Materials/OOMaterialSpecifier.h:71-98`, `kOOMaterialDefaultParallaxScale` `:100` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_rawSpecularExponentValue]` (73) | `RawSpecularExponentValue` `Core/Materials/OOMaterialSpecifier.cpp:64` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_diffuseColor]` (81) | `cxx_OOMaterialDiffuseColor` `Core/Materials/OOMaterialSpecifier.cpp:74` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_ambientColor]` (92) | `cxx_OOMaterialAmbientColor` `Core/Materials/OOMaterialSpecifier.cpp:85` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_specularColor]` (100) | `cxx_OOMaterialSpecularColor` `Core/Materials/OOMaterialSpecifier.cpp:93` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_specularModulateColor]` (111) | `cxx_OOMaterialSpecularModulateColor` `Core/Materials/OOMaterialSpecifier.cpp:104` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_emissionColor]` (120) | `cxx_OOMaterialEmissionColor` `Core/Materials/OOMaterialSpecifier.cpp:113` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_emissionModulateColor]` (131) | `cxx_OOMaterialEmissionModulateColor` `Core/Materials/OOMaterialSpecifier.cpp:124` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_illuminationModulateColor]` (140) | `cxx_OOMaterialIlluminationModulateColor` `Core/Materials/OOMaterialSpecifier.cpp:133` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_diffuseMapSpecifierWithDefaultName:]` (149) | `cxx_OOMaterialDiffuseMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:142` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_combinedSpecularMapSpecifier]` (154) | `cxx_OOMaterialCombinedSpecularMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:147` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_specularColorMapSpecifier]` (161) | `cxx_OOMaterialSpecularColorMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:154` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_specularExponentMapSpecifier]` (171) | `cxx_OOMaterialSpecularExponentMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:164` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_normalMapSpecifier]` (181) | `cxx_OOMaterialNormalMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:174` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_parallaxMapSpecifier]` (188) | `cxx_OOMaterialParallaxMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:181` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_normalAndParallaxMapSpecifier]` (200) | `cxx_OOMaterialNormalAndParallaxMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:192` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_emissionMapSpecifier]` (205) | `cxx_OOMaterialEmissionMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:197` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_illuminationMapSpecifier]` (210) | `cxx_OOMaterialIlluminationMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:202` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_emissionAndIlluminationMapSpecifier]` (215) | `cxx_OOMaterialEmissionAndIlluminationMapSpecifier` `Core/Materials/OOMaterialSpecifier.cpp:207` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_parallaxScale]` (222) | `cxx_OOMaterialParallaxScale` `Core/Materials/OOMaterialSpecifier.cpp:214` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_parallaxBias]` (227) | `cxx_OOMaterialParallaxBias` `Core/Materials/OOMaterialSpecifier.cpp:219` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_gammaCorrect]` (232) | `cxx_OOMaterialGammaCorrect` `Core/Materials/OOMaterialSpecifier.cpp:224` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_gloss]` (237) | `cxx_OOMaterialGloss` `Core/Materials/OOMaterialSpecifier.cpp:229` | ✅ |
| `-[NSDictionary(OOMateralProperties) oo_specularExponent]` (242) | `cxx_OOMaterialSpecularExponent` `Core/Materials/OOMaterialSpecifier.cpp:234` | ✅ |

**Differences:**

- Subtle (✅): every key spelling (`diffuse_color`/`diffuse`, `ambient_color`/`ambient`, `specular_color`/`specular`, `specular_modulate_color`, `emission_color`/`emission`, `emission_modulate_color`, `illumination_modulate_color`, the ten `*_map` keys, `parallax_scale`, `parallax_bias`, `gamma_correct`, `gloss`, `specular_exponent`/`shininess`, `light_map`) matches. So do the texture-specifier keys in `OOTexture.h:316-333`, including `illumination_mode`, `color`, `self_color`, `scale_factor`, `binding` and `extract_channel`.
- Subtle (✅): the defaults are the same. The raw exponent is -1 when absent. The exponent then becomes 128 if shaders are on and an exponent map exists, otherwise 10. The specular colour is 0.2 grey, alpha 1. The specular modulate colour is white. Parallax scale is 0.01 and bias is 0. Gloss is 0.375, clamped to 0–1. `gamma_correct` is `!no-gamma-correct`. White diffuse, emission-modulate and illumination-modulate colours are dropped, and so is a black emission colour. "nil is not white/black" is kept explicitly (`:80,119,128,137`).
- Subtle (✅): a nil `UNIVERSE` now answers `useShaders` false, where a message to nil gave NO — port `Core/Materials/OOMaterialSpecifier.cpp:238`

## OOMultiTextureMaterial

<a id="oomultitexturematerial"></a>
Upstream `src/Core/Materials/OOMultiTextureMaterial.m` ↔ `Core/Materials/OOMultiTextureMaterial.cpp`

**Status:** done — 9 rows: 7 ✅ 1 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOMultiTextureMaterial initWithName:configuration:]` (40) | `OOMultiTextureMaterial::initWithName` `Core/Materials/OOMultiTextureMaterial.cpp:46` (factory `:38`) | ✅ |
| `-[OOMultiTextureMaterial dealloc]` (108) | none (Refs release maps; comment `:119`) | ⚠️ |
| `-[OOMultiTextureMaterial descriptionComponents]` (118) | `OOMultiTextureMaterial::descriptionComponents` `Core/Materials/OOMultiTextureMaterial.cpp:122` | ✅ |
| `-[OOMultiTextureMaterial textureUnitCount]` (132) | `OOMultiTextureMaterial::textureUnitCount` `Core/Materials/OOMultiTextureMaterial.cpp:140` | ✅ |
| `-[OOMultiTextureMaterial countOfTextureUnitsWithBaseCoordinates]` (137) | `OOMultiTextureMaterial::countOfTextureUnitsWithBaseCoordinates` `Core/Materials/OOMultiTextureMaterial.cpp:145` | ✅ |
| `-[OOMultiTextureMaterial ensureFinishedLoading]` (142) | `OOMultiTextureMaterial::ensureFinishedLoading` `Core/Materials/OOMultiTextureMaterial.cpp:150` | ✅ |
| `-[OOMultiTextureMaterial apply]` (148) | `OOMultiTextureMaterial::apply` `Core/Materials/OOMultiTextureMaterial.cpp:158` | ✅ |
| `-[OOMultiTextureMaterial unapplyWithNext:]` (177) | `OOMultiTextureMaterial::unapplyWithNext` `Core/Materials/OOMultiTextureMaterial.cpp:189` | ✅ |
| `-[OOMultiTextureMaterial allTextures]` (194) | `OOMultiTextureMaterial::allTextures` `Core/Materials/OOMultiTextureMaterial.cpp:208` | ❌ |

**Differences:**

- ⚠️ **low** — `-dealloc`'s `[self willDealloc]` has no counterpart (proposed ADR-0056 amendment oo-smy item 3, comment `:119`). See OOMaterial — upstream `Core/Materials/OOMultiTextureMaterial.m:108-116` ↔ port `Core/Materials/OOMultiTextureMaterial.cpp:119-120`
- ❌ **low** — `allTextures` (debug monitor only) pushes a null `Ref` when `_emissionMap` is nil. Upstream's `+setWithObject:nil` raised, and `setWithObjects:diffuse, nil` dropped the emission entry. The debug inspector may now see a null texture entry — upstream `Core/Materials/OOMultiTextureMaterial.m:194-199` ↔ port `Core/Materials/OOMultiTextureMaterial.cpp:208-218`
- Subtle (✅): the emission/illumination generator choice is unchanged, and so is the `optionsSpecifier` fallback (`emissionSpec ?: illuminationSpec`). Stripping `emission_color`/`emission` from the configuration given to OOBasicMaterial is also unchanged, as are the texture-unit counts — port `Core/Materials/OOMultiTextureMaterial.cpp:63-116`

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

**Status:** done — 24 rows: 20 ✅ 3 ⚠️ 1 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| key constants `kOO*Key` (46-54) *(added)* | `Core/Materials/OOShaderMaterial.h:211-219` | ✅ |
| `+[OOShaderMaterial configurationDictionarySpecifiesShaderMaterial:]` (71) | `OOShaderMaterial::configurationDictionarySpecifiesShaderMaterial` `Core/Materials/OOShaderMaterial.cpp:74` | ✅ |
| `+[OOShaderMaterial shaderMaterialWithName:]` (88) | `OOShaderMaterial::shaderMaterialWithName` `Core/Materials/OOShaderMaterial.cpp:110` | ✅ |
| `-[OOShaderMaterial initWithName:]` (96) | `OOShaderMaterial::initWithName` `Core/Materials/OOShaderMaterial.cpp:121` | ✅ |
| `-[OOShaderMaterial dealloc]` (265) | `OOShaderMaterial::~OOShaderMaterial` `Core/Materials/OOShaderMaterial.cpp:278` | ⚠️ |
| `-[OOShaderMaterial bindUniform:]` (286) | `OOShaderMaterial::bindUniform` `Core/Materials/OOShaderMaterial.cpp:290` | ✅ |
| `-[OOShaderMaterial bindSafeUniform:]` (313) | `OOShaderMaterial::bindSafeUniform` `Core/Materials/OOShaderMaterial.cpp:309` | ❌ |
| `-[OOShaderMaterial setUniform:intValue:]` (334) | `OOShaderMaterial::setUniform(name, int)` `Core/Materials/OOShaderMaterial.cpp:329` | ✅ |
| `-[OOShaderMaterial setUniform:floatValue:]` (354) | `OOShaderMaterial::setUniform(name, float)` `Core/Materials/OOShaderMaterial.cpp:343` | ✅ |
| `-[OOShaderMaterial setUniform:vectorValue:]` (374) | `OOShaderMaterial::setUniform(name, GLfloat[4])` `Core/Materials/OOShaderMaterial.cpp:357` | ✅ |
| `-[OOShaderMaterial setUniform:vectorObjectValue:]` (394) | `OOShaderMaterial::setUniform(name, const oo::PList&)` `Core/Materials/OOShaderMaterial.cpp:371` | ✅ |
| `-[OOShaderMaterial setUniform:quaternionValue:asMatrix:]` (425) | `OOShaderMaterial::setUniform(name, Quaternion, bool)` `Core/Materials/OOShaderMaterial.cpp:396` | ✅ |
| `-[OOShaderMaterial addUniformsFromDictionary:withBindingTarget:]` (446) | `OOShaderMaterial::addUniformsFromDictionary` `Core/Materials/OOShaderMaterial.cpp:410` | ✅ |
| `-[OOShaderMaterial doApply]` (589) | `OOShaderMaterial::doApply` `Core/Materials/OOShaderMaterial.cpp:558` | ✅ |
| `-[OOShaderMaterial ensureFinishedLoading]` (617) | `OOShaderMaterial::ensureFinishedLoading` `Core/Materials/OOShaderMaterial.cpp:589` | ✅ |
| `-[OOShaderMaterial isFinishedLoading]` (628) | `OOShaderMaterial::isFinishedLoading` `Core/Materials/OOShaderMaterial.cpp:601` | ✅ |
| `-[OOShaderMaterial unapplyWithNext:]` (642) | `OOShaderMaterial::unapplyWithNext` `Core/Materials/OOShaderMaterial.cpp:615` | ✅ |
| `-[OOShaderMaterial setBindingTarget:]` (669) | `OOShaderMaterial::setBindingTarget` `Core/Materials/OOShaderMaterial.cpp:642` | ⚠️ |
| `-[OOShaderMaterial permitSpecular]` (676) | `OOShaderMaterial::permitSpecular` `Core/Materials/OOShaderMaterial.cpp:649` | ✅ |
| `-[OOShaderMaterial allTextures]` (682) | `OOShaderMaterial::allTextures` `Core/Materials/OOShaderMaterial.cpp:655` | ✅ |
| `-[OOShaderMaterial(OOPrivate) loadTexturesFromArray:unitCount:]` (692) | `OOShaderMaterial::loadTexturesFromArray` `Core/Materials/OOShaderMaterial.cpp:666` | ✅ |
| `-[OOShaderMaterial(OOPrivate) addTexturesFromArray:unitCount:]` (708) | `OOShaderMaterial::addTexturesFromArray` `Core/Materials/OOShaderMaterial.cpp:682` | ✅ |
| `MacrosToString()` (731) | `MacrosToString` `Core/Materials/OOShaderMaterial.cpp:704` | ⚠️ |
| `GetShaderSource()` (761) | `GetShaderSource` `Core/Materials/OOShaderMaterial.cpp:741` | ✅ |

**Differences:**

- ❌ **low** — `shader.uniform.unpermittedMethod` names the target class as `-[Entity foo]` for every entity (`TargetClassName` → hook `ShaderClassName`, which answers the literal `"Entity"`). Upstream printed `[target class]`, for example `ShipEntity` or `PlayerEntity`. The log text changes; the message class does not — upstream `Core/Materials/OOShaderMaterial.m:328` ↔ port `Core/Materials/OOShaderMaterial.cpp:323`, `Core/Entities/EntityShaderBindings.cpp:388-391`
- ⚠️ **low** — the `#define` lines `MacrosToString` writes come out in `std::map` key order, where upstream used `NSDictionary` hash order (comment `:710`). The preprocessor result is the same. The prefix text, and therefore the program cache key, differs from upstream's — upstream `Core/Materials/OOShaderMaterial.m:740-747` ↔ port `Core/Materials/OOShaderMaterial.cpp:710-723`
- ⚠️ **low** — the `bindingTarget` ivar is gone (it was stored and never read). `setBindingTarget` only forwards to the uniforms (bead oo-9ht.39.6, comment `:276-277`) — upstream `Core/Materials/OOShaderMaterial.m:669-674` ↔ port `Core/Materials/OOShaderMaterial.cpp:642-647`
- Subtle (✅): `configurationDictionarySpecifiesShaderMaterial` keeps upstream's typo: it tests `vertex_shader` twice and never tests `fragment_shader`. A config with only `fragment_shader` is still not a shader material — port `Core/Materials/OOShaderMaterial.cpp:83-86`
- Subtle (✅): the uniform parsing defaults are the same. The type defaults to `float`, or to `binding` when there is no value but there is a binding. A string that is not a number literal means `binding`. An array definition gives `vector` with a nil value, so it binds the zero vector (0,0,0,1), as upstream did. `scale` and `quatAsMatrix` carry over between keys (both are declared outside the loop). The option defaults are `clamped` NO, `normalized`/`normalised` NO, `asMatrix` YES and `bindToSubentity` NO, giving bind-to-super. Keys are walked in sorted order and the RANROT seed is saved and restored. `uGloss` defaults to 0.5 (not the specifier's 0.375) and `uGammaCorrect` to `!no-gamma-correct` — port `Core/Materials/OOShaderMaterial.cpp:410-556,257-269`
- Subtle (✅ low): a string `float` value is now parsed as a double and then narrowed to float, where `-[NSString floatValue]` parsed it as a float directly. The last ulp can differ in rare halfway cases — port `Core/Materials/OOShaderMaterial.cpp:496`
- Subtle (✅): `doApply` applies the uniforms in `std::map` name order where upstream used `-allValues` hash order. Each uniform has its own location, so GL state is the same. The early exit on an exception is kept — port `Core/Materials/OOShaderMaterial.cpp:576-584`
- Subtle (✅ low): `shader.load.noShader` prints `oo::DescriptionOf(configuration)` instead of `-[NSDictionary description]` — port `Core/Materials/OOShaderMaterial.cpp:233`

## OOShaderProgram

<a id="ooshaderprogram"></a>
Upstream `src/Core/Materials/OOShaderProgram.m` ↔ `Core/Materials/OOShaderProgram.cpp`

**Status:** done — 12 rows: 12 ✅ 0 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOShaderProgram shaderProgramWithVertexShader:]` (66) | `OOShaderProgram::shaderProgramWithVertexShader` `Core/Materials/OOShaderProgram.cpp:101` | ✅ |
| `+[OOShaderProgram shaderProgramWithVertexShaderName:]` (105) | `OOShaderProgram::shaderProgramWithVertexShaderName` `Core/Materials/OOShaderProgram.cpp:140` | ✅ |
| `-[OOShaderProgram dealloc]` (149) | `OOShaderProgram::~OOShaderProgram` `Core/Materials/OOShaderProgram.cpp:184` | ✅ |
| `-[OOShaderProgram apply]` (174) | `OOShaderProgram::apply` `Core/Materials/OOShaderProgram.cpp:207` | ✅ |
| `+[OOShaderProgram applyNone]` (186) | `OOShaderProgram::applyNone` `Core/Materials/OOShaderProgram.cpp:218` | ✅ |
| `-[OOShaderProgram program]` (197) | `OOShaderProgram::program` `Core/Materials/OOShaderProgram.cpp:228` | ✅ |
| `ValidateShaderObject()` (204) | `ValidateShaderObject` `Core/Materials/OOShaderProgram.cpp:235` | ✅ |
| `-[OOShaderProgram(OOPrivate) initWithVertexShaderSource:]` (269) | `OOShaderProgram::initWithVertexShaderSource` `Core/Materials/OOShaderProgram.cpp:300` | ✅ |
| `-[OOShaderProgram(OOPrivate) bindAttributes:]` (361) | `OOShaderProgram::bindAttributes` `Core/Materials/OOShaderProgram.cpp:391` | ✅ |
| `-[OOShaderProgram(OOPrivate) bindStandardMatrixUniforms]` (373) | `OOShaderProgram::bindStandardMatrixUniforms` `Core/Materials/OOShaderProgram.cpp:402` | ✅ |
| `GetShaderSource()` (403) | `GetShaderSource` `Core/Materials/OOShaderProgram.cpp:435` | ✅ |
| `GetGLSLInfoLog()` (443) | `GetGLSLInfoLog` `Core/Materials/OOShaderProgram.cpp:512` | ✅ |

**Differences:**

- Subtle (✅): the shader sources are assembled from the same three strings: the prefix (or `""`), `"#line 0\n"` and the source. Both trees turn an empty prefix into nil. The weak (non-retaining) program cache keyed by `cacheKey` is kept, the cached program is removed in the destructor, and a nil key is never cached — port `Core/Materials/OOShaderProgram.cpp:101-138,309-329`
- Subtle (✅): `bindStandardMatrixUniforms` keeps "a nil type string compares equal to `mat3`". The vertex-shader lookup failure still logs "failed to find fragment program", an upstream copy-paste kept as it was. The info log falls back to Latin-1 when it is not valid UTF-8 — port `Core/Materials/OOShaderProgram.cpp:414-417,455,533-545`

## OOShaderUniform

<a id="ooshaderuniform"></a>
Upstream `src/Core/Materials/OOShaderUniform.m` ↔ `Core/Materials/OOShaderUniform.cpp`

**Status:** done — 14 rows: 10 ✅ 2 ⚠️ 2 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOShaderUniform initWithName:shaderProgram:intValue:]` (50) | `OOShaderUniform::initWithName(…, GLint)` `Core/Materials/OOShaderUniform.cpp:45` | ✅ |
| `-[OOShaderUniform initWithName:shaderProgram:floatValue:]` (61) | `OOShaderUniform::initWithName(…, GLfloat)` `Core/Materials/OOShaderUniform.cpp:58` | ✅ |
| `-[OOShaderUniform initWithName:shaderProgram:vectorValue:]` (72) | `OOShaderUniform::initWithName(…, GLfloat[4])` `Core/Materials/OOShaderUniform.cpp:71` | ✅ |
| `-[OOShaderUniform initWithName:shaderProgram:colorValue:]` (83) | `OOShaderUniform::initWithName(…, OOColor*)` `Core/Materials/OOShaderUniform.cpp:84` | ✅ |
| `-[OOShaderUniform initWithName:shaderProgram:quaternionValue:asMatrix:]` (102) | `OOShaderUniform::initWithName(…, Quaternion, bool)` `Core/Materials/OOShaderUniform.cpp:104` | ✅ |
| `-[OOShaderUniform initWithName:shaderProgram:matrixValue:]` (121) | `OOShaderUniform::initWithName(…, OOMatrix)` `Core/Materials/OOShaderUniform.cpp:125` | ✅ |
| `-[OOShaderUniform initWithName:]` (132) (`…boundToObject:property:convertOptions:`) | `OOShaderUniform::initWithEntityTarget` `Core/Materials/OOShaderUniform.cpp:177` | ⚠️ |
| `-[OOShaderUniform dealloc]` (179) | `OOShaderUniform::~OOShaderUniform` `Core/Materials/OOShaderUniform.cpp:212` | ✅ |
| `-[OOShaderUniform description]` (188) | `OOShaderUniform::description` `Core/Materials/OOShaderUniform.cpp:217` | ❌ |
| `-[OOShaderUniform apply]` (272) | `OOShaderUniform::apply` `Core/Materials/OOShaderUniform.cpp:315` | ✅ |
| `-[OOShaderUniform setBindingTarget:]` (282) | `OOShaderUniform::setEntityBindingTarget` `Core/Materials/OOShaderUniform.cpp:325` | ❌ |
| `-[OOShaderUniform(OOPrivate) initWithName:shaderProgram:]` (364) | `OOShaderUniform::initWithName(name, program)` `Core/Materials/OOShaderUniform.cpp:373` | ✅ |
| `-[OOShaderUniform(OOPrivate) applySimple]` (394) | `OOShaderUniform::applySimple` `Core/Materials/OOShaderUniform.cpp:396` | ✅ |
| `-[OOShaderUniform(OOPrivate) applyBinding]` (414) | `OOShaderUniform::applyBinding` `Core/Materials/OOShaderUniform.cpp:416` | ⚠️ |

**Differences:**

- ❌ **med** — **`laserColor` can no longer be bound.** It is whitelisted in `shader_ship_binding_methods` (`Resources/Config/whitelist.plist:420`) and typed `color` in `shader-uniform-bindings.plist:57`. Upstream it bound through the selector: `-[ShipEntity laserColor]` returns `id`, which gives `kOOShaderUniformTypeObject`, and an OOColor becomes a vec4. The port resolves names only through the member table, which has no `laserColor` row, so the uniform is inactive and logs `shader.uniform.bind.failed … (target does not respond to selector)`. OXP shaders that read `laserColor` get the GLSL default, which is a visual change. `ShipEntity::laserColor()` exists (`Core/Entities/ShipEntity.h:806`). Every other whitelisted name (86 of 87 unique) has a row — upstream `Core/Materials/OOShaderUniform.m:282-357,506-518` ↔ port `Core/Materials/OOShaderUniform.cpp:349-369`, `Core/Entities/EntityShaderBindings.cpp:109,150-330`
- ❌ **low** — the `shader.uniform.bind.failed` reason is always "target does not respond to selector". Upstream distinguished `unsupported type "<enc>"`, the argument-count problem and the missing-IMP problem. For example, on Win64 `missileCount`, `missileCapacity` and `trumbleCount` (`NSUInteger` = `"Q"`, which has no template) fail in both trees but now give a different reason. The class is also printed as `Entity`, not the target's real class — upstream `Core/Materials/OOShaderUniform.m:346-356` ↔ port `Core/Materials/OOShaderUniform.cpp:367-369`
- ❌ **low** — in `description` (logged by `shader.uniform.set`), a binding prints `[<Entity 0x…> prop]` where upstream printed `[<ShipEntity 0x…> prop]`, the real class. The pointer is the entity, not the weak-reference proxy — upstream `Core/Materials/OOShaderUniform.m:197` ↔ port `Core/Materials/OOShaderUniform.cpp:227`
- ⚠️ **low** — the selector initialiser became `initWithEntityTarget`, which keeps the property *name* and resolves it per target through the entity hooks and member table (beads oo-9ht.39.6, oo-4aq6a.7; comment `:183-186`). The checks (null property, location -1 with its log line), the convert-option bits and bind-to-super are the same — port `Core/Materials/OOShaderUniform.cpp:177-210`
- ⚠️ **low** — in `applyBinding`, a binding of `kOOShaderUniformTypeObject` sets nothing. Upstream sent an `NSNumber` result as a float. The comment `:518-523` says no whitelisted method returns a number, and colour results now use the new `kOOShaderUniformTypeColor`. The int, float, vector, HPVector, quaternion, matrix and point paths, with clamp and normalize, are unchanged — upstream `Core/Materials/OOShaderUniform.m:506-519` ↔ port `Core/Materials/OOShaderUniform.cpp:518-533`
- Subtle (✅): the description shows `valueType` `"(null)"` and value `INVALID` for an unknown type, as `%@` of nil did. The colour type prints `object-binding`, as it did while it was an object — port `Core/Materials/OOShaderUniform.cpp:296-304,312`

## OOShaderUniformMethodType

<a id="ooshaderuniformmethodtype"></a>
Upstream `src/Core/Materials/OOShaderUniformMethodType.m` ↔ `Core/Materials/OOShaderUniformMethodType.cpp`

**Status:** done — 21 rows: 0 ✅ 2 ⚠️ 0 ❌ 19 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `OOShaderUniformTypeFromMethodSignature()` (59) | `OOShaderUniformTypeFromEncoding` `Core/Materials/OOShaderUniformMethodType.cpp:61` | ⚠️ |
| `InitTemplates()` (100) | `InitTemplates` `Core/Materials/OOShaderUniformMethodType.cpp:94` | ⚠️ |
| `CopyTemplateForSelector()` (127) | removed: templates are `kOOTypeEncoding<T>` literals (`Core/OOTypeEncodings.h`) | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedCharMethod]` (143) | removed: `kOOTypeEncoding<signed char>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedCharMethod]` (148) | removed: `kOOTypeEncoding<unsigned char>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedShortMethod]` (153) | removed: `kOOTypeEncoding<signed short>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedShortMethod]` (158) | removed: `kOOTypeEncoding<unsigned short>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedIntMethod]` (163) | removed: `kOOTypeEncoding<signed int>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedIntMethod]` (168) | removed: `kOOTypeEncoding<unsigned int>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass signedLongMethod]` (173) | removed: `kOOTypeEncoding<signed long>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass unsignedLongMethod]` (178) | removed: `kOOTypeEncoding<unsigned long>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass floatMethod]` (183) | removed: `kOOTypeEncoding<float>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass doubleMethod]` (188) | removed: `kOOTypeEncoding<double>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass vectorMethod]` (193) | removed: `kOOTypeEncoding<Vector>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass hpvectorMethod]` (199) | removed: `kOOTypeEncoding<HPVector>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass quaternionMethod]` (205) | removed: `kOOTypeEncoding<Quaternion>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass matrixMethod]` (211) | removed: `kOOTypeEncoding<OOMatrix>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass pointMethod]` (216) | removed: `kOOTypeEncoding<NSPoint>` | ➖ |
| `-[OOShaderUniformTypeMethodSignatureTemplateClass idMethod]` (221) | removed: `kOOTypeEncoding<OOObjCId>` | ➖ |
| `OOCallIntegerMethod()` (228) | removed: row getters + `StoreShaderBindingValue` `Core/Entities/EntityShaderBindings.cpp:119-141` → `OOShaderBindingValue::i` | ➖ |
| `OOCallFloatMethod()` (266) | removed: same, → `OOShaderBindingValue::f` | ➖ |

**Differences:**

- ⚠️ **low** — selector dispatch by name is replaced by a static member table (beads oo-9ht.158, oo-9ht.39.6.1, oo-4aq6a.7; header `Core/Materials/OOShaderUniformMethodType.h:66-72,129-132`). Each row's type is `@encode()` of the replaced method's return type, as measured. The template set is unchanged, except for the new `kOOShaderUniformTypeColor` (`OOColor*`, `:119`), which is inserted before `Object` in the enum. As upstream, `long long`/`unsigned long long` have no template — upstream `Core/Materials/OOShaderUniformMethodType.m:59-125` ↔ port `Core/Materials/OOShaderUniformMethodType.cpp:61-123`
- **Bindability check (emphasis):** every name in the four `shader_*_binding_methods` whitelists (identical in both trees) was compared with `kEntityShaderBindings`. 86 of the 87 unique names have a row. The one missing is **`laserColor`**; see OOShaderUniform ❌ med. No row exists that is not whitelisted.
- Subtle (✅): integer conversion is the same. `StoreShaderBindingValue` sign-extends `signed char` (BOOL), as the `CharReturnMsgSend` call did, and widens other integers and enums to `long long`. `double` and `float` go to `f` — port `Core/Entities/EntityShaderBindings.cpp:135-140`
- Subtle (✅ platform): `NSUInteger` rows (`missileCapacity`, `missileCount`, `trumbleCount`) are `"Q"` on Win64, which matches no template, so they cannot be bound there. Upstream GNUstep on Win64 also uses `unsigned long long`, so the behaviour is the same. On LP64 Linux they are `"L"` and can be bound in both — `Core/OOTypeEncodings.h` (NSUInteger → `unsigned long long`)

## OOSingleTextureMaterial

<a id="oosingletexturematerial"></a>
Upstream `src/Core/Materials/OOSingleTextureMaterial.m` ↔ `Core/Materials/OOSingleTextureMaterial.cpp`

**Status:** done — 10 rows: 9 ✅ 1 ⚠️ 0 ❌ 0 ➖

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSingleTextureMaterial initWithName:configuration:]` (35) | `OOSingleTextureMaterial::initWithName(name, configuration)` `Core/Materials/OOSingleTextureMaterial.cpp:48` (factory `:32`) | ✅ |
| `-[OOSingleTextureMaterial initWithName:texture:configuration:]` (50) | `OOSingleTextureMaterial::initWithName(name, texture, configuration)` `Core/Materials/OOSingleTextureMaterial.cpp:65` (factory `:40`) | ✅ |
| `-[OOSingleTextureMaterial dealloc]` (64) | none (`_texture` Ref; comment `:77`) | ⚠️ |
| `-[OOSingleTextureMaterial descriptionComponents]` (72) | `OOSingleTextureMaterial::descriptionComponents` `Core/Materials/OOSingleTextureMaterial.cpp:80` | ✅ |
| `-[OOSingleTextureMaterial doApply]` (77) | `OOSingleTextureMaterial::doApply` `Core/Materials/OOSingleTextureMaterial.cpp:88` | ✅ |
| `-[OOSingleTextureMaterial unapplyWithNext:]` (86) | `OOSingleTextureMaterial::unapplyWithNext` `Core/Materials/OOSingleTextureMaterial.cpp:98` | ✅ |
| `-[OOSingleTextureMaterial ensureFinishedLoading]` (93) | `OOSingleTextureMaterial::ensureFinishedLoading` `Core/Materials/OOSingleTextureMaterial.cpp:106` | ✅ |
| `-[OOSingleTextureMaterial isFinishedLoading]` (98) | `OOSingleTextureMaterial::isFinishedLoading` `Core/Materials/OOSingleTextureMaterial.cpp:112` | ✅ |
| `-[OOSingleTextureMaterial wantsNormalsAsTextureCoordinates]` (103) | `OOSingleTextureMaterial::wantsNormalsAsTextureCoordinates` `Core/Materials/OOSingleTextureMaterial.cpp:117` | ✅ |
| `-[OOSingleTextureMaterial allTextures]` (109) | `OOSingleTextureMaterial::allTextures` `Core/Materials/OOSingleTextureMaterial.cpp:123` | ✅ |

**Differences:**

- ⚠️ **low** — `-dealloc`'s `[self willDealloc]` has no counterpart (proposed ADR-0056 amendment oo-smy item 3, comment `:77`). See OOMaterial — upstream `Core/Materials/OOSingleTextureMaterial.m:64-70` ↔ port `Core/Materials/OOSingleTextureMaterial.cpp:77-78`
- Subtle (✅): a nil name or a texture that failed to load still makes the initialiser fail, and the caller falls back to OOBasicMaterial (`OOMaterialConvenienceCreators.cpp:327-329`). With no configuration, the name itself is the texture specifier — port `Core/Materials/OOSingleTextureMaterial.cpp:52-73`

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

