#pragma once
#include "../pch.h"
#include "directxtk/SimpleMath.h"
#include "IComponent.h"

using namespace DirectX::SimpleMath;
class Transform : public IComponent
{
public:
	Vector3 position{ 0,0,0 };
	Vector3 rotation{ 0,0,0 };
	Vector3 scale{ 1,1,1 };

	Matrix GetWorldTransform() const;

	std::shared_ptr<Transform> parent{};
};

