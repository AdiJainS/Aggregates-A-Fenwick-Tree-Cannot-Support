
#pragma once

#include <vector>
#include <numeric>
#include <stdexcept>

using Value = unsigned long long;

class GcdReference {
private:
    std::vector<Value> arr;

public:
    explicit GcdReference(const std::vector<Value>& a)
        : arr(a) {}

    void assign(int i, Value value) {
        if (i < 0 || i >= static_cast<int>(arr.size())) {
            throw std::out_of_range("Invalid index");
        }

        arr[i] = value;
    }
    Value query(int l, int r) const {
        if (l < 0 || l > r ||
            r > static_cast<int>(arr.size())) {
            throw std::out_of_range("Invalid range");
        }

        Value answer = 0;

        for (int i = l; i < r; i++) {
            answer = std::gcd(answer, arr[i]);
        }

        return answer;
    }
};