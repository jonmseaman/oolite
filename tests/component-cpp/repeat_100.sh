#!/bin/bash
# Run a C++ component program 100 times with --digest-out and require every run's digests to be
# the first run's (SC-002). Not part of tests/run.sh component: it is 100 start-ups.
#
#     tests/component-cpp/repeat_100.sh test_s1_police_kills_pirate
#
# Environment: BUILD_TYPE (default dev), RUNS (default 100).

set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." &> /dev/null && pwd)
program=${1:-}
if [[ -z "$program" ]]; then
    echo "usage: $0 <program>" >&2
    exit 2
fi
runs=${RUNS:-100}
build="$ROOT/build/meson_${BUILD_TYPE:-dev}"
exe="$build/tests/component-cpp/$program"
[[ -x "$exe" || ! -x "$exe.exe" ]] || exe="$exe.exe"
if [[ ! -x "$exe" ]]; then
    echo "$0: no $exe; build it first (./mk.sh build ${BUILD_TYPE:-dev})" >&2
    exit 2
fi

scratch="$build/component/repeat-$program"
rm -rf "$scratch"
mkdir -p "$scratch"
for ((i = 1; i <= runs; i++)); do
    if ! "$exe" --digest-out "$scratch/run$i.digests" > "$scratch/run$i.log" 2>&1; then
        echo "$0: run $i of $program failed; its output is in $scratch/run$i.log" >&2
        exit 1
    fi
    if [[ $i -gt 1 ]] && ! cmp -s "$scratch/run1.digests" "$scratch/run$i.digests"; then
        step=$(cmp "$scratch/run1.digests" "$scratch/run$i.digests" 2>&1 | sed -n 's/.* line \([0-9]*\).*/\1/p')
        echo "$0: run $i differs from run 1 at step ${step:-?} (files in $scratch)" >&2
        exit 1
    fi
done
rm -rf "$scratch"
echo "$runs runs, identical"
