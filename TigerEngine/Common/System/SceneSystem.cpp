#include "SceneSystem.h"

void SceneSystem::UpdateScene(float deltaTime)
{
	if(scenesMapping.empty()) return; // 씬 없어서 터지는거 방지

	currentScene->OnUpdate(deltaTime);
}

void SceneSystem::RenderScene()
{
	if(scenesMapping.empty()) return;

	currentScene->OnRender();
}

std::shared_ptr<Scene> SceneSystem::GetSceneByIndex(int index)
{
	return scenesMapping.find(index)->second;
}

void SceneSystem::AddScene(std::shared_ptr<Scene> scene)
{

}