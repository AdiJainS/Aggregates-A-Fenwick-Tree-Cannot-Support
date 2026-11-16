# Benchmarks

Build with a C++17 compiler, from the repository root:

```powershell
New-Item -ItemType Directory -Force .bench-deps | Out-Null
g++ -std=c++17 -O2 bench/max_benchmark.cpp -o .bench-deps/max_benchmark.exe
g++ -std=c++17 -O2 -DSTRUCTA_ENABLE_COUNTERS bench/max_benchmark.cpp -o .bench-deps/max_counts.exe
g++ -std=c++17 -O2 bench/gcd_benchmark.cpp -o .bench-deps/gcd_benchmark.exe
```

On this machine GCC 6.3 is too old for `std::gcd`. The local C++17 compiler is `../cpp-toolchain/ziglang/zig.exe`; replace `g++` in the commands with `& ../cpp-toolchain/ziglang/zig.exe c++` when building here. This compiler is not included in the repository.

Run a correctness check before timing. Arguments are `N OPERATIONS WIDTH UPDATE_PERCENT SEED RUNS WORKLOAD MODE`:

```powershell
.\.bench-deps\max_benchmark.exe 1000 10000 64 20 253 5 random check
.\.bench-deps\max_benchmark.exe 4096 1024 1 0 0 5 rebuild check
.\.bench-deps\max_counts.exe 4096 1024 1 0 0 5 rebuild counts
```

Workloads are `random`, `rebuild`, `decrease` and `query`. The last three use D5's generators. For D5, `OPERATIONS` is the generator parameter: number of decreases or queries. The generator may produce more operations than that parameter. Its query endpoints are inclusive; the A adapter changes them to half-open ranges. The prefix-only baseline is checked and measured only when all queries start at index 0.

`check` compares every maximum query with D4's oracle, including A's count. `time` measures the max-only A tree and B. `prefix` times the baseline separately; it rejects non-prefix queries. `counts` requires the counter build. B always updates its counters, including during timing; A's counters are disabled for timing. This is a limitation of the current B implementation.

For D5's full-scale query and decrease cases, `check-scale` checks every result against the known expected maximum and count. `counts-ab` collects A and B counters without running the prefix baseline, whose repeated full rebuilds are too expensive at this size:

```powershell
.\.bench-deps\max_benchmark.exe 1048576 1000000 1 0 0 5 query check-scale
.\.bench-deps\max_benchmark.exe 1048576 500000 1 0 0 5 decrease check-scale
.\.bench-deps\max_benchmark.exe 1048576 500000 1 0 0 5 decrease time
.\.bench-deps\max_counts.exe 1048576 500000 1 0 0 5 decrease counts-ab
python bench/plot_d5.py results/d5-scale/summary.csv results/d5-scale/timings.png
```

The decrease generator creates one query after each assignment, so its parameter of 500,000 produces one million operations. `check-scale` is specific to these two D5 generators; the general `check` mode uses D4's array oracle and is best kept to manageable sizes.

Run the full-scale random matrix with five repetitions:

```powershell
python bench/run_matrix.py .bench-deps/max_benchmark.exe results
python bench/summarize.py (Get-ChildItem results/max-n*.csv | ForEach-Object { $_.FullName }) | Set-Content -Encoding utf8 results/max-summary.csv
python bench/plot_width.py results/max-summary.csv results/max-width-u10.png --n 1000000 --operations 1000000 --update-percent 10 --seed 253 --workload random
python bench/run_matrix.py .bench-deps/gcd_benchmark.exe results --metric gcd
```

The matrix uses widths 1, 1024 and 1000000 and update shares 10, 50 and 90 percent. The `run_matrix.py --help` options change the sizes and seed. For GCD, it checks and times the oracle at width 1 and runs only the tree for wider queries. CSV timings include build and mixed replay separately. Summary spread is sample standard deviation. D5 is deterministic; its CSV uses seed 0 and -1 for random-workload fields that do not apply.

GCD benchmark arguments are `N OPERATIONS WIDTH UPDATE_PERCENT SEED RUNS MODE`, where mode is `check`, `tree` or `oracle`:

```powershell
.\.bench-deps\gcd_benchmark.exe 1000 10000 64 20 253 5 check
```

The GCD benchmark uses D1's GCD tree and D4's half-open GCD reference. The maximum benchmark checks D4's general oracle, which uses inclusive ranges. Neither benchmark measures cache misses. Store profiler output before drawing cache conclusions.

`src/structA`, `src/structB` and `adversary` contain snapshots of teammates' code used for these experiments. Their revisions and the important D1 branch reset are recorded in `results/README.md`. Replace them with the team's final versions before the final release, then rerun the checks and measurements.
