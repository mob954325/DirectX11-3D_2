#pragma once
#include "../pch.h"
#include "Transform.h"
#include "IComponent.h"
#include "IRenderComponent.h"
#include <string>
#include <vector>

/// <summary>
/// GameObject는 컴포넌트를 담고 있는 순수한 컨테이너
/// </summary>
class GameObject
{
public:
	GameObject();

	template<typename T>
	std::shared_ptr<T> AddComponent();

	std::shared_ptr<Transform> GetTransform() const;

protected:
	std::shared_ptr<Transform> transform;
	std::string name;
	std::vector<std::shared_ptr<IComponent>> components; // Update, Physics 등
	std::vector<std::shared_ptr<IRenderComponent>> renderComponents; // Mesh, Material, Color 등등
};

