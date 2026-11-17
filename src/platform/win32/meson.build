# Windows resources: the application icon and the process manifest.
# The icon is shared with the NSIS installer, so it stays in installers/win32.

if host_machine.system() == 'windows'
    windows = import('windows')
    oolite_sources += windows.compile_resources(
        'OOResourcesWin.rc',
        include_directories: include_directories('../../../installers/win32'),
        depend_files: files(
            '../../../installers/win32/Oolite.ico',
            'oolite.exe.manifest',
        ),
    )
endif
