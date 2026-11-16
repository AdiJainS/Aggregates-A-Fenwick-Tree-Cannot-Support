#pragma once

#include <stdexcept>
#include <vector>

#include "segment_tree.hpp"

namespace structA {

struct StructureACounters {
    SegmentTreeCounters maximum;
    SegmentTreeCounters gcd;
    std::uint64_t gcdRemainderSteps = 0;
};

class StructureA {
private:
    SegmentTree<MaxCountNode, MaxCountOps> maxTree_;
    SegmentTree<GcdValue, GcdOps> gcdTree_;
    int n_ = 0;
    bool built_ = false;

    void requireBuilt() const {
        if (!built_) {
            throw std::logic_error("StructureA: build() must be called first");
        }
    }

public:
    StructureA() = default;

    explicit StructureA(const std::vector<long long>& values) {
        build(values);
    }

    void build(const std::vector<long long>& values) {
        n_ = static_cast<int>(values.size());

        std::vector<MaxCountNode> maxLeaves;
        std::vector<GcdValue> gcdLeaves;
        maxLeaves.reserve(values.size());
        gcdLeaves.reserve(values.size());

        for (long long value : values) {
            maxLeaves.push_back({value, 1});
            gcdLeaves.push_back(gcdMagnitude(value));
        }

        maxTree_.build(maxLeaves);
        gcdTree_.build(gcdLeaves);
        built_ = true;
    }

    int size() const noexcept {
        return n_;
    }

    void assign(int index, long long value) {
        requireBuilt();
        if (index < 0 || index >= n_) {
            throw std::out_of_range("StructureA::assign: invalid index");
        }

        maxTree_.assign(index, MaxCountNode{value, 1});
        gcdTree_.assign(index, gcdMagnitude(value));
    }

    MaxCountNode queryMax(int l, int r) const {
        requireBuilt();
        return maxTree_.query(l, r);
    }

    GcdValue queryGCD(int l, int r) const {
        requireBuilt();
        return gcdTree_.query(l, r);
    }

    bool debugCheckAgainst(const std::vector<long long>& values) const {
        if (!built_ || static_cast<int>(values.size()) != n_) {
            return false;
        }

        std::vector<MaxCountNode> expectedMax;
        std::vector<GcdValue> expectedGcd;
        expectedMax.reserve(values.size());
        expectedGcd.reserve(values.size());

        for (long long value : values) {
            expectedMax.push_back({value, 1});
            expectedGcd.push_back(gcdMagnitude(value));
        }

        return maxTree_.debugCheckAgainst(expectedMax) &&
               gcdTree_.debugCheckAgainst(expectedGcd);
    }

    StructureACounters counters() const noexcept {
        return {
            maxTree_.counters(),
            gcdTree_.counters(),
            gcdTree_.euclidRemainderSteps()
        };
    }

    void resetCounters() const noexcept {
        maxTree_.resetCounters();
        gcdTree_.resetCounters();
    }
};

class GcdSegmentTree {
private:
    SegmentTree<GcdValue, GcdOps> tree_;

public:
    GcdSegmentTree() = default;

    explicit GcdSegmentTree(const std::vector<GcdValue>& values) {
        tree_.build(values);
    }

    void build(const std::vector<GcdValue>& values) {
        tree_.build(values);
    }

    void assign(int index, GcdValue value) {
        tree_.assign(index, value);
    }

    GcdValue query(int l, int r) const {
        return tree_.query(l, r);
    }

    SegmentTreeCounters counters() const noexcept {
        return tree_.counters();
    }

    std::uint64_t euclidRemainderSteps() const noexcept {
        return tree_.euclidRemainderSteps();
    }

    void resetCounters() const noexcept {
        tree_.resetCounters();
    }

    bool debugCheckAgainst(const std::vector<GcdValue>& values) const {
        return tree_.debugCheckAgainst(values);
    }
};

}
