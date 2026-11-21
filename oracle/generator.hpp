
#pragma once

#include "operations.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

inline std::vector<Operation> generateOperations(
    int n,
    int totalOps,
    unsigned long long seed
) {
    if (n <= 0 || totalOps < 0) {
        throw std::invalid_argument("Invalid generator size");
    }

    std::mt19937_64 rng(seed);

    std::uniform_int_distribution<int> indexDist(0, n - 1);
    std::uniform_int_distribution<int> typeDist(0, 2);
    std::uniform_int_distribution<long long> valueDist(0, 1000000);

    std::vector<Operation> operations;
    operations.reserve(totalOps);

    for (int i = 0; i < totalOps; i++) {
        int type = typeDist(rng);

        if (type == 0) {
            int index = indexDist(rng);
            long long value = valueDist(rng);

            operations.push_back({
                OpType::ASSIGN, index, 0, value
            });
        }
        else {
            int l = indexDist(rng);
            int r = indexDist(rng);

            if (l > r) std::swap(l, r);

            if (type == 1) {
                operations.push_back({
                    OpType::QUERY_MAX, l, r, 0
                });
            }
            else {
                operations.push_back({
                    OpType::QUERY_GCD, l, r, 0
                });
            }
        }
    }

    return operations;
}
