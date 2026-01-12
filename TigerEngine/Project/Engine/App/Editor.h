#pragma once
#include "pch.h"
#include "System/InputSystem.h"
#include "Entity/GameObject.h"

/// @brief imgui를 사용하고 렌더링 하는 객체
/// @date 26 01 07
/// @details
class Editor : public InputProcesser
{
public:
    void GetScreenSize(int width, int height) { screenWidth = width; screenHeight = height; }
    void Update();
    void Render(HWND& hwnd);
    void SelectObject(std::shared_ptr<GameObject> obj);

private:
    void RenderMenuBar(HWND& hwnd);
    void RenderHierarchy();
    void RenderInspector();

    template<typename T>
    void RenderComponentInfo(std::string name, std::shared_ptr<T> comp);

    void SaveCurrentScene(HWND& hwnd);
    void LoadScene(HWND& hwnd);
    
    std::weak_ptr<GameObject> selectedObject; // 현재 inspector 정보를 보고 있는 게임 오브젝트

    Matrix cameraView{};
    Matrix cameraProjection{};
    int screenWidth = 0;
    int screenHeight = 0;

public:
	void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker) override;
};
