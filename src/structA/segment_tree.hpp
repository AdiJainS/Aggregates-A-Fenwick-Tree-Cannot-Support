#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace structA {

using GcdValue = unsigned long long;

struct MaxCountNode {
    long long maximum = std::numeric_limits<long long>::lowest();
    int count = 0;

    bool operator==(const MaxCountNode& other) const noexcept {
        return maximum == other.maximum && count == other.count;
    }

    bool operator!=(const MaxCountNode& other) const noexcept {
        return !(*this == other);
    }
};

struct SegmentTreeCounters {
    std::uint64_t buildMerges = 0;
    std::uint64_t queryCalls = 0;
    std::uint64_t queryBlocks = 0;
    std::uint64_t queryMerges = 0;
    std::uint64_t updateVisits = 0;
    std::uint64_t updateMerges = 0;
};

struct MaxCountOps {
    MaxCountNode identity() const noexcept {
        return {
            std::numeric_limits<long long>::lowest(),
            0
        };
    }

    MaxCountNode merge(
        const MaxCountNode& left,
        const MaxCountNode& right
    ) const noexcept {
        if (left.maximum > right.maximum) return left;
        if (right.maximum > left.maximum) return right;
        return {left.maximum, left.count + right.count};
    }

    void resetCounters() const noexcept {}
    std::uint64_t euclidRemainderSteps() const noexcept { return 0; }
};

struct GcdOps {
#ifdef STRUCTA_ENABLE_COUNTERS
    mutable std::uint64_t remainderSteps = 0;
#endif

    GcdValue identity() const noexcept {
        return 0ULL;
    }

    GcdValue merge(GcdValue a, GcdValue b) const noexcept {
        while (b != 0ULL) {
            const GcdValue remainder = a % b;
#ifdef STRUCTA_ENABLE_COUNTERS
            ++remainderSteps;
#endif
            a = b;
            b = remainder;
        }
        return a;
    }

    void resetCounters() const noexcept {
#ifdef STRUCTA_ENABLE_COUNTERS
        remainderSteps = 0;
#endif
    }

    std::uint64_t euclidRemainderSteps() const noexcept {
#ifdef STRUCTA_ENABLE_COUNTERS
        return remainderSteps;
#else
        return 0;
#endif
    }
};

template <class Node, class Ops>
class SegmentTree {
private:
    int n_ = 0;
    int base_ = 1;
    std::vector<Node> tree_;
    Ops ops_{};
    mutable SegmentTreeCounters counters_{};

    void countBuildMerge() noexcept {
#ifdef STRUCTA_ENABLE_COUNTERS
        ++counters_.buildMerges;
#endif
    }

    void countQueryBlock() const noexcept {
#ifdef STRUCTA_ENABLE_COUNTERS
        ++counters_.queryBlocks;
#endif
    }

    void countQueryMerge() const noexcept {
#ifdef STRUCTA_ENABLE_COUNTERS
        ++counters_.queryMerges;
#endif
    }

    void countUpdateVisit() noexcept {
#ifdef STRUCTA_ENABLE_COUNTERS
        ++counters_.updateVisits;
#endif
    }

    void countUpdateMerge() noexcept {
#ifdef STRUCTA_ENABLE_COUNTERS
        ++counters_.updateMerges;
#endif
    }

public:
    SegmentTree() = default;

    explicit SegmentTree(const std::vector<Node>& values) {
        build(values);
    }

    void build(const std::vector<Node>& values) {
        n_ = static_cast<int>(values.size());
        base_ = 1;
        while (base_ < n_) {
            base_ <<= 1;
        }

        tree_.assign(static_cast<std::size_t>(2) * base_, ops_.identity());
        for (int i = 0; i < n_; ++i) {
            tree_[base_ + i] = values[i];
        }

        counters_ = {};
        ops_.resetCounters();
        for (int p = base_ - 1; p >= 1; --p) {
            tree_[p] = ops_.merge(tree_[2 * p], tree_[2 * p + 1]);
            countBuildMerge();
        }
    }

    int size() const noexcept {
        return n_;
    }

    int base() const noexcept {
        return base_;
    }

    void assign(int index, const Node& value) {
        if (index < 0 || index >= n_) {
            throw std::out_of_range("SegmentTree::assign: invalid index");
        }

        int p = base_ + index;
        tree_[p] = value;
        countUpdateVisit();

        while (p > 1) {
            p >>= 1;
            tree_[p] = ops_.merge(tree_[2 * p], tree_[2 * p + 1]);
            countUpdateVisit();
            countUpdateMerge();
        }
    }

    Node query(int l, int r) const {
        if (l < 0 || l > r || r > n_) {
            throw std::out_of_range("SegmentTree::query: invalid range");
        }

#ifdef STRUCTA_ENABLE_COUNTERS
        ++counters_.queryCalls;
#endif
        if (l == r) {
            return ops_.identity();
        }

        Node leftResult = ops_.identity();
        Node rightResult = ops_.identity();

        l += base_;
        r += base_;

        while (l < r) {
            if (l & 1) {
                leftResult = ops_.merge(leftResult, tree_[l]);
                countQueryBlock();
                countQueryMerge();
                ++l;
            }

            if (r & 1) {
                --r;
                rightResult = ops_.merge(tree_[r], rightResult);
                countQueryBlock();
                countQueryMerge();
            }

            l >>= 1;
            r >>= 1;
        }

        countQueryMerge();
        return ops_.merge(leftResult, rightResult);
    }

    bool debugCheckAgainst(const std::vector<Node>& expected) const {
        if (static_cast<int>(expected.size()) != n_) {
            return false;
        }

        for (int i = 0; i < n_; ++i) {
            if (!(tree_[base_ + i] == expected[i])) {
                return false;
            }
        }

        for (int i = n_; i < base_; ++i) {
            if (!(tree_[base_ + i] == ops_.identity())) {
                return false;
            }
        }

        for (int p = base_ - 1; p >= 1; --p) {
            const Node expectedParent =
                ops_.merge(tree_[2 * p], tree_[2 * p + 1]);
            if (!(tree_[p] == expectedParent)) {
                return false;
            }
        }

        return true;
    }

    SegmentTreeCounters counters() const noexcept {
        return counters_;
    }

    std::uint64_t euclidRemainderSteps() const noexcept {
        return ops_.euclidRemainderSteps();
    }

    void resetCounters() const noexcept {
        counters_ = {};
        ops_.resetCounters();
    }
};

inline GcdValue gcdMagnitude(long long value) noexcept {
    if (value >= 0) {
        return static_cast<GcdValue>(value);
    }
    return static_cast<GcdValue>(-(value + 1)) + 1ULL;
}

}
