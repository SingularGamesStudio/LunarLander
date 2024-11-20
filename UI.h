#pragma once
#include "Constants.h"
#include "Engine.h"
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
    static void start(string name, std::pair<int, int > pos, int scale = 1) {
        instance().pos[name] = pos;
        instance().scale[name] = scale;
        instance().value[name] = "";
    }
    static void _write(string name, string value, uint32_t color) {
        std::pair<int, int> cur = instance().pos[name];
        int y0 = cur.second;
        int scale = instance().scale[name];
        for (char c : value) {
            if (c == '\n') {
                cur.second = y0;
                cur.first += LETTER_H * scale + 3;
                continue;
            }
            for (int i = 0; i < LETTER_H; i++) {
                for (int j = 0; j < LETTER_W; j++) {
                    if (((letters[c][i] >> j) & 1) == 0)
                        continue;
                    for (int di = 0; di < scale; di++) {
                        for (int dj = 0; dj < scale; dj++) {
                            buffer[cur.first + i * scale + di][cur.second + j * scale + dj] = color;
                        }
                    }
                }
            }
            cur.second += LETTER_W * scale + 1;
        }
    }
    static void write(string name, string value, uint32_t color) {
        _write(name, instance().value[name], 0);
        instance().value[name] = value;
        _write(name, value, color);
    }
};