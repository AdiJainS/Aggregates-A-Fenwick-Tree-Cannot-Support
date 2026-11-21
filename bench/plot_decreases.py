import argparse
import csv
from collections import defaultdict
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description="Plot time against the decrease percentage")
    parser.add_argument("summary")
    parser.add_argument("output")
    parser.add_argument("--n", type=int, required=True)
    parser.add_argument("--operations", type=int, required=True)
    parser.add_argument("--width", type=int, required=True)
    parser.add_argument("--update-percent", type=int, required=True)
    parser.add_argument("--seed", type=int, required=True)
    parser.add_argument("--log-y", action="store_true")
    args = parser.parse_args()
    series = defaultdict(list)
    with open(args.summary, newline="", encoding="utf-8-sig") as file:
        for row in csv.DictReader(file):
            if (int(row["n"]) == args.n and int(row["operations"]) == args.operations and int(row["width"]) == args.width
                    and int(row["update_percent"]) == args.update_percent
                    and int(row["seed"]) == args.seed and row["workload"] == "random"
                    and row["decrease_percent"] not in ("", "-1")):
                series[row["structure"]].append((int(row["decrease_percent"]),
                    float(row["replay_mean_ms"]), float(row["replay_sd_ms"])))
    if not series:
        parser.error("No matching runs")
    for name, points in sorted(series.items()):
        points.sort()
        percent, mean, spread = zip(*points)
        plt.errorbar(percent, mean, yerr=spread, marker="o", capsize=3, label=name)
    plt.xlabel("Requested decreases (% of updates)")
    plt.ylabel("Mixed replay time (ms)")
    if args.log_y:
        plt.yscale("log")
    plt.title(f"n={args.n}, operations={args.operations}, width={args.width}\nupdates={args.update_percent}%, seed={args.seed}")
    plt.xticks([0, 50, 100])
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(args.output, dpi=160)


if __name__ == "__main__":
    main()
