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
    extern std::vector<std::pair<std::string, std::function<void(std::function<Object* (Transform)>)>>> levelBuilders;

    extern string state;
    extern int rocketChoice;
    extern int levelChoice;
    extern int componentsDestroyed;


};
void Cleanup();

Object* BuildRocket1(Transform at);
Object* BuildRocket2(Transform at);
Object* BuildRocket3(Transform at);
Object* BuildRocket4(Transform at);


void BuildLevel1(std::function<Object* (Transform)> rocketBuilder);
void BuildLevel2(std::function<Object* (Transform)> rocketBuilder);
void BuildLevel3(std::function<Object* (Transform)> rocketBuilder);
void BuildLevel4(std::function<Object* (Transform)> rocketBuilder);