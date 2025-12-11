#pragma once
#include "IRenderer.h"

// dxgi 어뎁터 조회용
#include <dxgi1_6.h>
#include <psapi.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "psapi.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <wrl/client.h>

/// <summary>
/// DirectX11 기능을 사용하는 클래스
/// </summary>
class DirectX11Renderer : public IRenderer
{
public:
	void Initialize(HWND hwnd, int width, int height) override;
	void OnResize(int width, int height) override;
	void BeginRender() override;
	void EndRender() override;

private:
	ComPtr<ID3D11Device>			device{};			// 디바이스
	ComPtr<ID3D11DeviceContext>		deviceContext{};	// 디바이스 컨텍스트
	ComPtr<IDXGISwapChain1>			swapChain{};		// 스왑체인 
	ComPtr<ID3D11RenderTargetView>	renderTargetView{};	// 랜더 타겟	

	D3D11_VIEWPORT					renderViewport{};
	ComPtr<ID3D11DepthStencilState> depthStencilState{};
	ComPtr<ID3D11DepthStencilView>	depthStencilView{};	// 뎊스 스텐실 뷰
};

