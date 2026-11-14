# Generated Meson configuration for sub-module: Core

oolite_sources += files(
    'AI.cpp',
    'AIGraphViz.cpp',
    'CollisionRegion.cpp',
    'GameController.cpp',
    'GuiDisplayGen.cpp',
    'HeadUpDisplay.cpp',
    'ALBufferedSound.cpp',
    'ALMusic.cpp',
    'ALSound.cpp',
    'ALSoundChannel.cpp',
    'ALSoundDecoder.cpp',
    'ALSoundMixer.cpp',
    'ALStreamedSound.cpp',
    'AsyncWorkManager.cpp',
    'BasicSoundReferencePoint.cpp',
    'CPUInfo.cpp',
    'Cache.cpp',
    'CacheManager.cpp',
      'Character.cpp',
    'Foundation.cpp',
    'Color.cpp',
    'Commodities.cpp',
    'CommodityMarket.cpp',
    'ConstToString.cpp',
    'ConvertCubeMapToLatLong.cpp',
    'ConvertSystemDescriptions.cpp',
    'Crosshairs.cpp',
    'DataFromOXZFile.cpp',
    'DebugGLDrawing.cpp',
    'Drawable.cpp',
    'EncodingConverter.cpp',
    'EntityFilterPredicate.cpp',
    'EquipmentType.cpp',
    'FullScreenController.cpp',
    'GraphicsResetManager.cpp',
    'Headless.cpp',
    'HPVector.cpp',
    'IsNumberLiteral.cpp',
    'JoystickManager.cpp',
    'JoystickProfile.cpp',
    'LogHeader.cpp',
    'LogOutputHandler.cpp',
    'Logging.cpp',
    'Matrix.cpp',
    'Mesh.cpp',
    'MeshToOctreeConverter.cpp',
    'MouseInteractionMode.cpp',
    'MusicController.cpp',
    'OXZManager.cpp',
    'OpenALController.cpp',
    'OpenGL.cpp',
    'OpenGLExtensionManager.cpp',
    'OpenGLMatrixManager.cpp',
    'OpenGLStateManager.cpp',
    'PListParsing.cpp',
    'PListGameTypes.cpp',
    'PlanetData.cpp',
    'PlanetDrawable.cpp',
    'PolygonSprite.cpp',
    'ProbabilisticTextureManager.cpp',
    'ProbabilitySet.cpp',
    'ProfilingStopwatch.cpp',
    'Quaternion.cpp',
    'RegExpMatcher.cpp',
    'RoleSet.cpp',
    'ShipGroup.cpp',
    'ShipLibraryDescriptions.cpp',
    'ShipRegistry.cpp',
    'SkyDrawable.cpp',
    'SoundSource.cpp',
    'SoundSourcePool.cpp',
    'StringExpander.cpp',
    'StringParsing.cpp',
    'SystemDescriptionManager.cpp',
    'TextureScaling.cpp',
    'TextureSprite.cpp',
    'Trumble.cpp',
    'Vector.cpp',
    'Voxel.cpp',
    'XMLExtensions.cpp',
    'Octree.cpp',
    'ResourceManager.cpp',
    'SpeechSynthesizer.cpp',
    'Universe.cpp',
    'legacy_random.cpp',
)

# oo::SpeechSynthesizer's espeak calls, or the silent stand-in when the build has no espeak.
if get_option('espeak')
    oolite_sources += files('SpeechSynthesizer_espeak.cpp')
else
    oolite_sources += files('SpeechSynthesizer_none.cpp')
endif

# OO_VERSION_FULL, OO_BUILD_DATE and OO_BUILDER (meson.build); on the include path through the
# include_directories('.') below, which covers this build directory.
configure_file(output: 'OOVersion.h', configuration: oo_version_conf)

oolite_includes += include_directories('.')

# Enter child subdirectories
subdir('Debug')
subdir('Entities')
subdir('Materials')
subdir('OXPVerifier')
subdir('Scripting')
subdir('Tables')
