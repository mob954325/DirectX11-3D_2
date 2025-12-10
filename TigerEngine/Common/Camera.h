#pragma once
#include <directxtk/SimpleMath.h>
#include "System/InputSystem.h"

using namespace DirectX::SimpleMath;

class FreeCamera : public InputProcesser
{
public:
	FreeCamera();
	Vector3 position{};
	Vector3 initPosition = { 0,0,-30.0f };
	Vector3 rotation{};
	Matrix world{};

	Vector3 inputVec{};
	float moveSpeed = 100.0f;
	float rotationSpeed = 0.004f;

	void Reset();
	void Update(float elapsedTime);

	void SetPosition(const Vector3& posVec);
	void SetRotation(const Vector3& rotVec);
	void GetCameraViewMatrix(Matrix& outMat);

	// 카메라 이동 함수
	virtual void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker);

	Vector3 GetForward();

protected:
	Vector3 GetRight();
	void AddPitch(float value);
	void AddYaw(float value);

	void SetInputVec(const Vector3& inputVec);
};