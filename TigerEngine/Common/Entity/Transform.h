#pragma once
#include "../pch.h"
#include "SimpleMath.h"
#include "IComponent.h"

#define RTTR_DLL
#include <rttr/registration>

class Transform : public IComponent
{
public:
	Vector3 position{ 0,0,0 };
	Vector3 rotation{ 0,0,0 };
	Vector3 scale{ 1,1,1 };

	Matrix GetWorldTransform() const;

	std::shared_ptr<Transform> parent{};
};

