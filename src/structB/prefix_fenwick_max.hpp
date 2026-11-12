#pragma once

#include <climits>
#include <stdexcept>
#include <vector>

class PrefixFenwickMax {
private:
    int n;
    int rebuilds;
    std::vector<long long> a;
    std::vector<long long> tree;

    int lowbit(int i) const {
        return i & -i;
    }

    void rebuild() {
        for (int i = 1; i <= n; i++) {
            tree[i] = a[i];
        }

        for (int i = 1; i <= n; i++) {
            int parent = i + lowbit(i);
            if (parent <= n && tree[i] > tree[parent]) {
                tree[parent] = tree[i];
            }
        }

        rebuilds++;
    }

    void raiseBlocks(int p, long long value) {
        int x = p;
        while (x <= n) {
            if (value > tree[x]) {
                tree[x] = value;
            }
            x = x + lowbit(x);
            raiseSteps++;
        }
    }

public:
    mutable long long queryBlocks;
    long long raiseSteps;

    PrefixFenwickMax(const std::vector<long long>& values) {
        n = (int)values.size();

        a.assign(n + 1, LLONG_MIN);
        tree.assign(n + 1, LLONG_MIN);

        for (int i = 0; i < n; i++) {
            a[i + 1] = values[i];
        }

        rebuilds = 0;
        rebuild();
        resetCounters();
    }

    void resetCounters() {
        rebuilds = 0;
        queryBlocks = 0;
        raiseSteps = 0;
    }

    long long queryPrefix(int r) const {
        if (r < 0 || r >= n) {
            throw std::out_of_range("Invalid prefix end");
        }

        long long result = LLONG_MIN;

        int i = r + 1;
        while (i > 0) {
            if (tree[i] > result) {
                result = tree[i];
            }
            i = i - lowbit(i);
            queryBlocks++;
        }

        return result;
    }

    void assign(int index, long long value) {
        if (index < 0 || index >= n) {
            throw std::out_of_range("Invalid index");
        }

        int p = index + 1;

        if (value < a[p]) {
            a[p] = value;
            rebuild();
        }
        else {
            a[p] = value;
            raiseBlocks(p, value);
        }
    }

    void assignIncreaseOnly(int index, long long value) {
        if (index < 0 || index >= n) {
            throw std::out_of_range("Invalid index");
        }

        int p = index + 1;
        a[p] = value;
        raiseBlocks(p, value);
    }

    int rebuildCount() const {
        return rebuilds;
    }
};