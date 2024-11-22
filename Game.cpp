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

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT,
//                                       VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 -
//  left button, 1 - right button) clear_buffer() - set all pixels in buffer to
//  'black' is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()



void startGame() {
    ClearMenu();
    Cleanup();
    global::levelBuilders[global::levelChoice].second(global::rocketBuilders[global::rocketChoice].second);
    for (auto obj : global::objects) {
        fixCenter(obj);
    }
    UI::start("rocket state", { 20, 500 }, 2);
    UI::start("tip", { 10, 10 }, 1);
    Timer::start("/land");
    Timer::start("/game");
    global::componentsDestroyed = 0;
    global::state = "Game";
}

void menu() {
    Cleanup();
    DrawRect(0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, 0);
    global::state = "Menu";
}

void evalControls(float dt) {
    std::unordered_set<char> pressed = {};
    for (char button : vector<char>{ VK_ESCAPE, VK_SPACE, VK_RIGHT,VK_LEFT, VK_UP, VK_DOWN, 'A', 'B' }) {
        if (is_key_pressed(button)) {
            pressed.insert(button);
        }
    }
    if (pressed.contains(VK_ESCAPE)) {
        schedule_quit_game();
        return;
    }

    for (auto& control : global::controls) {
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

    if (global::state == "Menu") {
        if (pressed.contains(VK_RIGHT) && Timer::elapsed("/r") > 0.3) {
            global::rocketChoice = (global::rocketChoice + 1) % (int)global::rocketBuilders.size();
            Timer::start("/r");
        }
        if (pressed.contains(VK_LEFT) && Timer::elapsed("/l") > 0.3) {
            global::rocketChoice = (global::rocketChoice - 1 + (int)global::rocketBuilders.size()) % (int)global::rocketBuilders.size();
            Timer::start("/l");
        }
        if (pressed.contains(VK_DOWN) && Timer::elapsed("/d") > 0.3) {
            global::levelChoice = (global::levelChoice + 1) % (int)global::levelBuilders.size();
            Timer::start("/d");
        }
        if (pressed.contains(VK_UP) && Timer::elapsed("/u") > 0.3) {
            global::levelChoice = (global::levelChoice - 1 + (int)global::levelBuilders.size()) % (int)global::levelBuilders.size();
            Timer::start("/u");
        }
        if (pressed.contains(VK_SPACE) && Timer::elapsed("/space") > 0.3) {
            startGame();
            Timer::start("/space");
        }
    }
    else {
        if (pressed.contains('A')) {
            menu();
        }
    }
    if (global::state == "Victory") {
        if (pressed.contains(VK_SPACE) && Timer::elapsed("/space") > 0.3) {
            menu();
            Timer::start("/space");
        }
    }
}

void tickPhysics(float dt) {
    using namespace global;
    for (auto& object : objects) {//gravity
        if (!object->physicsLocked && object->aliveCounter) {
            object->applyForce(dotUp * 20 * object->mass, object->transform.pos);
        }
    }
    for (int i = 0; i < colliders.size(); i++) {//find collisions
        for (int j = i + 1; j < colliders.size(); j++) {
            if (colliders[i]->parent->physicsLocked && colliders[j]->parent->physicsLocked ||
                colliders[i]->parent == colliders[j]->parent ||
                colliders[i]->hp <= 0 || colliders[j]->hp <= 0
                ) {
                continue;
            }
            auto inter = intersect(*colliders[i]->shape, *colliders[j]->shape);
            if (inter.first > 0) {
                resolveCollision(colliders[i], colliders[j], inter.second);
            }
        }
    }
    for (auto& object : objects) {
        if (object->name == "Rocket") {//check win condition
            RocketState(object);
        }
        if (!object->physicsLocked && object->aliveCounter) {//apply collisions
            object->evalCollisions(dt);
        }
    }
    for (auto& object : objects) {//change speed
        if (!object->physicsLocked && object->aliveCounter) {
            object->evalForces(dt);
        }
    }
    for (auto& object : objects) {//apply speed
        if (!object->physicsLocked && object->aliveCounter) {
            object->transform.pos += object->velocity * dt;
            object->transform.rot += object->rotationSpeed * dt;
        }
    }
}
/// Initialize game data
void initialize() {
    using namespace global;
    // set up backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
    Timer::start("/r");
    Timer::start("/l");
    Timer::start("/u");
    Timer::start("/d");
    Timer::start("/space");
}

/// Called to update game data,
/// dt - time elapsed since the previous update (in seconds)
void act(float dt) {
    global::lastDT = dt;
    if (!is_window_active())
        return;

    evalControls(dt);

    if (global::state == "Game") {
        tickPhysics(dt);
    }
}


/// fill buffer in this function
/// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors
/// (8 bits per R, G, B)
void draw() {
    if (!is_window_active()) return;
    if (global::state != "Victory") {
        for (auto& renderer : global::drawable) {
            renderer->Draw();
        }
    }
    if (global::state == "Game") {
        UI::write("tip", "Arrow keys to move\n<A> to exit to menu\n<ESC> to exit", colors::white);
        std::vector<Explosion*> del;
        for (Explosion* exp : global::explosions) {
            if (!exp->draw(colors::orange))
                del.push_back(exp);
        }
        for (Explosion* exp : del) {
            global::explosions.erase(exp);
            delete exp;
        }
    }
    if (global::state == "Menu") {
        DrawMenu();
    }
    Timer::print();
}

/// free game data in this function
void finalize() {
    Cleanup();
}
