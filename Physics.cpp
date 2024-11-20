#include "Components.h"

/// Shifts colliders so they do not intersect, applies rule of moments, and saves collisions in object
void _resolveCollision(PolyCollider* a, PolyCollider* b, Dot aShift, Dot at, double damping, bool second) {
    if (a->parent->physicsLocked) {
        std::swap(a, b);//ensure that a is dynamic
        aShift = aShift * -1;
    }
    Line norm = Line(at, at + aShift);
    if (b->parent->physicsLocked) { //dynamic and static objects
        a->parent->transform.pos += aShift;
        if (!second) {
            Dot aVel = norm.project(a->parent->velocity + at) - at;
            if (aVel.len() > eps)
                a->parent->velocity -= aVel * (1 + damping);
        }
        a->parent->collisions.push_back(Collision{ at, aShift, b });
    }
    else {//two dynamic objects
        if (!second) {
            Dot aVel = norm.project(a->parent->velocity + at) - at;
            Dot bVel = norm.project(b->parent->velocity + at) - at;// абсоллютно упругий удар
            Dot aVel1 = (aVel * (a->parent->mass - b->parent->mass) + bVel * 2 * b->parent->mass) / (a->parent->mass + b->parent->mass) * damping;
            Dot bVel1 = (bVel * (b->parent->mass - a->parent->mass) + aVel * 2 * a->parent->mass) / (a->parent->mass + b->parent->mass) * damping;
            a->parent->velocity -= aVel;
            b->parent->velocity -= bVel;
            if (aVel1.len() > eps) {
                a->parent->velocity += aVel1;
            }
            if (bVel1.len() > eps) {
                b->parent->velocity += bVel1;
            }
        }
        a->parent->transform.pos += aShift / 2;
        b->parent->transform.pos -= aShift / 2;
        a->parent->collisions.push_back(Collision{ at, aShift, b });//TODO:only add collision if bounce is small enough
        b->parent->collisions.push_back(Collision{ at, -aShift, a });
    }
}

void Object::evalForces(double dt) {
    for (std::pair<Dot, Dot> force : forces) {
        if (force.first.len() < eps)
            continue;
        velocity += (force.first / mass) * dt;
        Dot arm = Line(force.second, force.second + force.first).project(transform.pos) - transform.pos;
        double moment = force.first.len() * arm.len() * (arm % force.first > 0 ? 1 : -1);
        rotationSpeed += moment / inertia;
    }

    forces.clear();
}

void Object::evalCollisions(double dt) {
    for (Collision coll : collisions) {
        Line norm = Line(coll.at, coll.at + coll.norm);
        Dot force = dotZero;
        for (auto f : forces) {
            force += norm.project(coll.at + f.first) - coll.at;
        }
        if (force * coll.norm > 0) {
            continue;
        }
        applyForce(-force, coll.at);
    }
    collisions.clear();
}