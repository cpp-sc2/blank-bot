#pragma once
#include <chrono>
#include <iostream>
#include <map>
#include <string>

using namespace std;
using namespace std::chrono_literals;

bool profilerPrint = true;
int profilerThreshold = 0;

#define lastX 10

struct lastfew {
    long long last[lastX] = {0};
    int arm = 0;

    void add(long long l) {
        last[arm] = l;
        arm++;
        if (arm == lastX)
            arm = 0;
    }

    long long time() {
        long long t = 0;
        for (int i = 0; i < lastX; i++)
            t += last[i];
        return t/10;
    }
};

using timeus = std::chrono::time_point<std::chrono::steady_clock>;
map<string, long long> profilerMap;
map<string, int> profilerCoumt;
map<string, lastfew> profilerLast;

class Profiler {
public:
    string name;
    timeus mid_time;
    timeus start_time;
    bool enabled;

    Profiler(string name_) {
        name = name_;
        start_time = std::chrono::steady_clock::now();
        mid_time = start_time;
        enabled = true;
    }

    void disable() {
        enabled = false;
    }

    void enable() {
        enabled = true;
    }

    void subScope() {
        if (!enabled)
            return;
        mid_time = std::chrono::steady_clock::now();
    }

    void addCall(string sname, long long dt) {
        if (profilerMap.find(sname) == profilerMap.end()) {
            profilerMap[sname] = dt;
            profilerCoumt[sname] = 1;
            profilerLast[sname].add(dt);
        }
        else {
            profilerMap[sname] += dt;
            profilerCoumt[sname] += 1;
            profilerLast[sname].add(dt);
        }
    }

    void midLog(string mid) {
        if (!enabled)
            return;
        timeus now = std::chrono::steady_clock::now();
        long long dt = std::chrono::duration_cast<std::chrono::microseconds>(now - mid_time).count();
        if (profilerPrint && (dt > profilerThreshold)) {
            printf("<%s,%s - %.3fms>\n", name.c_str(), mid.c_str(), dt / 1000.0);
        }
        addCall(mid, dt);
        mid_time = now;
    }

    ~Profiler() {
        if (!enabled)
            return;
        timeus now = std::chrono::steady_clock::now();
        long long dt = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time).count();
        if (profilerPrint && (dt > profilerThreshold)) {
            printf("<%s - %.3fms>\n", name.c_str(), dt / 1000.0);
        }
        addCall("-"+name, dt);
    }
};

//#pragma once
//#include <chrono>
//#include <iostream>
//
//using namespace std;
//using namespace std::chrono_literals;
//
//bool profilerPrint = true;
//int profilerThreshold = 0;
//
//class Profiler {
//public:
//    string name;
//    clock_t start_time;
//
//    Profiler(string name_) {
//        name = name_;
//        start_time = clock();
//    }
//
//    void midLog(string mid) {
//        clock_t now = clock();
//        if (profilerPrint && (now - start_time > profilerThreshold)) {
//            printf("<%s,%s - %lums>\n", name.c_str(), mid.c_str(), now - start_time);
//        }
//        start_time = now;
//    }
//
//    ~Profiler() {
//        clock_t now = clock();
//        if (profilerPrint && (now - start_time > profilerThreshold)) {
//            printf("<%s - %lums>\n", name.c_str(), now - start_time);
//        }
//    }
//};