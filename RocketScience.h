#pragma once
#include<random>
#include<functional>
#include "Components.h"

namespace global {
    extern std::vector<Object*> objects;
    extern std::vector<PolyCollider*> colliders;
    extern std::vector<Drawable*> drawable;
    extern std::vector<Controlled*> controls;
    extern std::mt19937 rnd;

    extern std::vector<std::pair<std::string, std::function<Object* (Transform)>>> rocketBuilders;
    extern std::vector<std::pair<std::string, std::function<Transform()>>> levelBuilders;

    extern string state;
    extern int rocketChoice;
    extern int levelChoice;
    extern int componentsDestroyed;
};
void Cleanup();

Object* BuildRocket(Transform at);

Transform BuildLevel();