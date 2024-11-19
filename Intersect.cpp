#include "Geometry.h"
#include<iostream>

#include <algorithm>

double SeparatingAxis(std::vector<Dot>& aDots, std::vector<Dot>& bDots, const Line& axis) {
    Dot aMin = dotMax, aMax = dotMin, bMin = dotMax, bMax = dotMin;
    for (auto& dot : aDots) {
        aMin = std::min(aMin, axis.project(dot));
        aMax = std::max(aMax, axis.project(dot));
    }
    for (auto& dot : bDots) {
        bMin = std::min(bMin, axis.project(dot));
        bMax = std::max(bMax, axis.project(dot));
    }
    if (aMax < bMin) {
        return -(bMin - aMax).len();
    }
    if (bMax < aMin) {
        return -(aMin - bMax).len();
    }
    return std::min((bMin - aMax).len(), (aMin - bMax).len());
}

bool cmpAngle(const Line& a, const Line& b) { return a.k() < b.k(); }
bool eqAngle(const Line& a, const Line& b) { return (a.k() - b.k()) < eps; }
/// <summary>
/// returns minimal intersection of projections, and axis of projection.
/// returns a negative number if there the polygons do not intersect.
/// </summary>
std::pair<double, Line> intersect(const polygon& a, const polygon& b) {  // Using Separating Axis Theorem
    if (a.Radius() + b.Radius() < (a.Center().unLocal(*a.transform) - b.Center().unLocal(*b.transform)).len()) {
        return { -1, Line{} };
    }
    std::vector<Dot> aDots = std::vector<Dot>(a.dots.size());
    std::vector<Dot> bDots = std::vector<Dot>(b.dots.size());
    for (int i = 0; i < a.dots.size(); i++) {
        aDots[i] = a.dots[i].unLocal(*a.transform);
    }
    for (int i = 0; i < b.dots.size(); i++) {
        bDots[i] = b.dots[i].unLocal(*b.transform);
    }

    std::vector<Line> normals{};
    normals.push_back(
        Line{ aDots[0], aDots[0] + (aDots[0] - aDots.back()).norm() });
    for (int i = 1; i < aDots.size(); i++) {
        normals.push_back(
            Line{ aDots[i], aDots[i] + (aDots[i] - aDots[i - 1]).norm() });
    }
    normals.push_back(
        Line{ bDots[0], bDots[0] + (bDots[0] - bDots.back()).norm() });
    for (int i = 1; i < aDots.size(); i++) {
        normals.push_back(
            Line{ bDots[i], bDots[i] + (bDots[i] - bDots[i - 1]).norm() });
    }
    std::sort(normals.begin(), normals.end(), cmpAngle);  // Only unique axes
    normals.erase(std::unique(normals.begin(), normals.end(), eqAngle),
        normals.end());

    double minDist = INFINITY;
    Line bestAxis;
    for (Line& axis : normals) {
        double cur = SeparatingAxis(aDots, bDots, axis);
        if (cur < minDist) {
            minDist = cur;
            bestAxis = axis;
        }
        if (cur < -eps) {  // TODO:
            break;
        }
    }
    return { minDist, bestAxis };
}

bool box::Inside(Dot p) const {
    p = p.local(*transform).local(shift);
    if (abs(p.x) <= width / 2 && abs(p.y) <= height / 2) {
        return true;
    }
    return false;
}