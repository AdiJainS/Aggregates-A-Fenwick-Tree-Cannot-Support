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

    long long queryPos(int l, int r) const {
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
        }

        int right = r;
        while (right - lowbit(right) >= l) {
            if (bit1[right] > result) {
                result = bit1[right];
            }
            right = right - lowbit(right);
        }

        if (a[left] > result) {
            result = a[left];
        }

        return result;
    }

public:
    FenwickMax(const std::vector<long long>& values) {
        n = (int)values.size();

        a.assign(n + 1, LLONG_MIN);
        bit1.assign(n + 1, LLONG_MIN);
        bit2.assign(n + 1, LLONG_MIN);

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
    }

    long long queryMax(int l, int r) const {
        if (l < 0 || l > r || r >= n) {
            throw std::out_of_range("Invalid query range");
        }

        return queryPos(l + 1, r + 1);
    }

    void assign(int index, long long value) {
        if (index < 0 || index >= n) {
            throw std::out_of_range("Invalid index");
        }

        int p = index + 1;
        a[p] = value;

        int x = p;
        while (x <= n) {
            int start = x - lowbit(x) + 1;
            long long best = value;
            best = bigger(best, queryPos(start, p - 1));
            best = bigger(best, queryPos(p + 1, x));
            bit1[x] = best;
            x = x + lowbit(x);
        }

        x = p;
        while (x >= 1) {
            int end = x + lowbit(x) - 1;
            if (end > n) {
                end = n;
            }
            long long best = value;
            best = bigger(best, queryPos(x, p - 1));
            best = bigger(best, queryPos(p + 1, end));
            bit2[x] = best;
            x = x - lowbit(x);
        }
    }
};
