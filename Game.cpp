#include <memory.h>
#include <stdlib.h>

#include "Engine.h"
#include "Object.h"
#include "Render.h"

//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT,
//                                       VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 -
//  left button, 1 - right button) clear_buffer() - set all pixels in buffer to
//  'black' is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()
double pos = 0;

Object rocket;
PolyCollider coll;
PolygonRenderer render;
Object rocket1;
PolyCollider coll1;
PolygonRenderer render1;

// initialize game data in this function
void initialize() {
    // clear backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    coll.shape = new box{ &rocket.transform, 50, 30, Transform{Dot{400, 550}, Rot{pi / 4}} };
    coll.draw = true;
    coll.parent = &rocket;
    rocket.components.push_back(&coll);
    render.item = coll.shape;

    coll1.shape = new box{ &rocket1.transform, 50, 30, Transform{Dot{0, 550}, Rot{pi / 11}} };
    coll1.draw = true;
    coll1.parent = &rocket1;
    rocket1.components.push_back(&coll1);

    render1.item = coll1.shape;
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt) {
    if (is_key_pressed(VK_ESCAPE)) schedule_quit_game();
    pos += 0.5;
}


// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors
// (8 bits per R, G, B)
void draw() {
    coll1.shape->transform->pos += dotRight * pos;
    pos = 0;
    render.Draw(255);
    auto res = intersect(*coll1.shape, *coll.shape);
    render1.Draw((res.first > 0) ? 255 * 256 : 255 * 256 * 256);
}

// free game data in this function
void finalize() {}
