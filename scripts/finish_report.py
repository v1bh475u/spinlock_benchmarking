#!/usr/bin/env python3
"""Finish report: copy generated benchmark artifacts into docs location."""
from pathlib import Path
import shutil
import sys

REPO_ROOT = Path(__file__).resolve().parents[1]
OUT = REPO_ROOT / "benchmark-results"
DEST = REPO_ROOT / "docs" / "benchmarks" / "linux-local"


def main() -> int:
    DEST.mkdir(parents=True, exist_ok=True)

    # Copy metadata
    src_meta = OUT / "metadata.json"
    if src_meta.exists():
        shutil.copy2(src_meta, DEST / "metadata.json")
        print(f"Copied {src_meta} -> {DEST}")
    else:
        print("metadata.json not found", file=sys.stderr)

    # Copy report files
    report_dir = OUT / "report"
    if report_dir.exists():
        csv = report_dir / "benchmark.csv"
        summary = report_dir / "summary.md"
        plots = report_dir / "plots"
        if csv.exists():
            shutil.copy2(csv, DEST / "benchmark.csv")
        if summary.exists():
            shutil.copy2(summary, DEST / "summary.md")
        if plots.exists():
            dest_plots = DEST / "plots"
            if dest_plots.exists():
                shutil.rmtree(dest_plots)
            shutil.copytree(plots, dest_plots)
        print(f"Copied report -> {DEST}")
    else:
        print("report directory not found", file=sys.stderr)

    return 0


if __name__ == '__main__':
    raise SystemExit(main())
