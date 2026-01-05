#pragma once
#include "pch.h"
#include "System/SceneSystem.h"

class Editor
{
public:
    void Render(std::unique_ptr<SceneSystem>& sceneSystem);
    void SelectObject(std::shared_ptr<GameObject> obj);
private:
    void RenderHierarchy(std::unique_ptr<SceneSystem>& sceneSystem);
    void RenderInspector();
    
    std::shared_ptr<GameObject> selectedObject; // 현재 inspector 정보를 보고 있는 게임 오브젝트
};
