# macOS specific (M.2). No GNUstep block: Apple's own Foundation and Cocoa replace it
# (ADR-0010). SDL3/libpng/openal/vorbis/zlib come from the shared dependency() list in
# ../../meson.build, resolved via Homebrew's pkg-config on this platform. nspr and espeak
# are handled there too (nspr dropped; espeak deferred to M.8).
oolite_dependencies += [
    dependency('appleframeworks', modules: ['Foundation', 'Cocoa', 'OpenGL']),
]
