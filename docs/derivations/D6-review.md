# D6 derivation review

The current `D6-cost-model-draft.pdf` names Adi Jain and covers the segment tree, two-tree Fenwick, prefix baseline and array oracle. It is a draft: D1's current remote branch has no implementation, and the local D1 snapshot needs the owner's confirmation. B's counters are active in timed runs. The final comparison must be repeated after the team agrees on the release versions.

Before submitting:

- Explain why a segment-tree assignment visits one root path and why a query uses a bounded number of blocks per level.
- For D2's constructor, explain the block-scan cost. At `n = 2^h`, blocks of length `2^k` occur about `n / 2^(k+1)` times at each level, so scanning them contributes about `n/2` values per level. Across `h` levels this gives `O(n log n)` work; this is a model of the committed constructor, not the paper's linear construction.
- Explain why each decrease in the prefix baseline causes a rebuild and why the resulting workload term is proportional to `n` times the number of decreases.
- For GCD, state whether a count means tree blocks or Euclidean remainders. The tighter telescoping proof is D3's derivation; confirm its assumptions with the architect. The planning draft includes the updated build, query and update remainder-step terms.
- D2's `changedBlocks` and `loweredBlocks` count changes in stored values; they are not a count of every operation performed. Do not add those counts to other counters as if the work were disjoint.
- Replace the draft with a final authored PDF after confirming the team code and final results. Keep the original pre-measurement planning commit in the repository history. Do not describe a post-measurement adjustment as an original prediction.
- Review all code and recorded tool use before submission, and be ready to explain your own work.
