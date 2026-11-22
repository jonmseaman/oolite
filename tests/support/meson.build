# The shared Google Test pieces every C++ test program in the tree uses (tests/unit,
# tests/component, tools/plist2json5), and their own tests: meson test --suite gtest.
#
# oo_gtest_dep: Google Test, this directory's headers (oo_gtest.hpp, oo_gtest_init.hpp) and their
# definitions, for a program with a main of its own (the component programs, through
# oo_component.cpp). oo_gtest_main_dep: all that and the shared main (oo_gtest_main.cpp), for a
# program with none.

oo_gtest_inc = include_directories('.')

oo_gtest_init_lib = static_library(
    'oo_gtest_init',
    'oo_gtest_init.cpp',
    dependencies: gtest_dep,
    install: false,
)

oo_gtest_dep = declare_dependency(
    link_with: oo_gtest_init_lib,
    dependencies: gtest_dep,
    include_directories: oo_gtest_inc,
)

oo_gtest_main_lib = static_library(
    'oo_gtest_main',
    'oo_gtest_main.cpp',
    dependencies: oo_gtest_dep,
    install: false,
)

oo_gtest_main_dep = declare_dependency(
    link_with: oo_gtest_main_lib,
    dependencies: oo_gtest_dep,
)

test('test_gtest_harness',
    executable('test_gtest_harness', 'test_gtest_harness.cpp',
        dependencies: oo_gtest_main_dep, install: false),
    suite: 'gtest', timeout: 60, protocol: 'gtest')

# A filter that selects nothing is not a failure (runner.md R3).
test('gtest_harness_filter_matches_nothing',
    executable('test_gtest_harness_filtered', 'test_gtest_harness.cpp',
        dependencies: oo_gtest_main_dep, install: false),
    args: ['--gtest_filter=NoSuch.*'],
    suite: 'gtest', timeout: 60)

# Programs that must fail, each run by check_probe.py, which checks the exit status and the text
# that names the failure (runner.md R1, R2, R6).
oo_gtest_check_probe = files('check_probe.py')
oo_gtest_python = find_program('python3')
oo_gtest_probes = {
    'probe_gtest_fails': ['[  FAILED  ] probe.fails', '1 + 2', 'Which is: 3'],
    'probe_gtest_empty': ['does NOT link in any test case'],
    'probe_gtest_aborts': ['oo_gtest: abort() in case probe.aborts'],
}
foreach probe, expected : oo_gtest_probes
    probe_exe = executable(probe, probe + '.cpp', dependencies: oo_gtest_main_dep, install: false)
    probe_args = [oo_gtest_check_probe, probe_exe]
    foreach text : expected
        probe_args += ['--expect', text]
    endforeach
    test(probe, oo_gtest_python, args: probe_args, suite: 'gtest', timeout: 60)
endforeach
