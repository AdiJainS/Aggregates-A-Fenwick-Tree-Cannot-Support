import csv
import io
import pathlib
import subprocess
import sys


def run(command):
    subprocess.run(command, check=True)


def main():
    if len(sys.argv) != 3:
        raise SystemExit("Usage: python bench/run_decrease_study.py BENCHMARK_EXE NEW_OUTPUT_FOLDER")
    exe = sys.argv[1]
    folder = pathlib.Path(sys.argv[2])
    folder.mkdir(parents=True, exist_ok=False)
    prefix = folder / "prefix"
    prefix.mkdir()
    run([sys.executable, "bench/test_decrease.py", exe])
    for percent in (0, 50, 100):
        run([sys.executable, "bench/run_matrix.py", exe, str(folder), "--decrease-percent", str(percent)])
        base = [exe, "4096", "10000", "4096", "50", "253", "5", "random"]
        run(base + ["check", str(percent)])
        prefix_checksums = []
        for mode in ("time", "prefix"):
            result = subprocess.run(base + [mode, str(percent)], check=True, capture_output=True, text=True)
            rows = list(csv.DictReader(io.StringIO(result.stdout)))
            if len(rows) != (10 if mode == "time" else 5):
                raise ValueError(f"Unexpected row count for {mode} at {percent}%")
            for row in rows:
                prefix_checksums.append(row["checksum"])
            (prefix / f"d{percent}-{mode}.csv").write_text(result.stdout, encoding="utf-8")
        if len(set(prefix_checksums)) != 1:
            raise ValueError(f"Prefix workload checksums differ at {percent}%")
    raw = sorted(folder.glob("max-*.csv"))
    if len(raw) != 27:
        raise ValueError("Expected 27 full-scale configurations")
    for path in raw:
        with path.open(newline="", encoding="utf-8") as file:
            rows = list(csv.DictReader(file))
        if len(rows) != 10 or len({row["checksum"] for row in rows}) != 1:
            raise ValueError(f"Unexpected results in {path}")
    summary = subprocess.run([sys.executable, "bench/summarize.py", *map(str, raw)], check=True, capture_output=True, text=True)
    (folder / "summary.csv").write_text(summary.stdout, encoding="utf-8")
    raw_prefix = sorted(prefix.glob("*.csv"))
    summary = subprocess.run([sys.executable, "bench/summarize.py", *map(str, raw_prefix)], check=True, capture_output=True, text=True)
    (folder / "prefix-summary.csv").write_text(summary.stdout, encoding="utf-8")
    run([sys.executable, "bench/plot_decreases.py", str(folder / "summary.csv"), str(folder / "width1024-updates50.png"), "--n", "1000000", "--operations", "1000000", "--width", "1024", "--update-percent", "50", "--seed", "253"])
    run([sys.executable, "bench/plot_decreases.py", str(folder / "prefix-summary.csv"), str(folder / "prefix-decreases.png"), "--n", "4096", "--operations", "10000", "--width", "4096", "--update-percent", "50", "--seed", "253", "--log-y"])


if __name__ == "__main__":
    main()
