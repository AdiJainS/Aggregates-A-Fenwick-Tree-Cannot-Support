# D3 Architect: sensor replay

Author: Anushri Maheshwari

Build and run from the repository root:

```sh
c++ -std=c++17 -O2 -Wall -Wextra -pedantic D3/gcd_segment_tree.cpp -o /tmp/sensor-replay
/tmp/sensor-replay < D3/sample.in
```

Run all local checks in one command:

```sh
sh D3/run_checks.sh
```

Queries use zero-based [left,right): left included, right excluded. Input begins with n and the operation count, followed by n reading/calibration pairs. Use `U index reading calibration` to replace a sensor and `Q left right` to print maximum, its count, and the calibration gcd. An empty query prints `-1 0 0`. See the root INTERFACE.md for the full contract.

`--test` performs one million seeded small-array operations, with plain-array query comparisons, internal parent checks, deterministic limits and boundary cases, and rejection of invalid API arguments. Seed: 253. The seed generator uses unsigned long long wrapping arithmetic; exact generated values assume the usual 64-bit unsigned long long implementation.

`--scale` runs one million operations on one million sensors (500,000 updates and 500,000 queries), prints a CSV checksum, and validates 50 sampled queries using an array scan. This is a scale smoke test, not a complete full-scale differential test. The checksum wraps modulo the unsigned type's range.

Diagnostic counters on stderr include construction, tree operations, and small-test invariant audits; oracle arithmetic is excluded. They are not isolated benchmark timings. Five-run timing means/spread, cache profiling, the Structure B comparison, failure shrinking, adversarial generators, and the final tagged team release remain separate team deliverables.

The generic segment tree is the Architect application's runnable reference backend. Coordinate its replacement with Structure A's owner. A Structure B maximum-only adapter is not implemented here. The D3 proof is in docs/derivations/D3_Anushri_Maheshwari.pdf.
