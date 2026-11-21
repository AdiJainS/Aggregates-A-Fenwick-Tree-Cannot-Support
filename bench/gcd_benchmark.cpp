#include "../src/structA/structure_a.hpp"
#include "../oracle/gcd_reference.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

struct Operation {
    bool update;
    int l, r;
    Value value;
};

struct Workload {
    std::vector<Value> initial;
    std::vector<Operation> operations;
};

Workload generate(int n, int count, int width, int update_percent, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<Value> value(0, 1000000);
    std::uniform_int_distribution<int> index(0, n - 1);
    std::uniform_int_distribution<int> start(0, n - width);
    std::uniform_int_distribution<int> choice(0, 99);
    Workload w;
    w.initial.reserve(n);
    w.operations.reserve(count);
    for (int i = 0; i < n; ++i) w.initial.push_back(value(rng));
    for (int i = 0; i < count; ++i) {
        if (choice(rng) < update_percent)
            w.operations.push_back({true, index(rng), 0, value(rng)});
        else {
            int l = start(rng);
            w.operations.push_back({false, l, l + width, 0});
        }
    }
    return w;
}

void check(const Workload& w) {
    structA::GcdSegmentTree tree(w.initial);
    GcdReference reference(w.initial);
    for (std::size_t i = 0; i < w.operations.size(); ++i) {
        const auto& op = w.operations[i];
        if (op.update) {
            tree.assign(op.l, op.value);
            reference.assign(op.l, op.value);
        } else if (tree.query(op.l, op.r) != reference.query(op.l, op.r)) {
            throw std::runtime_error("GCD mismatch at operation " + std::to_string(i));
        }
    }
}

void countOperations(const Workload& w, int width, int update_percent, std::uint64_t seed) {
    structA::GcdSegmentTree tree(w.initial);
    tree.resetCounters();
    std::uint64_t checksum = 0;
    std::uint64_t queries = 0, updates = 0, oracle_elements = 0;
    for (const auto& op : w.operations) {
        if (op.update) {
            tree.assign(op.l, op.value);
            updates++;
        } else {
            checksum = checksum * 1315423911ULL + tree.query(op.l, op.r);
            oracle_elements += op.r - op.l;
            queries++;
        }
    }
    auto counts = tree.counters();
    std::cout << "n,operations,width,update_percent,seed,queries,updates,query_blocks,query_merges,update_visits,update_merges,euclid_remainders,oracle_elements_if_scanned,checksum\n";
    std::cout << w.initial.size() << ',' << w.operations.size() << ',' << width << ',' << update_percent << ','
              << seed << ',' << queries << ',' << updates << ',' << counts.queryBlocks << ',' << counts.queryMerges << ','
              << counts.updateVisits << ',' << counts.updateMerges << ',' << tree.euclidRemainderSteps() << ','
              << oracle_elements << ',' << checksum << '\n';
}

using Clock = std::chrono::steady_clock;

template<class Structure>
void measure(const char* name, const Workload& w, int width, int update_percent,
             std::uint64_t seed, int run) {
    auto before_build = Clock::now();
    Structure structure(w.initial);
    auto after_build = Clock::now();
    std::uint64_t checksum = 0;
    std::size_t queries = 0, updates = 0;
    for (const auto& op : w.operations) {
        if (op.update) {
            structure.assign(op.l, op.value);
            ++updates;
        } else {
            checksum = checksum * 1315423911ULL + structure.query(op.l, op.r);
            ++queries;
        }
    }
    auto after_replay = Clock::now();
    double build_ms = std::chrono::duration<double, std::milli>(after_build - before_build).count();
    double replay_ms = std::chrono::duration<double, std::milli>(after_replay - after_build).count();
    std::cout << name << ',' << w.initial.size() << ',' << w.operations.size() << ','
              << width << ',' << update_percent << ',' << seed << ',' << run << ','
              << queries << ',' << updates << ',' << std::fixed << std::setprecision(6)
              << build_ms << ',' << replay_ms << ',' << checksum << '\n';
}

int main(int argc, char** argv) {
    if (argc != 8) {
        std::cerr << "Usage: gcd_benchmark N OPERATIONS WIDTH UPDATE_PERCENT SEED RUNS MODE\n"
                     "MODE: check (both and verify), tree, oracle, or counts\n";
        return 1;
    }
    try {
        int n = std::stoi(argv[1]), count = std::stoi(argv[2]);
        int width = std::stoi(argv[3]), update_percent = std::stoi(argv[4]);
        std::uint64_t seed = std::stoull(argv[5]);
        int runs = std::stoi(argv[6]);
        std::string mode = argv[7];
        if (n < 1 || count < 1 || width < 1 || width > n || update_percent < 0 ||
            update_percent > 100 || runs < 1 || (mode != "check" && mode != "tree" && mode != "oracle" && mode != "counts"))
            throw std::invalid_argument("invalid workload parameters or mode");
        Workload workload = generate(n, count, width, update_percent, seed);
        if (mode == "counts") {
#ifndef STRUCTA_ENABLE_COUNTERS
            throw std::invalid_argument("Compile with -DSTRUCTA_ENABLE_COUNTERS for counts");
#else
            countOperations(workload, width, update_percent, seed);
            return 0;
#endif
        }
#ifdef STRUCTA_ENABLE_COUNTERS
        throw std::invalid_argument("Compile without STRUCTA_ENABLE_COUNTERS for timing");
#endif
        if (mode == "check") check(workload);
        std::cout << "structure,n,operations,width,update_percent,seed,run,queries,updates,build_ms,replay_ms,checksum\n";
        for (int run = 1; run <= runs; ++run) {
            if (mode != "oracle") measure<structA::GcdSegmentTree>("gcd_segment_tree", workload, width, update_percent, seed, run);
            if (mode != "tree") measure<GcdReference>("gcd_array_oracle", workload, width, update_percent, seed, run);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
