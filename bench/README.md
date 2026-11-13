# GCD benchmark

This first harness replays the same seeded GCD queries and point assignments against D3's segment tree and D4's array oracle. The two source files are on separate branches, so supply their directories when compiling. `gcd_benchmark.cpp` temporarily renames D3's example `main` when including its class. Replace this adapter when D3 exposes a header.

From the repository root, extract the two files into a temporary directory (PowerShell). Use a C++17 compiler with a standard library that implements `std::gcd` (GCC 7 or newer):

```powershell
New-Item -ItemType Directory -Force .bench-deps | Out-Null
git show origin/d3-architect:D3/gcd_segment_tree.cpp | Set-Content .bench-deps/gcd_segment_tree.cpp
git show origin/feature/oracle-d4:oracle/gcd_reference.hpp | Set-Content .bench-deps/gcd_reference.hpp
g++ -std=c++17 -O2 -I .bench-deps bench/gcd_benchmark.cpp -o .bench-deps/gcd_benchmark.exe
```

Run a small correctness check and record five timing runs:

```powershell
New-Item -ItemType Directory -Force results | Out-Null
.bench-deps/gcd_benchmark.exe 1000 10000 64 20 253 5 check | Set-Content results/gcd-smoke.csv
```

Arguments are array size, operation count, fixed query width, update percentage, seed, repetitions, and mode. `check` compares every query with the oracle before timing both structures. `tree` and `oracle` time just the chosen structure, useful when a wide-range oracle run takes too long. All ranges are zero-indexed and half-open. Values are non-negative integers from 0 to 1,000,000. The seed identifies the workload for a given build and standard library; save compiler details too.

CSV contains one row per run, with build and mixed-replay time in milliseconds, actual query/update counts, and a query-result checksum. Compute the mean and spread across the five rows for each configuration. Replay time includes dispatch and checksum work; it is not separate per-query and per-update latency. Workload generation and correctness checks are outside the timed region.

After collecting raw CSV files for several widths, summarize and plot them (plotting requires `matplotlib`):

```powershell
python bench/summarize.py results/gcd-width-1.csv results/gcd-width-64.csv > results/gcd-summary.csv
python bench/plot_width.py results/gcd-summary.csv results/gcd-width.png --n 1000 --operations 10000 --update-percent 20 --seed 253
```

The summary groups runs by structure, workload parameters and seed. Spread is the sample standard deviation; min and max are included as well. With one run, standard deviation is left blank; the assignment requires at least five runs. The plot shows mixed replay time with standard deviation bars; it does not show individual query latency. Do not combine results from different machines or compiler settings in one summary. Keep the raw CSV files and machine details alongside the figures.

The full assignment requires at least one million elements and one million operations. Start with manageable checks, then use `tree` mode for expensive wide ranges. Record any skipped oracle configurations. The harness does not yet measure operation counters, cache behaviour, or maximum queries; those are later D6 work.
