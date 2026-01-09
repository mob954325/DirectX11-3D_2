#pragma once
#include "../External/directxtk/SimpleMath.h"
#include "IComponent.h"
#include <System/InputSystem.h>

#define RTTR_DLL
#include <rttr/registration>

using namespace DirectX::SimpleMath;

class Camera : public IComponent, public InputProcesser
{
	RTTR_ENABLE(IComponent)
public:
	Camera() { SetName("Camera"); } 
	~Camera() {};

	Vector3 GetForward();
	Vector3 GetRight();
	Matrix GetView();
	
	void SetProjection(float povAngle, int width, int height, float targetNear, float targetFar);
	Matrix GetProjection() const;

	void AddPitch(float value);
	void AddYaw(float value);
	void SetInputVec(const Vector3& inputVec);

	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;

	float GetSpeed() { return moveSpeed; }
	void SetSpeed(float value) {  moveSpeed = value; }

	float GetRotateSpeed() { return rotSpeed; }
	void SetRotateSpeed(float value) { rotSpeed = value;}

	float GetPovAngle() { return povAngle; }
	void SetPovAngle(float angle) { povAngle = angle; }

	float GetNearDist() { return nearDist; }
	void SetNearDist(float value) { nearDist = value; }
	
	float GetFarDist() { return farDist; }
	void SetFarDist(float value) { farDist = value; }

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json data) override;

	void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker) override;
private:
	Matrix view = Matrix::Identity;
	Matrix projection = Matrix::Identity;

	Vector3 inputVec{};
	float moveSpeed = 100.0f;
	float rotSpeed = 0.004f;

	float povAngle = DirectX::XM_1DIV2PI;
	float nearDist = 0.01f;
	float farDist = 2000.0f;
};