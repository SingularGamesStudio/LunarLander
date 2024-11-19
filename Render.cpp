#include "Render.h"
#include "Engine.h"

#include<iostream>

void PolygonRenderer::IncrementalDraw(uint32_t color) {
    std::vector<std::pair<int, int>> toDel{};
    std::vector<std::pair<int, int>> toAdd{};
    int xLast = -1, yLast = -1;
    int yMinLast = -1, yMaxLast = -1;
    int yMin = -1;
    for (auto d : drawn) {
        int x = d.first;
        int y = d.second;
        if (xLast != -1 && (x != xLast || yLast == yMin || yLast >= yMaxLast || yLast <= yMinLast)) {
            if (!item->Inside(Dot(yLast, xLast))) {
                buffer[xLast][yLast] = 0;
                toDel.push_back({ xLast, yLast });
            }
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    std::pair<int, int> dot{ xLast + dx, yLast + dy };
                    if (dot.first < 0 || dot.second < 0 || dot.first >= SCREEN_HEIGHT || dot.second >= SCREEN_WIDTH)
                        continue;
                    if (item->Inside(Dot{ 1.0 * dot.second, 1.0 * dot.first })) {
                        buffer[dot.first][dot.second] = color;
                        toAdd.push_back(dot);
                    }
                }
            }
        }
        if (xLast != x) {
            yMinLast = yMin;
            yMaxLast = yLast;
            yMin = y;
        }
        xLast = x;
        yLast = y;
    }
    for (auto z : toDel) {
        drawn.erase(z);
    }
    for (auto z : toAdd) {
        drawn.insert(z);
    }

}

void PolygonRenderer::FullDraw(uint32_t color) {
    for (auto d : drawn) {
        if (!item->Inside(Dot(d.second, d.first))) {
            buffer[d.first][d.second] = 0;
        }
    }
    Dot centerNew = item->Center().unLocal(*item->transform);
    drawn.clear();
    int x0 = std::max(0.0, centerNew.x - item->Radius()), x1 = std::min(SCREEN_WIDTH - 1.0, centerNew.x + item->Radius());
    int y0 = std::max(0.0, centerNew.y - item->Radius()), y1 = std::min(SCREEN_HEIGHT - 1.0, centerNew.y + item->Radius());

    for (int i = x0; i <= x1; i++) {
        for (int j = y0; j <= y1; j++) {
            if (item->Inside(Dot(i, j))) {
                buffer[j][i] = color;
                drawn.insert({ j, i });
            }
        }
    }
}

void PolygonRenderer::Draw(uint32_t color, bool forceFull) {
    Dot centerOld = item->Center().unLocal(lastTransform);
    Dot centerNew = item->Center().unLocal(*item->transform);
    double shift = (centerNew - centerOld).len();//maximum global shift of any polygon point after last transform change
    shift += abs((item->transform->rot - lastTransform.rot).angle) * (item->Center().len() + item->Radius());

    if (shift < 1 && !forceFull) {
        IncrementalDraw(color);
    }
    else {
        FullDraw(color);
    }
    lastColor = color;
    lastTransform = *item->transform;
}
