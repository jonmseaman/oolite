# Generated Meson configuration for sub-module: SDL

oolite_sources += files(
    'GameController+SDLFullScreen.cpp',
    'MyOpenGLView.cpp',
    'MyOpenGLView+Input.cpp',
    'OOSDLJoystickManager.cpp',
    'main.cpp',
)

# The display's physical size (spec 008 research R7): EDID on Windows, Core
# Graphics on macOS, nothing elsewhere.
if host_machine.system() == 'windows'
    oolite_sources += files('OODisplaySizeWindows.cpp')
elif host_machine.system() == 'darwin'
    oolite_sources += files('OODisplaySizeMac.cpp')
else
    oolite_sources += files('OODisplaySizeLinux.cpp')
endif

oolite_includes += include_directories('.')

# Enter child subdirectories
subdir('EXRSnapshotSupport')
subdir('OOResourcesWin')
