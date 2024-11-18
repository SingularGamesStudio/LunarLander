#pragma once
#include"Geometry.h"
#include<string>


using std::string;

struct Object {
	Transform transform;
	string name;
	bool physicsLocked;

	Dot velocity;
	Dot massCenter;
	Dot mass;
};