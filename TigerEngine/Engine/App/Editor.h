#pragma once
#include "pch.h"
#include "System/SceneSystem.h"

class Editor
{
public:
    void Render(std::unique_ptr<SceneSystem>& sceneSystem);

private:
    void RenderHierarchy(std::unique_ptr<SceneSystem>& sceneSystem);
};
