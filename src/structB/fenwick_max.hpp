#pragma once

#include <climits>
#include <stdexcept>
#include <vector>

class FenwickMax {
private:
    int n;
    std::vector<long long> a;
    std::vector<long long> bit1;
    std::vector<long long> bit2;
    std::vector<long long> leftBefore;
    std::vector<long long> rightBefore;

    int lowbit(int i) const {
        return i & -i;
    }

    long long bigger(long long x, long long y) const {
        if (x > y) {
            return x;
        }
        return y;
    }

    long long slowMax(int from, int to) const {
        long long best = LLONG_MIN;
        for (int k = from; k <= to; k++) {
            if (a[k] > best) {
                best = a[k];
            }
        }
        return best;
    }

    long long queryPos(int l, int r, long long& steps) const {
        if (l > r) {
            return LLONG_MIN;
        }

        long long result = LLONG_MIN;

        int left = l;
        while (left + lowbit(left) <= r) {
            if (bit2[left] > result) {
                result = bit2[left];
            }
            left = left + lowbit(left);
            steps++;
        }

        int right = r;
        while (right - lowbit(right) >= l) {
            if (bit1[right] > result) {
                result = bit1[right];
            }
            right = right - lowbit(right);
            steps++;
        }

        if (a[left] > result) {
            result = a[left];
        }
        steps++;

        return result;
    }

public:
    mutable long long queries;
    mutable long long queryBlocks;
    long long assigns;
    long long assignBlocks;
    long long changedBlocks;
    long long loweredBlocks;

    FenwickMax(const std::vector<long long>& values) {
        n = (int)values.size();

        a.assign(n + 1, LLONG_MIN);
        bit1.assign(n + 1, LLONG_MIN);
        bit2.assign(n + 1, LLONG_MIN);
        leftBefore.assign(n + 2, LLONG_MIN);
        rightBefore.assign(n + 2, LLONG_MIN);

        for (int i = 0; i < n; i++) {
            a[i + 1] = values[i];
        }

        for (int i = 1; i <= n; i++) {
            int start = i - lowbit(i) + 1;
            bit1[i] = slowMax(start, i);

            int end = i + lowbit(i) - 1;
            if (end > n) {
                end = n;
            }
            bit2[i] = slowMax(i, end);
        }

        resetCounters();
    }

    void resetCounters() {
        queries = 0;
        queryBlocks = 0;
        assigns = 0;
        assignBlocks = 0;
        changedBlocks = 0;
        loweredBlocks = 0;
    }

    long long queryMax(int l, int r) const {
        if (l < 0 || l > r || r >= n) {
            throw std::out_of_range("Invalid query range");
        }

        queries++;
        return queryPos(l + 1, r + 1, queryBlocks);
    }

    void assignSlow(int index, long long value) {
        if (index < 0 || index >= n) {
            throw std::out_of_range("Invalid index");
        }

        assigns++;

        int p = index + 1;
        a[p] = value;

        int x = p;
        while (x <= n) {
            int start = x - lowbit(x) + 1;
            long long best = value;
            best = bigger(best, queryPos(start, p - 1, assignBlocks));
            best = bigger(best, queryPos(p + 1, x, assignBlocks));

            if (best != bit1[x]) {
                changedBlocks++;
            }
            if (best < bit1[x]) {
                loweredBlocks++;
            }
            bit1[x] = best;

            assignBlocks++;
            x = x + lowbit(x);
        }

        x = p;
        while (x >= 1) {
            int end = x + lowbit(x) - 1;
            if (end > n) {
                end = n;
            }
            long long best = value;
            best = bigger(best, queryPos(x, p - 1, assignBlocks));
            best = bigger(best, queryPos(p + 1, end, assignBlocks));

            if (best != bit2[x]) {
                changedBlocks++;
            }
            if (best < bit2[x]) {
                loweredBlocks++;
            }
            bit2[x] = best;

            assignBlocks++;
            x = x - lowbit(x);
        }
    }

    void assign(int index, long long value) {
        if (index < 0 || index >= n) {
            throw std::out_of_range("Invalid index");
        }

        assigns++;

        int p = index + 1;
        a[p] = value;

        long long run = LLONG_MIN;
        int q = p + 1;
        while (q <= n) {
            rightBefore[q] = run;
            run = bigger(run, bit2[q]);
            q = q + lowbit(q);
            assignBlocks++;
        }
        long long rightAll = run;

        run = LLONG_MIN;
        int u = p - 1;
        while (u >= 1) {
            leftBefore[u] = run;
            run = bigger(run, bit1[u]);
            u = u - lowbit(u);
            assignBlocks++;
        }
        long long leftAll = run;

        int x = p;
        while (x <= n) {
            long long best = value;

            int before = x - lowbit(x);
            if (before == 0) {
                best = bigger(best, leftAll);
            }
            else {
                best = bigger(best, leftBefore[before]);
            }

            if (x > p) {
                best = bigger(best, bigger(rightBefore[x], a[x]));
            }

            if (best != bit1[x]) {
                changedBlocks++;
            }
            if (best < bit1[x]) {
                loweredBlocks++;
            }
            bit1[x] = best;

            assignBlocks++;
            x = x + lowbit(x);
        }

        int y = p;
        while (y >= 1) {
            long long best = value;

            if (y < p) {
                best = bigger(best, bigger(leftBefore[y], a[y]));
            }

            int after = y + lowbit(y);
            if (after > n) {
                best = bigger(best, rightAll);
            }
            else {
                best = bigger(best, rightBefore[after]);
            }

            if (best != bit2[y]) {
                changedBlocks++;
            }
            if (best < bit2[y]) {
                loweredBlocks++;
            }
            bit2[y] = best;

            assignBlocks++;
            y = y - lowbit(y);
        }
    }

    bool sameBlocksAs(const FenwickMax& other) const {
        return bit1 == other.bit1 && bit2 == other.bit2;
    }
};