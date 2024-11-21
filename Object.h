#pragma once
#include <string>
#include <set>

#include "Geometry.h"
#include "Engine.h"

using std::string, std::vector;

struct PolyCollider;
struct Component;

struct Collision {
    Dot at;
    Dot norm;
    Dot parralelVelocity;
    std::pair<PolyCollider*, PolyCollider* > what;
};

///Physical object, attributes defined by adding components
struct Object {
    string name = "";

    //number of active colliders
    int aliveCounter = 1;

    Transform transform{};
    double rotationSpeed = 0;
    Dot velocity = dotZero;

    double mass = 1;
    double inertia = 1000;
    bool physicsLocked = false;

    std::vector<Collision> collisions{};
    std::vector<std::pair<Dot, Dot>> forces{};

    std::set<Component*> components{};

    Object() {}
    Object(Transform transform) : transform(transform) {}
    Object(Transform transform, string name) : transform(transform), name(name) {}

    void applyForce(Dot F, Dot where) {
        forces.push_back({ F, where });
    }

    void evalCollisions(double dt);

    void evalForces(double dt);
};

struct Component {
    Object* parent = nullptr;
    Component() {}
    Component(Object* parent) : parent(parent) {
        parent->components.insert(this);
    }
    virtual ~Component() {
        parent->components.erase(this);
    }
};

/// Controlled component can be activated on button press
struct Controlled {
    std::vector<char> buttonFilter{};
    bool active = false;
    Controlled() {}
    virtual void activate() = 0;
    virtual void stop() = 0;
};

struct Drawable {
    Drawable() {}
    virtual void Draw(bool forceFull = false) = 0;
    virtual void Clear() = 0;
};

double getRadius(Object* obj);

void fixCenter(Object* obj);