#pragma once
#include "pch.h"
#include "System/SceneSystem.h"

class Editor
{
public:
    void Render(std::unique_ptr<SceneSystem>& sceneSystem, HWND& hwnd);
    void SelectObject(std::shared_ptr<GameObject> obj);

private:
    void RenderMenuBar(std::unique_ptr<SceneSystem>& sceneSystem, HWND& hwnd);
    void RenderHierarchy(std::unique_ptr<SceneSystem>& sceneSystem);
    void RenderInspector();

    template<typename T>
    void RenderComponentInfo(std::string name, std::shared_ptr<T> comp);

    void SaveCurrentScene(std::unique_ptr<SceneSystem> &sceneSystem, HWND& hwnd);
    
    std::weak_ptr<GameObject> selectedObject; // 현재 inspector 정보를 보고 있는 게임 오브젝트
};
