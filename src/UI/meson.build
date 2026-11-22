# The interface layer: screens as model, view-model and views (src/UI/README.md).
# The sources here use no game class and no RmlUi, so the 'ui' unit suite builds them alone.
oolite_ui_sources = files(
    'NullUiHost.cpp',
    'ShipLibrary/GridShipLibraryView.cpp',
    'ShipLibrary/ShipLibraryScreen.cpp',
    'ShipLibrary/ShipLibraryViewModel.cpp',
)
oolite_sources += oolite_ui_sources

# The RmlUi look (src/UI/RmlUi), built on every platform and chosen at run time (ui-look).
# RmlUi's headers are system headers: the game's warnings are for its own code.
# Both by name: include_type does not reach a dependency's own dependencies.
oolite_dependencies += [
    dependency('rmlui', include_type: 'system'),
    dependency('rmlui-gl2', include_type: 'system'),
]
oolite_sources += files(
    'RmlUi/RmlShipLibraryView.cpp',
    'RmlUi/RmlUiHost.cpp',
)
