# D5: Adversarial workloads

This module generates deterministic workloads for Project 5,
"Aggregates a Fenwick tree cannot support."

## Files

- `generators.hpp`: initial arrays and operation sequences.
- `test_adversary.cpp`: prefix-baseline failure and rebuild checks,
  plus comparison with the two-tree Fenwick.
- `test_queries.cpp`: difficult-query checks against all ranges
  for small power-of-two sizes.
- `test_decreases.cpp`: repeated decreases and update-position checks.

The code reuses D4's `Operation` type and D2's Fenwick implementations.

## Operation conventions

Indices are zero-based. Query endpoints are inclusive.

An operation has four fields:

    {type, l, r, value}

For assignment, `l` is the updated index and `r` is unused.
For queries, `l` and `r` are the endpoints and `value` is unused.

## Workload 1: repeated prefix-baseline rebuilds

Function:

    makeRebuildWorkload(n, k)

Requires `n >= 1` and `1 <= k <= n`.

The initial array is:

    [n, n-1, ..., 1]

Positions 0 through k-1 are assigned zero, from left to right.
Each assignment is followed by a whole-array maximum query.

Each assignment removes the current unique global maximum.
The increase-only prefix update leaves stale maxima.

D2's correct prefix baseline rebuilds on every strict decrease.
Therefore, k decreases cause k rebuilds. Each rebuild costs
Theta(n), giving Theta(kn) total rebuild work.

This statement concerns the rebuilding baseline, not every
possible Fenwick-based implementation.

The workload contains 2k operations.

## Workload 2: difficult range queries

Function:

    makeQueryWorkload(n, queryCount)

Requires a power-of-two `n >= 4` and `queryCount >= 1`.

The initial array is:

    [1, 2, ..., n]

Every query covers [0, n-2], whose maximum is n-1.

For n = 2^h, the current two-tree query implementation takes:

    (h-1) left steps + (h-1) right steps + 1 final step
    = 2h-1 steps

Exhaustive checks over every range confirmed that this candidate
attains the maximum queryBlocks count for n = 4, 8, ..., 256.

These finite checks support the construction; they do not replace
a general maximality proof.

The workload contains queryCount operations.

## Workload 3: repeated decreases of a unique maximum

Function:

    makeDecreaseWorkload(n, decreases)

Requires a power-of-two `n >= 4` and `decreases >= 1`.

Initially, all values are zero except:

    a[0] = decreases + 1

Index 0 is assigned decreases, decreases-1, ..., 1.
Each assignment is followed by a whole-array maximum query.

Index 0 remains the unique maximum. Every affected stored block
must therefore lower its maximum.

For n = 2^h, each assignment in the current implementation records:

    assignBlocks = 2h + 2
    loweredBlocks = h + 2

For sizes 4 through 256, testing every possible update position
confirmed that index 0 attains both maximum counts.

The workload contains 2 * decreases operations.

## Build and run

Run from the repository root using a C++17 compiler.

Dependencies:

    oracle/operations.hpp
    src/structB/prefix_fenwick_max.hpp
    src/structB/fenwick_max.hpp

Compile:

```powershell
g++ -std=c++17 -O2 adversary/test_adversary.cpp -o adversary/test_adversary.exe
g++ -std=c++17 -O2 adversary/test_queries.cpp -o adversary/test_queries.exe
g++ -std=c++17 -O2 adversary/test_decreases.cpp -o adversary/test_decreases.exe
```

Run on Windows PowerShell:

```powershell
.\adversary\test_adversary.exe
.\adversary\test_queries.exe
.\adversary\test_decreases.exe
```

The query test exhaustively checks small arrays only.
It must not be extended to million-element exhaustive testing.

## Reproducibility and measurements

All three generators are deterministic and use no random seed.
Record the generator name, parameters and code revision when
saving experimental results.

Counters measure implementation steps, not elapsed time.
Timing experiments and figures belong to D6's benchmark harness.

Full-scale replay and integration with the team's correctness
and measurement harnesses remain to be completed.