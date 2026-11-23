#!/usr/bin/env python3
"""Merge the component tier's coverage profiles and report them.

    coverage_report.py --build-dir build/meson_coverage [--out DIR]

tests/run.sh coverage runs the component programs of a build made with
./mk.sh build coverage (meson option source_coverage), each writing Clang
source-based coverage profiles (LLVM_PROFILE_FILE) into <out>/raw/<group>/,
where a group is one test (--per-test) or "all". This script merges them with
llvm-profdata and reports the game's own code (src/) with llvm-cov, against
every component program the build made:

    <out>/component.profdata   every group merged
    <out>/summary.txt          llvm-cov report: per file, lines/functions/regions
    <out>/html/index.html      llvm-cov show: every line's count
    <out>/component.lcov       for an editor's coverage gutter
    <out>/by-file.tsv          per file: lines, covered, uncovered; most uncovered first
    <out>/by-test.tsv          with more than one group: lines each group covers, per file

The llvm tools are the ones next to the compiler meson used, so their profile
format matches it; LLVM_COV and LLVM_PROFDATA override them.

Exit status: 0 on success; 1 when there is no profile to report or a tool
fails; 2 on a usage error.
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
from collections import defaultdict
from pathlib import Path


def fail(message):
    print("coverage_report.py: %s" % message, file=sys.stderr)
    sys.exit(1)


def read_intro(build_dir, name):
    path = build_dir / "meson-info" / ("intro-%s.json" % name)
    if not path.is_file():
        fail("%s is not a configured meson build directory (no %s)" % (build_dir, path.name))
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def check_instrumented(build_dir):
    for option in read_intro(build_dir, "buildoptions"):
        if option["name"] == "source_coverage":
            if option["value"]:
                return
            break
    fail("%s is not built with -Dsource_coverage=true; ./mk.sh build coverage" % build_dir)


def llvm_tool(build_dir, name, override):
    """The named llvm tool beside the C++ compiler meson used, else on PATH."""
    if os.environ.get(override):
        return os.environ[override]
    compilers = read_intro(build_dir, "compilers")
    exelist = compilers.get("host", {}).get("cpp", {}).get("exelist", [])
    if exelist:
        compiler = shutil.which(exelist[-1])  # past a ccache wrapper
        if compiler:
            for directory in (Path(compiler).parent, Path(compiler).resolve().parent):
                tool = shutil.which(name, path=str(directory))
                if tool:
                    return tool
    tool = shutil.which(name)
    if not tool:
        fail("no %s beside the compiler or on PATH; set %s" % (name, override))
    return tool


def component_programs(build_dir, source_dir):
    """Every executable the component tier's meson.build defines, as built."""
    component = (source_dir / "tests" / "component" / "meson.build").resolve()
    programs = []
    for target in read_intro(build_dir, "targets"):
        if target["type"] != "executable":
            continue
        if Path(target["defined_in"]).resolve() != component:
            continue
        for filename in target["filename"]:
            if Path(filename).is_file():
                programs.append(filename)
    if not programs:
        fail("no component program is built in %s" % build_dir)
    return sorted(programs)


def run(command, output=None):
    """Runs command, its stdout to output (a path) when given; returns stdout."""
    result = subprocess.run(command, stdout=subprocess.PIPE, text=True)
    if result.returncode != 0:
        fail("%s exited %d" % (Path(command[0]).name, result.returncode))
    if output is not None:
        with open(output, "w", encoding="utf-8", newline="\n") as f:
            f.write(result.stdout)
    return result.stdout


def merge(profdata, raws, output):
    listing = output.with_suffix(".inputs")
    with open(listing, "w", encoding="utf-8", newline="\n") as f:
        f.write("".join("%s\n" % raw for raw in raws))
    run([profdata, "merge", "-sparse", "--input-files=%s" % listing, "-o", str(output)])
    listing.unlink()


def cov_args(programs, profile):
    args = [programs[0]]
    for program in programs[1:]:
        args += ["-object", program]
    return args + ["-instr-profile=%s" % profile]


def file_summaries(cov, programs, profile, src_dir):
    """{path relative to the source root: llvm-cov's summary of it}."""
    text = run([cov, "export", "-summary-only"] + cov_args(programs, profile) + [str(src_dir)])
    root = src_dir.parent
    summaries = {}
    for data in json.loads(text)["data"]:
        for entry in data["files"]:
            path = Path(entry["filename"])
            try:
                name = path.resolve().relative_to(root).as_posix()
            except ValueError:
                continue
            summaries[name] = entry["summary"]
    return summaries


def percent(covered, count):
    return 100.0 * covered / count if count else 100.0


def write_by_file(summaries, output):
    rows = []
    for name, summary in summaries.items():
        lines = summary["lines"]
        functions = summary["functions"]
        rows.append((lines["count"] - lines["covered"], name, lines["count"], lines["covered"],
                     functions["count"], functions["covered"]))
    rows.sort(key=lambda row: (-row[0], row[1]))
    with open(output, "w", encoding="utf-8", newline="\n") as f:
        f.write("file\tlines\tcovered\tuncovered\tline%\tfunctions\tfunctions covered\n")
        for uncovered, name, count, covered, fcount, fcovered in rows:
            f.write("%s\t%d\t%d\t%d\t%.1f\t%d\t%d\n" % (
                name, count, covered, uncovered, percent(covered, count), fcount, fcovered))
    return rows


def print_directories(rows):
    totals = defaultdict(lambda: [0, 0])
    for _, name, count, covered, _, _ in rows:
        directory = name.rsplit("/", 1)[0]
        totals[directory][0] += count
        totals[directory][1] += covered
    print("%-40s %8s %8s %7s" % ("directory", "lines", "covered", "line%"))
    for directory, (count, covered) in sorted(totals.items(), key=lambda kv: -(kv[1][0] - kv[1][1])):
        print("%-40s %8d %8d %6.1f%%" % (directory, count, covered, percent(covered, count)))


def write_by_test(groups, summaries_by_group, all_files, output):
    names = sorted(groups)
    with open(output, "w", encoding="utf-8", newline="\n") as f:
        f.write("file\ttests reaching\t%s\n" % "\t".join(names))
        for path in sorted(all_files):
            covered = [summaries_by_group[name].get(path, {}).get("lines", {}).get("covered", 0)
                       for name in names]
            reaching = sum(1 for value in covered if value)
            f.write("%s\t%d\t%s\n" % (path, reaching, "\t".join(str(value) for value in covered)))


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--build-dir", required=True, type=Path)
    parser.add_argument("--out", type=Path, help="default: <build-dir>/coverage")
    parser.add_argument("--source-dir", type=Path,
                        default=Path(__file__).resolve().parent.parent)
    args = parser.parse_args()

    build_dir = args.build_dir.resolve()
    source_dir = args.source_dir.resolve()
    src_dir = source_dir / "src"
    out = (args.out or build_dir / "coverage").resolve()
    check_instrumented(build_dir)

    groups = {}
    for group in sorted((out / "raw").glob("*")):
        raws = sorted(group.glob("*.profraw"))
        if raws:
            groups[group.name] = raws
    if not groups:
        fail("no .profraw under %s; did any component program run?" % (out / "raw"))

    cov = llvm_tool(build_dir, "llvm-cov", "LLVM_COV")
    profdata = llvm_tool(build_dir, "llvm-profdata", "LLVM_PROFDATA")
    programs = component_programs(build_dir, source_dir)

    profile = out / "component.profdata"
    merge(profdata, [raw for raws in groups.values() for raw in raws], profile)
    common = cov_args(programs, profile)
    sources = [str(src_dir)]
    run([cov, "report"] + common + sources, out / "summary.txt")
    run([cov, "export", "-format=lcov"] + common + sources, out / "component.lcov")
    shutil.rmtree(out / "html", ignore_errors=True)
    run([cov, "show", "-format=html", "-output-dir=%s" % (out / "html"),
         "-show-line-counts-or-regions", "-show-directory-coverage"] + common + sources)

    summaries = file_summaries(cov, programs, profile, src_dir)
    rows = write_by_file(summaries, out / "by-file.tsv")

    if len(groups) > 1:
        per_test = out / "per-test"
        per_test.mkdir(exist_ok=True)
        summaries_by_group = {}
        for name, raws in groups.items():
            group_profile = per_test / ("%s.profdata" % name)
            merge(profdata, raws, group_profile)
            summaries_by_group[name] = file_summaries(cov, programs, group_profile, src_dir)
            print("    %s" % name)
        write_by_test(groups, summaries_by_group, summaries, out / "by-test.tsv")

    count = sum(row[2] for row in rows)
    covered = sum(row[3] for row in rows)
    fcount = sum(row[4] for row in rows)
    fcovered = sum(row[5] for row in rows)
    print()
    print_directories(rows)
    print()
    print("src/: %d of %d lines (%.1f%%), %d of %d functions (%.1f%%), from %d profile group(s)"
          % (covered, count, percent(covered, count), fcovered, fcount,
             percent(fcovered, fcount), len(groups)))
    for name in ("summary.txt", "by-file.tsv", "by-test.tsv", "component.lcov",
                 "html/index.html"):
        if (out / name).exists():
            print("    %s" % (out / name))
    return 0


if __name__ == "__main__":
    sys.exit(main())
