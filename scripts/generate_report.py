#!/usr/bin/env python3
"""Generate CSV, Markdown, and PNG plots from Google Benchmark JSON."""

from __future__ import annotations

import argparse
import csv
import json
import math
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Row:
    scenario: str
    implementation: str
    threads: int
    real_time_ns: float
    cpu_time_ns: float
    items_per_second: float
    name: str


def clean_name(name: str) -> str:
    for suffix in ("_mean", "_median", "_stddev", "_cv"):
        if name.endswith(suffix):
            return name[: -len(suffix)]
    return name


def parse_benchmark(entry: dict) -> Row | None:
    aggregate = entry.get("aggregate_name")
    if aggregate and aggregate != "mean":
        return None

    raw_name = entry.get("name", "")
    name = clean_name(raw_name)
    if "/" not in name or "_" not in name:
        return None

    benchmark_name, thread_text = name.rsplit("/", 1)
    try:
        threads = int(thread_text)
    except ValueError:
        return None

    scenario, implementation = benchmark_name.split("_", 1)
    real_time = float(entry.get("real_time", math.nan))
    cpu_time = float(entry.get("cpu_time", math.nan))
    items = float(entry.get("items_per_second", math.nan))

    return Row(
        scenario=scenario,
        implementation=implementation,
        threads=threads,
        real_time_ns=real_time,
        cpu_time_ns=cpu_time,
        items_per_second=items,
        name=raw_name,
    )


def load_rows(path: Path) -> list[Row]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    rows = []
    for entry in payload.get("benchmarks", []):
        row = parse_benchmark(entry)
        if row is not None:
            rows.append(row)
    rows.sort(key=lambda r: (r.scenario, r.implementation, r.threads))
    return rows


def write_csv(rows: list[Row], path: Path) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=[
                "scenario",
                "implementation",
                "threads",
                "real_time_ns",
                "cpu_time_ns",
                "items_per_second",
                "name",
            ],
        )
        writer.writeheader()
        for row in rows:
            writer.writerow(row.__dict__)


def load_metadata(path: Path | None, benchmark_json: Path) -> dict:
    if path is None:
        candidate = benchmark_json.resolve().parent / "metadata.json"
    else:
        candidate = path.resolve()

    if not candidate.exists():
        return {}

    return json.loads(candidate.read_text(encoding="utf-8"))


def metadata_lines(metadata: dict) -> list[str]:
    if not metadata:
        return []

    fields = [
        ("Timestamp UTC", metadata.get("timestamp_utc", "")),
        ("Git commit", metadata.get("git_commit", "")),
        ("Git branch", metadata.get("git_branch", "")),
        ("Platform", metadata.get("platform", "")),
        ("Machine", metadata.get("machine", "")),
        ("Processor", metadata.get("processor", "")),
        ("Kernel", metadata.get("uname", "")),
        ("Compiler", metadata.get("compiler", "").splitlines()[0]),
        ("CMake", metadata.get("cmake", "").splitlines()[0]),
    ]

    lines = [
        "## Machine And Run Metadata",
        "",
        "| Field | Value |",
        "| --- | --- |",
    ]
    for label, value in fields:
        if value:
            lines.append(f"| {label} | `{value}` |")
    lines.append("")
    return lines


def best_rows(rows: list[Row]) -> list[Row]:
    best = {}
    for row in rows:
        key = (row.scenario, row.threads)
        current = best.get(key)
        if current is None or row.items_per_second > current.items_per_second:
            best[key] = row
    return [best[key] for key in sorted(best)]


def plot_markdown_lines(rows: list[Row]) -> list[str]:
    lines = ["## Plots", ""]
    scenarios = sorted({row.scenario for row in rows})
    metrics = [
        ("real_time_ns", "real time"),
        ("items_per_second", "throughput"),
    ]

    for scenario in scenarios:
        lower = scenario.lower()
        lines.append(f"### {scenario}")
        lines.append("")
        for metric, label in metrics:
            lines.append(f"![{scenario} {label}](plots/{lower}_{metric}.png)")
            lines.append("")

    return lines


def write_markdown(rows: list[Row], metadata: dict, path: Path) -> None:
    lines = [
        "# Spinlock Benchmark Report",
        "",
        "Generated from `benchmark.json` by `scripts/generate_report.py`.",
        "",
    ]

    lines.extend(metadata_lines(metadata))
    lines.extend(plot_markdown_lines(rows))
    lines.extend(
        [
        "## Fastest Implementation By Scenario",
        "",
        "| Scenario | Threads | Implementation | Real time (ns) | Items/s |",
        "| --- | ---: | --- | ---: | ---: |",
        ]
    )

    for row in best_rows(rows):
        lines.append(
            f"| {row.scenario} | {row.threads} | {row.implementation} | "
            f"{row.real_time_ns:.0f} | {row.items_per_second:.3f} |"
        )

    lines.extend(
        [
            "",
            "## Raw Mean Results",
            "",
            "| Scenario | Implementation | Threads | Real time (ns) | CPU time (ns) | Items/s |",
            "| --- | --- | ---: | ---: | ---: | ---: |",
        ]
    )

    for row in rows:
        lines.append(
            f"| {row.scenario} | {row.implementation} | {row.threads} | "
            f"{row.real_time_ns:.0f} | {row.cpu_time_ns:.0f} | {row.items_per_second:.3f} |"
        )

    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def plot_metric(rows: list[Row], scenario: str, metric: str, ylabel: str, out: Path) -> None:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    scenario_rows = [r for r in rows if r.scenario.lower() == scenario.lower()]
    implementations = sorted({r.implementation for r in scenario_rows})

    plt.style.use("seaborn-v0_8-whitegrid")
    colors = plt.get_cmap("tab10")

    fig, ax = plt.subplots(figsize=(11, 6.5), dpi=180)
    for index, implementation in enumerate(implementations):
        series = [r for r in scenario_rows if r.implementation == implementation]
        series.sort(key=lambda r: r.threads)
        x = [r.threads for r in series]
        y = [getattr(r, metric) for r in series]
        ax.plot(
            x,
            y,
            marker="o",
            linewidth=2.4,
            markersize=5,
            color=colors(index % 10),
            label=implementation,
        )

    ax.set_title(f"{scenario} {ylabel}")
    ax.set_xlabel("Threads")
    ax.set_ylabel(ylabel)
    ax.set_xticks(sorted({r.threads for r in scenario_rows}))
    ax.grid(True, which="major", alpha=0.35)
    ax.legend(loc="center left", bbox_to_anchor=(1.02, 0.5), frameon=True)
    if metric == "real_time_ns":
        ax.set_yscale("log")
    fig.tight_layout()
    fig.savefig(out)
    plt.close(fig)


def write_plots(rows: list[Row], out_dir: Path) -> None:
    plots_dir = out_dir / "plots"
    plots_dir.mkdir(parents=True, exist_ok=True)
    for scenario in sorted({row.scenario for row in rows}):
        lower = scenario.lower()
        plot_metric(
            rows,
            scenario,
            "real_time_ns",
            "Real time (ns, log scale)",
            plots_dir / f"{lower}_real_time_ns.png",
        )
        plot_metric(
            rows,
            scenario,
            "items_per_second",
            "Items per second",
            plots_dir / f"{lower}_items_per_second.png",
        )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate benchmark reports and plots.")
    parser.add_argument("benchmark_json", type=Path, help="Google Benchmark JSON file")
    parser.add_argument("--out-dir", type=Path, default=Path("benchmark-results/report"))
    parser.add_argument("--metadata-json", type=Path, default=None)
    parser.add_argument("--no-plots", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    out_dir = args.out_dir.resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    rows = load_rows(args.benchmark_json)
    if not rows:
        raise SystemExit(f"no mean benchmark rows found in {args.benchmark_json}")
    metadata = load_metadata(args.metadata_json, args.benchmark_json)

    write_csv(rows, out_dir / "benchmark.csv")
    write_markdown(rows, metadata, out_dir / "summary.md")
    if not args.no_plots:
        write_plots(rows, out_dir)

    print(f"Wrote CSV: {out_dir / 'benchmark.csv'}")
    print(f"Wrote report: {out_dir / 'summary.md'}")
    if not args.no_plots:
        print(f"Wrote plots: {out_dir / 'plots'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
