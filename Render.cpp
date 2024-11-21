#include "Components.h"
#include "Engine.h"
#include "Timer.h"
#include<format>
#include<chrono>
#include<iostream>

using std::max, std::min;
/// Draws only perimeter of the polygon, shifted by no more than 1 pixel
void PolygonRenderer::IncrementalDraw(uint32_t color) {//TODO:rewrite
    std::vector<std::pair<int, int>> toDel{};
    std::vector<std::pair<int, int>> toAdd{};
    int xLast = -1, yLast = -1;
    int yMinLast = -1, yMaxLast = -1;
    int yMin = -1;

    auto checkPos = [&](int x, int y) {
        if (!shape->Inside(Dot(y, x))) {
            //if (buffer[x][y] == lastColor)
            buffer[x][y] = 0;
            toDel.push_back({ x, y });
        }
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                std::pair<int, int> dot{ x + dx, y + dy };
                if (dot.first < 0 || dot.second < 0 || dot.first >= SCREEN_HEIGHT || dot.second >= SCREEN_WIDTH)
                    continue;
                if (shape->Inside(Dot{ 1.0 * dot.second, 1.0 * dot.first })) {
                    buffer[dot.first][dot.second] = color;
                    toAdd.push_back(dot);
                }
            }
        }
        };

    for (auto d : drawn) {
        int x = d.first;
        int y = d.second;
        if (xLast != x) {
            checkPos(xLast, yLast);
            for (int i = yMinLast + 1; i < yMaxLast; i++) {
                if (i <= yMin || i >= yLast)
                    checkPos(x - 2, i);
            }
            yMinLast = yMin;
            yMaxLast = yLast;
            yMin = y;
        }
        if (y == yMin || y >= yMaxLast || y <= yMinLast) {
            checkPos(x, y);
        }
        xLast = x;
        yLast = y;
    }
    checkPos(xLast, yLast);
    for (int i = yMin + 1; i < yLast; i++) {
        if (i < yMaxLast && i > yMinLast) {//last line
            checkPos(xLast, i);
        }
    }

    for (auto z : toDel) {
        drawn.erase(z);
    }
    for (auto z : toAdd) {
        drawn.insert(z);
    }

}
/// Redraws full polygon, very slow
void PolygonRenderer::FullDraw(uint32_t color) {
    for (auto d : drawn) {
        if (!shape->Inside(Dot(d.second, d.first))) {
            if (buffer[d.first][d.second] == lastColor)
                buffer[d.first][d.second] = 0;
        }
    }
    Dot centerNew = shape->Center().unLocal(*shape->transform);
    drawn.clear();
    int x0 = max(0.0, centerNew.x - shape->Radius() * 1.1), x1 = min(SCREEN_WIDTH - 1.0, centerNew.x + shape->Radius() * 1.1);
    int y0 = max(0.0, centerNew.y - shape->Radius() * 1.1), y1 = min(SCREEN_HEIGHT - 1.0, centerNew.y + shape->Radius() * 1.1);

    for (int i = x0; i <= x1; i++) {
        for (int j = y0; j <= y1; j++) {
            if (shape->Inside(Dot(i, j))) {
                buffer[j][i] = color;
                drawn.insert({ j, i });
            }
        }
    }
}

void drawSegment(Dot a, Dot b, uint32_t color, std::set<std::pair<int, int>>* saveTo) {
    Dot dir = (b - a);
    double len = dir.len();
    dir = dir / len;
    Dot cur = a;
    for (int i = 0; i <= len; i++) {
        int y = (int)(cur.y + 0.5 - (cur.y < 0)), x = (int)(cur.x + 0.5 - (cur.x < 0));
        if (y >= 0 && x >= 0 && y < SCREEN_HEIGHT && x < SCREEN_WIDTH) {
            buffer[y][x] = color;
            if (saveTo != nullptr)
                saveTo->insert({ y, x });
        }
        cur += dir;
    }
}

/// Redraws full polygon, very slow
void PolygonRenderer::EdgeDraw(uint32_t color) {
    for (auto d : drawn) {
        if (buffer[d.first][d.second] == lastColor)
            buffer[d.first][d.second] = 0;
    }
    drawn.clear();
    Dot next, cur;
    for (int i = 0; i < shape->dots.size(); i++) {
        next = shape->dots[i].unLocal(parent->transform);
        if (i == 0) {
            cur = shape->dots[shape->dots.size() - 1].unLocal(parent->transform);
        }
        else {
            cur = shape->dots[i - 1].unLocal(parent->transform);
        }
        drawSegment(cur, next, color, &drawn);
    }
}

void PolygonRenderer::Draw(uint32_t color, bool forceFull, bool edgeOnly) {
    if (edgeOnly) {
        Timer::start("edge draw");
        EdgeDraw(color);
        Timer::stop("edge draw");
    } else if (parent->physicsLocked) {
        if (!init) {
            FullDraw(color);
            init = true;
        }
    }
    else {
        Dot centerOld = shape->Center().unLocal(lastTransform);
        Dot centerNew = shape->Center().unLocal(*shape->transform);
        double shift = (centerNew - centerOld).len();//maximum global shift of any polygon point after last transform change
        shift += abs((shape->transform->rot - lastTransform.rot).angle) * (shape->Center().len() + shape->Radius());
        auto timer = std::chrono::system_clock::now();
        if (shift < 1 && !forceFull && init) {
            Timer::start("incremental draw");
            IncrementalDraw(color);
            Timer::stop("incremental draw");
        }
        else {
            Timer::start("full draw");
            FullDraw(color);
            init = true;
            Timer::stop("full draw");
        }
    }
    lastColor = color;
    lastTransform = *shape->transform;
}

void PolygonRenderer::Clear() {
    for (auto d : drawn) {
        if (buffer[d.first][d.second] == lastColor)
            buffer[d.first][d.second] = 0;
    }
    init = false;
    drawn.clear();
}

void DrawDebugLine(Line l) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        Dot dot = l.get(i);
        if (dot.x >= 0 && dot.y >= 0 && dot.x < SCREEN_WIDTH && dot.y < SCREEN_HEIGHT) {
            buffer[(int)dot.y][(int)dot.x] = 255 * 256 * 256 + 255 * 256 + 255;
        }
    }
}