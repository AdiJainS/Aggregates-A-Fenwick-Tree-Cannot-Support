#include <iostream>

#include "generators.hpp"
#include "../src/structB/fenwick_max.hpp"

int main() {
    for (int n = 4; n <= 256; n *= 2) {
        Workload workload = makeQueryWorkload(n, 1);
        FenwickMax tree(workload.initial);

        const Operation& candidate = workload.operations[0];

        tree.resetCounters();

        long long answer = tree.queryMax(candidate.l, candidate.r);
        long long candidateSteps = tree.queryBlocks;

        if (answer != n - 1) {
            std::cerr << "FAIL: wrong candidate answer\n";
            return 1;
        }

        long long maximumSteps = 0;
        int hardestL = 0;
        int hardestR = 0;

        // Check every valid range for this small array.
        for (int l = 0; l < n; l++) {
            for (int r = l; r < n; r++) {
                tree.resetCounters();

                long long actual = tree.queryMax(l, r);

                // Values are [1, 2, ..., n].
                if (actual != r + 1) {
                    std::cerr << "FAIL: wrong range answer\n";
                    return 1;
                }

                if (tree.queryBlocks > maximumSteps) {
                    maximumSteps = tree.queryBlocks;
                    hardestL = l;
                    hardestR = r;
                }
            }
        }

        std::cout << "n=" << n
                  << ", candidate steps=" << candidateSteps
                  << ", maximum steps=" << maximumSteps
                  << ", hardest range=[" << hardestL
                  << ", " << hardestR << "]\n";

        if (candidateSteps != maximumSteps) {
            std::cerr << "FAIL: candidate is not a maximum-step range\n";
            return 1;
        }
    }

    std::cout << "PASS: candidate maximises steps for all tested sizes\n";
    return 0;
}