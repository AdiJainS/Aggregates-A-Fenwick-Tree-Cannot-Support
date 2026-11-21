#pragma once

#include <stdexcept>
#include <vector>

#include "segment_tree.hpp"

namespace structA {

struct StructureACounters {
    SegmentTreeCounters maximum;
    SegmentTreeCounters gcd;
    unsigned long long gcdRemainderSteps = 0;
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
    StructureA() {}

    StructureA(const std::vector<long long>& values) {
        build(values);
    }

    void build(const std::vector<long long>& values) {
        n_ = (int)values.size();

        std::vector<MaxCountNode> maxLeaves;
        std::vector<GcdValue> gcdLeaves;

        for (int i = 0; i < n_; i++) {
            MaxCountNode node;
            node.maximum = values[i];
            node.count = 1;
            maxLeaves.push_back(node);
            gcdLeaves.push_back(gcdMagnitude(values[i]));
        }

        maxTree_.build(maxLeaves);
        gcdTree_.build(gcdLeaves);
        built_ = true;
    }

    int size() const {
        return n_;
    }

    void assign(int index, long long value) {
        requireBuilt();
        if (index < 0 || index >= n_) {
            throw std::out_of_range("StructureA::assign: invalid index");
        }

        MaxCountNode node;
        node.maximum = value;
        node.count = 1;
        maxTree_.assign(index, node);
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
        if (!built_ || (int)values.size() != n_) {
            return false;
        }

        std::vector<MaxCountNode> expectedMax;
        std::vector<GcdValue> expectedGcd;

        for (int i = 0; i < n_; i++) {
            MaxCountNode node;
            node.maximum = values[i];
            node.count = 1;
            expectedMax.push_back(node);
            expectedGcd.push_back(gcdMagnitude(values[i]));
        }

        return maxTree_.debugCheckAgainst(expectedMax) &&
               gcdTree_.debugCheckAgainst(expectedGcd);
    }

    StructureACounters counters() const {
        StructureACounters result;
        result.maximum = maxTree_.counters();
        result.gcd = gcdTree_.counters();
        result.gcdRemainderSteps = gcdTree_.euclidRemainderSteps();
        return result;
    }

    void resetCounters() const {
        maxTree_.resetCounters();
        gcdTree_.resetCounters();
    }
};

class GcdSegmentTree {
private:
    SegmentTree<GcdValue, GcdOps> tree_;

public:
    GcdSegmentTree() {}

    GcdSegmentTree(const std::vector<GcdValue>& values) {
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

    SegmentTreeCounters counters() const {
        return tree_.counters();
    }

    unsigned long long euclidRemainderSteps() const {
        return tree_.euclidRemainderSteps();
    }

    void resetCounters() const {
        tree_.resetCounters();
    }

    bool debugCheckAgainst(const std::vector<GcdValue>& values) const {
        return tree_.debugCheckAgainst(values);
    }
};

}
