
#include <iostream>
#include <vector>
#include <numeric>

#include "gcd_reference.hpp"
#include "generator.hpp"

using namespace std;

class BuggyTree {
private:
    vector<Value> arr;

public:
    BuggyTree(vector<Value> a) {
        arr = a;
    }

    void assign(int index, Value value) {
        (void)index;
        (void)value;
    }

    Value query(int l, int r) {
        Value answer = 0;

        for (int i = l; i < r; i++) {
            answer = gcd(answer, arr[i]);
        }

        return answer;
    }
};

int main() {
    int n = 100;
    int totalOps = 10000;
    unsigned long long seed = 42;

    vector<Value> arr(n, 10);

    GcdReference oracle(arr);
    BuggyTree tree(arr);

    vector<Operation> operations =
        generateOperations(n, totalOps, seed);

    int queriesChecked = 0;
    int failures = 0;

    for (int i = 0; i < totalOps; i++) {
        Operation op = operations[i];

        if (op.type == OpType::ASSIGN) {
            oracle.assign(op.l, op.value);
            tree.assign(op.l, op.value);
        }

        else if (op.type == OpType::QUERY_GCD) {
            Value expected = oracle.query(op.l, op.r + 1);
            Value actual = tree.query(op.l, op.r + 1);

            queriesChecked++;

            if (expected != actual) {
                failures++;

                if (failures == 1) {
                    cout << "First mismatch at operation: "
                         << i << "\n";
                    cout << "Expected: " << expected << "\n";
                    cout << "Actual: " << actual << "\n";
                }
            }
        }
    }

    cout << "Seed: " << seed << "\n";
    cout << "Total operations: " << totalOps << "\n";
    cout << "GCD queries checked: " << queriesChecked << "\n";
    cout << "Mismatches found: " << failures << "\n";

    if (failures > 0) {
        cout << "SUCCESS: Bug detected!\n";
    } else {
        cout << "No mismatches found.\n";
    }

    return failures > 0 ? 0 : 1;
}
