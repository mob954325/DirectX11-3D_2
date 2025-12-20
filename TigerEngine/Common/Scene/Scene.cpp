#include "Scene.h"	

void Scene::OnRender(std::unique_ptr<RenderQueue>& renderQueue)
{
	for (auto& obj : gameObjects)
	{
		std::shared_ptr<GameObject> gameObject = obj.second;		
		for(auto& rComp : gameObject->GetIRenderComponents())
		{
			rComp.lock()->OnRender(renderQueue);
		}
	}
}

void Scene::OnUpdate(float deltaTime)
{
	for (auto& obj : gameObjects)
	{
		std::shared_ptr<GameObject> gameObject = obj.second;		
		for(auto& rComp : gameObject->GetIComponents())
		{
			rComp.lock()->OnUpdate(deltaTime);
		}
	}
}

void Scene::ForEachGameObject(std::function<void(std::shared_ptr<GameObject>)> fn)
{
	for(auto& obj : gameObjects)
	{
		fn(obj.second);
	}
}

std::shared_ptr<GameObject> Scene::AddGameObject(std::string name)
{
	auto [it, success] = gameObjects.insert({name, std::make_shared<GameObject>(name)});

    return it->second;
}

std::shared_ptr<GameObject> Scene::GetGameObjectByName(std::string name)
{
    return gameObjects.find(name)->second;
}
