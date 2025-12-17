#include "ImguiRenderer.h"

#include <Windows.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

void ImguiRenderer::BeginRender()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImguiRenderer::Render()
{
    // Imgui 렌더링 내용
    ImGui::ShowDemoWindow();
    renderContents();
    
    // ==
    ImGui::Render();

    // ==
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& current_io = ImGui::GetIO();
    if (current_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImguiRenderer::EndRender()
{
}

void ImguiRenderer::SetRenderContants(std::function<void()> contants)
{
    renderContents = contants;
}

ImguiRenderer::~ImguiRenderer()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImguiRenderer::Initialize(HWND hwnd, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContex)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device.Get(), deviceContex.Get());
}
