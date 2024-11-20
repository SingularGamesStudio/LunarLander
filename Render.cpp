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

void PolygonRenderer::Draw(uint32_t color, bool forceFull) {
    if (parent->physicsLocked) {
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