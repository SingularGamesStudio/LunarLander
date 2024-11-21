#pragma once
#include "Constants.h"
#include "Engine.h"
#include "Object.h"
#include<unordered_map>
#include<string>

using std::string;

struct UI {
    static UI& instance() {
        static UI inst;
        return inst;
    }

    std::unordered_map<string, std::pair<int, int>> pos;
    std::unordered_map<string, int> scale;
    std::unordered_map<string, string> value;
    static void start(string name, std::pair<int, int > pos, int scale = 1);
    static void _write(string name, string value, uint32_t color);
    static void write(string name, string value, uint32_t color);
};

void DrawMenu();

void ClearMenu();

void RocketState(Object* rocket);

void DrawDebugLine(Line l);