#pragma once
#include "../pch.h"
#include "Component.h"

class Transform : public Component
{
	RTTR_ENABLE(Component)
public:
	Transform() { SetName("Transform"); }
	~Transform() = default;

	void OnUpdate(float delta) override;

	Vector3 position{ 0,0,0 };
	Vector3 rotation{ 0,0,0 };
	Vector3 scale{ 1,1,1 };

	Matrix worldMatrix{};

	Matrix GetWorldTransform() const;
	void Translate(Matrix mat);

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json data) override;

	bool dirty = false; // TODO 아직 사용 안함 사용하면 해당 주석 제거

	//std::shared_ptr<Transform> parent{};
};

