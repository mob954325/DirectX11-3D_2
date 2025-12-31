#pragma once
#include "../External/directxtk/SimpleMath.h"
#include "IComponent.h"

using namespace DirectX::SimpleMath;

class Camera : public IComponent
{
public:
	Vector3 GetForward(Matrix world);

	Matrix GetView();

	void SetProjection(float povAngle, int width, int height, float targetNear, float targetFar);
	Matrix GetProjection() const;

private:
	Matrix view = Matrix::Identity;
	Matrix projection = Matrix::Identity;

	float povAngle = DirectX::XM_1DIV2PI;
	float nearDist = 0.01f;
	float farDist = 2000.0f;
};