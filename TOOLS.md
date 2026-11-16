# TOOLS.md — Tools and AI Usage Declaration

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