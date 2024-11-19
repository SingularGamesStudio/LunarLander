#pragma once
#include "Geometry.h"
#include <set>

struct PolygonRenderer {
    polygon* item;
protected:
    uint32_t lastColor = 1;
    Transform lastTransform{ Dot{-19999, 123123}, Rot(3.0 * pi / 4) };
    std::set<std::pair<int, int>> drawn{};

    void IncrementalDraw(uint32_t color);
    void FullDraw(uint32_t color);
public:
    void Draw(uint32_t color, bool forceFull);
};