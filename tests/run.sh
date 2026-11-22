#!/bin/bash
# Run Oolite's test tiers from this directory, inside a private virtualenv.
#
#     tests/run.sh unit        meson suites: oofnd, core, ui, headers (needs ./mk.sh build dev)
#     tests/run.sh component   headless C++ programs, meson suite component; needs no display
#                              and no virtualenv (--cpp is accepted and means the same)
#     tests/run.sh smoke       launch_snapshot.py via mk.sh
#     tests/run.sh all         unit, component, then smoke, stopping at the first failure
#     tests/run.sh venv        create/refresh the virtualenv only
#
# For the component tier, -k <name> picks the programs whose meson test name contains <name>; a
# name that matches no program is an error:
#     tests/run.sh component -k skeleton
# For the unit tier, -k <filter> is a Google Test filter for every Google Test suite (the headers
# suite, which is not one, is left out):
#     tests/run.sh unit -k 'OOColor.*'
#
# Environment: BUILD_TYPE (default dev), OO_VENV (default build/.venv-tests), OO_APP_DIR.
# dev, not test: test links with LTO, and every core unit test re-runs LTO over the game's objects.
# The venv lives under build/, which is git-ignored. Delete it to force a clean reinstall.

set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." &> /dev/null && pwd)
cd "$ROOT"

BUILD_TYPE=${BUILD_TYPE:-dev}
VENV=${OO_VENV:-build/.venv-tests}
REQ_STAMP="$VENV/.requirements-stamp"
REQ_FILES=()  # no tier installs anything into the venv now

find_python() {
    local p
    for p in python3 python; do
        if "$p" --version > /dev/null 2>&1; then
            echo "$p"
            return
        fi
    done
    echo "tests/run.sh: no python3 on PATH" >&2
    return 1
}

# Windows Pythons put the interpreter in Scripts/, POSIX ones in bin/.
venv_python() {
    if [[ -x "$VENV/Scripts/python.exe" ]]; then
        echo "$VENV/Scripts/python.exe"
    else
        echo "$VENV/bin/python"
    fi
}

ensure_venv() {
    if [[ ! -x "$VENV/Scripts/python.exe" && ! -x "$VENV/bin/python" ]]; then
        echo "--> Creating virtualenv at $VENV"
        "$(find_python)" -m venv "$VENV"
    fi
    [[ ${#REQ_FILES[@]} -gt 0 ]] || return 0
    local py
    py=$(venv_python)
    # Reinstall only when a requirements file is newer than the last successful install.
    local stale=false f
    [[ -f "$REQ_STAMP" ]] || stale=true
    for f in "${REQ_FILES[@]}"; do
        [[ "$f" -nt "$REQ_STAMP" ]] && stale=true
    done
    if [[ "$stale" == true ]]; then
        echo "--> Installing test requirements into $VENV"
        local args=()
        for f in "${REQ_FILES[@]}"; do args+=(-r "$f"); done
        "$py" -m pip install --quiet --disable-pip-version-check "${args[@]}"
        touch "$REQ_STAMP"
    fi
}

require_build() {
    local app="${OO_APP_DIR:-build/meson_${BUILD_TYPE}/oolite.app}"
    if [[ ! -d "$app" ]]; then
        echo "tests/run.sh: no build at $app. Run ./mk.sh build $BUILD_TYPE first." >&2
        exit 1
    fi
    # The pytest tiers' own default is build/meson_test; hand them the build this run checks.
    app=$(cd "$app" && pwd)
    if command -v cygpath > /dev/null 2>&1; then
        app=$(cygpath -m "$app")
    fi
    export OO_APP_DIR="$app"
}

# unit [-k <gtest-filter>]
run_unit() {
    require_build
    local filter="" have_filter=false suite
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -k) filter=${2:-}; have_filter=true; shift 2 || shift ;;
            -k*) filter=${1#-k}; have_filter=true; shift ;;
            *) echo "tests/run.sh unit: unknown argument '$1'" >&2; return 2 ;;
        esac
    done
    if $have_filter && [[ -z "$filter" ]]; then
        echo "tests/run.sh unit: -k needs a Google Test filter" >&2
        return 2
    fi
    for suite in oofnd core ui headers; do
        if $have_filter; then
            [[ "$suite" == headers ]] && continue
            echo "--> meson test --suite $suite --test-args=--gtest_filter=$filter"
            meson test -C "build/meson_${BUILD_TYPE}" --suite "$suite" --print-errorlogs \
                "--test-args=--gtest_filter=$filter"
        else
            echo "--> meson test --suite $suite"
            meson test -C "build/meson_${BUILD_TYPE}" --suite "$suite" --print-errorlogs
        fi
    done
}

# component [--cpp] [-k <name>]: a name that matches no program is an error.
run_component() {
    [[ "${1:-}" == --cpp ]] && shift
    require_build
    local filter="" have_filter=false
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -k) filter=${2:-}; have_filter=true; shift 2 || shift ;;
            -k*) filter=${1#-k}; have_filter=true; shift ;;
            *) echo "tests/run.sh component: unknown argument '$1'" >&2; return 2 ;;
        esac
    done
    local dir="build/meson_${BUILD_TYPE}"
    if [[ "$have_filter" == false ]]; then
        echo "--> meson test --suite component"
        meson test -C "$dir" --suite component --print-errorlogs
        return
    fi
    # meson test --list prints "<suite> - <project>:<name>"; meson test takes the bare names.
    local names=() line name
    while IFS= read -r line; do
        name=${line##*:}
        [[ "$name" == *"$filter"* ]] && names+=("$name")
    done < <(meson test -C "$dir" --suite component --list 2> /dev/null | tr -d '\r' | grep ' - ')
    if [[ ${#names[@]} -eq 0 ]]; then
        echo "tests/run.sh: no C++ component program matches '$filter'" >&2
        return 1
    fi
    echo "--> meson test --suite component ${names[*]}"
    meson test -C "$dir" --suite component --print-errorlogs "${names[@]}"
}

run_smoke() {
    require_build
    ./mk.sh test "$BUILD_TYPE"
}

tier=${1:-}
[[ $# -gt 0 ]] && shift

case "$tier" in
    venv)      ensure_venv ;;
    unit)      run_unit "$@" ;;  # no Python needed; meson drives it
    component) run_component "$@" ;;
    smoke)     run_smoke ;;
    all)       run_unit; run_component; run_smoke ;;
    *)
        sed -n '2,21p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
        [[ -z "$tier" ]] || exit 2
        ;;
esac
