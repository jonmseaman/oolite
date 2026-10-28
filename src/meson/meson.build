cc = meson.get_compiler('c')
ccid = cc.get_id()

if (host_os == 'windows' or host_os == 'cygwin')
    subdir('windows')
else
    subdir('linux')
endif

if ccid == 'gcc'
    oolite_override_options += 'c_std=gnu99'
endif

if get_option('b_lto')
    if ccid == 'clang'
        add_project_arguments('-flto=thin', language: c_family)
        oolite_link_args += [
            '-flto=thin',
            '-Wl,--thinlto-cache-dir=thinlto_cache',
        ]
    else
        add_project_arguments('-flto=auto', language: c_family)
        oolite_link_args += ['-flto=auto']
    endif
endif
