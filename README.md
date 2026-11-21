# Aggregates a Fenwick Tree Cannot Support

**CS253 / MA253 · Project 5**

This project studies a sensor array that needs range maximum, the number of times that maximum occurs, and range GCD while individual values can increase or decrease. We compare a segment tree with a two-tree Fenwick structure and check their answers against a plain array.

A standard Fenwick tree can obtain a range sum by subtracting two prefix sums. This trick cannot recover an arbitrary range maximum: identical prefix maxima can hide different maxima in the requested range. Structure B stores additional block information; a prefix-only maximum tree is retained as a baseline.

## Team

| Role | Member | Roll number |
| --- | --- | --- |
| D1 · Structure A | Tia Mehra | 250041039 |
| D2 · Structure B | Vishruth Agrawal | 250001078 |
| D3 · Architect | Anushri Maheshwari | 250041005 |
| D4 · Oracle | Sushmita Patil | 250001069 |
| D5 · Adversary | Vanshika Agrawal | 250001076 |
| D6 · Analyst | Adi Jain | 250001083 |

## How the project works

1. **Define the input.** Each sensor has a reading and a calibration value. The replay accepts point assignments and range queries; see [INTERFACE.md](INTERFACE.md).
2. **Build Structure A.** A segment tree stores `(maximum, count)` and GCD aggregates. Assignments update a leaf and its ancestors; queries combine blocks covering the requested range.
3. **Build Structure B.** Two Fenwick-style trees store block maxima in opposite directions, allowing arbitrary range-maximum queries and assignments. The prefix-only baseline rebuilds on a decrease.
4. **Check answers.** A plain-array oracle scans ranges. Differential tests replay the same operations and compare answers, including maximum counts and GCD where supported.
5. **Try difficult inputs.** D5 generates queries with many block touches and repeated decreases of a unique maximum.
6. **Measure.** D6 generates seeded workloads, checks manageable cases, measures construction and replay separately, and collects counters in separate runs. CSVs and plots are in `results/`.
7. **Compare with predictions.** [REPORT.md](REPORT.md) describes the measurements and their limits; `docs/derivations/` contains the mathematical work for each role.

The runnable application in `app/` currently uses the architect's **reference segment-tree backend**. It does not yet select between the D1 and D2 implementations. Structure B implements range maximum; it does not by itself provide maximum counts or GCD. The A/B comparison is run through the separate benchmark harness in `bench/`.

## Requirements

- Git and a C++17 compiler with the C++17 standard library (for example, a recent GCC or Clang). GCC 6.3 lacks `std::gcd`, required by some tests.
- Python 3 for benchmark scripts. Matplotlib is needed only to generate plots: `python -m pip install matplotlib`.
- Windows Performance Recorder and Windows Performance Analyzer are optional, for cache profiling.

Commands below use **Windows PowerShell** from the repository root. On Linux or macOS, use a C++17 compiler and normal shell input redirection; [app/README.md](app/README.md) contains Unix examples.

## 1. Get the project

```powershell
git clone https://github.com/AdiJainS/Aggregates-A-Fenwick-Tree-Cannot-Support.git
cd Aggregates-A-Fenwick-Tree-Cannot-Support
```

Check that `g++ --version` and `python --version` work. Substitute another C++17 compiler if needed.

## 2. Build and run the sensor replay

```powershell
New-Item -ItemType Directory -Force .bench-deps | Out-Null
g++ -std=c++17 -O2 app/gcd_segment_tree.cpp -o .bench-deps/sensor-replay.exe
Get-Content app/sample.in | .\.bench-deps\sensor-replay.exe
```

The output should match [app/sample.out](app/sample.out):

```text
30 2 6
30 1 6
30 1 1
18 1 1
-1 0 0
```

To supply your own input, save it in a text file and run `Get-Content your-input.txt | .\.bench-deps\sensor-replay.exe`. Input format:

```text
n number_of_operations
reading_0 calibration_0
... one pair per sensor ...
Q left right
U index new_reading new_calibration
```

`Q left right` prints `maximum count gcd` over the **zero-based, half-open** range `[left, right)`; the right endpoint is excluded. `U` replaces both values at the given index. An empty range prints `-1 0 0`. See [INTERFACE.md](INTERFACE.md) for input restrictions.

## 3. Run correctness checks

Run the replay's seeded checks:

```powershell
.\.bench-deps\sensor-replay.exe --test
```

Compile and run the D1 query and assignment checks:

```powershell
g++ -std=c++17 -O2 src/structA/test_query.cpp -o .bench-deps/test-a-query.exe
g++ -std=c++17 -O2 src/structA/test_update.cpp -o .bench-deps/test-a-update.exe
.\.bench-deps\test-a-query.exe
.\.bench-deps\test-a-update.exe
```

Other tests are alongside their modules in `src/structA/`, `src/structB/`, `oracle/` and `adversary/`. On a Unix-like shell, `sh app/run_checks.sh` builds the application, checks the sample, runs `--test` and runs the larger `--scale` smoke test. The scale test checks sampled answers, not every full-scale query.

## 4. Build and run the maximum benchmark

```powershell
g++ -std=c++17 -O2 -DSTRUCTB_DISABLE_COUNTERS bench/max_benchmark.cpp -o .bench-deps/max_benchmark.exe
.\.bench-deps\max_benchmark.exe 1000 10000 64 20 253 5 random check
.\.bench-deps\max_benchmark.exe 1000 10000 64 20 253 5 random time
```

Arguments are `N OPERATIONS WIDTH UPDATE_PERCENT SEED RUNS WORKLOAD MODE [DECREASE_PERCENT]`. The first command checks a manageable workload against the oracle; the second prints timing results as CSV. `UPDATE_PERCENT` is the share of all operations that are updates. The optional decrease percentage is the intended share **among updates**.

For a separate counter build, enable A's counters and leave B's counter switch enabled:

```powershell
g++ -std=c++17 -O2 -DSTRUCTA_ENABLE_COUNTERS bench/max_benchmark.cpp -o .bench-deps/max_counts.exe
.\.bench-deps\max_counts.exe 4096 1024 1 0 0 5 rebuild counts
```

The timing build disables counters for both structures; counter runs use a different build. For full-scale random matrices, decrease sweeps, D5 workloads, GCD benchmarks and plots, follow [bench/README.md](bench/README.md). Use a new output folder so earlier results are not overwritten. Full-scale runs take substantially longer than the small example.

## 5. Read the results and derivations

- [REPORT.md](REPORT.md): predicted costs, timing comparisons, adversarial results and cache limitations.
- [results/README.md](results/README.md): machine, compiler, seeds, source revisions and CSV descriptions.
- [20 November results](results/timing-2026-11-20/README.md): counters-off timing runs and reproduction commands.
- [Cache profiling](results/cache/README.md): WPR recording commands and interpretation of sampled events.
- [D5 workloads](adversary/README.md): generators, arguments and verification.
- [TOOLS.md](TOOLS.md): tools and assistance used by the team.

## Repository layout

| Path | Contents |
| --- | --- |
| `src/structA/` | Segment tree, maximum/count and GCD wrappers, and tests |
| `src/structB/` | Two-tree and prefix-only Fenwick maximum implementations and tests |
| `app/` | Sensor replay reference application and sample input/output |
| `oracle/` | Array oracle, operation generator and differential tests |
| `adversary/` | Adversarial workload generators, analysis and checks |
| `bench/` | Benchmark harnesses and plotting/summarizing scripts |
| `results/` | Raw CSVs, figures, profiling summaries and run settings |
| `docs/derivations/` | Individual mathematical derivations |
| `presentation/` | D6 presentation material |

The recorded cache counts are sampled events, not exact miss totals or rates. The traces include setup and construction, so they alone do not establish which structure has better cache behavior. The team should agree on final implementation revisions and rerun affected checks before a release.
