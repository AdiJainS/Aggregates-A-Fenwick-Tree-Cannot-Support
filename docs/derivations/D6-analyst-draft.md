# D6: predicted costs for Project 5

Adi Jain, Analyst. Draft: 10 November 2026, before benchmark timings.

This is the model I plan to test for the sensor-array project. It describes the work done by each structure, rather than claiming a fixed number of nanoseconds per operation. The constants will depend on the implementation and machine. I will keep this draft as the pre-measurement record and put the measured comparison in `REPORT.md` later.

## Workload and notation

There are `n` array entries and `m` replayed operations. A query covers `w = r - l + 1` entries. Let `Qmax` and `Qgcd` be the numbers of range-maximum and range-gcd queries. Let `U+` and `U-` be the numbers of point assignments that raise and lower a value; `U0` counts unchanged assignments. Thus `m = Qmax + Qgcd + U+ + U- + U0`. Write `L = ceil(log2 n)` and let `V` bound the magnitude of an input value.

I will compare the same operation and the same generated input across implementations. In particular, a prefix-maximum Fenwick tree does **not** answer an arbitrary `[l, r]` maximum by subtracting two prefix maxima. Its prefix-query timings will be reported separately from arbitrary-range timings.

## Predictions by variant

| Variant | Build | Supported query | Point assignment |
| --- | --- | --- | --- |
| Segment tree, `(maximum, count)` | `O(n)` | Range maximum and its count: `O(log n)` | `O(log n)` |
| Segment tree, gcd | `O(n)` | Range gcd: `O(log n)` visited nodes; target `O(log n + log V)` remainder steps with D3's telescoping argument | `O(log n)` visited nodes; conservatively `O(log n log V)` remainder steps |
| Two-tree Fenwick maximum | `O(n)` | Arbitrary range maximum: `O(log(w + 1))` | `O(log n)` amortised, including decreases |
| Prefix-only Fenwick maximum | `O(n)` | Prefix maximum: `O(log n)` | Increase: `O(log n)`; decrease: `O(n)` if rebuilt |
| Plain array | `O(n)` to copy the input | Range maximum/count or gcd: `O(w)` aggregate steps | `O(1)` |

The segment tree combines a bounded number of nodes on each level. A full-array query can be cheaper than a typical range query because the root already stores its answer; the `O(log n)` line is a worst-case bound, not a promise that all widths cost the same. For max/count, each combine is constant work. For gcd, I will count both visited nodes and Euclidean remainder steps: counting each gcd as one unit would hide a value-dependent cost. The simple query bound is `O(log n log V)` remainder steps. D3 is responsible for the tighter `O(log n + log V)` bound; I will state the arithmetic model alongside that proof in the final version.

The two-tree Fenwick query walks intervals from the two ends of the requested range. Dima and Ceterchi analyse this as logarithmic in the range length. Their paper works through **minimum**; reversing the comparison gives maximum. Their update method reuses partial results while climbing the trees, giving amortised `O(log n)` even for an assignment that invalidates a stored extremum. Calling a fresh range query for every affected node would instead give an `O(log^2 n)` update and is not the implementation I am modelling. The number of invalidated cached maxima should matter to the constant factor, so I will record it rather than treating every decrease as equally costly.

The prefix-only baseline is deliberately limited. The increase-only update rule cannot repair a stored maximum after that maximum is lowered. For a correct baseline under arbitrary assignments, I will model rebuilding after each decrease. Its total work is therefore roughly proportional to `n(U- + 1) + L(Qprefix + U+)`, where `Qprefix` counts only valid prefix queries. A scan fallback for a non-prefix range would be a separate method with `O(w)` query work, not a Fenwick range query.

For the plain-array oracle, total range-query work is proportional to the sum of the requested widths, `sum(w_i)`, while assignments remain constant time. This makes it useful for correctness and short ranges, but predicts a steep cost increase for wide ranges.

## What should separate the curves

- Increasing range width should hurt the array scan roughly in proportion to `w`. The two-tree Fenwick query should grow logarithmically. The segment tree stays logarithmic in `n`, but alignment can make individual widths unusually cheap.
- A decrease-heavy workload should punish the rebuilding prefix baseline by about `n` work per decrease. For the two-tree structure, I expect the *number of stored maxima actually invalidated* to explain more than the decrease percentage alone.
- Segment-tree maximum and two-tree Fenwick maximum have the same logarithmic worst-case scale for general ranges. I cannot justify a numeric crossover before seeing their constants. I will test whether the two-tree query's shorter paths on narrow ranges outweigh its update work. A full-range segment-tree query may favour the segment tree.
- Repeated equal maxima need their own max/count checks. A scalar prefix-maximum baseline has no count result, so it cannot be included in a max/count timing comparison without a separately specified extension.

These are predictions, not results. I will not fit the predictions to the benchmark data and then call them pre-measurement predictions.

## Measurements to collect later

Use `n = 10^6` and at least `10^6` replayed operations for the full-scale runs. Vary query width (for example `1`, `16`, `256`, `4096`, `65536`, and `n`) and the query/update mix. Within updates, vary the decrease share and include D5's decreasing-update sequence. Keep the exact workload identical across structures and check answers against the oracle where it finishes in practical time.

For each timed configuration, run at least five repetitions and report the mean and spread. Record the seed for every plotted workload, the compiler and options, processor, and cache sizes. Separate timing runs from instrumented counter runs if the counters change timing noticeably. Useful counters are segment nodes visited and combines; Fenwick nodes visited and cached intervals recomputed; and array elements scanned. A cache explanation needs profiler output, not just a timing curve.

Before benchmarking, D2 and D3 need to settle which interface supplies maximum counts and gcd when the application selects Structure B. The project description names Structure B as a range-maximum structure; I will not silently claim it supports the other outputs.

## References

- CS 253 / MA 253 Lab Project Descriptions, Project 5 and sections 2-6, 2026.
- M. Dima and R. Ceterchi, [Efficient Range Minimum Queries using Binary Indexed Trees](https://ioinformatics.org/oi/pdf/v9_2015_39_44.pdf), *Olympiads in Informatics* 9 (2015), 39-44, especially sections 2.3-2.4.
