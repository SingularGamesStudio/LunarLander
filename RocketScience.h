#pragma once
#include<random>
#include "Components.h"

namespace global {
    extern std::vector<Object*> objects;
    extern std::vector<PolyCollider*> colliders;
    extern std::vector<Drawable*> drawable;
    extern std::vector<Controlled*> controls;
    extern std::mt19937 rnd;
};

Object* BuildRocket(Transform at);