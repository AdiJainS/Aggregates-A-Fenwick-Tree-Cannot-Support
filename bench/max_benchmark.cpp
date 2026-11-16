#include "../src/structA/segment_tree.hpp"
#include "../src/structB/fenwick_max.hpp"
#include "../src/structB/prefix_fenwick_max.hpp"
#include "../adversary/generators.hpp"
#include "../oracle/oracle.hpp"
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

class MaxTree {
    structA::SegmentTree<structA::MaxCountNode, structA::MaxCountOps> tree;
public:
    explicit MaxTree(const std::vector<long long>& values) {
        std::vector<structA::MaxCountNode> leaves;
        leaves.reserve(values.size());
        for (auto value : values) leaves.push_back({value, 1});
        tree.build(leaves);
    }
    void assign(int i, long long value) { tree.assign(i, {value, 1}); }
    long long queryMax(int l, int r) const { return tree.query(l, r + 1).maximum; }
    structA::MaxCountNode queryMaxCount(int l, int r) const { return tree.query(l, r + 1); }
    void resetCounters() { tree.resetCounters(); }
    structA::SegmentTreeCounters counters() const { return tree.counters(); }
};

class Prefix {
    PrefixFenwickMax tree;
public:
    explicit Prefix(const std::vector<long long>& values) : tree(values) {}
    void assign(int i, long long value) { tree.assign(i, value); }
    long long queryMax(int, int r) const { return tree.queryPrefix(r); }
    void resetCounters() { tree.resetCounters(); }
    long long rebuilds() const { return tree.rebuildCount(); }
    long long queryBlocks() const { return tree.queryBlocks; }
    long long raiseSteps() const { return tree.raiseSteps; }
};

Workload randomWorkload(int n, int count, int width, int updatePercent, std::uint64_t seed, int decreasePercent) {
    std::mt19937_64 random(seed);
    std::uniform_int_distribution<long long> value(0, 1000000);
    std::uniform_int_distribution<int> index(0, n - 1), start(0, n - width), choice(0, 99);
    Workload w;
    w.initial.reserve(n);
    w.operations.reserve(count);
    for (int i = 0; i < n; ++i) w.initial.push_back(value(random));
    std::vector<long long> current = w.initial;
    for (int i = 0; i < count; ++i) {
        if (choice(random) < updatePercent) {
            if (decreasePercent < 0) {
                w.operations.push_back({OpType::ASSIGN, index(random), 0, value(random)});
            } else {
                int i = index(random);
                long long next = current[i];
                if (choice(random) < decreasePercent) next--;
                else next++;
                current[i] = next;
                w.operations.push_back({OpType::ASSIGN, i, 0, next});
            }
        }
        else { int l = start(random); w.operations.push_back({OpType::QUERY_MAX, l, l + width - 1, 0}); }
    }
    return w;
}

struct UpdateCounts {
    int increases = 0;
    int decreases = 0;
    int unchanged = 0;
};

UpdateCounts countUpdates(const Workload& w) {
    std::vector<long long> current = w.initial;
    UpdateCounts counts;
    for (const auto& op : w.operations) {
        if (op.type == OpType::ASSIGN) {
            if (op.value < current[op.l]) counts.decreases++;
            else if (op.value > current[op.l]) counts.increases++;
            else counts.unchanged++;
            current[op.l] = op.value;
        }
    }
    return counts;
}

bool prefixOnly(const Workload& w) {
    for (const auto& op : w.operations) if (op.type == OpType::QUERY_MAX && op.l != 0) return false;
    return true;
}

void check(const Workload& w) {
    Oracle oracle(w.initial);
    MaxTree a(w.initial);
    FenwickMax b(w.initial);
    Prefix p(w.initial);
    bool usePrefix = prefixOnly(w);
    for (std::size_t i = 0; i < w.operations.size(); ++i) {
        const auto& op = w.operations[i];
        if (op.type == OpType::ASSIGN) {
            oracle.assign(op.l, op.value); a.assign(op.l, op.value); b.assign(op.l, op.value);
            if (usePrefix) p.assign(op.l, op.value);
        } else if (op.type == OpType::QUERY_MAX) {
            auto expected = oracle.queryMax(op.l, op.r);
            auto result = a.queryMaxCount(op.l, op.r);
            if (result.maximum != expected.maximum || result.count != expected.count ||
                b.queryMax(op.l, op.r) != expected.maximum ||
                (usePrefix && p.queryMax(op.l, op.r) != expected.maximum))
                throw std::runtime_error("Mismatch at operation " + std::to_string(i));
        }
    }
}

using Clock = std::chrono::steady_clock;
template<class T>
void timed(const char* name, const Workload& w, const std::string& kind, int width,
           int updatePercent, std::uint64_t seed, int run, int decreasePercent,
           const UpdateCounts& counts) {
    auto begin = Clock::now();
    T structure(w.initial);
    auto built = Clock::now();
    std::uint64_t checksum = 0;
    int queries = 0, updates = 0;
    for (const auto& op : w.operations) {
        if (op.type == OpType::ASSIGN) { structure.assign(op.l, op.value); ++updates; }
        else if (op.type == OpType::QUERY_MAX) {
            checksum = checksum * 1315423911ULL + static_cast<std::uint64_t>(structure.queryMax(op.l, op.r));
            ++queries;
        }
    }
    auto done = Clock::now();
    double build = std::chrono::duration<double, std::milli>(built - begin).count();
    double replay = std::chrono::duration<double, std::milli>(done - built).count();
    std::cout << name << ',' << w.initial.size() << ',' << w.operations.size() << ',' << width << ','
              << updatePercent << ',' << seed << ',' << run << ',' << queries << ',' << updates << ','
              << std::fixed << std::setprecision(6) << build << ',' << replay << ',' << checksum << ',' << kind << ','
              << decreasePercent << ',' << counts.increases << ',' << counts.decreases << ',' << counts.unchanged << '\n';
}

void counters(const Workload& w, bool includePrefix, int decreasePercent, const UpdateCounts& counts) {
    MaxTree a(w.initial);
    FenwickMax b(w.initial);
    Prefix p(w.initial);
    bool usePrefix = includePrefix && prefixOnly(w);
    a.resetCounters(); b.resetCounters(); p.resetCounters();
    for (const auto& op : w.operations) {
        if (op.type == OpType::ASSIGN) {
            a.assign(op.l, op.value); b.assign(op.l, op.value);
            if (usePrefix) p.assign(op.l, op.value);
        } else if (op.type == OpType::QUERY_MAX) {
            a.queryMax(op.l, op.r); b.queryMax(op.l, op.r);
            if (usePrefix) p.queryMax(op.l, op.r);
        }
    }
    auto c = a.counters();
    std::cout << "structure,query_blocks,update_steps,changed_blocks,lowered_blocks,rebuilds,decrease_percent,actual_increases,actual_decreases,actual_unchanged\n";
    std::cout << "segment_tree," << c.queryBlocks << ',' << c.updateVisits << ",,,," << decreasePercent << ',' << counts.increases << ',' << counts.decreases << ',' << counts.unchanged << '\n';
    std::cout << "fenwick," << b.queryBlocks << ',' << b.assignBlocks << ',' << b.changedBlocks << ',' << b.loweredBlocks << ",," << decreasePercent << ',' << counts.increases << ',' << counts.decreases << ',' << counts.unchanged << '\n';
    if (usePrefix) std::cout << "prefix_fenwick," << p.queryBlocks() << ',' << p.raiseSteps() << ",,," << p.rebuilds() << ',' << decreasePercent << ',' << counts.increases << ',' << counts.decreases << ',' << counts.unchanged << '\n';
}

void checkAdversarial(const Workload& w, const std::string& kind) {
    MaxTree a(w.initial);
    FenwickMax b(w.initial);
    long long expected = static_cast<long long>(w.initial.size()) - 1;
    for (std::size_t i = 0; i < w.operations.size(); ++i) {
        const auto& op = w.operations[i];
        if (op.type == OpType::ASSIGN) {
            a.assign(op.l, op.value);
            b.assign(op.l, op.value);
            expected = op.value;
        } else if (op.type == OpType::QUERY_MAX) {
            auto result = a.queryMaxCount(op.l, op.r);
            if (result.maximum != expected || result.count != 1 || b.queryMax(op.l, op.r) != expected)
                throw std::runtime_error("Mismatch at operation " + std::to_string(i));
        }
    }
    std::cout << "All " << w.operations.size() << " operations checked for " << kind << '\n';
}

int main(int argc, char** argv) {
    if (argc != 9 && argc != 10) {
        std::cerr << "Usage: max_benchmark N OPERATIONS WIDTH UPDATE_PERCENT SEED RUNS WORKLOAD MODE [DECREASE_PERCENT]\n"
                     "WORKLOAD: random, rebuild, decrease, query; MODE: check, check-scale, time, prefix, counts, counts-ab\n";
        return 1;
    }
    try {
        int n = std::stoi(argv[1]), count = std::stoi(argv[2]), width = std::stoi(argv[3]);
        int updatePercent = std::stoi(argv[4]), runs = std::stoi(argv[6]);
        std::uint64_t seed = std::stoull(argv[5]);
        std::string kind = argv[7], mode = argv[8];
        int decreasePercent = -1;
        if (argc == 10) decreasePercent = std::stoi(argv[9]);
        if (n < 1 || count < 1 || width < 1 || width > n || updatePercent < 0 || updatePercent > 100 || runs < 1)
            throw std::invalid_argument("Invalid parameters");
        if ((argc == 10 && (decreasePercent < 0 || decreasePercent > 100)) || (argc == 10 && kind != "random"))
            throw std::invalid_argument("DECREASE_PERCENT must be 0 to 100 and used with random workloads");
        Workload w;
        if (kind == "random") w = randomWorkload(n, count, width, updatePercent, seed, decreasePercent);
        else if (kind == "rebuild") w = makeRebuildWorkload(n, count);
        else if (kind == "decrease") w = makeDecreaseWorkload(n, count);
        else if (kind == "query") w = makeQueryWorkload(n, count);
        else throw std::invalid_argument("Unknown workload");
        if (kind != "random") { width = -1; updatePercent = -1; seed = 0; }
        UpdateCounts updateCounts = countUpdates(w);
        if (mode == "check") { check(w); std::cout << "All maximum queries matched the oracle\n"; }
        else if (mode == "check-scale") {
            if (kind != "query" && kind != "decrease") throw std::invalid_argument("check-scale requires query or decrease workload");
            checkAdversarial(w, kind);
        } else if (mode == "counts" || mode == "counts-ab") {
#ifndef STRUCTA_ENABLE_COUNTERS
            throw std::invalid_argument("Compile with -DSTRUCTA_ENABLE_COUNTERS for counts");
#else
            counters(w, mode == "counts" && kind != "random", decreasePercent, updateCounts);
#endif
        } else if (mode == "time" || mode == "prefix") {
#ifdef STRUCTA_ENABLE_COUNTERS
            throw std::invalid_argument("Compile without STRUCTA_ENABLE_COUNTERS for timing");
#endif
            if (mode == "prefix" && !prefixOnly(w)) throw std::invalid_argument("Prefix mode requires prefix queries");
            std::cout << "structure,n,operations,width,update_percent,seed,run,queries,updates,build_ms,replay_ms,checksum,workload,decrease_percent,actual_increases,actual_decreases,actual_unchanged\n";
            for (int run = 1; run <= runs; ++run) {
                if (mode == "time") {
                    timed<MaxTree>("segment_tree", w, kind, width, updatePercent, seed, run, decreasePercent, updateCounts);
                    timed<FenwickMax>("fenwick", w, kind, width, updatePercent, seed, run, decreasePercent, updateCounts);
                } else timed<Prefix>("prefix_fenwick", w, kind, width, updatePercent, seed, run, decreasePercent, updateCounts);
            }
        } else throw std::invalid_argument("Unknown mode");
    } catch (const std::exception& e) { std::cerr << "Error: " << e.what() << '\n'; return 1; }
}
