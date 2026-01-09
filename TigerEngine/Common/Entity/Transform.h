#pragma once
#include "pch.h"
#include "IComponent.h"

class Transform : public IComponent
{
	RTTR_ENABLE(IComponent)
public:
	Transform() { SetName("Transform"); }
	~Transform() = default;

	Vector3 position{ 0,0,0 };
	Vector3 rotation{ 0,0,0 };
	Vector3 scale{ 1,1,1 };

	Matrix GetWorldTransform() const;
	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json data) override;

	//std::shared_ptr<Transform> parent{};
};

