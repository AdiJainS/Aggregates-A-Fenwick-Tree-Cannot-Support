#pragma once

#include <stdexcept>
#include <vector>

#include "../oracle/operations.hpp"

struct Workload {
    std::vector<long long> initial;
    std::vector<Operation> operations;
};

// Generate k decreases, each followed by a whole-array maximum query.
// Requires n >= 1 and 1 <= k <= n.
inline Workload makeRebuildWorkload(int n, int k) {
    if (n < 1 || k < 1 || k > n) {
        throw std::invalid_argument("Require n >= 1 and 1 <= k <= n");
    }

    Workload workload;

    for (int i = 0; i < n; i++) {
        workload.initial.push_back(n - i);
    }

    for (int i = 0; i < k; i++) {
        workload.operations.push_back({
            OpType::ASSIGN, i, 0, 0
        });

        workload.operations.push_back({
            OpType::QUERY_MAX, 0, n - 1, 0
        });
    }

    return workload;
}