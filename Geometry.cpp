#include "Geometry.h"
#include "Engine.h"
#include<iostream>

#include <algorithm>

Rot angle(const Dot& a, const Dot& b) { return Rot{ atan2(a % b, a * b) }; }

Dot Dot::rotated(const Rot& rot) const {
    return (angle(dotRight, *this) + rot).vector() * len();
}

Dot Dot::local(const Transform& base) const {
    Dot res = Dot{ *this - base.pos };
    return res.rotated(-base.rot);
}

Dot Dot::unLocal(const Transform& base) const {
    return rotated(base.rot) + base.pos;
}

double polygon::Radius() const {
    if (r < 0) {
        for (auto& dot : dots) {
            r = std::max(r, (dot - Center()).len());
        }
    }
    return r;
}

Dot polygon::Center() const {
    if (center.len() < eps) {
        for (auto& dot : dots) {
            center += dot;
        }
        center = center / dots.size();
    }
    return center;

}

bool inAngle(Dot A, Dot B, Dot C, Dot p) {
    return ((C - B) % (A - B) > 0) == ((C - B) % (p - B) > 0);
}

bool polygon::Inside(Dot p) const {//Using binary search over angle
    int l = 1, r = dots.size() - 1;
    p = p.local(*transform);
    if (!inAngle(dots[l], dots[0], dots[r], p) || !inAngle(dots[r], dots[0], dots[l], p))
        return false;
    while (r - l > 1) {
        int m = (l + r) / 2;
        if (inAngle(dots[l], dots[0], dots[m], p)) {
            r = m;
        }
        else {
            l = m;
        }
    }
    return inAngle(dots[0], dots[l], dots[r], p);
}

bool box::Inside(Dot p) const {
    p = p.local(*transform).local(shift);
    if (abs(p.x) <= width / 2 && abs(p.y) <= height / 2) {
        return true;
    }
    return false;
}