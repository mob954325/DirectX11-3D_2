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