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
    GcdSegmentTree sensors({12, 18, 0, 30, 42});
    cout << sensors.query(0, 5) << '\n';
    cout << sensors.query(1, 4) << '\n';

    sensors.assign(1, 5);
    cout << sensors.query(0, 5) << '\n';


    sensors.assign(1, 0);
    cout << sensors.query(0, 3) << '\n';
    cout << sensors.query(2, 2) << '\n';
}
