#pragma once
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono_literals;

bool profilerPrint = true;
int profilerThreshold = 0;

using timeus = std::chrono::time_point<std::chrono::steady_clock>;

class Profiler {
public:
    string name;
    timeus mid_time;
    timeus start_time;
    bool enabled;

    Profiler(string name_) {
        name = name_;
        start_time = std::chrono::steady_clock::now();
        mid_time = std::chrono::steady_clock::now();
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

    void midLog(string mid) {
        if (!enabled)
            return;
        timeus now = std::chrono::steady_clock::now();
        long long dt = std::chrono::duration_cast<std::chrono::microseconds>(now - mid_time).count();
        if (profilerPrint && (dt > profilerThreshold)) {
            printf("<%s,%s - %.3fms>\n", name.c_str(), mid.c_str(), dt / 1000.0);
        }
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