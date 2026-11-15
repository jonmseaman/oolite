# Third-party code copied into the tree: MiniZip 1.1, miniz, tinyexr and stb_image_write. It is
# not Oolite's C++, so it is not formatted, tidied or built with oolite_cpp_args. This file only
# lists sources and include directories; src/meson.build builds them as one library, after the
# platform files have added their project arguments.

oolite_third_party_sources = files(
    'minizip/ioapi.cpp',
    'minizip/unzip.cpp',
    'stb/stb_image_write.cpp',
)
# The EXR snapshot (src/SDL/OOSaveEXRSnapshot.cpp) is Windows only.
if host_machine.system() == 'windows'
    oolite_third_party_sources += files(
        'miniz/miniz.cpp',
        'tinyexr/tinyexr.cpp',
    )
endif

oolite_third_party_includes = include_directories(
    'minizip',
    'miniz',
    'tinyexr',
    'stb',
)
