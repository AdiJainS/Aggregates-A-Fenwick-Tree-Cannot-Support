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

## D4 — Oracle Owner

**Tool Used:** ChatGPT (OpenAI)

### Purpose of Use

ChatGPT was used as a learning and development aid to understand the concepts of oracle-based testing, differential testing, invariant checking, reproducible random testing, and failure minimization.

It was also used to clarify the mathematical reasoning behind why prefix differences work for sums but not for range maximum queries.

### Generated Material

AI assistance included initial C++ code suggestions and drafts for the array-based oracle, random operation generator, differential testing framework, failure logging, failure shrinking, and test integration. It also helped organize and explain the D4 mathematical derivation.

### Work and Verification

The D4 module was organized and integrated into the team repository. The suggested implementations were compiled and tested locally, with compatibility adjustments made for the different data structure interfaces.

The following tests were executed:

- **Structure A:** 100,000 operations, with no reported mismatches and invariant checks passing.
- **Structure B:** 1,000,000 operations, with no reported mismatches and final internal block consistency checks passing.
- **Failure minimization:** A deliberately injected fault was detected, and a five-operation sequence was reduced to two operations.

The code and proof were reviewed as part of the D4 development and testing process.

### Modifications

The initial suggestions were organized into separate project files and adapted to the team's indexing conventions, query interfaces, and test requirements. Any additional implementation changes are reflected in the Git commit history.

### Verification Responsibility

All reported test results were obtained through local compilation and execution. The final D4 implementation and its correctness testing are included in the project repository.

## D5 — Adversarial workloads

Tool: ChatGPT / Codex

Used for:
- Explaining the project requirements and prefix-maximum limitation.
- Drafting C++ adversarial generators and verification programs.
- Deriving expected operation counters.
- Assisted Drafting the D5 proof and supporting documentation.

Student work:
- Manually added the code to the local VS Code project.
- Compiled and ran the programs.
- Verified the reported small-size and full-scale results.
- Exported the derivation to PDF.
- Reviewed and simplified the code and explanations.

Changes after generation:
- Selected a simpler runner using a source-code workload selector.
- Simplified output and retained answer and counter checks.
