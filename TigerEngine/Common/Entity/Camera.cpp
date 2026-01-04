#include "Camera.h"
#include <Entity/GameObject.h>

Vector3 Camera::GetForward()
{
	Matrix world = owner->GetTransform()->GetWorldTransform();
	return -world.Forward();
}

Vector3 Camera::GetRight()
{
	Matrix world = owner->GetTransform()->GetWorldTransform();
    return world.Right();
}

Matrix Camera::GetView()
{
	Matrix world = owner->GetTransform()->GetWorldTransform();
	Vector3 eye = world.Translation();
	Vector3 target = world.Translation() + GetForward();
	Vector3 up = world.Up();

	this->view = DirectX::XMMatrixLookAtLH(eye, target, up);
	return view;
}

void Camera::AddPitch(float value)
{
	auto& rot = owner->GetTransform()->rotation;
	rot.x += value;

	if(rot.x > XM_PI)
	{
		rot.x -= XM_2PI;
	}
	else if(rot.x < -XM_PI)
	{
		rot.x += XM_2PI;
	}
}

void Camera::AddYaw(float value)
{
	auto& rot = owner->GetTransform()->rotation;
	rot.y += value;

	if(rot.y > XM_PI)
	{
		rot.y -= XM_2PI;
	}
	else if(rot.y < -XM_PI)
	{
		rot.y += XM_2PI;
	}
}

void Camera::SetInputVec(const Vector3 &inputVec)
{
	this->inputVec += inputVec;
	this->inputVec.Normalize();
}

void Camera::OnInitialize()
{
	InputSystem::Instance().Register(this);
}

void Camera::OnStart()
{
}

void Camera::OnUpdate(float delta)
{
	auto& transform = owner->GetTransform();
	auto& position = transform->position;
	auto& rotation = transform->rotation;
	if (inputVec.Length() > 0.0f)
	{
		position += inputVec * moveSpeed * delta;
		inputVec = Vector3::Zero;
	}

	auto& world = transform->GetWorldTransform();
	world = Matrix::CreateFromYawPitchRoll(rotation) * Matrix::CreateTranslation(position);	
}

void Camera::SetProjection(float povAngle, int width, int height, float targetNear, float targetFar)
{
	projection = DirectX::XMMatrixPerspectiveFovLH(povAngle, width / (float)height, targetNear, targetFar);

	this->povAngle = povAngle;
	this->nearDist = targetNear;
	this->farDist = targetFar;
}

Matrix Camera::GetProjection() const
{
	return projection;
}

void Camera::OnInputProcess(const Keyboard::State &KeyState, const Keyboard::KeyboardStateTracker &KeyTracker, const Mouse::State &MouseState, const Mouse::ButtonStateTracker &MouseTracker)
{
	Vector3 forward = GetForward();
	Vector3 right = GetRight();

	if (KeyTracker.IsKeyPressed(Keyboard::Keyboard::R))
	{
		//Reset();
	}

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::W))
	{
		SetInputVec(forward);
	}
	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::S))
	{
		SetInputVec(-forward);
	}

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::A))
	{
		SetInputVec(-right);
	}
	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::D))
	{
		SetInputVec(right);
	}

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::E))
	{
		Matrix& world = owner->GetTransform()->GetWorldTransform();		
		SetInputVec(-world.Up());
	}
	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::Q))
	{
		Matrix& world = owner->GetTransform()->GetWorldTransform();		
		SetInputVec(world.Up());
	}

	// 속도 추가
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::D1))
	{
		moveSpeed = 20;
	}
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::D2))
	{
		moveSpeed = 100;
	}
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::D3))
	{
		moveSpeed = 200;
	}

	Singleton<InputSystem>::Instance().m_Mouse->SetMode(MouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
	if (MouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(MouseState.x), float(MouseState.y), 0.f) * rotSpeed;
		AddPitch(delta.y);
		AddYaw(delta.x);
	}
}
