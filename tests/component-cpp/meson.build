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
# Add a program as one entry: its name (the source is <name>.cpp) and its options: 'args',
# 'suite' (default 'component'), 'timeout' (default 60 s), 'verbose' (print its output even when
# it passes, for a program whose output is a record, like the skeleton's probes), 'source' (another
# program's source), 'needs_trap' (registered only where the GL trap is linked).
# Every program links the GL trap where there is one.

component_programs = {
    'test_skeleton': {'verbose': true},
    'test_boot': {'verbose': true},
    'test_gl_trap': {'needs_trap': true, 'verbose': true},
    # The C++ counterparts of tests/component/features, each with its original's base name.
    'test_s1_police_kills_pirate': {'verbose': true},
    'test_s2_pirate_attacks_target_registers_attacker': {'verbose': true},
    'test_s3_hostile_spawn_drives_ship_into_attack': {'verbose': true},
    'test_s4_missile_armed_ship_kills_by_missile': {'verbose': true},
    'test_s5_escorts_converge_on_their_mother': {'verbose': true},
    # Runs test_s1_police_kills_pirate twice and compares the digests of every step.
    'test_repeat': {'verbose': true, 'timeout': 120},
    # One ship, one behaviour (spec 011 US6). Each takes --without-weapon, which must make it fail.
    'test_laser_fires': {},
    'test_ship_attacks': {},
    'test_mining_laser': {},
    # Every ship and equipment item in the core data, one outcome each (contracts/census.md).
    'component_census': {'test_name': 'census', 'timeout': 600, 'verbose': true},
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

# oo_component: what every program but the skeleton is written against.
component_support_sources = files('oo_component.cpp')

# The GL trap (Linux and macOS): every GL function the game's objects import, defined to print its
# name and abort, so a GL call from a headless test fails it by name (gl_trap_gen.py). It needs the
# real objects: with LTO they are bitcode, and its definitions would meet the GL headers' at link
# time, so it is left out and the programs say so (test_boot prints "GL trap: not linked").
component_trap_sources = []
component_trap_args = ['-DOO_COMPONENT_GL_TRAP_ABSENT="not generated on ' + host_machine.system() + '"']
if host_machine.system() in ['linux', 'darwin']
    if get_option('b_lto')
        message('C++ component tests: the GL trap is left out because b_lto is on; build with -Db_lto=false to link it')
        component_trap_args = ['-DOO_COMPONENT_GL_TRAP_ABSENT="b_lto is on"']
    else
        component_nm = find_program('nm', 'llvm-nm', required: false)
        if component_nm.found()
            component_trap_sources = custom_target('gl_trap',
                input: component_game_objects,
                output: 'gl_trap.cpp',
                build_by_default: true,
                command: [find_program('python3'), files('gl_trap_gen.py'), '--nm', component_nm, '-o', '@OUTPUT@', '@INPUT@'],
            )
            component_trap_args = ['-DOO_COMPONENT_GL_TRAP=1']
        else
            message('C++ component tests: the GL trap is left out because no nm was found')
            component_trap_args = ['-DOO_COMPONENT_GL_TRAP_ABSENT="no nm"']
        endif
    endif
endif

component_env = environment()
component_env.set('OO_COMPONENT_SOURCE_ROOT', meson.project_source_root())
component_env.set('OO_COMPONENT_BUILD_ROOT', meson.project_build_root())

# The game reads Resources from its working directory: the application directory, which on macOS
# is the bundle's Contents.
component_app_dir = host_machine.system() == 'darwin' ? prog_dir / 'Contents' : prog_dir

component_trapped = component_trap_args.contains('-DOO_COMPONENT_GL_TRAP=1')

foreach name, options : component_programs
    if options.get('needs_trap', false) and not component_trapped
        continue
    endif
    exe = executable(
        name,
        options.get('source', name + '.cpp'),
        name == 'test_skeleton' ? [] : component_support_sources,
        component_trap_sources,
        cpp_args: component_trap_args,
        objects: component_game_objects,
        include_directories: [oolite_includes, include_directories('../unit/oofnd')],
        dependencies: oolite_dependencies,
        override_options: oolite_override_options,
        install: false,
    )
    test(options.get('test_name', name), exe,
        args: options.get('args', []),
        suite: options.get('suite', 'component'),
        timeout: options.get('timeout', 60),
        verbose: options.get('verbose', false),
        workdir: component_app_dir,
        env: component_env,
        depends: oolite_app_bundle,
    )
endforeach
