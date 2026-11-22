#!/usr/bin/env python3
"""Runs a probe program and checks how it fails.

    check_probe.py <program> [--arg <arg>]... [--exit nonzero|zero] [--expect <text>]...

Exits 0 when the program's exit status is as asked (default nonzero) and its
combined stdout and stderr contain every --expect text; otherwise prints what
differed and exits 1.
"""

import argparse
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("program")
    parser.add_argument("--arg", action="append", default=[])
    parser.add_argument("--exit", choices=["nonzero", "zero"], default="nonzero")
    parser.add_argument("--expect", action="append", default=[])
    # A Google Test flag meant for the suite's other programs (tests/run.sh unit -k <filter> passes
    # one to every test in the suite) is not the probe's.
    options, extra = parser.parse_known_args()
    unknown = [a for a in extra if not a.startswith("--gtest_")]
    if unknown:
        parser.error(f"unrecognized arguments: {' '.join(unknown)}")

    run = subprocess.run([options.program] + options.arg, capture_output=True, text=True,
                         errors="replace", timeout=60, check=False)
    output = run.stdout + run.stderr
    ok = (run.returncode != 0) if options.exit == "nonzero" else (run.returncode == 0)
    if not ok:
        print(f"{options.program}: exit status {run.returncode}, expected {options.exit}")
    for text in options.expect:
        if text not in output:
            print(f"{options.program}: output lacks {text!r}")
            ok = False
    if not ok:
        print("--- output ---")
        print(output)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
