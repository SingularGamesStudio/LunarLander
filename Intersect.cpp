#include "Geometry.h"
#include "Timer.h"
#include "Components.h"
#include<iostream>
#include<format>
#include <algorithm>
#include <chrono>

using std::min, std::max;
/// Calculate minimal and maximal positions of projection on axis. Returns two max/min, if <two>=true
void minMaxProjection(std::vector<Dot> dots, int dotsCount, Line axis, std::pair<double, Dot>* min, std::pair<double, Dot>* max, bool two = false) {
    for (int i = 0; i < dotsCount; i++) {
        Dot dot = dots[i];
        double projPos = axis.projectLocal(dot);
        if (projPos < min[0].first) {
            if (two) {
                min[1] = min[0];
            }
            min[0] = { projPos, dot };
        }
        else if (two && projPos < min[1].first) {
            min[1] = { projPos, dot };
        }
        if (projPos > max[0].first) {
            if (two) {
                max[1] = max[0];
            }
            max[0] = { projPos, dot };
        }
        else if (two && projPos > max[1].first) {
            max[1] = { projPos, dot };
        }
    }
}
/// Check whether exists a separating axis between a and b, normal to <axis>
double SeparatingAxis(std::vector<Dot>& aDots, int aCount, std::vector<Dot>& bDots, int bCount, const Line& axis) {
    std::pair<double, Dot> aMin = { INFINITY, {} }, aMax = { -INFINITY, {} }, bMin = { INFINITY, {} }, bMax = { -INFINITY, {} };
    minMaxProjection(aDots, aCount, axis, &aMin, &aMax);
    minMaxProjection(bDots, bCount, axis, &bMin, &bMax);
    if (aMax.first < bMin.first) {
        return -(bMin.first - aMax.first);
    }
    if (bMax.first < aMin.first) {
        return -(aMin.first - bMax.first);
    }
    return min((aMax.first - bMin.first), (bMax.first - aMin.first));
}

bool cmpAngle(const Line& a, const Line& b) { return a.k() < b.k(); }
bool eqAngle(const Line& a, const Line& b) {
    return abs(a.k() - b.k()) < eps;
}

/// Depth of intersection (negative if none), and normal line
std::pair<double, Line> intersect(const Poly& a, const Poly& b) {  // Using Separating Axis Theorem
    Timer::start("short collision check");
    if (a.Radius() + b.Radius() < (a.Center().unLocal(*a.transform) - b.Center().unLocal(*b.transform)).len()) {
        Timer::stop("short collision check");//objects are too far
        return { -1, Line{} };
    }
    Timer::stop("short collision check");
    Timer::start("long collision check");
    static std::vector<Dot> aDots = std::vector<Dot>(15);//we do not have more than 15-edge polygons
    static std::vector<Dot> bDots = std::vector<Dot>(15);
    for (int i = 0; i < a.dots.size(); i++) {
        aDots[i] = a.dots[i].unLocal(*a.transform);
    }
    for (int i = 0; i < b.dots.size(); i++) {
        bDots[i] = b.dots[i].unLocal(*b.transform);
    }

    static std::vector<Line> normals(30);// we need only to check edges as axes
    int normalCount = 0;
    normals[normalCount++] = Line{ aDots[0], aDots[0] + (aDots[0] - aDots.back()).norm() };
    for (int i = 1; i < a.dots.size(); i++) {
        normals[normalCount++] = Line{ aDots[i], aDots[i] + (aDots[i] - aDots[i - 1]).norm() };
    }
    normals[normalCount++] = Line{ bDots[0], bDots[0] + (bDots[0] - bDots.back()).norm() };
    for (int i = 1; i < b.dots.size(); i++) {
        normals[normalCount++] = Line{ bDots[i], bDots[i] + (bDots[i] - bDots[i - 1]).norm() };
    }
    std::sort(normals.begin(), normals.end(), cmpAngle);  // Only unique axes
    normalCount = std::unique(normals.begin(), normals.end(), eqAngle) - normals.begin();

    double minDist = INFINITY;
    Line bestAxis;
    for (int i = 0; i < normalCount; i++) {
        double cur = SeparatingAxis(aDots, a.dots.size(), bDots, b.dots.size(), normals[i]);
        if (cur < minDist) {
            minDist = cur;
            bestAxis = normals[i];
        }
        if (cur < -eps) {
            break; //no intersection
        }
    }
    Timer::stop("long collision check");
    return { minDist, bestAxis };
}

/// Shifts colliders so they do not intersect, applies rule of moments, and saves collisions in object
void resolveCollision(PolyCollider* a, PolyCollider* b, Line norm) {
    Timer::start("collision point");
    static std::vector<Dot> aDots = std::vector<Dot>(15);
    static std::vector<Dot> bDots = std::vector<Dot>(15);
    for (int i = 0; i < a->shape->dots.size(); i++) {
        aDots[i] = a->shape->dots[i].unLocal(*a->shape->transform);
    }
    for (int i = 0; i < b->shape->dots.size(); i++) {
        bDots[i] = b->shape->dots[i].unLocal(*b->shape->transform);
    }
    std::pair<double, Dot> aMin[2] = { {INFINITY, {}}, {INFINITY, {}} }, aMax[2] = { { -INFINITY, {} }, { -INFINITY, {} } };
    minMaxProjection(aDots, a->shape->dots.size(), norm, aMin, aMax, true);
    std::pair<double, Dot> bMin[2] = { {INFINITY, {}}, {INFINITY, {}} }, bMax[2] = { { -INFINITY, {} }, { -INFINITY, {} } };
    minMaxProjection(bDots, b->shape->dots.size(), norm, bMin, bMax, true);
    if (aMax[0].first < bMin[0].first || bMax[0].first < aMin[0].first)
        return; //no collision
    if (aMax[0].first - bMin[0].first < bMax[0].first - aMin[0].first) {
        std::swap(a, b);//ensure that a is on top
        std::swap(aMax, bMax);
        std::swap(aMin, bMin);
        std::swap(aDots, bDots);
    }
    Dot aShift = (norm.project(bMax[0].second) - norm.project(aMin[0].second)) * 1.1; //if we shift a by this vector, there would be no collision
    Timer::stop("collision point");
    Timer::start("physics");
    if (abs(aMin[0].first - aMin[1].first) > eps) {
        _resolveCollision(a, b, aShift, aMin[0].second);
    }
    else if (abs(bMax[0].first - bMax[1].first) > eps) {
        _resolveCollision(a, b, aShift, bMax[0].second);
    }
    else {//collision of edges, replaced by two half-collisions at end points of the intersection
        std::vector<Dot>dots = { aMin[0].second, aMin[1].second, bMax[0].second, bMax[1].second };
        std::pair<double, Dot> Min[2] = { {INFINITY, {}}, {INFINITY, {}} }, Max[2] = { { -INFINITY, {} }, { -INFINITY, {} } };
        minMaxProjection(dots, 4, norm.norm(), Min, Max, true);
        _resolveCollision(a, b, aShift / 2, Min[1].second);
        _resolveCollision(a, b, aShift / 2, Max[1].second, true);
    }
    Timer::stop("physics");
}

double getRadius(Object* obj) {
    double r = 0;
    for (auto c : obj->components) {
        if (dynamic_cast<PolyCollider*>(c) != NULL) {
            for (Dot dot : dynamic_cast<PolyCollider*>(c)->shape->dots) {
                r = max(r, dot.len());
            }
        }
    }
    return r;
}
