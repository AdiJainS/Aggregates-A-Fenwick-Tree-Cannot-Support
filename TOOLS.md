# TOOLS.md

## D6: Analyst

| Tool | Work |
| --- | --- |
| Codex (OpenAI) | Expanded the cost estimates and references. |
| Codex (OpenAI) | Helped create the GCD benchmark and build instructions. |
| Codex (OpenAI) | Helped draft the cost model and derivation. |
| Codex (OpenAI) | Helped with the maximum benchmark, integration, result summaries and plots. |
| Codex (OpenAI) | Helped set up cache profiling and draft the presentation. |
| ReportLab (Python library) | Generated the derivation PDF. |
| Windows Performance Recorder | Collected the cache profiling traces. |
| Matplotlib (Python library) | Created benchmark graphs. |
| Tracerpt | Exported the profiling traces for analysis. |

20 November update: Codex helped add the D6 counters-off build, rerun the maximum benchmarks and audit the newer WPR trace counts. The timing and counter builds were checked against the oracle and D5 expected answers; tracerpt exports were filtered by benchmark thread lifetimes. Earlier measurements were preserved.

## D2: Structure B owner (Vishruth Agrawal, 250001078)

**Tool used:** Claude (Anthropic), and ChatGPT (OpenAI)

### Verification

Every test result for Structure B comes from compiling and running the tests locally. I can explain the code and the D2 proof, and I checked the proof steps against the running example in the derivation.
