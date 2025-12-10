#pragma once
#include "directxtk/SimpleMath.h"
#include <string>

using namespace DirectX::SimpleMath;

class GameObject
{
public:
	Vector3 position{ 0,0,0 };
	Vector3 rotation{ 0,0,0 }; // euler
	Vector3 Scale{ 1,1,1 };

	std::string name;
};

