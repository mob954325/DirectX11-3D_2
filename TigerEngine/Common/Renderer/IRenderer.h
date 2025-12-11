#pragma once
#include "../../Common/pch.h"

/// <summary>
/// 그래픽 랜더러 클래스가 상속받는 인터페이스
/// </summary>
class IRenderer
{
public:
	virtual void Initialize(HWND hwnd, int width, int height) {};
	virtual void OnResize(int width, int height) {};
	virtual void BeginRender() {};
	virtual void EndRender() {};
};