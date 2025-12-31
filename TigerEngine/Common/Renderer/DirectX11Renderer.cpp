#include "DirectX11Renderer.h"
#include "../Helper.h"
#include <algorithm>

#define USE_FLIPMODE 1

void DirectX11Renderer::Initialize(HWND hwnd, int width, int height)
{
	// 1. D3D11 Device, DeviceContext ����

	// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
	UINT creationFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	/// https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/overviews-direct3d-11-devices-layers
	/// ����� ������ �����ϴ� ����̽��� ���鵵�� �÷��� �߰�
	/// 
	/// ����� ������ �������� �߰� �Ű� ���� �� �ϰ��� ��ȿ�� �˻�(��: ���̴� ��ũ �� ���ҽ� ���ε� ��ȿ�� �˻�, �Ű� ���� �ϰ��� ��ȿ�� �˻� �� ���� ���� ����)�� �����մϴ�.
	/// �ش� ������ ����� ��� ���ø����̼��� ����� ��������.
	creationFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif //  _DEBUG

	// �׷��� ī�� �ϵ������ �������� ȣȯ�Ǵ� ���� ���� DirectX ��ɷ����� �����Ͽ� ����̹��� �۵��Ѵ�.
	// �������̽��� Direct3D11������ GPU ����̹��� D3D12 ����̹��� �۵��� �� �ִ�.
	D3D_FEATURE_LEVEL featureLevels[] =
	{	// 0�� index���� ������� �õ�
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0,D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL actualFeatureLevel;	// ���� feature level ���� ����

	HR_T(D3D11CreateDevice
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlag,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&device,
		&actualFeatureLevel,
		&deviceContext
	));

	// 2. ����ü�� ������ ���� DXGI Factory ����
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

	ComPtr<IDXGIFactory2> pFactory;
	HR_T(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
#if USE_FLIPMODE == 1
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
	swapChainDesc.BufferCount = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;

	// �ϳ��� �ȼ��� ä�� RGBA �� 8��Ʈ �������� ǥ��
	// Unsigned Normalized Integer 8��Ʈ ����(0~255)�ܰ踦 �ε��Ҽ������� ����ȭ�� 0.0~1.0���� �����Ͽ� ǥ���Ѵ�.
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ���� ü���� �� ���۰� ������ ������������ ���� ��� ������� ���
	swapChainDesc.SampleDesc.Count = 1;	// ��Ƽ ���ø� ��� ����
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE; // ������ ���� ���� | recommand for flip mode ?
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ��ü ȭ�� ��ȯ�� ���
	swapChainDesc.Scaling = DXGI_SCALING_NONE; // 

	HR_T(pFactory->CreateSwapChainForHwnd
	(
		device.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain.GetAddressOf()
	));

	// 3. ����Ÿ�� �� ����.
	ComPtr<ID3D11Texture2D> pBackBufferTexture;
	HR_T(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(device->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, renderTargetView.GetAddressOf()));

	// 4. viewport ����
	renderViewport = {};
	renderViewport.TopLeftX = 0;
	renderViewport.TopLeftY = 0;
	renderViewport.Width = (float)width;
	renderViewport.Height = (float)height;
	renderViewport.MinDepth = 0.0f;
	renderViewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &renderViewport);

	// 5. �X�� ���ٽ� �� ����
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // https://learn.microsoft.com/ko-kr/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	// �X�� ���Ľ� ���� ����
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;                // ���� �׽�Ʈ Ȱ��ȭ
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // ���� ���� ������Ʈ ���
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; // ���� Z ���� �տ� ��ġ�ǵ��� ����
	depthStencilDesc.StencilEnable = FALSE;            // ���ٽ� �׽�Ʈ ��Ȱ��ȭ

	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	// create depthStencil texture
	ComPtr<ID3D11Texture2D> pTextureDepthStencil;
	HR_T(device->CreateTexture2D(&descDepth, nullptr, pTextureDepthStencil.GetAddressOf()));

	// create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // ���Ǵ� ���ҽ� ������ ��� ���� : https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ne-d3d11-d3d11_dsv_dimension 
	descDSV.Texture2D.MipSlice = 0;
	HR_T(device->CreateDepthStencilView(pTextureDepthStencil.Get(), &descDSV, depthStencilView.GetAddressOf()));
}

void DirectX11Renderer::OnResize(int width, int height)
{
	// SwapChain ��������
	swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	// RTV �����
	//renderTargetView.Reset();
	ComPtr<ID3D11Texture2D> pBackBufferTexture;
	HR_T(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(device->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, renderTargetView.GetAddressOf()));
}

void DirectX11Renderer::BeginRender()
{
#if USE_FLIPMODE == 1
	// Flip ��忡���� �������� �����ؾ��Ѵ�.
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get()); // depthStencilView ���
#endif	
	// ȭ�� ĥ�ϱ�.
	Color color(0.1f, 0.2f, 0.3f, 1.0f);
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0); // �������� 1.0f�� �ʱ�ȭ.
}

void DirectX11Renderer::EndRender()
{
	swapChain->Present(0, 0);
}

void DirectX11Renderer::ProcessScene(std::shared_ptr<Scene> scene, std::shared_ptr<IRenderPass> renderPass)
{
	renderQueue.Clear();

	renderPass->Execute(deviceContext, scene);	
	// getrenderable from scene
	auto renderComps = scene->GetRenderables();

	// TODO renderqueue 구성완료하기
	// add queue
	std::for_each(renderComps.begin(), renderComps.end(), [this](auto comp)
	{ 
		// 이거 Data없을 때의 예외처리 필요함
		renderQueue.AddCommand(comp->GetCommand()); 
	});	

	// execute pass, queue
	renderQueue.Execute(deviceContext);
}

ComPtr<ID3D11Device> DirectX11Renderer::GetDevice() const
{
    return device;
}

ComPtr<ID3D11DeviceContext> DirectX11Renderer::GetDeviceContext() const
{
    return deviceContext;
}
