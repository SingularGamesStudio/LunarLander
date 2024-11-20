#include <memory.h>
#include <stdlib.h>
#include<random>
#include<format>
#include "Windows.h"
#include "Timer.h"
#include "Engine.h"
#include "Components.h"
#include "RocketScience.h"
#include "Constants.h"
#include "UI.h"
#include <queue>

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT,
//                                       VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 -
//  left button, 1 - right button) clear_buffer() - set all pixels in buffer to
//  'black' is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

double getRadius(Object* obj) {
    double r = 0;
    for (auto c : obj->components) {
        if (dynamic_cast<PolyCollider*>(c) != NULL) {
            for (Dot dot : dynamic_cast<PolyCollider*>(c)->shape->dots) {
                r = max(r, dot.len());
            }
        }
    }
    return r;
}

void fixCenters() {
    for (auto obj : global::objects) {
        int cnt = 0;
        Dot massCenter = dotZero;
        for (auto c : obj->components) {
            if (dynamic_cast<PolyCollider*>(c) != NULL) {
                for (Dot dot : dynamic_cast<PolyCollider*>(c)->shape->dots) {
                    massCenter += dot;
                    cnt++;
                }
            }
        }
        massCenter = massCenter / cnt;
        for (auto c : obj->components) {
            if (dynamic_cast<PolyCollider*>(c) != NULL) {
                auto coll = dynamic_cast<PolyCollider*>(c);
                for (Dot& dot : coll->shape->dots) {
                    dot -= massCenter;
                }
                if (dynamic_cast<box*>(coll->shape) != NULL) {
                    dynamic_cast<box*>(coll->shape)->shift.pos = dotZero;
                }
            }
        }
        obj->transform.pos += massCenter;
        double r = getRadius(obj);
        obj->inertia = obj->mass * r * r;
    }
}

void RocketState(Object* rocket, double dt) {
    bool done = false;
    static double timer = 0;
    for (Controlled* cont : global::controls) {
        if (cont->active) {
            UI::write("rocket state", "Thrusters enabled", colors::white);
            timer = 0;
            done = true;
            break;
        }
    }
    static std::queue<bool> onAir = {};
    static int onAirCount = 0;
    static std::vector<Collision> hits;
    if (onAir.size() > 9) {
        if (onAir.front())
            onAirCount--;
        onAir.pop();
    }
    onAir.push(rocket->collisions.empty());
    if (onAir.back()) {
        onAirCount++;
    }
    else {
        hits = rocket->collisions;
    }
    if (!done) {
        if (rocket->velocity.len() > 5 || abs(rocket->rotationSpeed) > 5) {
            timer = 0;
            UI::write("rocket state", "Rocket unstable", colors::white);
            return;
        }
        if (onAirCount >= 10) {
            timer = 0;
            UI::write("rocket state", "Rocket in the air", colors::white);
            return;
        }
        int score = 0;
        for (auto& coll : hits) {
            if (coll.hit == nullptr)
                continue;
            if (coll.hit->score == 0) {
                timer = 0;
                UI::write("rocket state", "Unsuitable terrain for landing", colors::white);
                return;
            }
            else {
                score += coll.hit->score;
            }
        }
        timer += dt;
        if (timer < 5) {
            UI::write("rocket state", std::format("LANDING COMMENCING: {:.3f}", 5 - timer), colors::white);
        }
        else {
            UI::write("rocket state", std::format("ROCKET LANDED, SCORE: {}", score), colors::white);
        }
    }

}

/// Initialize game data
void initialize() {
    using namespace global;
    // set up backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    UI::start("rocket state", { 20, 500 }, 2);


    objects.push_back(new Object(Transform({ 0, 0 }, 0), "Ground"));
    objects.back()->physicsLocked = true;
    auto floorRenderer = newBoxRenderer(objects.back(), Transform({ SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8 }, 0), SCREEN_WIDTH, SCREEN_HEIGHT / 4, 255);
    drawable.push_back(floorRenderer);
    colliders.push_back(floorRenderer);
    for (int i = 0; i < 5; i++) {
        objects.push_back(new Object(Transform({ double(rnd() % (SCREEN_WIDTH - 100) + 50), double(rnd() % (SCREEN_HEIGHT / 2)) }, pi / (rnd() % 100) * 200.0), "Box" + std::to_string(i)));
        auto boxRenderer = newBoxRenderer(objects.back(), Transform({ 0, 0 }, 0), 50, 50, rnd() % 255 * 256 + rnd() % 255 + rnd() % 255 * 256 * 256);
        drawable.push_back(boxRenderer);
        colliders.push_back(boxRenderer);
    }
    objects.push_back(new Object(Transform({ double(rnd() % (SCREEN_WIDTH - 100) + 50), double(rnd() % (SCREEN_HEIGHT / 2)) }, pi / (rnd() % 100) * 200.0), "tri"));
    auto triangleRenderer = new PolygonRenderer(objects.back(), new polygon(&objects.back()->transform, { {100, 100}, {100, 200}, {0, 0} }));
    triangleRenderer->color = 255 * 256;
    drawable.push_back(triangleRenderer);
    colliders.push_back(triangleRenderer);
    BuildRocket(Transform({ 200, 200 }, 0));
    fixCenters();
}
void DrawDebugLine(Line l) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        Dot dot = l.get(i);
        if (dot.x >= 0 && dot.y >= 0 && dot.x < SCREEN_WIDTH && dot.y < SCREEN_HEIGHT) {
            buffer[(int)dot.y][(int)dot.x] = 255 * 256 * 256 + 255 * 256 + 255;
        }
    }
}

/// Called to update game data,
/// dt - time elapsed since the previous update (in seconds)
void act(float dt) {
    using namespace global;
    //if (!is_window_active()) return;
    if (is_key_pressed(VK_ESCAPE)) schedule_quit_game();

    for (auto& object : objects) {
        if (!object->physicsLocked) {
            object->applyForce(dotUp * 9.8 * object->mass, object->transform.pos);
        }
    }
    for (int i = 0; i < colliders.size(); i++) {
        for (int j = i + 1; j < colliders.size(); j++) {
            if (colliders[i]->parent->physicsLocked && colliders[j]->parent->physicsLocked) {
                continue;
            }
            if (colliders[i]->parent == colliders[j]->parent) {
                continue;
            }
            auto inter = intersect(*colliders[i]->shape, *colliders[j]->shape);
            if (inter.first > 0) {
                if (colliders[i]->parent->name == "Box1") {
                    colliders[i]->parent->name = "Box1";
                }
                //DrawLine(inter.second);
                resolveCollision(colliders[i], colliders[j], inter.second);
            }
        }
    }
    std::unordered_set<char> pressed = {};
    for (char button : vector<char>{ VK_SPACE, VK_RIGHT,VK_LEFT, VK_UP, VK_DOWN, 'A', 'B' }) {
        if (is_key_pressed(button)) {
            pressed.insert(button);
        }
    }
    for (auto& control : controls) {
        bool active = false;
        for (char button : control->buttonFilter) {
            if (pressed.contains(button)) {
                active = true;
                break;
            }
        }
        if (active)
            control->activate();
        else
            control->stop();
    }
    //OutputDebugStringA(std::format("{}\n", cnt).c_str());
    for (auto& object : objects) {
        if (object->name == "Rocket") {
            RocketState(object, dt);
        }
        if (!object->physicsLocked) {
            object->evalCollisions(dt);
        }
    }
    for (auto& object : objects) {
        if (!object->physicsLocked) {
            object->evalForces(dt);
        }
    }
    for (auto& object : objects) {
        if (!object->physicsLocked) {
            object->transform.pos += object->velocity * dt;
            if (abs(object->rotationSpeed) > 0.05)
                object->transform.rot += object->rotationSpeed * dt;
        }
    }
}


/// fill buffer in this function
/// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors
/// (8 bits per R, G, B)
void draw() {
    //if (!is_window_active()) return;
    for (auto& renderer : global::drawable) {
        renderer->Draw();
    }
    Timer::print();
}

/// free game data in this function
void finalize() {
    using namespace global;
}
