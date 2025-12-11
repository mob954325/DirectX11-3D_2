#pragma once
#include "directxtk/SimpleMath.h"
#include "IComponent.h"
#include <string>
#include <vector>

using namespace DirectX::SimpleMath;

/// <summary>
/// Transform과 컴포넌트를 담고 있는 컨테이너
/// </summary>
class GameObject
{
public:
	Vector3 position{ 0,0,0 };
	Vector3 rotation{ 0,0,0 };
	Vector3 Scale{ 1,1,1 };

protected:
	std::string name;
	std::vector<IComponent> components;
};

