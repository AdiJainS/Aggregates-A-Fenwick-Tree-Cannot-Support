import csv
import io
import subprocess
import sys


def main():
    if len(sys.argv) != 3:
        raise SystemExit("Usage: python bench/test_gcd_counts.py TIMING_EXE COUNTER_EXE")
    normal, counts = sys.argv[1:]
    for width, update in ((1, 0), (64, 20), (1000, 100)):
        args = ["1000", "10000", str(width), str(update), "253", "1"]
        timing = subprocess.run([normal, *args, "check"], capture_output=True, text=True, check=True)
        counted = subprocess.run([counts, *args, "counts"], capture_output=True, text=True, check=True)
        timing_rows = list(csv.DictReader(io.StringIO(timing.stdout)))
        row = next(csv.DictReader(io.StringIO(counted.stdout)))
        assert timing_rows[0]["checksum"] == row["checksum"]
        assert int(row["queries"]) + int(row["updates"]) == 10000
        assert int(row["oracle_elements_if_scanned"]) == int(row["queries"]) * width
        if update == 0:
            assert int(row["update_visits"]) == 0
        if update == 100:
            assert int(row["query_blocks"]) == 0
    args = ["1000", "100", "64", "20", "253", "1"]
    assert subprocess.run([normal, *args, "counts"], capture_output=True).returncode != 0
    assert subprocess.run([counts, *args, "tree"], capture_output=True).returncode != 0
    print("GCD counter checks passed")


if __name__ == "__main__":
    main()
