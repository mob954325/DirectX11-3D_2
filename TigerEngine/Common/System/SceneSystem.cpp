#include "SceneSystem.h"

void SceneSystem::UpdateScene(float deltaTime)
{
	currentScene->OnUpdate(deltaTime);
}

void SceneSystem::RenderScene()
{
	currentScene->OnRender();
}

std::shared_ptr<Scene> SceneSystem::GetSceneByIndex(int index)
{
	return scenesMapping.find(index)->second;
}

void SceneSystem::AddScene(std::shared_ptr<Scene> scene)
{

}