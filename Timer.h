#pragma once
#include<unordered_map>
#include<chrono>
#include<string>
#include<format>
#include "Windows.h"

using std::string;

struct Timer {
    static Timer& instance() {
        static Timer inst;
        return inst;
    }

    std::unordered_map<string, std::chrono::duration<double>> value;
    std::unordered_map<string, std::chrono::system_clock::time_point> startTime;
    static void start(string name) {
        auto t0 = std::chrono::system_clock::now();
        if (instance().startTime[name] != std::chrono::system_clock::time_point{}) {
            instance().value[name] += t0 - instance().startTime[name];
        }
        instance().startTime[name] = std::chrono::system_clock::now();
    }
    static double elapsed(string name) {
        if (instance().startTime[name] == std::chrono::system_clock::time_point{}) {
            return -1;
        }
        return (std::chrono::system_clock::now() - instance().startTime[name]).count();
    }
    static void stop(string name) {
        auto t0 = std::chrono::system_clock::now();
        if (instance().startTime[name] == std::chrono::system_clock::time_point{}) {
            return;
        }
        instance().value[name] += t0 - instance().startTime[name];
        instance().startTime[name] = {};
    }
    static void reset(string name) {
        instance().value[name] = {};
        instance().startTime[name] = {};
    }
    static double get(string name) {
        return instance().value[name].count();
    }

    static void print() {
        OutputDebugStringA("timers:\n");
        for (auto p : instance().value) {
            OutputDebugStringA(std::format("{}: {:.4f} ", p.first, p.second.count()).c_str());
        }
        OutputDebugStringA("\n");
    }
};