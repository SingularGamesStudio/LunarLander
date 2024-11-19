#pragma once
#include <string>

#include "Geometry.h"
#include "Engine.h"

using std::string, std::vector;

struct Object;

struct Component {
    Object* parent;
};

struct Object {
    Transform transform;
    string name;
    bool physicsLocked = false;

    Dot velocity;
    Dot massCenter;
    double mass = 0;

    vector<Component*> components;

    void applyForce(Dot F, Dot where, double dt) {
        // TODO:
        velocity += ((F / mass) * dt);
    }
};

struct PolyCollider : public Component {
    polygon* shape;
    bool draw;
};