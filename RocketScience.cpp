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

    std::vector<std::pair<std::string, std::function<Object* (Transform)>>> rocketBuilders = {
        {"Basic", std::function<Object * (Transform)>(BuildRocket)},
        {"Basic", std::function<Object * (Transform)>(BuildRocket)},
        {"Basic", std::function<Object * (Transform)>(BuildRocket)},
        {"Basic", std::function<Object * (Transform)>(BuildRocket)},
    };
    std::vector<std::pair<std::string, std::function<Transform()>>> levelBuilders = {
        {"Lvl1", std::function<Transform()>(BuildLevel)},
        {"Lvl2", std::function<Transform()>(BuildLevel)},
        {"Lvl3", std::function<Transform()>(BuildLevel)},
        {"Lvl4", std::function<Transform()>(BuildLevel)},
    };

    string state = "Menu";
    int rocketChoice = 0;
    int levelChoice = 0;
    int componentsDestroyed = 0;
};

void Cleanup() {
    for (Drawable* draw : global::drawable) {
        draw->Clear();
    }
    for (Object* obj : global::objects) {
        while (!obj->components.empty()) {
            Component* todel = *obj->components.begin();
            delete todel;
        }
        delete obj;
    }
    global::objects.clear();
    global::colliders.clear();
    global::drawable.clear();
    global::controls.clear();
}

Object* BuildRocket(Transform at) {
    using namespace global;
    objects.push_back(new Object(Transform(at), "Rocket"));
    auto body = newBoxRenderer(objects.back(), Transform({ 0, 0 }, 0), 50, 100, colors::white);
    body->setUpGameProperties(2, 200, 7, "Hull");
    drawable.push_back(body);
    colliders.push_back(body);
    auto head = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, -49}, {0, -85}, {-25, -49} }));
    head->setUpGameProperties(5, 50, 2, "Head");
    head->color = colors::white - 1;
    drawable.push_back(head);
    colliders.push_back(head);
    auto thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, 75}, {0, 50}, {-25, 75} }));
    thrusterCollider->setUpGameProperties(1, 100, 1, "Thruster");
    thrusterCollider->color = colors::brown;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);

    auto primaryThruster = new Thruster(objects.back(), thrusterCollider, { 0, 75 }, { 0, -500 });
    primaryThruster->buttonFilter = { VK_UP };
    controls.push_back(primaryThruster);
    drawable.push_back(primaryThruster);
    auto rightThruster = new Thruster(objects.back(), body, { 25, -15 }, { -50, 0 });
    rightThruster->buttonFilter = { VK_LEFT };
    controls.push_back(rightThruster);
    drawable.push_back(rightThruster);
    auto leftThruster = new Thruster(objects.back(), body, { -25, -15 }, { 50, 0 });
    leftThruster->buttonFilter = { VK_RIGHT };
    controls.push_back(leftThruster);
    drawable.push_back(leftThruster);
    return objects.back();
}

void mapEdges() {
    using namespace global;
    objects.push_back(new Object(Transform({ 0, 0 }, 0), "leftScreen"));
    objects.back()->physicsLocked = true;
    auto screen = newBoxRenderer(objects.back(), Transform({ -SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2 }, 0), SCREEN_HEIGHT + 2, SCREEN_HEIGHT, colors::yellow);
    screen->setUpGameProperties(1, 1000000000, 1, "Screen edge");
    drawable.push_back(screen);
    colliders.push_back(screen);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "rightScreen"));
    objects.back()->physicsLocked = true;
    screen = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH + SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2 }, 0), SCREEN_HEIGHT + 2, SCREEN_HEIGHT, colors::yellow);
    screen->setUpGameProperties(1, 1000000000, 1, "Screen edge");
    drawable.push_back(screen);
    colliders.push_back(screen);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "bottomScreen"));
    objects.back()->physicsLocked = true;
    screen = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH / 2, -SCREEN_WIDTH / 2 }, 0), SCREEN_WIDTH, SCREEN_WIDTH + 2, colors::yellow);
    screen->setUpGameProperties(10, 1000000000, 1, "Screen top");
    drawable.push_back(screen);
    colliders.push_back(screen);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "bottomScreen"));
    objects.back()->physicsLocked = true;
    screen = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH / 2, SCREEN_HEIGHT + SCREEN_WIDTH / 2 }, 0), SCREEN_WIDTH, SCREEN_WIDTH + 2, colors::yellow);
    screen->setUpGameProperties(1, 1000000000, 1, "Screen edge");
    drawable.push_back(screen);
    colliders.push_back(screen);
}

Transform BuildLevel() {
    using namespace global;

    mapEdges();
    for (int i = 0; i < 50; i++) {
        objects.push_back(new Object(Transform({ double(rnd() % (SCREEN_WIDTH - 100) + 50), double(rnd() % (SCREEN_HEIGHT / 2)) }, pi / (rnd() % 100) * 200.0), "Box" + std::to_string(i)));
        auto boxRenderer = newBoxRenderer(objects.back(), Transform({ 0, 0 }, 0), 50, 50, rnd() % 255 * 256 + rnd() % 255 + rnd() % 255 * 256 * 256);
        boxRenderer->setUpGameProperties(2, 600, 1, "Box");
        drawable.push_back(boxRenderer);
        colliders.push_back(boxRenderer);
    }
    objects.push_back(new Object(Transform({ double(rnd() % (SCREEN_WIDTH - 100) + 50), double(rnd() % (SCREEN_HEIGHT / 2)) }, pi / (rnd() % 100) * 200.0), "tri"));
    auto triangleRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {100, 100}, {100, 200}, {0, 0} }));
    triangleRenderer->setUpGameProperties(5, 1200, 2, "Triangle");
    triangleRenderer->color = 255 * 256;
    drawable.push_back(triangleRenderer);
    colliders.push_back(triangleRenderer);

    return Transform({ 200, 200 }, 0);
}