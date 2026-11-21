import argparse
import csv
import hashlib
import sys


def events(path):
    with open(path, newline="", encoding="cp1252", errors="replace") as file:
        for row in csv.reader(file, skipinitialspace=True):
            yield row


def main():
    parser = argparse.ArgumentParser(description="Count CacheMisses samples from a tracerpt CSV")
    parser.add_argument("csv_file")
    parser.add_argument("trace_file")
    parser.add_argument("--process", default="max_benchmark_cache_current.exe")
    args = parser.parse_args()
    processes = {}
    for row in events(args.csv_file):
        if len(row) > 27 and row[0] == "Process" and row[27] == args.process:
            pid = int(row[20], 16)
            if row[1] == "Start":
                processes[pid] = [int(row[16]), None]
            elif row[1] == "End" and pid in processes:
                processes[pid][1] = int(row[16])
    if len(processes) != 1:
        raise ValueError("Expected one benchmark process per trace")
    pid = list(processes)[0]
    start, end = processes[pid]
    if end is None:
        raise ValueError("Missing process end event")
    threads = {}
    for row in events(args.csv_file):
        if len(row) > 20 and row[0] == "Thread" and row[1] in ("Start", "End", "DCStart", "DCEnd"):
            stamp = int(row[16])
            if int(row[19], 16) == pid and start <= stamp <= end:
                tid = int(row[20], 16)
                if row[1] in ("Start", "DCStart"):
                    threads.setdefault(tid, []).append([stamp, end])
                elif tid in threads:
                    threads[tid][-1][1] = stamp
    raw_count = 0
    count = 0
    for row in events(args.csv_file):
        if len(row) > 21 and row[1] == "PmcCounterProf" and row[21] == "10":
            tid = int(row[20])
            stamp = int(row[16])
            if tid in threads:
                raw_count += 1
                for thread_start, thread_end in threads[tid]:
                    if thread_start <= stamp <= thread_end:
                        count += 1
                        break
    with open(args.trace_file, "rb") as file:
        trace_hash = hashlib.file_digest(file, "sha256").hexdigest()
    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow(["trace", "sha256", "process", "pid", "source_id", "sample_interval", "process_seconds", "thread_id_samples", "lifetime_samples", "outside_lifetime"])
    writer.writerow([args.trace_file, trace_hash, args.process, pid, 10, 65536, f"{(end - start) / 10000000:.6f}", raw_count, count, raw_count - count])


if __name__ == "__main__":
    main()
