#pragma once

#include <climits>
#include <stdexcept>
#include <vector>

namespace structA {

typedef unsigned long long GcdValue;

struct MaxCountNode {
    long long maximum = LLONG_MIN;
    int count = 0;

    bool operator==(const MaxCountNode& other) const { // Operator Overloading -> checking whether MaxCountNode a{5, 2}; MaxCountNode b{5, 2}; maximum and count values are equal or not 
        return maximum == other.maximum && count == other.count; // other -> name given to 2nd object
    }

    bool operator!=(const MaxCountNode& other) const {// Defines not equal to func
        return !(*this == other); // calls the previous defined operator== and reverses its result using !
    }
};

struct SegmentTreeCounters { // Stores the performance statistics of Segment Tree
    unsigned long long buildMerges = 0; // Counts how many merge operations happen during tree construction.
    unsigned long long queryCalls = 0; // Counts how many range queries are executed.
    unsigned long long queryBlocks = 0; //Counts how many individual tree blocks are selected while answering queries
    unsigned long long queryMerges = 0; //Counts how many merges are performed during range queries.
    unsigned long long updateVisits = 0; //Counts how many tree nodes are visited during point updates.
    unsigned long long updateMerges = 0; //Counts how many merge operations happen during point updates.
};

struct MaxCountOps { //Defines the operations used when the Segment Tree stores maximum values and their frequencies.
    MaxCountNode identity() const {
        MaxCountNode result;
        result.maximum = LLONG_MIN;
        result.count = 0;
        return result; // Returns the identity node: (-infinity,0)
    }

    MaxCountNode merge(const MaxCountNode& left, const MaxCountNode& right) const { //Defines a function that takes two nodes and combines their information.
        if (left.maximum > right.maximum) return left;
        if (right.maximum > left.maximum) return right;
        MaxCountNode result; // else both max are equal
        result.maximum = left.maximum;
        result.count = left.count + right.count;
        return result;
    }

    void resetCounters() const {} //Defines an empty function because maximum/count does not need operation-specific counters.

    unsigned long long euclidRemainderSteps() const {//Defines a function that returns the number of Euclidean algorithm remainder operations.
        return 0;
    }
};

struct GcdOps {//Defines the operations used when the Segment Tree stores GCD values.
#ifdef STRUCTA_ENABLE_COUNTERS 
    mutable unsigned long long remainderSteps = 0;
#endif

    GcdValue identity() const { // Defines the identity function for GCD.
        return 0ULL; // gcd(a,0) = a , ULL -> unsigned long long
    }

    //for merge function we are implementing the Euclidean algorithm:
    GcdValue merge(GcdValue a, GcdValue b) const { //Defines a function that takes two numbers and calculates their GCD.
        while (b != 0ULL) {
            GcdValue remainder = a % b;
#ifdef STRUCTA_ENABLE_COUNTERS // Checks whether performance counting is enabled.
            remainderSteps++; 
#endif
            a = b;
            b = remainder;
        }
        return a;
    }

    void resetCounters() const { //Defines a function to reset the GCD operation counter.
#ifdef STRUCTA_ENABLE_COUNTERS
        remainderSteps = 0; //Resets the counter to zero.
#endif
    }

    unsigned long long euclidRemainderSteps() const { //Defines a function that returns the number of remainder operations.
#ifdef STRUCTA_ENABLE_COUNTERS //Checks whether counting is enabled.
        return remainderSteps;
#else //Specifies what happens when counting is disabled.
        return 0; //Returns zero because no count was maintained.
#endif
    }
};

//Defining Generic Segment Tree Class
//Declares a template with two parameters:
//- Node: What each tree position stores.
//- Ops: How two nodes are combined.
template <class Node, class Ops>
class SegmentTree {
private:
    int n_ = 0; //Stores the number of array elements.
    int base_ = 1; //Stores the smallest power of two greater than or equal to n_.
    std::vector<Node> tree_; //Creates a vector that stores the entire Segment Tree, including leaves and internal nodes.
    Ops ops_;//Creates an object of the operation type. It will be either MaxCountOps or GcdOps.
    mutable SegmentTreeCounters counters_; //mutable allows these counters to be modified inside const member functions

public://Makes the following functions accessible outside the class.
    SegmentTree() {} //Constructor, Creates an empty Segment Tree

    SegmentTree(const std::vector<Node>& values) {//Defines another constructor that accepts an input vector by constant reference.
        build(values);
    }

    void build(const std::vector<Node>& values) {
        n_ = (int)values.size();
        base_ = 1; //leaf capacity
        while (base_ < n_) {
            base_ = base_ * 2;
        }

        tree_.assign(2 * base_, ops_.identity());//Creates a vector containing 2 * base_ elements and initializes every position with the identity element.
        for (int i = 0; i < n_; i++) {
            tree_[base_ + i] = values[i];//Adding values in leaf positions
        }

        counters_ = SegmentTreeCounters(); //Resets the Segment Tree performance counters by creating a fresh counter object.
        ops_.resetCounters();//Resets the operation-specific counter
        for (int p = base_ - 1; p >= 1; p--) { // starts building internal nodes by merging left and right child values 
            tree_[p] = ops_.merge(tree_[2 * p], tree_[2 * p + 1]);
#ifdef STRUCTA_ENABLE_COUNTERS
            counters_.buildMerges++;
#endif
        }
    }

    int size() const { //getter func for returning org array size 
        return n_;
    }

    int base() const { //getter function that returns the leaf capacity.
        return base_;
    }

    void assign(int index, const Node& value) { //point update function
        if (index < 0 || index >= n_) { //if index is out of valid array range
            throw std::out_of_range("SegmentTree::assign: invalid index");
        }

        int p = base_ + index; 
        tree_[p] = value; //Replaces the old leaf value with the new value.
#ifdef STRUCTA_ENABLE_COUNTERS
        counters_.updateVisits++; //Increments the number of nodes visited during updates.
#endif

        while (p > 1) { //Continues updating ancestors until root is reached 
            p = p / 2;
            tree_[p] = ops_.merge(tree_[2 * p], tree_[2 * p + 1]); //Recalculating parent
#ifdef STRUCTA_ENABLE_COUNTERS
            counters_.updateVisits++;
            counters_.updateMerges++;
#endif
        }
    }

    Node query(int l, int r) const { //for finding rang [l,r)
        if (l < 0 || l > r || r > n_) {
            throw std::out_of_range("SegmentTree::query: invalid range");
        }

#ifdef STRUCTA_ENABLE_COUNTERS
        counters_.queryCalls++;
#endif
        if (l == r) {
            return ops_.identity();
        }

        Node leftResult = ops_.identity();
        Node rightResult = ops_.identity();

        l = l + base_; //Converting left array index into the corresponding leaf position
        r = r + base_;

        while (l < r) {
            if (l % 2 == 1) {
                leftResult = ops_.merge(leftResult, tree_[l]);
#ifdef STRUCTA_ENABLE_COUNTERS
                counters_.queryBlocks++;
                counters_.queryMerges++;
#endif
                l++;
            }

            if (r % 2 == 1) {
                r--;
                rightResult = ops_.merge(tree_[r], rightResult);
#ifdef STRUCTA_ENABLE_COUNTERS
                counters_.queryBlocks++;
                counters_.queryMerges++;
#endif
            }

            l = l / 2;
            r = r / 2;
        }

#ifdef STRUCTA_ENABLE_COUNTERS
        counters_.queryMerges++;
#endif
        return ops_.merge(leftResult, rightResult);
    }

    bool debugCheckAgainst(const std::vector<Node>& expected) const { //check whether the segment tree is correctly constructed and whether all its nodes contain the correct values.
        if ((int)expected.size() != n_) {
            return false;
        }

        for (int i = 0; i < n_; i++) {
            if (!(tree_[base_ + i] == expected[i])) {
                return false;
            }
        }

        for (int i = n_; i < base_; i++) {
            if (!(tree_[base_ + i] == ops_.identity())) {
                return false;
            }
        }

        for (int p = base_ - 1; p >= 1; p--) {
            Node expectedParent = ops_.merge(tree_[2 * p], tree_[2 * p + 1]);
            if (!(tree_[p] == expectedParent)) {
                return false;
            }
        }

        return true;
    }

    SegmentTreeCounters counters() const {
        return counters_;
    }

    unsigned long long euclidRemainderSteps() const {
        return ops_.euclidRemainderSteps();
    }

    void resetCounters() const {
        counters_ = SegmentTreeCounters();
        ops_.resetCounters();
    }
};

inline GcdValue gcdMagnitude(long long value) {
    if (value >= 0) {
        return (GcdValue)value;
    }
    return (GcdValue)(-(value + 1)) + 1ULL;
}

}
