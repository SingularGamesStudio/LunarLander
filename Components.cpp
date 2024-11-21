#include "Components.h"
#include "Constants.h"
#include "RocketScience.h"

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

void PolyCollider::TakeDamage(double dmg) {
    if (hp <= 0)
        return;
    if (dmg >= damageTreshold) {
        hp -= dmg;
        if (hp <= 0) {
            if (parent->name == "Rocket")
                global::componentsDestroyed++;
            //TODO:explosion
            fixCenter(parent);
        }
    }
}

void Thruster::activate() {
    if (base->hp > 0) {
        parent->applyForce(force.rotated(parent->transform.rot), shift.unLocal(parent->transform));
        active = true;
    }
    else
        stop();
}

void Thruster::stop() {
    if (active) {
        active = false;
        Clear();
    }
}

void Thruster::Clear() {
    visual.Clear();
}

void Thruster::Draw(bool forceFull) {
    if (!active)
        return;
    visual.Draw(forceFull);
}