# What every C++ test program in the tree links (tests/unit, tests/component, tools/plist2json5),
# and its own tests: meson test --suite gtest.
#
# oo_gtest_main_dep: Google Test with its own main (gtest_main) and this directory's oo_gtest.hpp,
# for a program with no main. oo_gtest_dep: the same without the main, for a program that has one
# (the component programs, through oo_component.cpp).

oo_gtest_inc = include_directories('.')

oo_gtest_dep = declare_dependency(
    dependencies: gtest_dep,
    include_directories: oo_gtest_inc,
)

oo_gtest_main_dep = declare_dependency(
    dependencies: gtest_main_dep,
    include_directories: oo_gtest_inc,
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

# A program with a failing case must fail and show both values (runner.md R1): check_probe.py runs
# it and checks the exit status and the text.
test('probe_gtest_fails', find_program('python3'),
    args: [files('check_probe.py'),
        executable('probe_gtest_fails', 'probe_gtest_fails.cpp',
            dependencies: oo_gtest_main_dep, install: false),
        '--expect', '[  FAILED  ] probe.fails', '--expect', '1 + 2', '--expect', 'Which is: 3'],
    suite: 'gtest', timeout: 60)
