#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;
using Value = unsigned long long;

unsigned long long gcd_merges = 0, remainder_steps = 0, max_merges = 0;

struct MaxCount {
    long long maximum;
    int count;
    bool operator==(const MaxCount& other) const {
        return maximum == other.maximum && count == other.count;
    }
};

struct GcdMerge {
    Value operator()(Value a, Value b) const {
        ++gcd_merges;
        while (b != 0) {
            ++remainder_steps;
            Value remainder = a % b;
            a = b;
            b = remainder;
        }
        return a;
    }
};

struct MaxMerge {
    MaxCount operator()(MaxCount a, MaxCount b) const {
        ++max_merges;
        if (a.maximum > b.maximum) return a;
        if (b.maximum > a.maximum) return b;
        return {a.maximum, a.count + b.count};
    }
};

template<class T, class Merge>
class SegmentTree {
    int base = 1;
    T identity;
    Merge merge;
    vector<T> tree;
public:
    SegmentTree(const vector<T>& values, T empty) : identity(empty) {
        while (base < static_cast<int>(values.size())) base *= 2;
        tree.assign(2 * base, identity);
        for (int i = 0; i < static_cast<int>(values.size()); ++i)
            tree[base + i] = values[i];
        for (int i = base - 1; i > 0; --i)
            tree[i] = merge(tree[2*i], tree[2*i+1]);
    }

    void assign(int index, T value) {
        int position = base + index;
        tree[position] = value;
        while (position > 1) {
            position /= 2;
            tree[position] = merge(tree[2*position], tree[2*position+1]);
        }
    }

    bool valid() const {
        for (int i = 1; i < base; ++i)
            if (!(tree[i] == merge(tree[2*i], tree[2*i+1]))) return false;
        return true;
    }

    T query(int left, int right) const {
        T first = identity, last = identity;
        left += base;
        right += base;
        while (left < right) {
            if (left & 1) first = merge(first, tree[left++]);
            if (right & 1) last = merge(tree[--right], last);
            left /= 2;
            right /= 2;
        }
        return merge(first, last);
    }
};

struct Sensor {
    long long reading;
    Value calibration;
};

struct Answer {
    long long maximum;
    int count;
    Value gcd;
};

class SensorReplay {
    int size;
    SegmentTree<MaxCount, MaxMerge> readings;
    SegmentTree<Value, GcdMerge> calibrations;

    static vector<MaxCount> reading_values(const vector<Sensor>& sensors) {
        vector<MaxCount> values;
        for (const Sensor& sensor : sensors) values.push_back({sensor.reading, 1});
        return values;
    }
    static vector<Value> calibration_values(const vector<Sensor>& sensors) {
        vector<Value> values;
        for (const Sensor& sensor : sensors) values.push_back(sensor.calibration);
        return values;
    }
public:
    explicit SensorReplay(const vector<Sensor>& sensors)
        : size(static_cast<int>(sensors.size())),
          readings(reading_values(sensors), {-1, 0}),
          calibrations(calibration_values(sensors), 0) {}

    void assign(int index, Sensor sensor) {
        if (index < 0 || index >= size) throw runtime_error("Invalid index");
        if (sensor.reading < 0 || sensor.reading > 1000000000 ||
            sensor.calibration > 1000000000)
            throw runtime_error("Values must be between 0 and 1000000000");
        readings.assign(index, {sensor.reading, 1});
        calibrations.assign(index, sensor.calibration);
    }

    bool valid() const {
        return readings.valid() && calibrations.valid();
    }

    Answer query(int left, int right) const {
        if (left < 0 || left > right || right > size)
            throw runtime_error("Invalid range");
        MaxCount result = readings.query(left, right);
        return {result.maximum, result.count, calibrations.query(left, right)};
    }
};

Sensor read_sensor() {
    long long reading, calibration;
    if (!(cin >> reading >> calibration) || reading < 0 || reading > 1000000000 ||
        calibration < 0 || calibration > 1000000000)
        throw runtime_error("Expected reading and calibration in [0, 1000000000]");
    return {reading, static_cast<Value>(calibration)};
}

int replay() {
    int n, operations;
    if (!(cin >> n >> operations) || n < 0 || n > 1000000 || operations < 0)
        throw runtime_error("Invalid array size or operation count");
    vector<Sensor> sensors(n);
    for (Sensor& sensor : sensors) sensor = read_sensor();
    SensorReplay application(sensors);
    for (int step = 0; step < operations; ++step) {
        char operation;
        if (!(cin >> operation)) throw runtime_error("Missing operation");
        if (operation == 'U') {
            int index;
            if (!(cin >> index)) throw runtime_error("Missing index");
            Sensor sensor = read_sensor();
            application.assign(index, sensor);
        } else if (operation == 'Q') {
            int left, right;
            if (!(cin >> left >> right)) throw runtime_error("Missing range");
            Answer result = application.query(left, right);
            cout << result.maximum << ' ' << result.count << ' ' << result.gcd << '\n';
        } else {
            throw runtime_error("Use U for assignment or Q for query");
        }
    }
    return 0;
}

unsigned long long next_random(unsigned long long& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state;
}

Value oracle_gcd(Value a, Value b) {
    while (b != 0) {
        Value remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

Answer scan(const vector<Sensor>& sensors, int left, int right) {
    Answer result{-1, 0, 0};
    for (int i = left; i < right; ++i) {
        if (sensors[i].reading > result.maximum) {
            result.maximum = sensors[i].reading;
            result.count = 1;
        } else if (sensors[i].reading == result.maximum) {
            ++result.count;
        }
        result.gcd = oracle_gcd(result.gcd, sensors[i].calibration);
    }
    return result;
}

void check_range(const vector<Sensor>& sensors, const SensorReplay& app,
                 int left, int right, bool audit = true) {
    if (audit && !app.valid()) throw runtime_error("Internal node invariant failed");
    Answer actual = app.query(left, right);
    Answer expected = scan(sensors, left, right);
    if (actual.maximum != expected.maximum || actual.count != expected.count ||
        actual.gcd != expected.gcd)
        throw runtime_error("Array comparison failed for range " +
                            to_string(left) + " " + to_string(right));
}

int tests(int operations) {
    SensorReplay boundary_check({{10, 6}, {99, 35}, {7, 10}});
    Answer single = boundary_check.query(0, 1);
    if (single.maximum != 10 || single.count != 1 || single.gcd != 6)
        throw runtime_error("Left-inclusive, right-exclusive boundary test failed");
    Answer empty = boundary_check.query(1, 1);
    if (empty.maximum != -1 || empty.count != 0 || empty.gcd != 0)
        throw runtime_error("Empty range boundary test failed");
    Answer tail = boundary_check.query(2, 3);
    if (tail.maximum != 7 || tail.count != 1 || tail.gcd != 10)
        throw runtime_error("Right boundary at array size failed");

    for (int n = 0; n <= 17; ++n) {
        vector<Sensor> sensors(n, {0, 0});
        SensorReplay app(sensors);
        for (int step = 0; step <= n; ++step) {
            for (int left = 0; left <= n; ++left)
                for (int right = left; right <= n; ++right)
                    check_range(sensors, app, left, right);
            if (step < n) {
                sensors[step] = {7, static_cast<Value>(step % 2 ? 89 : 144)};
                app.assign(step, sensors[step]);
            }
        }
        for (int i = 0; i < n; ++i) {
            sensors[i] = {0, 0};
            app.assign(i, sensors[i]);
            for (int left = 0; left <= n; ++left)
                for (int right = left; right <= n; ++right)
                    check_range(sensors, app, left, right);
        }
    }
    vector<Sensor> limits{{1000000000, 1000000000}, {1000000000, 0}, {0, 1}};
    SensorReplay limit_app(limits);
    check_range(limits, limit_app, 0, 3);
    limit_app.assign(0, {0, 0});
    limits[0] = {0, 0};
    check_range(limits, limit_app, 0, 3);
    unsigned long long seed = 253;
    const int n = 37;
    vector<Sensor> sensors(n, {0, 0});
    SensorReplay app(sensors);
    for (int step = 0; step < operations; ++step) {
        unsigned long long choice = next_random(seed);
        if ((choice >> 32) % 2) {
            int index = static_cast<int>((next_random(seed) >> 32) % n);
            long long reading = static_cast<long long>((next_random(seed) >> 32) % 13);
            Value calibration = (next_random(seed) >> 32) % 1001;
            sensors[index] = {reading, calibration};
            app.assign(index, sensors[index]);
        } else {
            int left = static_cast<int>((next_random(seed) >> 32) % (n+1));
            int right = static_cast<int>((next_random(seed) >> 32) % (n+1));
            if (left > right) swap(left, right);
            check_range(sensors, app, left, right);
        }
    }
    bool rejected = false;
    try { app.assign(n, {1, 1}); } catch (const exception&) { rejected = true; }
    if (!rejected) throw runtime_error("Invalid index was accepted");
    rejected = false;
    try { app.query(2, 1); } catch (const exception&) { rejected = true; }
    if (!rejected) throw runtime_error("Invalid range was accepted");
    rejected = false;
    try { app.assign(0, {-1, 1}); } catch (const exception&) { rejected = true; }
    if (!rejected) throw runtime_error("Negative reading was accepted");
    cout << "PASS: edge cases and " << operations << " seeded operations, seed=253\n";
    return 0;
}

int scale_run() {
    const int n = 1000000, operations = 1000000;
    unsigned long long seed = 253;
    vector<Sensor> sensors(n);
    for (Sensor& sensor : sensors) {
        sensor.reading = static_cast<long long>((next_random(seed) >> 32) % 1000000001);
        sensor.calibration = (next_random(seed) >> 32) % 1000000001;
    }
    SensorReplay app(sensors);
    Value checksum = 0;
    int updates = 0, queries = 0;
    for (int step = 0; step < operations; ++step) {
        if (step % 2 == 0) {
            int index = static_cast<int>((next_random(seed) >> 32) % n);
            long long reading = static_cast<long long>((next_random(seed) >> 32) % 1000000001);
            Value calibration = (next_random(seed) >> 32) % 1000000001;
            Sensor value{reading, calibration};
            app.assign(index, value);
            sensors[index] = value;
            ++updates;
        } else {
            int left = static_cast<int>((next_random(seed) >> 32) % n);
            int width = 1 + static_cast<int>((next_random(seed) >> 32) % (n-left));
            Answer result = app.query(left, left + width);
            checksum = checksum * 31 + static_cast<Value>(result.maximum) +
                       static_cast<Value>(result.count) + result.gcd;
            if (queries % 10000 == 0) check_range(sensors, app, left, left + width, false);
            ++queries;
        }
    }
    cout << "elements,operations,updates,queries,seed,checksum\n";
    cout << n << ',' << operations << ',' << updates << ',' << queries
         << ",253," << checksum << '\n';
    return 0;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    try {
        int result;
        if (argc == 1) result = replay();
        else if (argc == 2 && string(argv[1]) == "--test") result = tests(1000000);
        else if (argc == 2 && string(argv[1]) == "--scale") result = scale_run();
        else throw runtime_error("Usage: sensor_replay [--test|--scale]");
        if (argc > 1) {
            cerr << "gcd_merges=" << gcd_merges << " remainder_steps=" << remainder_steps
                 << " max_merges=" << max_merges << '\n';
        }
        return result;
    } catch (const exception& error) {
        cerr << error.what() << '\n';
        return 1;
    }
}
