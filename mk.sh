#!/bin/bash

# macOS ships bash 3.2; the build scripts need bash 4+, e.g. from Homebrew.
if ((BASH_VERSINFO[0] < 4)); then
    for newer_bash in /opt/homebrew/bin/bash /usr/local/bin/bash; do
        if [[ -x "$newer_bash" ]]; then
            exec "$newer_bash" "$0" "$@"
        fi
    done
    echo "❌ bash 4 or later is required (on macOS: brew install bash)" >&2
    exit 1
fi

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
pushd "$SCRIPT_DIR" > /dev/null

set -u -o pipefail  # Strict expansions

cleanup_and_exit() {  # Error handling trap
    local exit_code=$?

    if [[ $exit_code -eq 0 ]]; then
        exit_code=1  # force 0 exit code to 1 to indicate an error
    fi
    echo "❌ Oolite build failed on line $1 with exit code $exit_code!" >&2
    popd > /dev/null 2>&1 || true  # Always pop the directory stack before exiting
    if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then  # Exit only if not sourced
        exit "$exit_code"
    fi
}
trap 'cleanup_and_exit ${LINENO}' ERR  # Trap any command errors (ERR) passing ${LINENO} to know where it failed

output_meson_log() {
    meson_log="$1/meson-logs/meson-log.txt"
    if [[ -f "$meson_log" ]]; then
        echo -e "\n=== MESON LOG START ===" >&2
        cat "$meson_log" >&2
        echo -e "=== MESON LOG END ===\n" >&2
    fi
}

# --- Feature Flags & Options ---
NATIVE_FILE=""
VER_FULL=""
BUILDTIME=""
GITHUB_REPOSITORY=""
CLEAN_BUILD=false
PRINT_MESON_LOG=false
SETUP_FLAGS=() # Array to cleanly store additional meson setup arguments
COMPILE_FLAGS=() # Array to cleanly store additional meson compile arguments
CONFIGURE_FLAGS=() # Array to cleanly store additional meson configure arguments
INSTALL_FLAGS=() # Array to cleanly store additional meson install arguments
MESON_WRAP=() # Command prefix for meson setup and compile (the Visual Studio flavours set it)

# The Visual Studio flavours' libraries: vcpkg.json, installed under build/vcpkg_installed.
VCPKG_PREFIX="$SCRIPT_DIR/build/vcpkg_installed/x64-windows"
VCPKG_INI="$SCRIPT_DIR/build/vcpkg_installed/vcpkg.ini"

# vswhere, vcpkg and meson --vsenv find Visual Studio through the ProgramFiles variables. An MSYS2
# login shell filters them out of its environment, and ProgramFiles(x86) is not a valid shell name,
# so it cannot be exported either; env(1) can still pass it. cygpath -F reads the folders from the OS.
with_windows_env() {
    env "ProgramFiles=$(cygpath -w "$(cygpath -F 38)")" \
        "ProgramFiles(x86)=$(cygpath -w "$(cygpath -F 42)")" \
        "ProgramData=$(cygpath -w "$(cygpath -F 35)")" \
        "$@"
}

VSWHERE="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"

# A property of the newest Visual Studio install, which is the one meson --vsenv loads.
vs_property() {
    with_windows_env "$VSWHERE" -latest -products '*' -property "$1" | tr -d '\r'
}

# meson's backend for that install. Named, not "vs": meson stores the resolved name and refuses a
# reconfigure that passes "vs" again.
vs_backend() {
    local major
    major=$(vs_property installationVersion)
    case "${major%%.*}" in
        18) echo vs2026 ;;
        17) echo vs2022 ;;
        16) echo vs2019 ;;
        *)
            echo "❌ Unsupported Visual Studio version '$major'" >&2
            exit 1
            ;;
    esac
}

vcpkg_install() {
    local vcpkg="${VCPKG_ROOT:+$VCPKG_ROOT/vcpkg.exe}"
    if [[ -z "$vcpkg" ]]; then
        # Visual Studio ships vcpkg.
        vcpkg="$(cygpath -u "$(vs_property installationPath)")/VC/vcpkg/vcpkg.exe"
    fi
    echo "--> Installing vcpkg libraries with: $vcpkg"
    # vcpkg builds the ports with Visual Studio's tools, so it gets a plain Windows environment:
    # under the MSYS2 one, the ports' resource-compiler steps fail (zlib: "RC1107: invalid usage").
    local sysroot
    sysroot=$(cygpath -w "$(cygpath -F 36)")
    if ! env -i "SystemRoot=$sysroot" "windir=$sysroot" "SystemDrive=${sysroot%%\\*}" \
        "PATH=$sysroot\\System32;$sysroot;$sysroot\\System32\\WindowsPowerShell\\v1.0" \
        "TEMP=$(cygpath -w "$(cygpath -F 28)")\\Temp" "TMP=$(cygpath -w "$(cygpath -F 28)")\\Temp" \
        "USERPROFILE=$(cygpath -w "$(cygpath -F 40)")" "LOCALAPPDATA=$(cygpath -w "$(cygpath -F 28)")" \
        "APPDATA=$(cygpath -w "$(cygpath -F 26)")" "NUMBER_OF_PROCESSORS=$(nproc)" \
        "PROCESSOR_ARCHITECTURE=AMD64" "ProgramFiles=$(cygpath -w "$(cygpath -F 38)")" \
        "ProgramFiles(x86)=$(cygpath -w "$(cygpath -F 42)")" "ProgramData=$(cygpath -w "$(cygpath -F 35)")" \
        ${VCPKG_ROOT:+"VCPKG_ROOT=$VCPKG_ROOT"} \
        "$vcpkg" install --triplet x64-windows --x-install-root="$(cygpath -w "$SCRIPT_DIR/build/vcpkg_installed")"; then
        echo "❌ vcpkg install failed!" >&2
        exit 1
    fi
    # pkg-config must see vcpkg's libraries only: on the default search path MSYS2's MinGW builds
    # of the same libraries come first, and their headers do not compile under cl.exe.
    # pkg_config_libdir is stored in the build directory, so a regeneration from Visual Studio
    # keeps it.
    printf '%s\n' "# Written by mk.sh (vcpkg_install) for the Visual Studio flavours." "[properties]" \
        "pkg_config_libdir = ['$(cygpath -m "$VCPKG_PREFIX/lib/pkgconfig")']" > "$VCPKG_INI"
}

# Debugging from Visual Studio: the game must run from the staged oolite.app (Resources, AddOns,
# DLLs), not from the linker's output in src/. The post_build project stages it and depends on the
# game, so as the startup project (backend_startup_project) F5 builds, stages and launches.
# Visual Studio reads a project's debugger settings from its .vcxproj.user file, which meson does
# not write.
vs_debug_settings() {
    local build_dir="$1" project
    for project in "$build_dir"/src/*@@post_build@cus.vcxproj; do
        [[ -f "$project" ]] || continue
        printf '%s\r\n' \
            '<?xml version="1.0" encoding="utf-8"?>' \
            '<!-- Written by mk.sh (vs_debug_settings). -->' \
            '<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">' \
            '  <PropertyGroup>' \
            '    <LocalDebuggerCommand>$(SolutionDir)oolite.app\oolite.exe</LocalDebuggerCommand>' \
            '    <LocalDebuggerWorkingDirectory>$(SolutionDir)oolite.app</LocalDebuggerWorkingDirectory>' \
            '    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>' \
            '  </PropertyGroup>' \
            '</Project>' > "$project.user"
    done
}

clean() {
    echo "--> Cleaning target build directory: $1"
    rm -rf "$1"
}

meson_setup() {
    local build_dir="build/meson_$1"
    echo "--> Running Meson setup for: $1"
    local meson_opts=("${@:2}")
    if [[ -n "${VER_FULL:-}" ]]; then
        export VER_FULL
    fi
    if [[ -n "${BUILDTIME:-}" ]]; then
        export BUILDTIME
    fi
    if [[ -n "${GITHUB_REPOSITORY:-}" ]]; then
        export GITHUB_REPOSITORY
    fi
    # meson-private/coredata.dat: a Visual Studio backend directory has no build.ninja.
    if [[ -d "$build_dir" ]] && [[ -f "$build_dir/build.ninja" || -f "$build_dir/meson-private/coredata.dat" ]]; then
        echo "🔄 Directory exists, attempting to reconfigure..."
        if ! ${MESON_WRAP[@]+"${MESON_WRAP[@]}"} meson setup "$build_dir" "${meson_opts[@]}" ${SETUP_FLAGS[@]+"${SETUP_FLAGS[@]}"} --native-file "${NATIVE_FILE}" --reconfigure; then
            echo "❌ Meson reconfiguration failed!" >&2
            if [[ "$PRINT_MESON_LOG" == true ]]; then
                output_meson_log "$build_dir"
            fi
            exit 1
        fi
    else
        echo "🏗️ Creating new build configuration..."
        if ! ${MESON_WRAP[@]+"${MESON_WRAP[@]}"} meson setup "$build_dir" "${meson_opts[@]}" ${SETUP_FLAGS[@]+"${SETUP_FLAGS[@]}"} --native-file "${NATIVE_FILE}"; then
            echo "❌ Meson initial setup failed!" >&2
            if [[ "$PRINT_MESON_LOG" == true ]]; then
                output_meson_log "$build_dir"
            fi
            exit 1
        fi
    fi
    # clangd and clang-tidy (-p .) read the compile database from this directory's root.
    if [[ -f "$build_dir/compile_commands.json" ]]; then
        cp -f "$build_dir/compile_commands.json" compile_commands.json
    fi
}

meson_compile() {
    local build_dir="build/meson_$1"
    echo "--> Running Meson build for: $1"
    if ! ${MESON_WRAP[@]+"${MESON_WRAP[@]}"} meson compile -C "$build_dir" ${COMPILE_FLAGS[@]+"${COMPILE_FLAGS[@]}"}; then
        echo "❌ Meson compile failed!" >&2
        if [[ "$PRINT_MESON_LOG" == true ]]; then
            output_meson_log "$build_dir"
        fi
        exit 1
    fi
}

meson_configure() {
    local build_dir="build/meson_$1"
    echo "--> Running Meson configure for: $1"
    local meson_opts=("${@:2}")
    if ! meson configure "$build_dir" "${meson_opts[@]}" ${CONFIGURE_FLAGS[@]+"${CONFIGURE_FLAGS[@]}"}; then
        echo "❌ Meson configure failed!" >&2
        if [[ "$PRINT_MESON_LOG" == true ]]; then
            output_meson_log "$build_dir"
        fi
        exit 1
    fi
}

meson_install() {
    local build_dir="build/meson_$1"
    echo "--> Running Meson install for: $1"
    if ! meson install -C "$build_dir" ${INSTALL_FLAGS[@]+"${INSTALL_FLAGS[@]}"}; then
        echo "❌ Meson install failed!" >&2
        if [[ "$PRINT_MESON_LOG" == true ]]; then
            output_meson_log "$build_dir"
        fi
        exit 1
    fi
}

show_help() {  # Script Help Menu
    echo "Usage: $0 [options] <action> <build_type>"
    echo "       $0 [options] <global_action>"
    echo ""
    echo "Options:"
    echo -e "  \033[36m--setup-flags=\"...\"\033[0m            Pass additional arguments directly to 'meson setup'"
    echo -e "  \033[36m--compile-flags=\"...\"\033[0m          Pass additional arguments directly to 'meson compile'"
    echo -e "  \033[36m--configure-flags=\"...\"\033[0m        Pass additional arguments directly to 'meson configure'"
    echo -e "  \033[36m--install-flags=\"...\"\033[0m          Pass additional arguments directly to 'meson install'"
    echo -e "  \033[36m--native-file=\"...\"\033[0m            Specify native file (defaults to clang.ini)"
    echo -e "  \033[36m--ver-full=\"...\"\033[0m               Specify full version string"
    echo -e "  \033[36m--buildtime=\"...\"\033[0m              Specify build time"
    echo -e "  \033[36m--github-repository=\"...\"\033[0m      Specify target GitHub repository"
    echo -e "  \033[36m--meson-log\033[0m                    Output the Meson log file contents"
    echo ""
    echo "Build Type Actions (Requires build_type as second parameter):"
    echo -e "  \033[36msetup <build_type>\033[0m             Setup a release build directory"
    echo -e "  \033[36mcompile <build_type>\033[0m           Compile a build directory"
    echo -e "  \033[36mbuild <build_type>\033[0m             Setup and compile a build directory"
    echo -e "  \033[36mconfigure <build_type>\033[0m         Modify build options of an existing build directory"
    echo -e "  \033[36minstall <build_type>\033[0m           Install an existing build directory"
    echo -e "  \033[36mtest <build_type>\033[0m              Run test suites (deployment build_type excluded)"
    echo -e "  \033[36mclean <build_type>\033[0m             Clean a specific build_type's directory"
    echo -e "  \033[36moutput-meson-log <build_type>\033[0m  Print the meson log file for a build directory"
    echo -e "  \033[36mflatpak-internal <build_type>\033[0m  Build flatpak dependencies internally"
    echo -e "  \033[36mpkg-flatpak <build_type>\033[0m       Package a Flatpak application"
    echo -e "  \033[36mpkg-appimage <build_type>\033[0m      Package a Linux AppImage installer"
    echo -e "  \033[36mpkg-win <build_type>\033[0m           Package a Windows NSIS installer"
    echo -e "  \033[36mpkg-dmg <build_type>\033[0m           Package a macOS disk image (.dmg)"
    echo ""
    echo "Global Actions:"
    echo -e "  \033[36mclean-all\033[0m                      Remove generated artifacts for all builds"
    echo -e "  \033[36mformat\033[0m                         Reformat src/ in place with clang-format (as the CI check expects)"
    echo -e "  \033[36mformat-check\033[0m                   Report src/ files clang-format would change, without editing"
    echo -e "  \033[36mhelp\033[0m                           Show this breakdown menu"
    echo ""
    echo "Build Types:"
    echo -e "  \033[32mdeployment, test, dev, debug\033[0m"
    echo -e "  \033[32mvs-clang, vs-msvc\033[0m              Visual Studio solution, clang-cl or cl.exe (libraries from vcpkg)"
}

validate_build_type() {
    local build_type="$1"
    if [[ "$build_type" != "deployment" && "$build_type" != "test" && "$build_type" != "dev" && "$build_type" != "debug" && "$build_type" != "vs-clang" && "$build_type" != "vs-msvc" ]]; then
        echo "❌ Invalid build_type '$build_type'. Expected: deployment, test, dev, debug, vs-clang or vs-msvc." >&2
        exit 1
    fi
}

execute_target() {  # Target Execution Logic
    local action="$1"
    local build_type="${2:-}"

    case "$action" in
        setup)
            validate_build_type "$build_type"
            if [[ "$build_type" == "deployment" ]]; then
                # Arguments passed individually (acts like an array)
                meson_setup "deployment" "-Ddeployment_release=true" "-Ddebug=false" "-Dstrip_bin=true" "-Db_lto=true"
            elif [[ "$build_type" == "test" ]]; then
                meson_setup "test" "-Ddebug=false" "-Dstrip_bin=true" "-Db_lto=true"
            elif [[ "$build_type" == "dev" ]]; then
                meson_setup "dev" "-Ddev_release=true" "-Ddebug=false" "-Dstrip_bin=false"
            elif [[ "$build_type" == "debug" ]]; then
                meson_setup "debug" "-Ddebug=true" "-Dstrip_bin=false"
            elif [[ "$build_type" == vs-* ]]; then
                vcpkg_install
                # A Visual Studio solution (build/meson_<type>/oolite.sln). --vsenv loads the
                # newest Visual Studio's environment even though MSYS's compilers are on PATH.
                # The vcpkg libraries are release builds (/MD), so the game uses the same CRT.
                # espeak-ng has no vcpkg port, so speech is off in these builds.
                meson_setup "$build_type" "--vsenv" "--backend=$(vs_backend)" "-Ddebug=true" "-Dstrip_bin=false" \
                    "-Db_vscrt=md" "-Despeak=false" "--native-file=$VCPKG_INI" \
                    "-Dvcpkg_bin_dir=$VCPKG_PREFIX/bin" "-Dbackend_startup_project=post_build"
                vs_debug_settings "build/meson_$build_type"
            fi
            ;;
        compile)
            validate_build_type "$build_type"
            meson_compile "$build_type"
            ;;
        build)
            validate_build_type "$build_type"
            execute_target "setup" "$build_type"
            execute_target "compile" "$build_type"
            ;;
        configure)
            validate_build_type "$build_type"
            meson_configure "$build_type"
            ;;
        install)
            validate_build_type "$build_type"
            meson_install "$build_type"
            ;;
        test)
            validate_build_type "$build_type"
            source tests/run_test_fn.sh && run_test "$build_type"
            ;;
        clean)
            if [[ "$build_type" == "appimage" ]]; then
                clean "build/appimage"
            elif [[ "$build_type" == "flatpak" ]]; then
                clean "build/flatpak"
            else
                validate_build_type "$build_type"
                clean "build/meson_$build_type"
            fi
            ;;
        output-meson-log)
            validate_build_type "$build_type"
            output_meson_log "build/meson_$build_type"
            ;;
        flatpak-internal)
            validate_build_type "$build_type"
            execute_target "build" "$build_type"
            if ! meson_configure "$build_type" "--prefix=/app"; then
                echo "❌ Flatpak meson configure with prefix /app failed!" >&2
                exit 1
            fi
            if ! meson_install "$build_type"; then
                echo "❌ Flatpak meson install failed!" >&2
                exit 1
            fi
            if ! source installers/flatpak/flatpak_postbuild_fn.sh; then
                echo "❌ Failed to source flatpak_postbuild_fn.sh!" >&2
                exit 1
            fi
            if ! flatpak_postbuild "meson_${build_type}/oolite.app"; then
                echo "❌ Flatpak post build failed!" >&2
                exit 1
            fi
            ;;
        pkg-flatpak)
            validate_build_type "$build_type"
            if ! source installers/flatpak/create_flatpak_fn.sh; then
                echo "❌ Failed to source create_flatpak_fn.sh!" >&2
                exit 1
            fi
            if ! create_flatpak "$build_type" "$GITHUB_REPOSITORY"; then
                echo "❌ Flatpak generation failed!" >&2
                exit 1
            fi
            ;;
        pkg-appimage)
            validate_build_type "$build_type"
            local appdir=$(realpath -m "build/appimage/oolite.AppDir")
            if ! meson_configure "$build_type" "--prefix=$appdir"; then
                echo "❌ AppImage meson configure with prefix /app failed!" >&2
                exit 1
            fi
            if ! meson_install "$build_type"; then
                echo "❌ AppImage meson install failed!" >&2
                exit 1
            fi
            if ! source installers/appimage/create_appimage_fn.sh; then
                echo "❌ Failed to source create_appimage_fn.sh!" >&2
                exit 1
            fi
            if ! create_appimage "$build_type" "meson_${build_type}/oolite.app"; then
                echo "❌ AppImage generation failed!" >&2
                exit 1
            fi
            ;;
        pkg-win)
            validate_build_type "$build_type"
            if ! source installers/win32/create_nsis_fn.sh; then
                echo "❌ Failed to source create_nsis_fn.sh!" >&2
                exit 1
            fi
            if ! create_nsis "$build_type" "meson_${build_type}/oolite.app"; then
                echo "❌ NSIS generation failed!" >&2
                exit 1
            fi
            ;;
        pkg-dmg)
            validate_build_type "$build_type"
            if ! source installers/macos/create_dmg_fn.sh; then
                echo "❌ Failed to source create_dmg_fn.sh!" >&2
                exit 1
            fi
            if ! create_dmg "$build_type" "meson_${build_type}/oolite.app"; then
                echo "❌ DMG generation failed!" >&2
                exit 1
            fi
            ;;
        *)
            echo "❌ Fatal structural error handling action '$action'" >&2
            exit 1
            ;;
    esac
}

ACTION=""
BUILD_TYPE=""

# --- Flexible Argument Parser (Allows flags and positionals anywhere) ---
while [[ $# -gt 0 ]]; do
    case "$1" in
        --setup-flags=*)
            read -r -a flags_array <<< "${1#*=}"
            SETUP_FLAGS+=("${flags_array[@]}")
            shift
            ;;
        --compile-flags=*)
            read -r -a flags_array <<< "${1#*=}"
            COMPILE_FLAGS+=("${flags_array[@]}")
            shift
            ;;
        --configure-flags=*)
            read -r -a flags_array <<< "${1#*=}"
            CONFIGURE_FLAGS+=("${flags_array[@]}")
            shift
            ;;
        --install-flags=*)
            read -r -a flags_array <<< "${1#*=}"
            INSTALL_FLAGS+=("${flags_array[@]}")
            shift
            ;;
        --native-file=*)
            NATIVE_FILE="${1#*=}"
            shift
            ;;
        --ver-full=*)
            VER_FULL="${1#*=}"
            shift
            ;;
        --buildtime=*)
            BUILDTIME="${1#*=}"
            shift
            ;;
        --github-repository=*)
            GITHUB_REPOSITORY="${1#*=}"
            shift
            ;;
        --meson-log|--output-meson-log)
            PRINT_MESON_LOG=true
            shift
            ;;
        help|--help|-h)
            show_help
            exit 0
            ;;
        -*)
            echo "❌ Unknown option '$1'" >&2
            show_help
            exit 1
            ;;
        *)
            # Process sequential text items dynamically
            if [[ -z "$ACTION" ]]; then
                ACTION="$1"
            elif [[ -z "$BUILD_TYPE" ]]; then
                BUILD_TYPE="$1"
            else
                echo "❌ Unexpected extra argument '$1'." >&2
                show_help
                exit 1
            fi
            shift
            ;;
    esac
done

if [[ -z "$ACTION" ]]; then
    show_help  # Fallback to help menu if no action was provided
    exit 1
fi

# Intercept global action 'clean-all' before validating build_type requirements
if [[ "$ACTION" == "clean-all" ]]; then
    echo "--> Cleaning all build artifacts..."
    rm -rf build/meson_*
    rm -rf build/flatpak
    rm -rf build/appimage
    trap - ERR
    popd > /dev/null
    echo "✅ Oolite task 'clean-all' completed successfully"
    exit 0
fi

# Intercept global actions 'format' and 'format-check', which mirror the CI clang-format check
if [[ "$ACTION" == "format" || "$ACTION" == "format-check" ]]; then
    if ! command -v clang-format > /dev/null; then
        echo "❌ clang-format not found in PATH." >&2
        exit 1
    fi
    # Keep in step with clang-format-version in .github/workflows/build-all.yaml:
    # other major versions wrap some expressions differently and fail the CI check.
    ci_clang_format_major=22
    if [[ "$(clang-format --version)" != *"version $ci_clang_format_major."* ]]; then
        echo "⚠️  CI uses clang-format $ci_clang_format_major; $(clang-format --version) may format differently." >&2
    fi
    format_files=()
    while IFS= read -r -d '' file; do
        format_files+=("$file")
    done < <(git ls-files -z -- 'src/*.h' 'src/*.hpp' 'src/*.c' 'src/*.cpp' 'src/*.m' 'src/*.mm')
    if [[ "$ACTION" == "format" ]]; then
        echo "--> Formatting ${#format_files[@]} files with $(clang-format --version)"
        clang-format -i "${format_files[@]}"
    else
        echo "--> Checking ${#format_files[@]} files with $(clang-format --version)"
        clang-format --dry-run -Werror "${format_files[@]}"
    fi
    trap - ERR
    popd > /dev/null
    echo "✅ Oolite task '$ACTION' completed successfully"
    exit 0
fi

# Everything past this point strictly requires a build_type parameter
if [[ -z "$BUILD_TYPE" ]]; then
    echo "❌ Error: Action '$ACTION' requires a target build_type parameter." >&2
    show_help
    exit 1
fi

if [[ "$BUILD_TYPE" == vs-* ]]; then
    MESON_WRAP=(with_windows_env)  # meson --vsenv runs vswhere
    # MSYS2 points PKG_CONFIG_PATH at its MinGW libraries, which pkg-config searches before
    # pkg_config_libdir (vcpkg's, set by vcpkg_install).
    unset PKG_CONFIG_PATH
fi
if [[ -z "$NATIVE_FILE" && "$BUILD_TYPE" == "vs-clang" ]]; then
    NATIVE_FILE="clang-cl.ini"
elif [[ -z "$NATIVE_FILE" && "$BUILD_TYPE" == "vs-msvc" ]]; then
    NATIVE_FILE="msvc.ini"
fi
if [[ -z "$NATIVE_FILE" ]]; then
    if [[ "$(uname -s)" == "Darwin" ]]; then
        NATIVE_FILE="darwin.ini"  # Apple clang with the system linker
    else
        NATIVE_FILE="clang.ini"  # Apply default if it wasn't passed as an option
    fi
fi

execute_target "$ACTION" "$BUILD_TYPE"

if [[ "$PRINT_MESON_LOG" == true ]]; then
    output_meson_log "build/meson_$BUILD_TYPE"
fi

trap - ERR  # Successful Exit
popd > /dev/null

echo "✅ Oolite task '$ACTION $BUILD_TYPE' completed successfully"

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    exit 0
fi