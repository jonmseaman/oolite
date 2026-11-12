# Windows specific

add_project_arguments(
    '-DWIN32',
    '-DWINVER=0x0A00',
    language: c_family,
)
if not msvc_syntax
    oolite_link_args += ['-mwindows']  # MinGW; MSVC links take win_subsystem from src/meson.build
else
    # The sources are UTF-8, not the console code page cl.exe assumes. NOMINMAX keeps <windows.h>'s
    # min and max macros off std::numeric_limits<T>::max().
    add_project_arguments('/utf-8', '-DNOMINMAX', language: c_family)
    if ccid == 'msvc'
        # The standard preprocessor: the traditional one expands __VA_ARGS__ as a single argument,
        # which the OOEXPAND_* argument-mapping macros (OOStringExpander.h) rely on not happening.
        add_project_arguments('/Zc:preprocessor', language: c_family)
    endif
endif

if ccid == 'clang' and get_option('pdb')
    add_project_arguments('-g', '-gcodeview', language: c_family)
    oolite_link_args += ['-Wl,-pdb=']
endif

dep_libs = [
    'glu32',
    'opengl32',
    'dwmapi',
    'winmm',
    'ws2_32',
]
foreach lib : dep_libs
    oolite_dependencies += [cc.find_library(lib, required: true)]
endforeach
if (ccid == 'gcc')
    add_project_arguments('-D_WIN32_WINNT=0x0A00', language: c_family)
endif
