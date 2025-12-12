#pragma once
#include "IComponent.h"
#include <string>
#include <vector>

using namespace DirectX::SimpleMath;

/// <summary>
/// GameObject는 컴포넌트를 담고 있는 순수한 컨테이너
/// </summary>
class GameObject
{
protected:
	std::string name;
	std::vector<IComponent> components;
};

