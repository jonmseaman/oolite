# Unit tests of the interface layer's view-models and classic views (meson test --suite ui, or
# tests/run.sh unit). They build src/UI's own sources (oolite_ui_sources: no game class, no RmlUi)
# with fakes for the game's side, so every platform builds and runs them.
ui_tests = ['test_ship_library_view_model', 'test_grid_ship_library_view', 'test_pointer_mapping']

foreach name : ui_tests
    exe = executable(name,
        [name + '.cpp', oolite_ui_sources],
        include_directories: include_directories('../../../src'),
        dependencies: [gtest_main_dep, dependency('sdl3', include_type: 'system')],
        override_options: ['cpp_std=c++23'],
        cpp_args: oolite_cpp_args,
        build_by_default: true,
        install: false,
    )
    test(name, exe, suite: 'ui')
endforeach

# The RmlUi view with RmlUi itself, a renderer that draws nothing, and the game's own document,
# stylesheet and fonts from the source tree.
rml_view_test = executable('test_rml_ship_library_view',
    ['test_rml_ship_library_view.cpp', oolite_ui_sources, files('../../../src/UI/RmlUi/RmlShipLibraryView.cpp')],
    include_directories: include_directories('../../../src'),
    dependencies: [gtest_main_dep, dependency('sdl3', include_type: 'system'),
                   dependency('rmlui', include_type: 'system')],
    override_options: ['cpp_std=c++23'],
    cpp_args: oolite_cpp_args + ['-DOO_TEST_UI_RESOURCES="' + (meson.project_source_root() / 'Resources' / 'UI') + '"'],
    build_by_default: true,
    install: false,
)
test('test_rml_ship_library_view', rml_view_test, suite: 'ui')
