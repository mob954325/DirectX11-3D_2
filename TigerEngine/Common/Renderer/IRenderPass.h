#pragma once
#include "../pch.h"
#include "../Scene/Scene.h"
// #include "../???/Camera"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

/// <summary>
/// 렌더 패스가 상속받는 클래스 ( depth only pass 등등 ) 
/// </summary>
class IRenderPass
{
public:
	virtual ~IRenderPass() = default;

	virtual void Execute(
		ComPtr<ID3D11DeviceContext>& context,
		std::shared_ptr<Scene> scene
		// const Camera& camera // ?
	) = 0;
};