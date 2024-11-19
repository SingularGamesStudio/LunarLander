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