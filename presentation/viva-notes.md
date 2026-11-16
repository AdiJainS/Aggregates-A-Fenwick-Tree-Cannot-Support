# D6 speaking notes

Use `D6-slide.pptx` as a draft for your part of the team presentation. The numbers are preliminary until D1 confirms the implementation and D2 provides a timing build without counters. Review the code and derivation yourself before presenting.

## Suggested 3-4 minute explanation

**Problem and model (about 45 seconds)**

Our project compares range maximum structures under point assignments. I modeled the segment tree and the two-tree Fenwick structure with logarithmic query and update work. For the prefix-only baseline, a decrease may force a full rebuild, so a workload with many decreases can include a term proportional to `n` times the number of decreases. The array oracle scans the requested range.

**Benchmark design (about 45 seconds)**

I generate the initial array and operations once from a recorded seed. Each structure receives the same operations. Small runs compare every query with the oracle. I measure construction separately from replay, repeat timing five times, and report the mean and sample standard deviation. I collect operation counters in separate runs. B currently keeps its counters active even during timing, so the A/B comparison has that limitation.

**Example result (about 60 seconds)**

The chart shows the prefix baseline on 4,096 values, 10,000 operations, and a 50% update mix. The three cases request 0%, 50% and 100% decreases among updates. The actual decrease counts were 0, 2,532 and 5,051. Mean replay times were 0.21, 13.80 and 27.69 milliseconds; standard deviations were 0.007, 0.032 and 0.900 milliseconds. The result is consistent with rebuilding after each decrease. All queries in this example are full-array prefix queries, which the baseline supports. These are replay times for the entire workload, not per-operation latency.

**Limits and conclusion (about 30 seconds)**

The full-scale runs use one million values and one million operations, but A comes from an older unconfirmed snapshot. D1 needs to confirm the final version, and D2 needs a counters-off timing build for a fairer comparison. WPR could not start a cache trace without the required privilege, so I have no cache-miss result. I would rerun the final comparisons after integration.

## Viva questions

**Why not subtract two prefix maxima?**
Maximum has no inverse: knowing the maximum of two prefixes does not determine the maximum of the range between them. For example, arrays `[10, 2]` and `[10, 7]` have the same prefix maxima at both positions but different maxima in the second position.

**Why does the prefix tree rebuild after a decrease?**
A stored block maximum may have come from the lowered element. The old maximum alone does not reveal the next largest value, so this baseline recomputes its blocks from the array.

**Why use the same seed?**
The benchmark creates one workload and replays it for each structure. This gives both implementations the same initial values, query ranges and assignments. The seed records how the workload was generated; code revision and compiler details must be recorded too.

**Why check against the oracle before timing?**
A fast answer is useful only if it is correct. The harness checks each result on manageable workloads, outside the timed section. Full-scale timing checksums help detect disagreements but do not prove all results are correct.

**Why can a full-range query be faster than a medium range?**
The segment tree may return a full array from one stored root. A medium range can require several blocks. The actual work depends on the range and its alignment, even when both have the same worst-case bound.

**What do the decrease percentages mean?**
They are percentages of updates, not of all operations. Each selected update changes the current value by one. Random selection means the actual percentage may differ; the CSV records the actual counts.

**Does the result prove one structure has better cache behavior?**
No. Timing and software counters alone do not measure cache misses. The WPR attempt failed for lack of privilege, and a hardware trace still needs to be collected and analyzed.

**Why are the A/B numbers preliminary?**
The D1 branch was reset after the snapshot used for the experiments, and B updates counters during timing while A has its counters disabled. Both can affect the final comparison.

**What do you own?**
The D6 cost model, workload and benchmark harness, measurement and counter scripts, figures, and the comparison in the report. The data structures and oracle belong to the other team roles.
