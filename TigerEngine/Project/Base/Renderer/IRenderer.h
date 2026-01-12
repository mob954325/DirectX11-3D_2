#pragma once
#include "../pch.h"
#include "../Datas/MaterialData.h"
#include "../Datas/Mesh.h"

/// @brief 그래픽 api를 사용하는 클래스가 상속받는 클래스
class IRenderer
{
public:
	virtual void Initialize(HWND hwnd, int width, int height) = 0;
	virtual void OnResize(int width, int height) = 0;
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
};