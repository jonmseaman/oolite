# The C++ half of the component tier: programs that start the real game in their own process
# with no window, no GL context and no sound, and step it without waiting on the clock.
# meson test -C build/meson_<type> --suite component, or tests/run.sh component --cpp.
#
# Every platform builds them (no host guard, unlike tests/unit). Each program links every game
# object but main's, extracted from the oolite target so it is compiled exactly as the game is,
# with the game's include directories and dependencies but not its link arguments (-mwindows
# would hide the program's output). The harness is tests/unit/oofnd/oo_test.hpp.
#
# A program runs in the built application directory, because the game reads Resources from its
# working directory. OO_COMPONENT_SOURCE_ROOT (layouts, fixtures) and OO_COMPONENT_BUILD_ROOT
# (scratch) are in its environment; run by hand, a program finds both itself.
#
# Add a program as one entry: its name (the source is <name>.cpp) and its options.

component_programs = {
    'test_skeleton': {},
}

fs = import('fs')

# Every game object but main's: a program needs the whole game graph.
component_game_sources = []
foreach f : oolite_sources
    if fs.relative_to(f, meson.project_source_root()).replace('\\', '/') != 'src/SDL/main.cpp'
        component_game_sources += [f]
    endif
endforeach
component_game_objects = oolite_bin.extract_objects(component_game_sources)

component_support_sources = []

component_env = environment()
component_env.set('OO_COMPONENT_SOURCE_ROOT', meson.project_source_root())
component_env.set('OO_COMPONENT_BUILD_ROOT', meson.project_build_root())

foreach name, options : component_programs
    exe = executable(
        name,
        name + '.cpp',
        component_support_sources,
        objects: component_game_objects,
        include_directories: [oolite_includes, include_directories('../unit/oofnd')],
        dependencies: oolite_dependencies,
        override_options: oolite_override_options,
        install: false,
    )
    test(name, exe,
        args: options.get('args', []),
        suite: options.get('suite', 'component'),
        timeout: options.get('timeout', 60),
        workdir: prog_dir,
        env: component_env,
        depends: oolite_app_bundle,
    )
endforeach
