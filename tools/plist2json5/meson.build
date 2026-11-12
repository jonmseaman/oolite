# tools/plist2json5 converts property lists to JSON5 (Plist2JSON5.hpp has the rules). Plain C++ on
# the header-only oofnd plist code, so it builds, and its tests run, on every platform:
#   meson test -C build/meson_<type> --suite plist2json5
# To convert files: build/meson_<type>/tools/plist2json5/plist2json5 [--strings|--typed] <path>...

plist2json5_inc = include_directories('.', '../../tests/unit/oofnd')

plist2json5 = executable('plist2json5', 'plist2json5.cpp',
    include_directories: [oofnd_inc, plist2json5_inc],
    install: false, override_options: oofnd_override_options)

test('test_plist2json5',
    executable('test_plist2json5', 'test_plist2json5.cpp',
        include_directories: [oofnd_inc, plist2json5_inc],
        install: false, override_options: oofnd_override_options),
    suite: 'plist2json5', timeout: 60)

# Every core plist converts and reads back as the same data, in both modes.
plist2json5_resources = meson.project_source_root() / 'Resources'
test('plist2json5_resources_strings', plist2json5,
    args: ['--check', '--strings', plist2json5_resources],
    suite: 'plist2json5', timeout: 120)
test('plist2json5_resources_typed', plist2json5,
    args: ['--check', '--typed', plist2json5_resources],
    suite: 'plist2json5', timeout: 120)
