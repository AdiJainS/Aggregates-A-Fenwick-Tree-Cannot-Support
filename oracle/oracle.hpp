
#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <climits>

struct MaxResult {
    long long maximum;
    int count;

    bool operator==(const MaxResult& other) const {
        return maximum == other.maximum &&
               count == other.count;
    }
};

class Oracle {
private:
    std::vector<long long> arr;

    void checkRange(int l, int r) const {
        if (l < 0 || l > r || r >= (int)arr.size()) {
            throw std::out_of_range("Invalid query range");
        }
    }

public:
    explicit Oracle(const std::vector<long long>& values)
        : arr(values) {}

    void assign(int index, long long value) {
        if (index < 0 || index >= (int)arr.size()) {
            throw std::out_of_range("Invalid index");
        }

        arr[index] = value;
    }

    MaxResult queryMax(int l, int r) const {
        checkRange(l, r);

        long long mx = LLONG_MIN;
        int count = 0;

        for (int i = l; i <= r; i++) {
            if (arr[i] > mx) {
                mx = arr[i];
                count = 1;
            }
            else if (arr[i] == mx) {
                count++;
            }
        }

        return {mx, count};
    }

    long long queryGCD(int l, int r) const {
        checkRange(l, r);

        long long result = 0;

        for (int i = l; i <= r; i++) {
            result = std::gcd(result, arr[i]);
        }

        return result;
    }
};
