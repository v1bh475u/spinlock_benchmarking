#!/usr/bin/env python3
"""Build, test, and run the spinlock Google Benchmark suite on Linux."""

from __future__ import annotations

import argparse
import json
import platform
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]


def run(args: list[str], cwd: Path = REPO_ROOT, capture: bool = False) -> str:
    print("+ " + " ".join(args), flush=True)
    completed = subprocess.run(
        args,
        cwd=cwd,
        check=True,
        text=True,
        stdout=subprocess.PIPE if capture else None,
        stderr=subprocess.STDOUT if capture else None,
    )
    return completed.stdout.strip() if capture and completed.stdout else ""


def probe(args: list[str]) -> str:
    try:
        return run(args, capture=True)
    except (FileNotFoundError, subprocess.CalledProcessError):
        return ""


def write_metadata(out_dir: Path, args: argparse.Namespace) -> None:
    metadata = {
        "timestamp_utc": datetime.now(timezone.utc).isoformat(),
        "repository": str(REPO_ROOT),
        "git_commit": probe(["git", "rev-parse", "HEAD"]),
        "git_branch": probe(["git", "rev-parse", "--abbrev-ref", "HEAD"]),
        "platform": platform.platform(),
        "machine": platform.machine(),
        "processor": platform.processor(),
        "python": sys.version,
        "uname": probe(["uname", "-a"]),
        "lscpu": probe(["lscpu"]),
        "cmake": probe(["cmake", "--version"]),
        "compiler": probe([args.cxx, "--version"]) if args.cxx else "",
        "benchmark_args": {
            "build_dir": str(args.build_dir),
            "build_type": args.build_type,
            "repetitions": args.repetitions,
            "min_time": args.min_time,
            "benchmark_filter": args.benchmark_filter,
        },
    }
    (out_dir / "metadata.json").write_text(json.dumps(metadata, indent=2), encoding="utf-8")


def benchmark_executable(build_dir: Path) -> Path:
    exe = build_dir / "spinlock_bench"
    if not exe.exists():
        raise FileNotFoundError(f"benchmark executable not found: {exe}")
    return exe


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Configure, build, test, and run spinlock benchmarks."
    )
    parser.add_argument("--build-dir", type=Path, default=REPO_ROOT / "build-release")
    parser.add_argument("--out-dir", type=Path, default=REPO_ROOT / "benchmark-results")
    parser.add_argument("--build-type", default="Release")
    parser.add_argument("--generator", default="Ninja")
    parser.add_argument("--cxx", default="")
    parser.add_argument("--repetitions", type=int, default=10)
    parser.add_argument("--min-time", default="0.5s")
    parser.add_argument("--benchmark-filter", default=".*")
    parser.add_argument("--skip-build", action="store_true")
    parser.add_argument("--skip-tests", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    build_dir = args.build_dir.resolve()
    out_dir = args.out_dir.resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    if not args.skip_build:
        configure = [
            "cmake",
            "-S",
            str(REPO_ROOT),
            "-B",
            str(build_dir),
            "-DCMAKE_BUILD_TYPE=" + args.build_type,
        ]
        if args.generator:
            configure.extend(["-G", args.generator])
        if args.cxx:
            configure.append("-DCMAKE_CXX_COMPILER=" + args.cxx)
        run(configure)
        run(["cmake", "--build", str(build_dir), "--config", args.build_type, "--parallel"])

    if not args.skip_tests:
        run(
            [
                "ctest",
                "--test-dir",
                str(build_dir),
                "--build-config",
                args.build_type,
                "--output-on-failure",
            ]
        )

    results_json = out_dir / "benchmark.json"
    results_txt = out_dir / "benchmark.txt"
    bench_cmd = [
        str(benchmark_executable(build_dir)),
        "--benchmark_filter=" + args.benchmark_filter,
        "--benchmark_repetitions=" + str(args.repetitions),
        "--benchmark_report_aggregates_only=true",
        "--benchmark_time_unit=ns",
        "--benchmark_min_time=" + args.min_time,
        "--benchmark_out=" + str(results_json),
        "--benchmark_out_format=json",
    ]

    output = run(bench_cmd, capture=True)
    results_txt.write_text(output + "\n", encoding="utf-8")
    write_metadata(out_dir, args)

    print(f"Wrote benchmark JSON: {results_json}")
    print(f"Wrote benchmark text: {results_txt}")
    print(f"Wrote metadata: {out_dir / 'metadata.json'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
