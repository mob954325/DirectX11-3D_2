#include "Scene.h"	

void Scene::OnRender()
{
	for (auto& obj : gameObjects)
	{
		std::shared_ptr<GameObject> gameObject = obj.second;		
		for(auto& rComp : gameObject->GetIRenderComponents())
		{
			rComp->OnRender();
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
			rComp->OnUpdate(deltaTime);
		}
	}
}