#pragma once

#include <stdexcept>
#include <vector>

#include "../oracle/operations.hpp"

struct Workload {
    std::vector<long long> initial;
    std::vector<Operation> operations;
};

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

inline Workload makeQueryWorkload(int n, int queryCount) {
    if (n < 4 || (n & (n - 1)) != 0 || queryCount < 1) {
        throw std::invalid_argument(
            "Require power-of-two n >= 4 and queryCount >= 1"
        );
    }

    Workload workload;

    for (int i = 0; i < n; i++) {
        workload.initial.push_back(i + 1);
    }

    for (int i = 0; i < queryCount; i++) {
        workload.operations.push_back({
            OpType::QUERY_MAX, 0, n - 2, 0
        });
    }

    return workload;
}

inline Workload makeDecreaseWorkload(int n, int decreases) {
    if (n < 4 || (n & (n - 1)) != 0 || decreases < 1) {
        throw std::invalid_argument(
            "Require power-of-two n >= 4 and decreases >= 1"
        );
    }

    Workload workload;

    workload.initial.assign(n, 0);
    workload.initial[0] = static_cast<long long>(decreases) + 1;

    for (int i = 0; i < decreases; i++) {
        long long value = static_cast<long long>(decreases) - i;

        workload.operations.push_back({
            OpType::ASSIGN, 0, 0, value
        });

        workload.operations.push_back({
            OpType::QUERY_MAX, 0, n - 1, 0
        });
    }

    return workload;
}