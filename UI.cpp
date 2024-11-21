#include "UI.h"
#include "RocketScience.h"
#include "Timer.h"
#include<queue>

void UI::_write(string name, string value, uint32_t color) {
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

void UI::write(string name, string value, uint32_t color) {
    _write(name, instance().value[name], 0);
    instance().value[name] = value;
    _write(name, value, color);
}

void UI::start(string name, std::pair<int, int > pos, int scale) {
    instance().pos[name] = pos;
    instance().scale[name] = scale;
}

namespace Layout {
    const std::pair<int, int> RocketPreviewPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    const int RocketPreviewWidth = 400;
    const std::pair<int, int> RocketHalfSize = { 50, 100 };
    const std::pair<int, int> LevelSelectPos = { SCREEN_WIDTH - 200, 8 * 11 };
    const std::pair<int, int> ControlsPos = { 0, 600 };
}

void DrawMenu() {
    if (global::objects.empty()) {
        size_t cnt = global::rocketBuilders.size();
        for (size_t i = 0; i < cnt; i++) {
            Transform pos = Transform(Dot(Layout::RocketPreviewPos.first - Layout::RocketPreviewWidth / 2 + Layout::RocketPreviewWidth * (int)i / ((int)cnt - 1), Layout::RocketPreviewPos.second), 0);
            Object* obj = global::rocketBuilders[i].second(pos);
            obj->physicsLocked = true;
        }
    }
    UI::start("title", { 0, 25 }, 10);
    UI::write("title", "LUNAR LANDER", colors::white);
    size_t cnt = global::rocketBuilders.size();
    for (size_t i = 0; i < cnt; i++) {
        Dot pos = Dot(Layout::RocketPreviewPos.first - Layout::RocketPreviewWidth / 2 + Layout::RocketPreviewWidth * (int)i / ((int)cnt - 1) - Layout::RocketHalfSize.first,
            Layout::RocketPreviewPos.second + Layout::RocketHalfSize.second);
        UI::start("rocketName" + std::to_string(i), { pos.y, pos.x }, 2);
        uint32_t color = colors::white;
        if (global::rocketChoice == i) {
            color = colors::yellow;
            UI::write("rocketPointer", "", color);
            UI::start("rocketPointer", { pos.y + 2 * 8, pos.x + Layout::RocketHalfSize.first - 8 }, 2);
            UI::write("rocketPointer", "^", color);
        }
        UI::write("rocketName" + std::to_string(i), global::rocketBuilders[i].first, color);
    }
    cnt = global::levelBuilders.size();
    for (size_t i = 0; i < cnt; i++) {
        Dot pos = Dot(Layout::LevelSelectPos.first,
            Layout::LevelSelectPos.second + (int)i * (2 * 8 + 2));
        UI::start("levelName" + std::to_string(i), { pos.y, pos.x }, 2);
        uint32_t color = colors::white;
        if (global::levelChoice == i) {
            color = colors::yellow;
            UI::write("levelPointer", "", color);
            UI::start("levelPointer", { pos.y, pos.x - (2 * 8 + 2) }, 2);
            UI::write("levelPointer", ">", color);
        }
        UI::write("levelName" + std::to_string(i), global::levelBuilders[i].first, color);
    }
    UI::start("controls", { Layout::ControlsPos.second, Layout::ControlsPos.first }, 3);
    UI::write("controls", "    Arrow keys to select rocket/level\n              ESC to exit\n\n           PRESS SPACE TO START", colors::white);
}
void ClearMenu() {
    UI::write("title", "", colors::white);
    UI::write("levelPointer", "", colors::white);
    UI::write("rocketPointer", "", colors::white);
    UI::write("controls", "", colors::white);
    size_t cnt = global::rocketBuilders.size();
    for (size_t i = 0; i < cnt; i++) {
        UI::write("rocketName" + std::to_string(i), "", colors::white);
    }
    cnt = global::levelBuilders.size();
    for (size_t i = 0; i < cnt; i++) {
        UI::write("levelName" + std::to_string(i), "", colors::white);
    }
}

void RocketState(Object* rocket) {
    bool done = false;
    for (Controlled* cont : global::controls) {
        if (cont->active) {
            UI::write("rocket state", "Thrusters enabled", colors::white);
            Timer::start("/land");
            done = true;
            break;
        }
    }
    static std::queue<bool> onAir = {};
    static int onAirCount = 0;
    static std::vector<Collision> hits;//TODO:
    if (onAir.size() > 9) {
        if (onAir.front())
            onAirCount--;
        onAir.pop();
    }
    onAir.push(rocket->collisions.empty());
    if (onAir.back()) {
        onAirCount++;
    }
    else {
        hits = rocket->collisions;
    }
    if (!done) {
        if (rocket->velocity.len() > 5 || abs(rocket->rotationSpeed) > 5) {
            Timer::start("/land");
            UI::write("rocket state", "Rocket unstable", colors::white);
            return;
        }
        if (onAirCount >= 10) {
            Timer::start("/land");
            UI::write("rocket state", "Rocket in the air", colors::white);
            return;
        }
        int score = 0;
        for (auto& coll : hits) {
            if (coll.hit == nullptr)
                continue;
            if (coll.hit->score == 0) {
                Timer::start("/land");
                UI::write("rocket state", "Unsuitable terrain for landing", colors::white);
                return;
            }
            else {
                score += coll.hit->score;
            }
        }
        if (Timer::elapsed("/land") < 5) {
            UI::write("rocket state", std::format("LANDING COMMENCING: {:.3f}", 5 - Timer::elapsed("/land")), colors::white);
        }
        else {
            UI::write("rocket state", std::format("ROCKET LANDED, SCORE: {}", score), colors::white);
        }
    }
}