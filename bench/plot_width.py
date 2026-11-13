import argparse
import csv
from collections import defaultdict

import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description="Plot mean GCD replay time against query width")
    parser.add_argument("summary", help="CSV produced by summarize.py")
    parser.add_argument("output", help="output image, for example results/gcd-width.png")
    parser.add_argument("--n", required=True, type=int)
    parser.add_argument("--operations", required=True, type=int)
    parser.add_argument("--update-percent", required=True, type=int)
    parser.add_argument("--seed", required=True, type=int)
    args = parser.parse_args()

    series = defaultdict(list)
    with open(args.summary, newline="", encoding="utf-8-sig") as file:
        for row in csv.DictReader(file):
            if (int(row["n"]) == args.n and int(row["operations"]) == args.operations
                    and int(row["update_percent"]) == args.update_percent
                    and int(row["seed"]) == args.seed):
                if not row["replay_sd_ms"]:
                    parser.error("at least two runs are needed to plot standard deviation")
                series[row["structure"]].append((int(row["width"]),
                    float(row["replay_mean_ms"]), float(row["replay_sd_ms"])))

    if not series:
        parser.error("no rows match the selected configuration")
    for name, points in sorted(series.items()):
        points.sort()
        widths, means, deviations = zip(*points)
        plt.errorbar(widths, means, yerr=deviations, marker="o", capsize=3, label=name)
    plt.xscale("log")
    plt.xlabel("Query width (elements)")
    plt.ylabel("Mixed replay time (ms)")
    plt.title(f"GCD: n={args.n}, updates={args.update_percent}%, seed={args.seed}")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(args.output, dpi=160)


if __name__ == "__main__":
    main()
