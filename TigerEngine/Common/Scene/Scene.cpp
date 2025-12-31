#include "Scene.h"	
#include <Entity/GameObject.h>

void Scene::OnRender(std::unique_ptr<RenderQueue>& renderQueue)
{
	for (auto& obj : gameObjects)
	{
		std::shared_ptr<GameObject> gameObject = obj.second;		
		if(gameObject->IsDestory()) continue;
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

void Scene::AddGameObject(std::shared_ptr<GameObject> obj)
{
	gameObjects.insert({obj->GetName(), obj});
}

std::shared_ptr<GameObject> Scene::AddGameObjectByName(std::string name)
{
	auto obj = std::make_shared<GameObject>(this, name);

	gameObjects.insert({name, obj});
    return obj;
}

std::shared_ptr<GameObject> Scene::GetGameObjectByName(std::string name)
{
    return gameObjects.find(name)->second;
}

void Scene::AddRenderable(std::shared_ptr<RenderComponent> comp)
{
	renderableComponents.push_back(comp);
}

std::vector<std::shared_ptr<RenderComponent>>& Scene::GetRenderables()
{
	return renderableComponents;
}
