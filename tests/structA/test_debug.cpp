#include <iostream>
#include <vector>
#include "structure_a.hpp"

int main() {
    structA::GcdSegmentTree empty;
    if (!empty.debugCheckAgainst({})) {
        std::cerr << "Empty tree invariant check failed\n";
        return 1;
    }

    std::vector<structA::GcdValue> values = {12, 18, 30, 42};
    structA::GcdSegmentTree tree(values);
    tree.resetCounters();
    tree.query(0, 3);
    auto before = tree.euclidRemainderSteps();
    if (!tree.debugCheckAgainst(values) || tree.euclidRemainderSteps() != before) {
        std::cerr << "Invariant check changed the GCD counter\n";
        return 1;
    }
    std::cout << "Debug checks passed\n";
}
