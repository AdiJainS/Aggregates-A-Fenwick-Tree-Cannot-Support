import csv
import io
import subprocess
import sys


def main():
    if len(sys.argv) != 2:
        raise SystemExit("Usage: python bench/test_decrease.py PATH_TO_MAX_BENCHMARK")
    exe = sys.argv[1]
    base = [exe, "128", "2000", "64", "80", "253", "2", "random"]
    for percent in (0, 75, 100):
        subprocess.run(base + ["check", str(percent)], check=True, capture_output=True, text=True)
        result = subprocess.run(base + ["time", str(percent)], check=True, capture_output=True, text=True)
        rows = list(csv.DictReader(io.StringIO(result.stdout)))
        assert len(rows) == 4
        assert len({row["checksum"] for row in rows}) == 1
        for row in rows:
            increases = int(row["actual_increases"])
            decreases = int(row["actual_decreases"])
            unchanged = int(row["actual_unchanged"])
            assert increases + decreases + unchanged == int(row["updates"])
            assert unchanged == 0
            if percent == 0:
                assert decreases == 0
            if percent == 100:
                assert increases == 0
    for percent in ("-1", "101"):
        result = subprocess.run(base + ["check", percent], capture_output=True, text=True)
        assert result.returncode != 0
    checksums = []
    for mode in ("time-a", "time-b"):
        result = subprocess.run(base + [mode, "50"], check=True, capture_output=True, text=True)
        rows = list(csv.DictReader(io.StringIO(result.stdout)))
        assert len(rows) == 2
        assert len({row["structure"] for row in rows}) == 1
        checksums.append(rows[0]["checksum"])
    assert checksums[0] == checksums[1]
    print("Decrease workload checks passed")


if __name__ == "__main__":
    main()
