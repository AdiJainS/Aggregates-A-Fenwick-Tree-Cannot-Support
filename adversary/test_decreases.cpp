#include <iostream>
#include <vector>

#include "generators.hpp"
#include "../src/structB/fenwick_max.hpp"

int main() {
    for (int n = 4; n <= 256; n *= 2) {
        Workload workload = makeDecreaseWorkload(n, 10);
        FenwickMax tree(workload.initial);

        long long candidateSteps = 0;
        long long candidateLowered = 0;

        // Replay repeated decreases and verify every query.
        long long expected = workload.initial[0];

        for (const Operation& op : workload.operations) {
            if (op.type == OpType::ASSIGN) {
                tree.resetCounters();
                tree.assign(op.l, op.value);

                expected = op.value;

                if (candidateSteps == 0) {
                    candidateSteps = tree.assignBlocks;
                    candidateLowered = tree.loweredBlocks;
                }

                if (tree.assignBlocks != candidateSteps ||
                    tree.loweredBlocks != candidateLowered) {
                    std::cerr << "FAIL: repeated decrease counts changed\n";
                    return 1;
                }
            }
            else if (op.type == OpType::QUERY_MAX) {
                if (tree.queryMax(op.l, op.r) != expected) {
                    std::cerr << "FAIL: wrong maximum after decrease\n";
                    return 1;
                }
            }
        }

        long long maximumSteps = 0;
        long long maximumLowered = 0;

        // Try every update position, each with its own unique maximum.
        for (int index = 0; index < n; index++) {
            std::vector<long long> values(n, 0);
            values[index] = 2;

            FenwickMax candidate(values);
            candidate.assign(index, 1);

            if (candidate.assignBlocks > maximumSteps) {
                maximumSteps = candidate.assignBlocks;
            }

            if (candidate.loweredBlocks > maximumLowered) {
                maximumLowered = candidate.loweredBlocks;
            }

            if (candidate.queryMax(0, n - 1) != 1) {
                std::cerr << "FAIL: wrong maximum in position check\n";
                return 1;
            }
        }

        std::cout << "n=" << n
                  << ", steps=" << candidateSteps
                  << ", maximum steps=" << maximumSteps
                  << ", lowered=" << candidateLowered
                  << ", maximum lowered=" << maximumLowered
                  << '\n';

        if (candidateSteps != maximumSteps ||
            candidateLowered != maximumLowered) {
            std::cerr << "FAIL: index 0 did not reach both maxima\n";
            return 1;
        }
    }

    std::cout << "PASS: repeated decreases reach both tested maxima\n";
    return 0;
}