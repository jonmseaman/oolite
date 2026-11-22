# Unit tests of converted game classes:
# meson test -C build/meson_<type> --suite core, or tests/run.sh unit.
#
# One executable per tests/unit/core/test_<Class>.mm. It links the GAME'S OWN objects for the
# class, its Objective-C bridge and whatever else they reference (extracted from the oolite
# target, so compiled exactly as the game compiles them), with the game's include directories and
# dependencies but not its link arguments (-mwindows would hide the test's output). Add a converted
# class as one entry: its test name and the game sources whose objects it needs (['*']: all but main).
# Every test_*.cpp in this directory needs an entry in this list to be built.

openal_tests = ['test_OOOpenALController', 'test_OOSound', 'test_OOALBufferedSound', 'test_OOALStreamedSound',
    'test_OOMusic', 'test_OOSoundChannel', 'test_OOSoundMixer', 'test_MyOpenGLView']

core_tests = {
    'test_OOJSEngineTimeManagement': ['Core/Scripting/OOJSEngineTimeManagement.cpp', 'Core/ProfilingStopwatch.cpp'],
    'test_OOJSTimeLimiter': ['*'],
    'test_OOJSGuiScreenKeyDefinition': ['Core/Scripting/OOJSGuiScreenKeyDefinition.cpp'],
    'test_OOJSInterfaceDefinition': ['Core/Scripting/OOJSInterfaceDefinition.cpp'],
    'test_OoliteEquipmentControlScript': [],	# Resources/Scripts/oolite-equipment-control.js (upstream #369)
    'test_OOJSPopulatorDefinition': ['Core/Scripting/OOJSPopulatorDefinition.cpp'],
    'test_OOPListScript': ['*'],
    'test_OOScript': ['*'],
    'test_OOJSScript': ['*'],
    'test_OOJavaScriptEngine': ['*'],
    'test_OOJSSystem': ['*'],
    'test_OOJSPlayerShip': ['*'],
    'test_OOJSShip': ['*'],
    'test_OOCache': ['Core/Cache.cpp'],
    'test_OOCacheManager': ['Core/CacheManager.cpp'],
    'test_OOEncodingConverter': ['Core/EncodingConverter.cpp', 'Core/Cache.cpp'],
    'test_OOFullScreenController': ['Core/FullScreenController.cpp'],
    'test_OOSoundReferencePoint': ['Core/BasicSoundReferencePoint.cpp'],
    'test_OOColor': ['Core/Color.cpp'],
    'test_OOJoystickProfile': ['Core/JoystickProfile.cpp'],
    'test_OOSDLJoystickManager': ['SDL/SDLJoystickManager.cpp', 'Core/JoystickManager.cpp', 'Core/JoystickProfile.cpp'],
    'test_OOJoystickDecoding': ['Core/JoystickProfile.cpp'],	# upstream #742's layout and dead zone
    'test_OOJoystickManager': ['Core/JoystickManager.cpp', 'Core/JoystickProfile.cpp', 'SDL/SDLJoystickManager.cpp'],
    'test_OORegExpMatcher': ['Core/RegExpMatcher.cpp'],
    'test_OOFileScannerVerifierStage': ['Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOAIStateMachineVerifierStage': ['Core/OXPVerifier/AIStateMachineVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOCheckJSSyntaxVerifierStage': ['Core/OXPVerifier/CheckJSSyntaxVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOCheckPListSyntaxVerifierStage': ['Core/OXPVerifier/CheckPListSyntaxVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOCheckDemoShipsPListVerifierStage': ['Core/OXPVerifier/CheckDemoShipsPListVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOCheckRequiresPListVerifierStage': ['Core/OXPVerifier/CheckRequiresPListVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOCheckEquipmentPListVerifierStage': ['Core/OXPVerifier/CheckEquipmentPListVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOTextureVerifierStage': ['Core/OXPVerifier/TextureVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOCheckShipDataPListVerifierStage': ['Core/OXPVerifier/CheckShipDataPListVerifierStage.cpp', 'Core/OXPVerifier/ModelVerifierStage.cpp', 'Core/OXPVerifier/AIStateMachineVerifierStage.cpp', 'Core/OXPVerifier/TextureVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOModelVerifierStage': ['Core/OXPVerifier/ModelVerifierStage.cpp', 'Core/OXPVerifier/TextureVerifierStage.cpp', 'Core/OXPVerifier/FileScannerVerifierStage.cpp', 'Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OODrawable': ['Core/Drawable.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp'],
    'test_OOMaterial': ['Core/Materials/Material.cpp'],
    'test_OOShaderUniformMethodType': ['Core/Materials/ShaderUniformMethodType.cpp', 'Core/Matrix.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/Quaternion.cpp', 'Core/legacy_random.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOBasicMaterial': ['Core/Materials/BasicMaterial.cpp', 'Core/Materials/Material.cpp', 'Core/Materials/MaterialSpecifier.cpp', 'Core/Color.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOSingleTextureMaterial': ['Core/Materials/SingleTextureMaterial.cpp', 'Core/Materials/BasicMaterial.cpp', 'Core/Materials/Material.cpp', 'Core/Materials/MaterialSpecifier.cpp', 'Core/Color.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOMaterialConvenienceCreators': ['Core/Materials/MaterialConvenienceCreators.cpp', 'Core/Materials/ShaderMaterial.cpp', 'Core/Materials/SingleTextureMaterial.cpp', 'Core/Materials/MultiTextureMaterial.cpp', 'Core/Materials/BasicMaterial.cpp', 'Core/Materials/Material.cpp', 'Core/Materials/MaterialSpecifier.cpp', 'Core/Materials/ShaderProgram.cpp', 'Core/Materials/ShaderUniform.cpp', 'Core/Materials/ShaderUniformMethodType.cpp', 'Core/OpenGLMatrixManager.cpp', 'Core/Color.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/PListGameTypes.cpp', 'Core/IsNumberLiteral.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/Quaternion.cpp', 'Core/Matrix.cpp', 'Core/legacy_random.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOMultiTextureMaterial': ['Core/Materials/MultiTextureMaterial.cpp', 'Core/Materials/BasicMaterial.cpp', 'Core/Materials/Material.cpp', 'Core/Materials/MaterialSpecifier.cpp', 'Core/Color.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOShaderMaterial': ['Core/Materials/ShaderMaterial.cpp', 'Core/Materials/BasicMaterial.cpp', 'Core/Materials/Material.cpp', 'Core/Materials/MaterialSpecifier.cpp', 'Core/Color.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/PListGameTypes.cpp', 'Core/IsNumberLiteral.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/Quaternion.cpp', 'Core/Matrix.cpp', 'Core/legacy_random.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OODefaultShaderSynthesizer': ['*'],
    'test_OOShaderUniform': ['Core/Materials/ShaderUniform.cpp', 'Core/Materials/ShaderUniformMethodType.cpp', 'Core/Color.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/Quaternion.cpp', 'Core/Matrix.cpp', 'Core/legacy_random.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOShaderProgram': ['Core/Materials/ShaderProgram.cpp', 'Core/OpenGLMatrixManager.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/Matrix.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/Quaternion.cpp', 'Core/legacy_random.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOOXPVerifierStage': ['Core/OXPVerifier/OXPVerifierStage.cpp'],
    'test_OOOXPVerifier': ['*'],
    'test_OOPListSchemaVerifier': ['Core/OXPVerifier/PListSchemaVerifier.cpp', 'Core/PListGameTypes.cpp', 'Core/IsNumberLiteral.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/Quaternion.cpp', 'Core/legacy_random.cpp'],
    'test_OOOpenALController': ['Core/OpenALController.cpp'],
    'test_OOSound': ['Core/ALSound.cpp', 'Core/OpenALController.cpp'],
    'test_OOALSoundDecoder': ['Core/ALSoundDecoder.cpp'],
    'test_OOALBufferedSound': ['Core/ALBufferedSound.cpp', 'Core/ALSound.cpp', 'Core/ALSoundDecoder.cpp', 'Core/OpenALController.cpp'],
    'test_OOALStreamedSound': ['Core/ALStreamedSound.cpp', 'Core/ALSound.cpp', 'Core/ALSoundDecoder.cpp', 'Core/OpenALController.cpp'],
    'test_OOMusic': ['Core/ALMusic.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp', 'Core/ALSound.cpp', 'Core/SoundSource.cpp', 'Core/OpenALController.cpp'],
    'test_OOSoundChannel': ['Core/ALSoundChannel.cpp', 'Core/ALSound.cpp', 'Core/OpenALController.cpp'],
    'test_OOSoundMixer': ['Core/ALSoundMixer.cpp', 'Core/ALSound.cpp', 'Core/OpenALController.cpp'],
    'test_OOSoundSource': ['Core/SoundSource.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp'],
    'test_OOSoundSourcePool': ['Core/SoundSourcePool.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp'],
    'test_OOMusicController': ['Core/MusicController.cpp'],
    'test_OOShipGroup': ['Core/ShipGroup.cpp'],
    'test_OOGraphicsResetManager': ['Core/GraphicsResetManager.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOEnumFlags': [],	# OOEnumFlags.h, OOEnumTable.h, OODebugFlags.h
    'test_OOMathsHeaders': [],	# the maths headers stand alone (upstream #748)
    'test_OOOpenGLOnly': [],	# src/Core/OOOpenGLOnly.h (upstream #748)
    'test_OOOpenGLExtensionManager': ['Core/OpenGLExtensionManager.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOOpenGLStateManager': ['Core/OpenGLStateManager.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp'],
    'test_OOOpenGLStateVerifier': ['Core/OpenGLStateManager.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp'],
    'test_OOTextureSprite': ['Core/TextureSprite.cpp', 'Core/OpenGLStateManager.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLExtensionManager.cpp'],
    'test_OOTexture': ['*'],
    'test_OONullTexture': ['*'],
    'test_OOConcreteTexture': ['*'],
    'test_OOTextureScaling': ['Core/TextureScaling.cpp', 'Core/Materials/PixMap.cpp'],
    'test_OOTextureLoader': ['*'],
    'test_OOTextureGenerator': ['*'],
    'test_OOPNGTextureLoader': ['*'],
    'test_OOPixMapTextureLoader': ['*'],
    'test_OOCombinedEmissionMapGenerator': ['*'],
    'test_OOStandaloneAtmosphereGenerator': ['*'],
    'test_OOPlanetTextureGenerator': ['*'],
    'test_OOCrosshairs': ['Core/Crosshairs.cpp', 'Core/Color.cpp'],
    'test_OOOpenGLMatrixManager': ['Core/OpenGLMatrixManager.cpp', 'Core/Matrix.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp', 'Core/OpenGLExtensionManager.cpp', 'Core/OpenGL.cpp', 'Core/OpenGLStateManager.cpp'],
    'test_OOCharacter': ['Core/Character.cpp', 'Core/legacy_random.cpp'],
    'test_OOTrumble': ['Core/Trumble.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp'],
    'test_OOCommodities': ['Core/Commodities.cpp', 'Core/CommodityMarket.cpp', 'Core/legacy_random.cpp'],
    'test_OOCommodityMarket': ['Core/CommodityMarket.cpp'],
    'test_OOEquipmentType': ['Core/EquipmentType.cpp', 'Core/Color.cpp'],
    'test_EnumNameFallbacks': ['*'],
    'test_OOConstToString': ['Core/ConstToString.cpp', 'Core/EquipmentType.cpp', 'Core/Color.cpp', 'Core/CommodityMarket.cpp'],
    'test_OOPriorityQueue': [],
    'test_OORoleSet': ['Core/RoleSet.cpp', 'Core/legacy_random.cpp'],
    'test_OOProbabilitySet': ['Core/ProbabilitySet.cpp', 'Core/legacy_random.cpp'],
    'test_OOShipRegistry': ['Core/ShipRegistry.cpp', 'Core/RoleSet.cpp', 'Core/CacheManager.cpp', 'Core/ProbabilitySet.cpp', 'Core/PListGameTypes.cpp', 'Core/IsNumberLiteral.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/Quaternion.cpp', 'Core/Color.cpp', 'Core/legacy_random.cpp'],
    'test_OOProbabilisticTextureManager': ['Core/ProbabilisticTextureManager.cpp', 'Core/legacy_random.cpp'],
    'test_CollisionRegion': ['*'],
    'test_Octree': ['*'],
    'test_OOJSVector': ['Core/Scripting/OOJSVector.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/HPVector.cpp', 'Core/Vector.cpp', 'Core/Quaternion.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSDock': ['Core/Scripting/OOJSDock.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOJSSoundSource': ['Core/Scripting/OOJSSoundSource.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOJSSound': ['Core/Scripting/OOJSSound.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOJSSun': ['Core/Scripting/OOJSSun.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp'],
    'test_OOJSOolite': ['Core/Scripting/OOJSOolite.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOJSEquipmentInfo': ['Core/Scripting/OOJSEquipmentInfo.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/EquipmentType.cpp', 'Core/Color.cpp'],
    'test_OOJSPlayer': ['Core/Scripting/OOJSPlayer.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSEntity': ['Core/Scripting/OOJSEntity.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOConstToJSString.cpp', 'Core/Quaternion.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSMissionVariables': ['Core/Scripting/OOJSMissionVariables.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/IsNumberLiteral.cpp'],
    'test_OOJSMission': ['Core/Scripting/OOJSMission.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOConstToJSString.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSStation': ['Core/Scripting/OOJSStation.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Commodities.cpp', 'Core/CommodityMarket.cpp', 'Core/Scripting/OOJSInterfaceDefinition.cpp', 'Core/EquipmentType.cpp', 'Core/Color.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSGlobal': ['Core/Scripting/OOJSGlobal.cpp', 'Core/Scripting/OOConstToJSString.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Color.cpp', 'Core/Scripting/OOJSGuiScreenKeyDefinition.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSPlanet': ['Core/Scripting/OOJSPlanet.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Color.cpp'],
    'test_OOJSSystemInfo': ['Core/Scripting/OOJSSystemInfo.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/SystemDescriptionManager.cpp', 'Core/Commodities.cpp', 'Core/CommodityMarket.cpp', 'Core/IsNumberLiteral.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSManifest': ['Core/Scripting/OOJSManifest.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Commodities.cpp', 'Core/CommodityMarket.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSShipGroup': ['Core/Scripting/OOJSShipGroup.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/ShipGroup.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOJSVisualEffect': ['Core/Scripting/OOJSVisualEffect.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Color.cpp', 'Core/Vector.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSWaypoint': ['Core/Scripting/OOJSWaypoint.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Quaternion.cpp', 'Core/Vector.cpp', 'Core/HPVector.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSFlasher': ['Core/Scripting/OOJSFlasher.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Color.cpp'],
    'test_OOJSExhaustPlume': ['Core/Scripting/OOJSExhaustPlume.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp'],
    'test_OOJSQuaternion': ['Core/Scripting/OOJSQuaternion.cpp', 'Core/Scripting/OOJSVector.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/HPVector.cpp', 'Core/Vector.cpp', 'Core/Quaternion.cpp', 'Core/legacy_random.cpp'],
    'test_OOJSClock': ['Core/Scripting/OOJSClock.cpp', 'Core/Scripting/OOJSClasses.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp'],
    'test_OOJSWormhole': ['Core/Scripting/OOJSWormhole.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOMeshToOctreeConverter': ['*'],
    'test_OOMesh': ['*'],
    'test_OOPolygonSprite': ['*'],
    'test_OOVector': ['Core/Vector.cpp', 'Core/legacy_random.cpp'],
    'test_OOMatrix': ['*'],
    'test_OOQuaternion': ['*'],
    'test_OOHPVector': ['*'],
    'test_OOFastArithmetic': [],
    'test_OOProfilingStopwatch': ['Core/ProfilingStopwatch.cpp'],
    'test_OOAsyncWorkManager': ['Core/AsyncWorkManager.cpp', 'Core/CPUInfo.cpp'],
    'test_OOAsyncWorkManagerShutdown': ['Core/AsyncWorkManager.cpp', 'Core/CPUInfo.cpp'],
    'test_AI': ['Core/AI.cpp'],
    'test_OOJSFunction': ['Core/Scripting/OOJSFunction.cpp'],
    'test_OOJSPrivateObject': ['Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOJSTimer': ['Core/Scripting/OOJSTimer.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOScriptTimer.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OOScriptTimer': ['Core/Scripting/OOScriptTimer.cpp'],
    'test_OOJSFrameCallbacks': ['Core/Scripting/OOJSFrameCallbacks.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp'],
    'test_OOJSCall': ['Core/Scripting/OOJSCall.cpp', 'Core/Materials/ShaderUniformMethodType.cpp'],
    'test_OOSystemDescriptionManager': ['Core/SystemDescriptionManager.cpp', 'Core/legacy_random.cpp'],
    'test_GuiDisplayGen': ['*'],
    'test_MyOpenGLView': ['*'],
    'test_ResourceManager': ['*'],
    'test_PlayerEntityStickProfile': ['*'],
    'test_HeadUpDisplay': ['*'],
    'test_GameController': ['*'],
    'test_Entity': ['*'],
    'test_Universe': ['*'],
    'test_SpeechSynthesizer': ['*'],
    'test_ValueType': ['*'],
    'test_LogIndentScope': [],
    'test_ShipEntity': ['*'],
    'test_PlayerEntity': ['*'],
    'test_SaveEnumValues': ['*'],
    'test_EntityScriptEvents': ['*'],	# upstream #780's script events
    'test_ProxyPlayerEntity': ['*'],
    'test_StationEntity': ['*'],
    'test_DockEntity': ['*'],
    'test_ShipEntityAI': ['*'],
    'test_OOEntityWithDrawable': ['*'],
    'test_OOPlanetEntity': ['*'],
    'test_OOPlanetDrawable': ['*'],
    'test_EntityOOJavaScriptExtensions': ['*'],
    'test_OOJSCallEntityMethods': ['*'],
    'test_OOEntityCalledByName': ['*'],
    'test_EntityShaderBindings': ['*'],
    'test_WormholeEntity': ['*'],
    'test_OOSunEntity': ['*'],
    'test_OOExhaustPlumeEntity': ['*'],
    'test_SkyEntity': ['*'],
    'test_OOSkyDrawable': ['*'],
    'test_OOWaypointEntity': ['*'],
    'test_EntityDisposalReaders': ['*'],
    'test_ScriptFinalisationRelease': ['*'],
    'test_OOFlasherEntity': ['*'],
    'test_OOVisualEffectEntity': ['*'],
    'test_OOFlashEffectEntity': ['*'],
    'test_OOPlasmaShotEntity': ['*'],
    'test_OOECMBlastEntity': ['*'],
    'test_OOLightParticleEntity': ['*'],
    'test_OOSparkEntity': ['*'],
    'test_OOPlasmaBurstEntity': ['*'],
    'test_DustEntity': ['*'],
    'test_OOQuiriumCascadeEntity': ['*'],
    'test_OOLaserShotEntity': ['*'],
    'test_OOBreakPatternEntity': ['*'],
    'test_OOParticleSystem': ['*'],
    'test_OORingEffectEntity': ['*'],
    'test_OODebugMonitor': ['Core/Debug/DebugMonitor.cpp', 'Core/Color.cpp'],
    'test_OOJSConsole': ['Core/Debug/JSConsole.cpp', 'Core/Scripting/OOJSEngineTimeManagement.cpp', 'Core/ProfilingStopwatch.cpp', 'Core/Scripting/OOJSEngineNativeWrappers.cpp', 'Core/Scripting/OOJSPrivateObject.cpp', 'Core/Scripting/OOJSClasses.cpp'],
    'test_OODebugTCPConsoleClient': ['Core/Debug/DebugTCPConsoleClient.cpp', 'Core/Debug/TCPStreamDecoder.cpp', 'Core/Debug/TCPStreamDecoderAbstractionLayer.cpp'],
    'test_OODebugSupport': ['Core/Debug/DebugSupport.cpp'],
    'test_OODebugStandards': ['Core/Debug/DebugStandards.cpp'],
    'test_OOOXZManager': ['*'],
}

fs = import('fs')

foreach name, game_sources : core_tests
    # The oolite target's own File objects for these sources (extract_objects matches Files, and
    # a files() made here would name a different directory).
    wanted = []
    foreach s : game_sources
        wanted += ['src/' + s]
    endforeach
    # ['*'] is every game object but main's, for a class whose object needs the game graph
    # (Universe, Entity ivars).
    whole_game = game_sources == ['*']
    sources = []
    foreach f : oolite_sources
        rel = fs.relative_to(f, meson.project_source_root()).replace('\\', '/')
        if rel in wanted or (whole_game and rel != 'src/SDL/main.cpp')
            sources += [f]
        endif
    endforeach
    assert(whole_game or sources.length() == game_sources.length(), name + ': a game source in core_tests is not in the oolite target')
    exe = executable(
        'core_' + name,
        name + '.cpp',
        objects: oolite_bin.extract_objects(sources),
        include_directories: oolite_includes,
        dependencies: oolite_dependencies + [gtest_main_dep],
        override_options: oolite_override_options,
        install: false,
    )
    # 'openal': the tests that open the OpenAL device (alcOpenDevice). A sanitized run leaves them
    # out with --no-suite openal: the prebuilt libopenal-1.dll's onexit registration trips ASan's
    # allocator-ownership check inside ucrtbase.dll, a third-party report no suppression reaches
    # (tests/unit/asan-suppressions.txt). The game's sanitized launch uses --nosound instead.
    test(name, exe, suite: name in openal_tests ? ['core', 'openal'] : 'core', timeout: 60,
        protocol: 'gtest')
endforeach
