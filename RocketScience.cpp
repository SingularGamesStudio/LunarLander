#include<random>
#include "Components.h"
#include "RocketScience.h"
#include "Constants.h"

namespace global {
    std::vector<Object*> objects{};
    std::vector<PolyCollider*> colliders{};
    std::vector<Drawable*> drawable{};
    std::vector<Controlled*> controls{};
    std::mt19937 rnd(time(NULL));
    std::set<Explosion*> explosions{};

    std::vector<std::pair<std::string, std::function<Object* (Transform)>>> rocketBuilders = {
        {"   Basic", std::function(BuildRocket1)},
        {"    UFO", std::function(BuildRocket2)},
        {"   Plane", std::function(BuildRocket3)},
        {"  Advanced", std::function(BuildRocket4)},
    };
    std::vector<std::pair<std::string, std::function<void(std::function<Object* (Transform)>)>>> levelBuilders = {
        {"Lvl1: Volcano", std::function(BuildLevel1)},
        {"Lvl2: Lock and key", std::function(BuildLevel2)},
        {"Lvl3: Classic (no HP)", std::function(BuildLevel3)},
        {"Lvl4: Chaos!", std::function(BuildLevel4)},
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
    for (Explosion* exp : global::explosions) {
        delete exp;
    }
    global::objects.clear();
    global::colliders.clear();
    global::drawable.clear();
    global::controls.clear();
    global::explosions.clear();
}

Object* BuildRocket1(Transform at) {
    using namespace global;
    objects.push_back(new Object(Transform(at), "Rocket"));
    auto body = newBoxRenderer(objects.back(), Transform({ 0, 0 }, 0), 50, 100, colors::white);
    body->setUpGameProperties(2, 2000, 7, "Hull");
    drawable.push_back(body);
    colliders.push_back(body);
    auto head = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, -49}, {0, -85}, {-25, -49} }));
    head->setUpGameProperties(5, 500, 2, "Head");
    head->color = colors::white - 1;
    drawable.push_back(head);
    colliders.push_back(head);
    auto thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, 75}, {0, 50}, {-25, 75} }));
    thrusterCollider->setUpGameProperties(1, 1000, 1, "Thruster");
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
Object* BuildRocket2(Transform at) {
    using namespace global;
    objects.push_back(new Object(Transform(at), "Rocket"));
    objects.back()->inertiaMod = 10;
    auto body = newBoxRenderer(objects.back(), Transform({ 0, 0 }, -pi / 3), 50, 100, colors::grey);
    body->setUpGameProperties(2, 500, 2, "Hull");
    drawable.push_back(body);
    colliders.push_back(body);
    body = newBoxRenderer(objects.back(), Transform({ 0, 0 }, pi / 3), 50, 100, colors::grey);
    body->setUpGameProperties(2, 500, 2, "Hull");
    drawable.push_back(body);
    colliders.push_back(body);
    body = newBoxRenderer(objects.back(), Transform({ 0, -25 }, 0), 50, 50, colors::green);
    body->setUpGameProperties(10, 500, 2, "Cabin");
    drawable.push_back(body);
    colliders.push_back(body);
    body = newBoxRenderer(objects.back(), Transform({ 0, 25 }, 0), 50, 50, colors::grey);
    body->setUpGameProperties(2, 500, 2, "Hull");
    drawable.push_back(body);
    colliders.push_back(body);
    auto thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, 0}, {60, 5}, {75, 65} }));
    thrusterCollider->setUpGameProperties(1, 1200, 1, "Leg");
    thrusterCollider->color = colors::white;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);

    auto rightThruster = new Thruster(objects.back(), thrusterCollider, { 75, 65 }, { 0, -300 });
    rightThruster->buttonFilter = { VK_LEFT, VK_UP };
    controls.push_back(rightThruster);
    drawable.push_back(rightThruster);

    thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {-25, 0}, {-60, 5}, {-75, 65} }));
    thrusterCollider->setUpGameProperties(1, 1200, 1, "Leg");
    thrusterCollider->color = colors::white;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);

    auto leftThruster = new Thruster(objects.back(), thrusterCollider, { -75, 65 }, { 0, -300 });
    leftThruster->buttonFilter = { VK_RIGHT, VK_UP };
    controls.push_back(leftThruster);
    drawable.push_back(leftThruster);
    return objects.back();
}

Object* BuildRocket3(Transform at) {
    using namespace global;
    objects.push_back(new Object(Transform(at), "Rocket"));
    auto body = newBoxRenderer(objects.back(), Transform({ -10, 0 }, 0), 80, 30, colors::white);
    body->setUpGameProperties(7, 1000, 6, "Hull");
    drawable.push_back(body);
    colliders.push_back(body);
    auto tail = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {-50, 15}, {-20, 15}, {-40, -45} }));
    tail->setUpGameProperties(20, 300, 2, "Tail");
    tail->color = colors::white - 1;
    drawable.push_back(tail);
    colliders.push_back(tail);
    auto head = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {30, 15}, {50, 15}, {30, -15} }));
    head->setUpGameProperties(10, 300, 2, "Head");
    head->color = colors::white - 1;
    drawable.push_back(head);
    colliders.push_back(head);
    auto land = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {35, 15}, {40, 15}, {45, 20} }));
    land->setUpGameProperties(1, 2000, 0, "Landing gear");
    land->color = colors::grey;
    drawable.push_back(land);
    colliders.push_back(land);
    land = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {-45, 15}, {-40, 15}, {-35, 20} }));
    land->setUpGameProperties(1, 2000, 0, "Landing gear");
    land->color = colors::grey;
    drawable.push_back(land);
    colliders.push_back(land);

    auto primaryThruster = new Thruster(objects.back(), body, { -50, 0 }, { 500, 0 });
    primaryThruster->buttonFilter = { VK_RIGHT };
    controls.push_back(primaryThruster);
    drawable.push_back(primaryThruster);
    auto upThruster = new Thruster(objects.back(), body, { 30, 15 }, { 0, -5 });
    upThruster->buttonFilter = { VK_UP };
    controls.push_back(upThruster);
    drawable.push_back(upThruster);
    upThruster = new Thruster(objects.back(), body, { 30, -15 }, { 0, 3 });
    upThruster->buttonFilter = { VK_DOWN };
    controls.push_back(upThruster);
    drawable.push_back(upThruster);
    return objects.back();
}
Object* BuildRocket4(Transform at) {
    using namespace global;
    objects.push_back(new Object(Transform(at), "Rocket"));
    auto body = newBoxRenderer(objects.back(), Transform({ 0, 0 }, 0), 50, 100, colors::white);
    objects.back()->inertiaMod = 2;
    body->setUpGameProperties(2, 2000, 5, "Hull");
    drawable.push_back(body);
    colliders.push_back(body);
    auto body1 = newBoxRenderer(objects.back(), Transform({ 40, 20 }, 0), 30, 80, colors::white);
    body1->setUpGameProperties(2, 2000, 3, "Hull");
    drawable.push_back(body1);
    colliders.push_back(body1);
    auto body2 = newBoxRenderer(objects.back(), Transform({ -40, 20 }, 0), 30, 80, colors::white);
    body2->setUpGameProperties(2, 2000, 3, "Hull");
    drawable.push_back(body2);
    colliders.push_back(body2);
    auto head = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, -49}, {0, -85}, {-25, -49} }));
    head->setUpGameProperties(5, 500, 2, "Head");
    head->color = colors::white - 1;
    drawable.push_back(head);
    colliders.push_back(head);
    auto thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, 75}, {0, 50}, {-25, 75} }));
    thrusterCollider->setUpGameProperties(1, 800, 1, "Thruster");
    thrusterCollider->color = colors::brown;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);
    auto primaryThruster = new Thruster(objects.back(), thrusterCollider, { 0, 75 }, { 0, -500 });
    primaryThruster->buttonFilter = { VK_UP };
    controls.push_back(primaryThruster);
    drawable.push_back(primaryThruster);

    thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {55, 78}, {40, 60}, {25, 78} }));
    thrusterCollider->setUpGameProperties(1, 1000, 1, "Thruster");
    thrusterCollider->color = colors::brown;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);
    auto rightThruster = new Thruster(objects.back(), thrusterCollider, { 40, 78 }, { 0, -200 });
    rightThruster->buttonFilter = { VK_LEFT, VK_UP };
    controls.push_back(rightThruster);
    drawable.push_back(rightThruster);
    rightThruster = new Thruster(objects.back(), body1, { 40, -20 }, { 0, 20 });
    rightThruster->buttonFilter = { VK_RIGHT, VK_DOWN };
    controls.push_back(rightThruster);
    drawable.push_back(rightThruster);

    thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {-55, 78}, {-40, 60}, {-25, 78} }));
    thrusterCollider->setUpGameProperties(1, 1000, 1, "Thruster");
    thrusterCollider->color = colors::brown;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);
    auto leftThruster = new Thruster(objects.back(), thrusterCollider, { -40, 78 }, { 0, -200 });
    leftThruster->buttonFilter = { VK_RIGHT, VK_UP };
    controls.push_back(leftThruster);
    drawable.push_back(leftThruster);
    leftThruster = new Thruster(objects.back(), body2, { -40, -20 }, { 0, 20 });
    leftThruster->buttonFilter = { VK_LEFT, VK_DOWN };
    controls.push_back(leftThruster);
    drawable.push_back(leftThruster);

    thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {25, 30}, {60, 35}, {75, 95} }));
    thrusterCollider->setUpGameProperties(1, 1200, 1, "Leg");
    thrusterCollider->color = colors::grey;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);

    thrusterCollider = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {-25, 30}, {-60, 35}, {-75, 95} }));
    thrusterCollider->setUpGameProperties(1, 1200, 1, "Leg");
    thrusterCollider->color = colors::grey;
    drawable.push_back(thrusterCollider);
    colliders.push_back(thrusterCollider);

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

Rot randomAngle() {
    return pi * (global::rnd() % 200) / 100.0;
}

PolygonRenderer* Boulder(Dot pos, double r) {
    using namespace global;
    objects.push_back(new Object(Transform({ pos }, 0), "Boulder"));
    std::vector<Rot> rots(5 + rnd() % 5);
    for (int i = 0; i < rots.size(); i++) {
        rots[i] = randomAngle();
    }
    std::sort(rots.begin(), rots.end());
    std::vector<Dot> dots(rots.size());
    for (int i = 0; i < rots.size(); i++) {
        dots[i] = rots[i].vector() * r;
    }
    auto boulderRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, dots));
    boulderRenderer->setUpGameProperties(9, 50000, 20, "Boulder");
    boulderRenderer->color = colors::grey;
    drawable.push_back(boulderRenderer);
    colliders.push_back(boulderRenderer);
    return boulderRenderer;
}

void BuildLevel1(std::function<Object* (Transform)> rocketBuilder) {
    using namespace global;

    mapEdges();
    for (int i = 0; i < 9; i++) {
        auto boulder = Boulder(Dot{ double(SCREEN_WIDTH - 40 * (i / 3) - 20), SCREEN_HEIGHT * 3 / 4 - double(40 * (i % 3)) - 20 }, 20);
        int val1 = rnd() % 200;
        int val2 = rnd() % 100;
        boulder->parent->velocity = Dot(-100 + val2, -400 + val1);
    }
    for (int i = 0; i < 4; i++) {
        auto boulder = Boulder(Dot{ double(SCREEN_WIDTH - 40 * (i / 2) - 100), SCREEN_HEIGHT / 3 - double(40 * (i % 2)) - 20 }, 20);
        int val1 = rnd() % 100;
        int val2 = rnd() % 100;
        boulder->parent->velocity = Dot(-150 + val2, -50 + val1);
    }
    objects.push_back(new Object(Transform({ 0, 0 }, 0), "lava"));
    objects.back()->physicsLocked = 1;
    auto lavaRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH * 15 / 16, SCREEN_HEIGHT * 7 / 8 }, 0), SCREEN_WIDTH / 8 - 2, SCREEN_HEIGHT / 4 - 2, colors::orange);
    lavaRenderer->setUpGameProperties(3, 1200, 2, "lava");
    drawable.push_back(lavaRenderer);
    colliders.push_back(lavaRenderer);
    objects.push_back(new Object(Transform({ 0, 0 }, 0), "mountain"));
    objects.back()->physicsLocked = 1;
    auto triangleRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {SCREEN_WIDTH / 3, SCREEN_HEIGHT}, {SCREEN_WIDTH * 7 / 8, SCREEN_HEIGHT / 3}, {SCREEN_WIDTH * 7 / 8, SCREEN_HEIGHT} }));
    triangleRenderer->setUpGameProperties(3, 1200, 2, "Mountain");
    triangleRenderer->color = colors::grey;
    drawable.push_back(triangleRenderer);
    colliders.push_back(triangleRenderer);

    rocketBuilder(Transform({ 200, 200 }, 0));
}

void BuildLevel3(std::function<Object* (Transform)> rocketBuilder) {
    using namespace global;

    mapEdges();
    std::vector<Dot> dots = { {0, SCREEN_HEIGHT / 2}, {SCREEN_WIDTH, SCREEN_HEIGHT / 2} };
    for (int i = 0; i < 15 + rnd() % 15; i++) {
        dots.push_back({ (double)(rnd() % SCREEN_WIDTH), (double)(rnd() % (SCREEN_HEIGHT * 2 / 3) + SCREEN_HEIGHT / 3) });
    }
    std::sort(dots.begin(), dots.end());
    Dot last = dots[0];
    int good1 = rnd() % dots.size(), good2 = rnd() % dots.size();
    for (int i = 0; i < dots.size(); i++) {
        Dot cur = dots[i];
        if (i == good1 || i == good2)
            cur.y = last.y;
        objects.push_back(new Object(Transform({ 0, 0 }, 0), "moon"));
        objects.back()->physicsLocked = 1;
        auto mountainRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform,
            { last, {cur.x - 1, cur.y}, {cur.x - 1, SCREEN_HEIGHT - 1}, {last.x, SCREEN_HEIGHT - 1} }));
        if (i == good1 || i == good2) {
            mountainRenderer->setUpGameProperties(10, 1200, 2, "Landing pad");
            mountainRenderer->color = colors::green;
        }
        else {
            mountainRenderer->setUpGameProperties(2, 1200, 2, "Moon");
            mountainRenderer->color = colors::grey;
        }
        drawable.push_back(mountainRenderer);
        colliders.push_back(mountainRenderer);

        last = cur;
    }

    auto rocket = rocketBuilder(Transform({ 100, 100 }, 0));
    rocket->velocity = { (double)(rnd() % 100), (double)(rnd() % 25) };
    for (auto coll : rocket->components) {
        if (dynamic_cast<PolyCollider*>(coll) != NULL)
            dynamic_cast<PolyCollider*>(coll)->hp = 1;
    }

}

void BuildLevel2(std::function<Object* (Transform)> rocketBuilder) {
    using namespace global;

    mapEdges();
    objects.push_back(new Object(Transform({ 0, 0 }, 0), "wall"));
    objects.back()->physicsLocked = 1;
    auto wallRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH / 3, SCREEN_HEIGHT * 5 / 6 }, 0), 50, SCREEN_HEIGHT / 3, colors::brown);
    wallRenderer->setUpGameProperties(1, 1200, 2, "bunker wall");
    drawable.push_back(wallRenderer);
    colliders.push_back(wallRenderer);
    objects.push_back(new Object(Transform({ 0, 0 }, 0), "wall"));
    objects.back()->physicsLocked = 1;
    wallRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 5 / 6 }, 0), 50, SCREEN_HEIGHT / 3, colors::brown);
    wallRenderer->setUpGameProperties(1, 1200, 2, "bunker wall");
    drawable.push_back(wallRenderer);
    colliders.push_back(wallRenderer);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "door"));
    objects.back()->inertiaMod = 0.1;
    wallRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH * 1 / 2, SCREEN_HEIGHT * 2 / 3 - 25 }, 0), SCREEN_WIDTH * 1 / 2, 50, colors::grey);
    wallRenderer->setUpGameProperties(0, 12000000, 5, "door");
    drawable.push_back(wallRenderer);
    colliders.push_back(wallRenderer);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "wall"));
    objects.back()->inertiaMod = 0.1;
    wallRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH - 200, SCREEN_HEIGHT * 5 / 6 }, 0), 100, SCREEN_HEIGHT * 1 / 3, colors::white);
    wallRenderer->setUpGameProperties(0, 120000, 1, "wall");
    drawable.push_back(wallRenderer);
    colliders.push_back(wallRenderer);
    objects.push_back(new Object(Transform({ 0, 0 }, 0), "wall"));
    objects.back()->inertiaMod = 0.1;
    wallRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH - 200, SCREEN_HEIGHT * 1 / 2 }, 0), 100, SCREEN_HEIGHT * 1 / 3, colors::white);
    wallRenderer->setUpGameProperties(0, 120000, 1, "wall");
    drawable.push_back(wallRenderer);
    colliders.push_back(wallRenderer);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "wall"));
    objects.back()->inertiaMod = 0.1;
    wallRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH - 200, SCREEN_HEIGHT * 1 / 3 - 25 }, 0), 400, 50, colors::white);
    wallRenderer->setUpGameProperties(0, 120000, 1, "wall");
    drawable.push_back(wallRenderer);
    colliders.push_back(wallRenderer);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "mountain"));
    objects.back()->physicsLocked = 1;
    auto triangleRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {0, SCREEN_HEIGHT}, {SCREEN_WIDTH / 3 - 25, SCREEN_HEIGHT}, {SCREEN_WIDTH / 3 - 25, SCREEN_HEIGHT * 2 / 3} }));
    triangleRenderer->setUpGameProperties(0, 1200, 2, "Mountain");
    triangleRenderer->color = colors::grey;
    drawable.push_back(triangleRenderer);
    colliders.push_back(triangleRenderer);

    objects.push_back(new Object(Transform({ 0, 0 }, 0), "landing pad"));
    objects.back()->physicsLocked = 1;
    auto landRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, {
        {SCREEN_WIDTH / 3 + 50, SCREEN_HEIGHT - 1}, {SCREEN_WIDTH / 3 + 75, SCREEN_HEIGHT - 50}, {SCREEN_WIDTH * 2 / 3 - 75, SCREEN_HEIGHT - 50}, {SCREEN_WIDTH * 2 / 3 - 50, SCREEN_HEIGHT - 1} }));
    landRenderer->setUpGameProperties(20, 1200, 2, "landing pad");
    landRenderer->color = colors::green;
    drawable.push_back(landRenderer);
    colliders.push_back(landRenderer);
    landRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, {
        {SCREEN_WIDTH / 3 + 65, SCREEN_HEIGHT - 10}, {SCREEN_WIDTH / 3 + 90, SCREEN_HEIGHT - 40}, {SCREEN_WIDTH * 2 / 3 - 90, SCREEN_HEIGHT - 40}, {SCREEN_WIDTH * 2 / 3 - 65, SCREEN_HEIGHT - 10} }));
    landRenderer->setUpGameProperties(20, 1200, 2, "landing pad");
    landRenderer->color = colors::green;
    drawable.push_back(landRenderer);
    colliders.push_back(landRenderer);


    auto rocket = rocketBuilder(Transform({ 100, 100 }, 0));
}

void BuildLevel4(std::function<Object* (Transform)> rocketBuilder) {
    using namespace global;

    mapEdges();
    for (int i = 0; i < 50; i++) {
        Transform pos = Transform({ double(rnd() % (SCREEN_WIDTH - 100) + 50), double(rnd() % (SCREEN_HEIGHT / 2)) }, pi / (rnd() % 100) * 200.0);
        if ((pos.pos - Dot{ 200, 200 }).len() < 100) {
            i--;
            continue;
        }
        objects.push_back(new Object(pos, "Box" + std::to_string(i)));
        auto boxRenderer = newBoxRenderer(objects.back(), Transform({ 0, 0 }, 0), 50, 50, rnd() % 255 * 256 + rnd() % 255 + rnd() % 255 * 256 * 256);
        boxRenderer->setUpGameProperties(2, 100, 2, "Box");
        drawable.push_back(boxRenderer);
        colliders.push_back(boxRenderer);
    }
    objects.push_back(new Object(Transform({ double(rnd() % (SCREEN_WIDTH - 100) + 50), double(rnd() % (SCREEN_HEIGHT / 2)) }, pi / (rnd() % 100) * 200.0), "tri"));
    auto triangleRenderer = new PolygonRenderer(objects.back(), new Poly(&objects.back()->transform, { {100, 100}, {100, 200}, {0, 0} }));
    triangleRenderer->setUpGameProperties(5, 1200, 2, "Triangle");
    triangleRenderer->color = 255 * 256;
    drawable.push_back(triangleRenderer);
    colliders.push_back(triangleRenderer);

    rocketBuilder(Transform({ 200, 200 }, 0));
}