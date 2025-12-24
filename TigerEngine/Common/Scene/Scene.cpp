#include "Scene.h"	

void Scene::OnRender(std::unique_ptr<RenderQueue>& renderQueue)
{
	for (auto& obj : gameObjects)
	{
		std::shared_ptr<GameObject> gameObject = obj.second;		
		if(gameObject->IsDestory()) continue;

		for(auto& rComp : gameObject->GetIRenderComponents())
		{
			rComp->OnRender(renderQueue);
		}
	}
}

void Scene::OnUpdate(float deltaTime)
{
	for (auto& obj : gameObjects)
	{
		std::shared_ptr<GameObject> gameObject = obj.second;	
		if(gameObject->IsDestory()) continue;

		for(auto& rComp : gameObject->GetIComponents())
		{
			rComp->OnUpdate(deltaTime);
		}
	}
}

void Scene::CheckDestroy()
{
	for(auto it = gameObjects.begin(); it != gameObjects.end();)
	{
		if(it->second->IsDestory())
		{
			it = gameObjects.erase(it);
		}
		else
		{
			it++;
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
	auto obj = std::make_shared<GameObject>(name);
	gameObjects.insert({name, obj});

    return obj;
}

std::shared_ptr<GameObject> Scene::GetGameObjectByName(std::string name)
{
    return gameObjects.find(name)->second;
}
