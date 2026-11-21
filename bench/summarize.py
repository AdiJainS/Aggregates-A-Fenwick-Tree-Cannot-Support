import argparse
import csv
import statistics
import sys
from collections import defaultdict


def main():
    parser = argparse.ArgumentParser(description="Summarize benchmark runs")
    parser.add_argument("input", nargs="+", help="raw benchmark CSV files")
    args = parser.parse_args()

    groups = defaultdict(list)
    keys = ("structure", "n", "operations", "width", "update_percent", "seed", "workload", "decrease_percent")
    for path in args.input:
        with open(path, newline="", encoding="utf-8-sig") as file:
            for row in csv.DictReader(file):
                key = tuple(row.get(name, "") for name in keys)
                groups[key].append(row)

    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow((*keys, "actual_increases", "actual_decreases", "actual_unchanged", "runs", "build_mean_ms", "build_sd_ms",
                     "replay_mean_ms", "replay_sd_ms", "replay_min_ms", "replay_max_ms"))
    for key, rows in sorted(groups.items()):
        if len({row["checksum"] for row in rows}) != 1:
            raise ValueError(f"inconsistent checksums for {key}")
        for field in ("actual_increases", "actual_decreases", "actual_unchanged"):
            if len({row.get(field, "") for row in rows}) != 1:
                raise ValueError(f"inconsistent {field} for {key}")
        build = [float(row["build_ms"]) for row in rows]
        replay = [float(row["replay_ms"]) for row in rows]
        def sd(values):
            if len(values) < 2:
                return ""
            return f"{statistics.stdev(values):.6f}"
        writer.writerow((*key, rows[0].get("actual_increases", ""), rows[0].get("actual_decreases", ""),
                         rows[0].get("actual_unchanged", ""), len(rows), f"{statistics.mean(build):.6f}",
                         sd(build), f"{statistics.mean(replay):.6f}",
                         sd(replay), f"{min(replay):.6f}", f"{max(replay):.6f}"))


if __name__ == "__main__":
    main()
