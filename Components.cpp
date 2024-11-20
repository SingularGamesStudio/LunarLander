#include "Components.h"

PolyCollider* newBoxCollider(Object* parent, Transform shift, int w, int h) {
    PolyCollider* res = new PolyCollider(parent);
    res->shape = new box(&parent->transform, w, h, shift);
    return res;
}

PolygonRenderer* newBoxRenderer(Object* parent, Transform shift, int w, int h, uint32_t color) {
    PolygonRenderer* res = new PolygonRenderer(parent);
    res->shape = new box(&parent->transform, w, h, shift);
    res->color = color;
    return res;
}

void Thruster::activate() {
    parent->applyForce(force.rotated(parent->transform.rot), shift.unLocal(parent->transform));
    active = true;
}

void Thruster::stop() {
    active = false;
    Clear();
}

void Thruster::Clear() {
    visual.Clear();
}

void Thruster::Draw(bool forceFull) {
    if (!active)
        return;
    visual.Draw(forceFull);
}