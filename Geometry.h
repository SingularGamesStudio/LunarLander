#pragma once
#include <cmath>
#include <optional>
#include <utility>
#include <vector>
#include <unordered_set>

const double pi = acos(-1);
const double eps = 1e-9;

struct Transform;
struct Rot;

struct Dot {
    double x;
    double y;

    Dot() : x(0), y(0) {}
    Dot(const double x, const double y) : x(x), y(y) {}

    Dot operator+(const Dot& other) const {
        return Dot{ x + other.x, y + other.y };
    }
    Dot& operator+=(const Dot& other) {
        return *this = Dot{ x + other.x, y + other.y };
    }
    Dot operator-(const Dot& other) const {
        return Dot{ x - other.x, y - other.y };
    }
    Dot& operator-=(const Dot& other) {
        return *this = Dot{ x - other.x, y - other.y };
    }
    Dot operator-() const { return Dot{ -x, -y }; }
    Dot operator*(const double by) const { return Dot{ x * by, y * by }; }
    Dot operator/(const double by) const { return Dot{ x / by, y / by }; }
    /// dot product
    double operator*(const Dot& other) const {
        return x * other.x + y * other.y;
    }
    /// cross product
    double operator%(const Dot& other) const {
        return x * other.y - y * other.x;
    }
    /// dots are compared lexicografically
    bool operator<(const Dot& other) const {
        if (abs(x - other.x) > eps) {
            return x < other.x;
        }
        return y < other.y;
    }
    /// dots are compared lexicografically
    bool operator> (const Dot& other) const {
        if (abs(x - other.x) > eps) {
            return x > other.x;
        }
        return y > other.y;
    }

    double len() const { return sqrt(x * x + y * y); }
    double squareLen() const { return x * x + y * y; }

    Dot rotated(const Rot& rot) const;
    Dot norm() const { return Dot{ -y, x }; }
    /// Local position of dot in coordinates of base
    Dot local(const Transform& base) const;
    /// Global position of dot that is in coordinates of base
    Dot unLocal(const Transform& base) const;
};

const Dot dotRight = Dot{ 1, 0 };
const Dot dotUp = Dot{ 0, 1 };
const Dot dotZero = Dot{ 0, 0 };
const Dot dotMax = Dot{ INFINITY, INFINITY };
const Dot dotMin = Dot{ -INFINITY, -INFINITY };

/// Rotation
struct Rot {
    double angle;
    Rot() : angle() {}
    Rot(const double angle)
        : angle(angle - floor(angle / (2.0 * pi)) * 2.0 * pi) {
    }
    Rot operator+(const Rot& other) const { return Rot{ angle + other.angle }; }
    Rot& operator+=(const Rot& other) { return *this = Rot{ angle + other.angle }; }
    Rot operator-(const Rot& other) const { return Rot{ angle - other.angle }; }
    Rot& operator-=(const Rot& other) { return *this = Rot{ angle - other.angle }; }
    Rot operator-() const { return Rot{ -angle }; }
    Rot operator*(const double by) const { return Rot{ angle * by }; }
    Dot vector() const { return Dot{ cos(angle), sin(angle) }; }
};
/// Signed angle from direction a to direction b
Rot angle(const Dot& a, const Dot& b);

class Line {
protected:
    Dot p1;
    Dot p2;

public:
    Line() {}
    Line(Dot p1, Dot p2) : p1(p1), p2(p2) {}
    Line(Dot p1, double k) : p1(p1), p2(Dot(p1.x + 1, p1.y + k)) {}
    Line(double k, double b) : p1(Dot(0, b)), p2(Dot(1, k + b)) {}

    Dot project(const Dot& point) const {
        Dot diff = (p2 - p1);
        return p1 + diff * ((point - p1) * diff) / diff.len() / diff.len();
    }
    /// Signed distance from line beginning to point projection
    double projectLocal(const Dot& point) const {
        Dot proj = project(point);
        if (proj > p1) {
            return (proj - p1).len();
        }
        else {
            return -(proj - p1).len();
        }
    }

    Line norm() const {
        return Line(p1, p1 + (p2 - p1).norm());
    }
    /// Angle coefficient of the line 
    double k() const {
        Dot temp = p2 - p1;
        return abs(temp.x) / (abs(temp.y) + eps);
    }
    /// Point on line with given y
    Dot get(double y) {
        if (abs((p2 - p1).y) < eps) {
            return Dot(-1, y);
        }
        return Dot(p1.x + (y - p1.y) * k(), y);
    }
};
/// Position and rotation of an object
struct Transform {
    Dot pos;
    Rot rot;
    Transform() : pos(), rot(0) {}
    Transform(const Dot& pos, const Rot& rot) : pos(pos), rot(rot) {}
};

struct polygon {
    Transform* transform;
    std::vector<Dot> dots;

    polygon() : transform(), dots() {}
    polygon(Transform* transform, std::vector<Dot> dots)
        : transform(transform), dots(dots) {
    }

    virtual bool Inside(Dot p) const;

protected:// used for collider and draw area estimation
    mutable Dot center{ 0, 0 };
    mutable double r = -1;
public:
    double Radius() const;
    Dot Center() const;
};

struct box : public polygon {
    double width, height;
    Transform shift;

    box(Transform* transform, double w, double h, Transform shift) : width(w), height(h), shift(shift) {
        std::vector<Dot> dots;
        for (auto delta : std::vector<std::pair<int, int>>{ {1, 1}, {1, -1}, {-1, -1}, {-1, 1} }) {
            Dot dot = Dot{ delta.first * w / 2, delta.second * h / 2 };
            dots.push_back(dot.unLocal(shift));
        }
        this->transform = transform;
        this->dots = dots;
    }

    virtual bool Inside(Dot p) const override;
};
/// Depth of intersection (negative if none), and normal line
std::pair<double, Line> intersect(const polygon& a, const polygon& b);