# Tools used

| Tool | Part used for | Changes and checks after generation |
| --- | --- | --- |
| Codex (OpenAI) | Draft and expansion of `docs/derivations/D6-analyst-draft.md` | Added total-work equations, workload details, and references for Fenwick trees, the two-tree range query, segment trees, and Euclid's algorithm. Checked the Dima-Ceterchi claims against the paper and kept its timings separate from our predictions. Adi still needs to review and revise the draft before submission. |
| Codex (OpenAI) | Initial GCD benchmark in `bench/` | Generated the harness and build instructions using D3's and D4's interfaces. Adi needs to review the code and understand the measurements before submission. |
| Codex (OpenAI) | GCD result summary and width plot scripts | Added CSV grouping, mean and spread calculation, and plotting instructions. Adi needs to review the scripts and check the figures against the raw data. |
| Codex (OpenAI) | Maximum benchmark and integration | Added a benchmark using the D1, D2, D4 and D5 implementations, with separate timing and counter builds. Adi needs to review the code, confirm the D1 revision with its owner, and interpret the results. |
| Codex (OpenAI), ReportLab | D6 draft PDF and report | Generated the draft derivation from the cost model and summarized the measured CSVs. The report identifies the provisional D1 snapshot and measurement limits. Adi needs to review the derivation and analysis. |
| Codex (OpenAI) | D5 scale benchmark | Added checks for the two deterministic scale workloads, ran five timing repetitions and compared B's counters with D5's documented totals. |

Add each teammate's tools and their edits here as their work is merged.
