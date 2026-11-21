import argparse
import csv
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description="Plot D5 scale timings")
    parser.add_argument("summary")
    parser.add_argument("output")
    args = parser.parse_args()
    with open(args.summary, newline="", encoding="utf-8-sig") as file:
        rows = list(csv.DictReader(file))
    workloads = ["query", "decrease"]
    structures = ["segment_tree", "fenwick"]
    positions = [0, 1]
    for i, name in enumerate(structures):
        selected = []
        for workload in workloads:
            matching = [row for row in rows if row["structure"] == name and row["workload"] == workload]
            if len(matching) != 1 or not matching[0]["replay_sd_ms"]:
                parser.error(f"Expected one summary row with spread for {name}, {workload}")
            selected.append(matching[0])
        plt.bar([x + (i - 0.5) * 0.35 for x in positions],
                [float(row["replay_mean_ms"]) for row in selected],
                width=0.35, yerr=[float(row["replay_sd_ms"]) for row in selected],
                capsize=3, label=name)
    plt.xticks(positions, ["Difficult queries", "Repeated decreases"])
    plt.ylabel("Mixed replay time (ms)")
    plt.title("D5 workloads: 1,048,576 elements, 1,000,000 operations")
    plt.legend()
    plt.tight_layout()
    plt.savefig(args.output, dpi=160)


if __name__ == "__main__":
    main()
