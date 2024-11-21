#pragma once
#include"Object.h"
#include "Windows.h"
#include<format>

template <>
struct std::hash<std::pair<int, int>> {
    inline size_t operator()(const std::pair<int, int>& ptr) const {
        return ptr.first * ptr.second;
    }
};

struct PolyCollider : public Component {
    Poly* shape = nullptr;

    int score = 1;//the collider has a score, if the rocket lands on it, the score is equal to multiplication of the contacting colliders.
    string name = "";//for scoring purposes
    int hp = 1;
    double mass = 1;

    PolyCollider() {}
    PolyCollider(Object* parent) : Component(parent) {}
    PolyCollider(Object* parent, Poly* shape) : shape(shape), Component(parent) {}

    void setUpGameProperties(int score, int hp, double mass, const string& name) {
        this->score = score;
        this->hp = hp;
        this->mass = mass;
        this->name = name;
    }

    virtual ~PolyCollider() {
        delete shape;
    }

    void TakeDamage(double dmg);
};

PolyCollider* newBoxCollider(Object* parent, Transform shift, int w, int h);
/// Shifts colliders so they do not intersect, applies rule of moments, and saves collisions in object
void _resolveCollision(PolyCollider* a, PolyCollider* b, Dot aShift, Dot at, double floorDamping = 0.2, double airDamping = 0.8, bool second = false);
/// Shifts colliders so they do not intersect, applies rule of moments, and saves collisions in object
void resolveCollision(PolyCollider* a, PolyCollider* b, Line norm);

/// PolygonCollider that could be rendered
struct PolygonRenderer : PolyCollider, Drawable {
    uint32_t color = 0;

    PolygonRenderer() {}
    PolygonRenderer(Object* parent) : PolyCollider(parent) {}
    PolygonRenderer(Object* parent, Poly* shape) : PolyCollider(parent, shape) {
        drawn.reserve(shape->Radius() * 8);
    }

protected:
    bool init = false;
    uint32_t lastColor = 1;
    Transform lastTransform{};
    std::unordered_set<std::pair<int, int>> drawn{};

    //void IncrementalDraw(uint32_t color);
    //void FullDraw(uint32_t color);
    void EdgeDraw(uint32_t color);
public:
    void Draw(uint32_t color, bool forceFull = false, bool edgeOnly = true);
    virtual void Draw(bool forceFull = false) override {
        Draw(color, forceFull);
    }
    virtual void Clear() override;
    void reserve() {
        drawn.reserve(shape->Radius() * 8);
    }
};

PolygonRenderer* newBoxRenderer(Object* parent, Transform shift, int w, int h, uint32_t color);

/// Applies constant force if button is pressed. Also has visual effects.
struct Thruster : public Controlled, Drawable, Component {
    Dot shift;
    Dot force;
    PolyCollider* base = nullptr;
protected:
    PolygonRenderer visual;
public:
    Thruster() {}
    Thruster(Object* parent, PolyCollider* base) : Component(parent), base(base) {}
    Thruster(Object* parent, PolyCollider* base, Dot shift, Dot force) : Component(parent), base(base), shift(shift), force(force) {
        Poly* poly = new Poly{ &(parent->transform), std::vector<Dot>{shift - force / 10 * 2, shift + force.norm() / 10 / 3, shift - force.norm() / 10 / 3} };
        visual = PolygonRenderer{ parent };
        visual.shape = poly;
        visual.reserve();
        visual.color = 0xffa200;
    }

    virtual void activate() override;
    virtual void stop() override;
    virtual void Draw(bool forceFull = false) override;
    virtual void Clear() override;

    friend void fixCenter(Object* obj);
};

struct Explosion {
    Dot position;
    int scale;
    double nextFrame;
    std::unordered_set<std::pair<int, int>> drawn{};
};