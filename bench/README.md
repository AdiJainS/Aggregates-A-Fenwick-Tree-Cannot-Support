# GCD benchmark

The benchmark runs the same GCD queries and point updates on D3's segment tree and D4's array oracle. It includes `oracle/gcd_reference.hpp` directly. D3's code is on a separate branch, and the benchmark temporarily renames its example `main`. Replace that adapter when D3 provides a header.

From the repository root, extract D3's file into a temporary directory (PowerShell). Use a C++17 compiler with a standard library that implements `std::gcd` (GCC 7 or newer):

```powershell
New-Item -ItemType Directory -Force .bench-deps | Out-Null
git show origin/d3-architect:D3/gcd_segment_tree.cpp | Set-Content .bench-deps/gcd_segment_tree.cpp
g++ -std=c++17 -O2 -I .bench-deps bench/gcd_benchmark.cpp -o .bench-deps/gcd_benchmark.exe
```

Run a small correctness check and record five timing runs:

```powershell
New-Item -ItemType Directory -Force results | Out-Null
.bench-deps/gcd_benchmark.exe 1000 10000 64 20 253 5 check | Set-Content results/gcd-smoke.csv
```

Arguments are array size, operation count, fixed query width, update percentage, seed, repetitions, and mode. `check` compares every query with `GcdReference` before timing both structures. `tree` and `oracle` time just the chosen structure, useful when a wide-range oracle run takes too long. GCD benchmark ranges are zero-indexed and half-open. D4's separate general `Oracle` and its generator use inclusive right endpoints; an adapter must convert those endpoints before using them with this benchmark. Values are non-negative integers from 0 to 1,000,000. The seed identifies the workload for a given build and standard library; save compiler details too.

The CSV has one row per run, with build time, replay time, query and update counts, and a checksum. Replay time includes dispatch and checksum work. Workload generation and correctness checks are outside the timed region.

After collecting raw CSV files for several widths, summarize and plot them (plotting requires `matplotlib`):

```powershell
python bench/summarize.py results/gcd-width-1.csv results/gcd-width-64.csv > results/gcd-summary.csv
python bench/plot_width.py results/gcd-summary.csv results/gcd-width.png --n 1000 --operations 10000 --update-percent 20 --seed 253
```

The summary groups runs by structure, workload and seed. It reports the mean, sample standard deviation, minimum and maximum. Standard deviation is blank for a single run; the assignment requires at least five. The graph shows mixed replay time with standard deviation bars. Keep raw CSVs and machine details with the figures, and don't combine runs from different machines or compiler settings.

The full assignment requires at least one million elements and one million operations. Start with manageable checks, then use `tree` mode for expensive wide ranges. Record any skipped oracle configurations. The harness does not yet measure operation counters, cache behaviour, or maximum queries; those are later D6 work.
