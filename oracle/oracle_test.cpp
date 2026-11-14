
#include <iostream>
#include <cassert>
#include "oracle.hpp"

int main() {
    std::vector<long long> a = {12, 8, 15, 15, 6, 9};

    Oracle oracle(a);

    // Test 1: Maximum and frequency
    auto ans = oracle.queryMax(0, 5);

    assert(ans.maximum == 15);
    assert(ans.count == 2);

    // Test 2: GCD
    assert(oracle.queryGCD(0, 2) == 1);

    // Test 3: Decreasing update
    oracle.assign(2, 4);

    ans = oracle.queryMax(0, 5);

    assert(ans.maximum == 15);
    assert(ans.count == 1);

    // Test 4: GCD after update
    assert(oracle.queryGCD(0, 2) == 4);

    // Test 5: Single-element query
    ans = oracle.queryMax(2, 2);

    assert(ans.maximum == 4);
    assert(ans.count == 1);

    std::cout << "All Oracle tests passed!\n";

    return 0;
}
