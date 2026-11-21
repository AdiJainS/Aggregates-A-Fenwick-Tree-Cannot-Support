# Interface and invariants

Project 5, Architect (D3, Anushri Maheshwari). The first part lists the common interface of the structures as the code in `src/` and `include/` implements it today. The second part is the architect's contract for the sensor-replay application in `app/`.

## Part 1: Common structure interface

All structures are built from an array of `long long` values, support point assignment, and answer range queries on a zero-based index.

| Operation | Structure A (`StructureA`, `src/structA/`) | Structure B (`FenwickMax`, `src/structB/`) | Baseline (`PrefixFenwickMax`, `src/structB/`) |
| --- | --- | --- | --- |
| Build | `StructureA(values)` | `FenwickMax(values)` | `PrefixFenwickMax(values)` |
| Assign | `assign(index, value)` | `assign(index, value)` | rebuilds the tree on a decrease |
| Maximum query | `queryMax(l, r)` returns maximum and count | `queryMax(l, r)` returns the maximum only | prefix queries only |
| Gcd query | `queryGCD(l, r)` | not supported | not supported |
| Query range | half-open `[l, r)` | inclusive `[l, r]` | prefix `[0, r]` |

Shared types live in `include/`. `include/operations.hpp` defines `Operation` (`ASSIGN`, `QUERY_MAX`, `QUERY_GCD`, with fields `l`, `r`, `value`), which the oracle, the generators and the benchmarks all use. Its `l` and `r` are inclusive for the maximum oracle. The gcd reference in `oracle/gcd_reference.hpp` uses half-open ranges like Structure A.

The two range conventions differ. The benchmark harness converts between them at the boundary (see `bench/max_benchmark.cpp`). Any new code that calls both structures must do the same.

Shared invariants, each checked in `tests/` or `oracle/`:

1. After any sequence of assignments, every query returns what a plain array scan returns (`oracle/differential_test.cpp`, `oracle/test_real_d1.cpp`, `oracle/test_scale_d2.cpp`).
2. Structure A: each internal node equals the merge of its two children; padding leaves hold the identity (`tests/structA/test_invariants.cpp`).
3. Structure B: each stored block value equals the true maximum of its block after every assignment, including a decrease. A fast `assign` leaves the same blocks as a fresh build (`tests/structB/test_blocks.cpp`).
4. Structure B: one query visits at most `2*floor(log2 n)+1` blocks and one assignment takes at most `4*(floor(log2 n)+1)` steps (`tests/structB/test_counters.cpp`).
5. The prefix baseline returns correct prefix maxima after a decrease only because it rebuilds (`tests/adversary/test_adversary.cpp`).

## Part 2: Sensor replay application contract

Author: Anushri Maheshwari. Project 5, D3 Architect.

This contract describes the current implementation and integration boundary.

## Input and output

Use zero-based half-open ranges [left,right). Require 0 <= left <= right <= n. n is between 0 and 1,000,000. Operation count is a nonnegative int. All supplied values are integers in [0,1,000,000,000]. Bad input prints an error to stderr and exits with status 1; output already produced is not rolled back. Extra tokens after the declared operations are ignored.

First line: `n operations`. Then n pairs: `reading calibration`. Reading and calibration are separate fields at the same sensor index.

- `U index reading calibration`: replace both fields at one valid sensor index. There is no output.
- `Q left right`: print `maximum count gcd` for [left,right).

Empty queries print `-1 0 0`. Since readings are nonnegative, -1 represents the mathematical maximum identity minus infinity on this restricted domain. Zero is a valid reading and must have a positive count in a nonempty all-zero range. Calibration gcd is always nonnegative; gcd(0,0)=0.

C++ boundary: `SensorReplay(vector<Sensor>)`, `assign(int,Sensor)`, and `Answer query(int,int) const`. Constructor input must already satisfy the stated size/value contract; stdin parsing checks it before constructing. Public assign/query methods check their arguments. Answer fields are `maximum`, `count`, and `gcd`.

## Backend integration

The included templated SegmentTree is a standalone reference backend for the Architect application. It does not replace the Structure A owner's final module. Its contract is a value type, associative merge, identity, constructor from leaves, assignment, and ordered half-open query. Replace this backend with the team's Structure A API through an adapter.

Structure B supports range maximum; the project's B specification does not promise count-of-maximum or gcd. A B adapter may supply maximum while A continues supplying count and gcd, but label this hybrid clearly and measure B's maximum-only cost separately. No B implementation or adapter is claimed in this snapshot. Do not silently present this application as a completed A-versus-B comparison.

## Structural invariants

1. Reading leaf i equals {current reading,1}; calibration leaf i equals current calibration.
2. Padding leaves equal their monoid identities: {-1,0} and 0.
3. Every parent equals merge(left child,right child).
4. A maximum merge selects the larger value; equal maxima add their counts. A gcd merge computes Euclid's gcd.
5. Query blocks are disjoint and cover exactly the requested interval. The two accumulators preserve left-to-right order, including for noncommutative monoids.
6. An assignment updates both leaves and their ancestor paths before another command runs.

The public replay interface preserves these conditions for increases, decreases, repeated assignments, and assignments to zero. The generic tree itself assumes valid indices supplied by its adapter.

## Complexity and model

Storage is O(n). Maximum queries and updates cost O(log(n+1)). Gcd queries cost O(log(n+1) + log(V+1)) under a unit-cost remainder word RAM. Two query accumulators each have a telescoping gcd chain; their final merge adds at most O(1+log(V+1)), preserving the additive bound. A safe gcd update bound is O((1+log(n+1))(1+log(V+1))). Arithmetic uses unsigned long long, guaranteed at least 64 bits; values are restricted to 1e9. Indices/counts use int and the stated n limit ensures no tree-size or count overflow on an implementation with at least 32-bit int.
