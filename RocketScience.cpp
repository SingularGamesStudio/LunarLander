#include<random>
#include "Components.h"
#include "RocketScience.h"
#include "Constants.h"

namespace global {
    std::vector<Object*> objects{};
    std::vector<PolyCollider*> colliders{};
    std::vector<Drawable*> drawable{};
    std::vector<Controlled*> controls{};
    std::mt19937 rnd(42);
};

Object* BuildRocket(Transform at) {
    using namespace global;
    objects.push_back(new Object(Transform(at), "Rocket"));
    objects.back()->mass = 1;

    auto body = newBoxRenderer(objects.back(), Transform({ 0, 0 }, 0), 50, 100, colors::white);
    drawable.push_back(body);
    colliders.push_back(body);
    auto head = new PolygonRenderer(objects.back(), new polygon(&objects.back()->transform, { {25, -49}, {0, -85}, {-25, -49} }));
    head->color = colors::white - 1;
    drawable.push_back(head);
    colliders.push_back(head);
    auto thrusterCollider = new PolygonRenderer(objects.back(), new polygon(&objects.back()->transform, { {25, 75}, {0, 50}, {-25, 75} }));
    thrusterCollider->color = colors::brown;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);

    auto primaryThruster = new Thruster(objects.back(), { 0, 75 }, { 0, -50 });
    primaryThruster->buttonFilter = { VK_UP };
    controls.push_back(primaryThruster);
    drawable.push_back(primaryThruster);
    auto rightThruster = new Thruster(objects.back(), { 25, -15 }, { -5, 0 });
    rightThruster->buttonFilter = { VK_LEFT };
    controls.push_back(rightThruster);
    drawable.push_back(rightThruster);
    auto leftThruster = new Thruster(objects.back(), { -25, -15 }, { 5, 0 });
    leftThruster->buttonFilter = { VK_RIGHT };
    controls.push_back(leftThruster);
    drawable.push_back(leftThruster);
    return objects.back();
}