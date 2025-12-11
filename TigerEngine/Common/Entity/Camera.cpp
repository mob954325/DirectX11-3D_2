#include "Camera.h"

Vector3 Camera::GetForward(Matrix world)
{
	return -world.Forward();
}

void Camera::SetView(Matrix world)
{
	Vector3 eye = world.Translation();
	Vector3 target = eye + GetForward(world);
	Vector3 up = world.Up();

	this->view = DirectX::XMMatrixLookAtLH(eye, target, up);
}

Matrix Camera::GetView()
{
	return view;
}

void Camera::SetProjection(float povAngle, int width, int height, float targetNear, float targetFar)
{
	DirectX::XMMatrixPerspectiveFovLH(povAngle, width / height, targetNear, targetFar);

	this->povAngle = povAngle;
	this->nearDist = targetNear;
	this->farDist = targetFar;
}

Matrix Camera::GetProjection()
{
	return projection;
}
