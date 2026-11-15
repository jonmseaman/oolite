# MiniZip 1.1 (Gilles Vollant, zlib licence; MiniZip64_info.txt). A local subproject only because
# third_party/ is meson's subproject_dir: it names its sources and headers, and src/meson.build
# builds them into the game's oolite_third_party library with the game's project arguments.
project('minizip', 'cpp', meson_version: '>= 1.4.0')

sources = files('ioapi.cpp', 'unzip.cpp')
includes = include_directories('.')
