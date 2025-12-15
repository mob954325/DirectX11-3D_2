#pragma once
#include "../pch.h"
#include "../Scene/Scene.h"
#include "Renderer/RenderQueue.h"
#include <map>
#include <vector>

class SceneSystem
{
public:
	void UpdateScene(float deltaTime);
	void RenderScene(std::unique_ptr<RenderQueue>& renderQueue);

	std::shared_ptr<Scene> GetSceneByIndex(int index);
	void AddScene(std::shared_ptr<Scene> scene);

private:
	std::map<int, std::shared_ptr<Scene>> scenesMapping;
	std::shared_ptr<Scene> currentScene{};
};

