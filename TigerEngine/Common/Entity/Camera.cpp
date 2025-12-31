#include "Camera.h"
#include <Entity/GameObject.h>

Vector3 Camera::GetForward(Matrix world)
{
	return -world.Forward();
}

Matrix Camera::GetView()
{
	Matrix world = owner->GetTransform()->GetWorldTransform();
	Vector3 eye = world.Translation();
	Vector3 target = eye + GetForward(world);
	Vector3 up = world.Up();

	this->view = DirectX::XMMatrixLookAtLH(eye, target, up);
	return view;
}

void Camera::SetProjection(float povAngle, int width, int height, float targetNear, float targetFar)
{
	DirectX::XMMatrixPerspectiveFovLH(povAngle, width / height, targetNear, targetFar);

	this->povAngle = povAngle;
	this->nearDist = targetNear;
	this->farDist = targetFar;
}

Matrix Camera::GetProjection() const
{
	return projection;
}
