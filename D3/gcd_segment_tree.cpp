#include <iostream>
#include <vector>

using namespace std;


using Value = unsigned long long;

inline Value euclid(Value a, Value b) {
    while (b != 0) {
        Value r = a % b;

        a = b;
        b = r;
    }

    return a;
}

class GcdSegmentTree {
    int n_, base_ = 1;
    vector<Value> tree_;

public:
    explicit GcdSegmentTree(const vector<Value>& a) : n_(static_cast<int>(a.size())) {
        while (base_ < n_) base_ *= 2;

        tree_.assign(2 * base_, 0);
        for (int i = 0; i < n_; ++i) tree_[base_ + i] = a[i];
        for (int p = base_ - 1; p > 0; --p)
            tree_[p] = euclid(tree_[2*p], tree_[2*p+1]);
    }


    void assign(int i, Value value) {
        int p = base_ + i;
        tree_[p] = value;

        while (p > 1) {
            p /= 2;
            tree_[p] = euclid(tree_[2*p], tree_[2*p+1]);
        }
    }


    Value query(int l, int r) const {
        Value answer = 0;
        l += base_;
        r += base_;

        while (l < r) {
            if (l & 1) answer = euclid(answer, tree_[l++]);
            if (r & 1) answer = euclid(answer, tree_[--r]);
            l /= 2;
            r /= 2;
        }
        return answer;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, operations;
    if (!(cin >> n >> operations) || n < 0 || n > 1000000 || operations < 0) {
        cerr << "Invalid array size or operation count\n";
        return 1;
    }
    vector<Value> values(n);
    long long input;
    for (int i = 0; i < n; ++i) {
        if (!(cin >> input) || input < 0 || input > 1000000000) {
            cerr << "Values must be between 0 and 1000000000\n";
            return 1;
        }
        values[i] = input;
    }
    GcdSegmentTree tree(values);
    for (int step = 0; step < operations; ++step) {
        char operation;
        if (!(cin >> operation)) {
            cerr << "Missing operation\n";
            return 1;
        }
        if (operation == 'U') {
            int index;
            if (!(cin >> index >> input) || index < 0 || index >= n ||
                input < 0 || input > 1000000000) {
                cerr << "Invalid assignment\n";
                return 1;
            }
            tree.assign(index, input);
        } else if (operation == 'Q') {
            int left, right;
            if (!(cin >> left >> right) || left < 0 || left > right || right > n) {
                cerr << "Invalid range\n";
                return 1;
            }
            cout << tree.query(left, right) << '\n';
        } else {
            cerr << "Use U for assignment or Q for query\n";
            return 1;
        }
    }
}
