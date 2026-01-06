#include "Scene.h"	
#include <Entity/GameObject.h>

void Scene::OnRender(std::unique_ptr<RenderQueue>& renderQueue)
{
	for(auto it = gameObjects.begin(); it != gameObjects.end(); it++)
	{
		auto gameObject = it->second;
		// .. rendering
	}
}

void Scene::OnUpdate(float deltaTime)
{
	for(auto it = gameObjects.begin(); it != gameObjects.end(); it++)
	{
		auto gameObject = it->second;
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
		auto gameObject = it->second;
		if(gameObject->IsDestory())
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

std::vector<std::weak_ptr<RenderComponent>>& Scene::GetRenderables()
{
	return renderableComponents;
}
