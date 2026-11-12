#include <iostream>
#include <vector>

#include "generators.hpp"
#include "../src/structB/fenwick_max.hpp"
#include "../src/structB/prefix_fenwick_max.hpp"

long long arrayMax(const std::vector<long long>& values) {
    long long answer = values[0];

    for (long long value : values) {
        if (value > answer) {
            answer = value;
        }
    }

    return answer;
}

int main() {
    const int n = 4;
    const int k = 4;

    Workload workload = makeRebuildWorkload(n, k);

    PrefixFenwickMax baseline(workload.initial);
    PrefixFenwickMax increaseOnly(workload.initial);
    FenwickMax twoTree(workload.initial);

    std::vector<long long> reference = workload.initial;

    int checked = 0;
    int staleAnswers = 0;

    for (const Operation& op : workload.operations) {
        if (op.type == OpType::ASSIGN) {
            reference[op.l] = op.value;

            baseline.assign(op.l, op.value);
            increaseOnly.assignIncreaseOnly(op.l, op.value);
            twoTree.assign(op.l, op.value);
        }
        else if (op.type == OpType::QUERY_MAX) {
            long long expected = arrayMax(reference);
            long long baselineAnswer = baseline.queryPrefix(op.r);
            long long twoTreeAnswer = twoTree.queryMax(op.l, op.r);
            long long stale = increaseOnly.queryPrefix(op.r);

            checked++;

            std::cout << "Expected: " << expected
                      << ", baseline: " << baselineAnswer
                      << ", two-tree: " << twoTreeAnswer
                      << ", increase-only: " << stale << '\n';

            if (baselineAnswer != expected ||
                twoTreeAnswer != expected) {
                std::cerr << "FAIL: incorrect answer\n";
                return 1;
            }

            if (stale != expected) {
                staleAnswers++;
            }
        }
    }

    std::cout << "Queries checked: " << checked << '\n';
    std::cout << "Baseline rebuilds: "
              << baseline.rebuildCount() << '\n';
    std::cout << "Increase-only wrong answers: "
              << staleAnswers << '\n';

    std::cout << "Two-tree assignment steps: "
              << twoTree.assignBlocks << '\n';
    std::cout << "Two-tree lowered blocks: "
              << twoTree.loweredBlocks << '\n';
    std::cout << "Two-tree query steps: "
              << twoTree.queryBlocks << '\n';

    if (checked != k ||
        baseline.rebuildCount() != k ||
        staleAnswers != k) {
        std::cerr << "FAIL: unexpected counts\n";
        return 1;
    }

    std::cout << "PASS\n";
    return 0;
}