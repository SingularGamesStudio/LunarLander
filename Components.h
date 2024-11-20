#pragma once
#include"Object.h"
#include "Windows.h"
#include<format>

struct PolyCollider : public Component {
    polygon* shape = nullptr;

    //the collider has a score, if the rocket lands on it, the score is equal to multiplication of the contacting colliders.
    int score = 1;

    PolyCollider() {}
    PolyCollider(Object* parent) : Component(parent) {}
    PolyCollider(Object* parent, polygon* shape) : shape(shape), Component(parent) {}

    virtual ~PolyCollider() {
        delete shape;
    }
};

PolyCollider* newBoxCollider(Object* parent, Transform shift, int w, int h);
/// Shifts colliders so they do not intersect, applies rule of moments, and saves collisions in object
void _resolveCollision(PolyCollider* a, PolyCollider* b, Dot aShift, Dot at, double damping = 0.3, bool second = false);
/// Shifts colliders so they do not intersect, applies rule of moments, and saves collisions in object
void resolveCollision(PolyCollider* a, PolyCollider* b, Line norm);

/// PolygonCollider that could be rendered
struct PolygonRenderer : PolyCollider, Drawable {
    uint32_t color = 0;

    PolygonRenderer() {}
    PolygonRenderer(Object* parent) : PolyCollider(parent) {}
    PolygonRenderer(Object* parent, polygon* shape) : PolyCollider(parent, shape) {}

protected:
    bool init = false;
    uint32_t lastColor = 1;
    Transform lastTransform{};
    std::set<std::pair<int, int>> drawn{};

    void IncrementalDraw(uint32_t color);
    void FullDraw(uint32_t color);
public:
    void Draw(uint32_t color, bool forceFull = false);
    virtual void Draw(bool forceFull = false) override {
        Draw(color, forceFull);
    }
    virtual void Clear() override;
};

PolygonRenderer* newBoxRenderer(Object* parent, Transform shift, int w, int h, uint32_t color);

/// Applies constant force if button is pressed. Also has visual effects.
struct Thruster : public Controlled, Drawable, Component {
    Dot shift;
    Dot force;
protected:
    PolygonRenderer visual;
public:
    Thruster() {}
    Thruster(Object* parent) : Component(parent) {}
    Thruster(Object* parent, Dot shift, Dot force) : Component(parent), shift(shift), force(force) {
        polygon* poly = new polygon{ &(parent->transform), std::vector<Dot>{shift - force * 2, shift + force.norm() / 3, shift - force.norm() / 3} };
        visual = PolygonRenderer{ parent };
        visual.shape = poly;
        visual.color = 0xffa200;
    }

    virtual void activate() override;
    virtual void stop() override;
    virtual void Draw(bool forceFull = false) override;
    virtual void Clear() override;
};

