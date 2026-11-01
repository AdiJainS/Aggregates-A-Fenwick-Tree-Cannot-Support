#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;
using Value = unsigned long long;

struct MaxCount {
    long long maximum;
    int count;
};

struct GcdMerge {
    Value operator()(Value a, Value b) const {
        while (b != 0) {
            Value remainder = a % b;
            a = b;
            b = remainder;
        }
        return a;
    }
};

struct MaxMerge {
    MaxCount operator()(MaxCount a, MaxCount b) const {
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

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    try {
        return replay();
    } catch (const exception& error) {
        cerr << error.what() << '\n';
        return 1;
    }
}
