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

void DrawRect(int y0, int x0, int h, int w, uint32_t color) {
    for (int i = y0; i < y0 + h; i++) {
        for (int j = x0; j < x0 + w; j++) {
            buffer[i][j] = color;
        }
    }
}

namespace Layout {
    const std::pair<int, int> WinScreenShift = { 200, 200 };
    const std::pair<int, int> EndTitlePos = { 250, 250 };
    const std::pair<int, int> EndStatsPos = { 400, 300 };
}

void DrawWinScreen(std::queue<std::pair<PolyCollider*, PolyCollider*>> hits) {
    global::state = "Victory";
    DrawRect(Layout::WinScreenShift.first, Layout::WinScreenShift.second, SCREEN_HEIGHT - Layout::WinScreenShift.first, SCREEN_WIDTH - Layout::WinScreenShift.second * 2, 0);
    int score = 100;
    string scoreExlp = "Base score: 100\n";
    score -= global::componentsDestroyed * 10;
    scoreExlp += std::format("Rocket parts destroyed: -{}x10\n", global::componentsDestroyed);
    score -= (int)Timer::elapsed("/game");
    scoreExlp += std::format("Time passed: -{}\nLanding:\n", (int)Timer::elapsed("/game"));
    while (!hits.empty()) {
        auto hit = hits.front();
        hits.pop();
        if (hit.first != nullptr && hit.second != nullptr) {
            score += hit.first->score * hit.second->score;
            scoreExlp += std::format("{}x{}: +{}x{}\n", hit.first->name, hit.second->name, hit.first->score, hit.second->score);
        }
    }
    scoreExlp += "\n      (space to exit)";
    UI::start("end", Layout::EndTitlePos, 8);
    UI::write("end", std::format("VICTORY!\nSCORE:{}", score), colors::white);
    UI::write("stats", "", colors::white);
    UI::start("stats", Layout::EndStatsPos, 1);
    UI::write("stats", scoreExlp, colors::white);
}

void DrawLoseScreen() {
    global::state = "Victory";
    DrawRect(Layout::WinScreenShift.first, Layout::WinScreenShift.second, SCREEN_HEIGHT - Layout::WinScreenShift.first, SCREEN_WIDTH - Layout::WinScreenShift.second * 2, 0);
    string scoreExlp = "\n\n\n\n\n\n          (space to exit)";
    UI::start("end", Layout::EndTitlePos, 8);
    UI::write("end", " DEFEAT!\n ROCKET\nDESTROYED", colors::white);
    UI::write("stats", "", colors::white);
    UI::start("stats", Layout::EndStatsPos, 1);
    UI::write("stats", scoreExlp, colors::white);
}

void RocketState(Object* rocket) {
    if (rocket->aliveCounter <= 0) {
        DrawLoseScreen();
        return;
    }
    bool done = false;
    for (Controlled* cont : global::controls) {
        if (cont->active) {
            UI::write("rocket state", "Thrusters enabled", colors::white);
            Timer::start("/land");
            done = true;
            break;
        }
    }
    static std::queue<int> onAir = {};
    static int onAirCount = 0;
    static std::queue<std::pair<PolyCollider*, PolyCollider*>> hits;
    if (onAir.size() > 9) {
        if (onAir.front() != 0) {
            for (int i = 0; i < onAir.front(); i++) {
                hits.pop();
            }
        }
        else onAirCount--;
        onAir.pop();
    }
    onAir.push(rocket->collisions.size());
    if (onAir.back() == 0) {
        onAirCount++;
    }
    else {
        for (auto& coll : rocket->collisions) {
            hits.push(coll.what);
            if (coll.what.first == nullptr || coll.what.second == nullptr)
                continue;
            if (!done && coll.what.first->score * coll.what.second->score == 0) {
                Timer::start("/land");
                UI::write("rocket state", "Unsuitable terrain for landing", colors::white);
                done = true;
            }
        }
    }
    if (!done) {
        if (rocket->velocity.len() > velWinTreshold || abs(rocket->rotationSpeed) > rotWinTreshold) {
            Timer::start("/land");
            UI::write("rocket state", "Rocket unstable", colors::white);
            return;
        }
        if (onAirCount >= 10) {
            Timer::start("/land");
            UI::write("rocket state", "Rocket in the air", colors::white);
            return;
        }

        if (Timer::elapsed("/land") < 3) {
            UI::write("rocket state", std::format("LANDING COMMENCING: {:.3f}", 5 - Timer::elapsed("/land")), colors::white);
        }
        else {
            DrawWinScreen(hits);
        }
    }
}