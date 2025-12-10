#include "pch.h"
#include "Camera.h"

FreeCamera::FreeCamera()
{
	Reset();
}

void FreeCamera::Reset()
{
	world = Matrix::Identity;
	position = initPosition;
	rotation = Vector3::Zero;
}

void FreeCamera::Update(float elapsedTime)
{
	if (inputVec.Length() > 0.0f)
	{
		position += inputVec * moveSpeed * elapsedTime;
		inputVec = Vector3::Zero;
	}

	world = Matrix::CreateFromYawPitchRoll(rotation) * 
		Matrix::CreateTranslation(position);	
}

void FreeCamera::SetPosition(const Vector3& posVec)
{
	position = posVec;
}

void FreeCamera::SetRotation(const Vector3& rotVec)
{
	rotation = rotVec;
}

void FreeCamera::GetCameraViewMatrix(Matrix& outMat)
{
	Vector3 eye = world.Translation();
	Vector3 target = world.Translation() + GetForward();
	Vector3 up = world.Up();
	outMat = XMMatrixLookAtLH(eye, target, up);
}

void FreeCamera::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{	
	Vector3 forward = GetForward();
	Vector3 right = GetRight();

	if (KeyTracker.IsKeyPressed(Keyboard::Keyboard::R))
	{
		Reset();
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
		SetInputVec(-world.Up());
	}
	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::Q))
	{
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

	InputSystem::Instance->m_Mouse->SetMode(MouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
	if (MouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(MouseState.x), float(MouseState.y), 0.f) * rotationSpeed;
		AddPitch(delta.y);
		AddYaw(delta.x);
	}
}

Vector3 FreeCamera::GetForward()
{
	return -world.Forward();
}

Vector3 FreeCamera::GetRight()
{
	return world.Right();
}

void FreeCamera::AddPitch(float value)
{
	rotation.x += value;

	if (rotation.x > XM_PI)
	{
		rotation.x -= XM_2PI;
	}
	else if (rotation.x < -XM_PI)
	{
		rotation.x += XM_2PI;
	}
}

void FreeCamera::AddYaw(float value)
{
	rotation.y += value;

	if (rotation.y > XM_PI)
	{
		rotation.y -= XM_2PI;
	}
	else if (rotation.y < -XM_PI)
	{
		rotation.y += XM_2PI;
	}
}

void FreeCamera::SetInputVec(const Vector3& inputVec)
{
	this->inputVec += inputVec;	
	this->inputVec.Normalize();
}