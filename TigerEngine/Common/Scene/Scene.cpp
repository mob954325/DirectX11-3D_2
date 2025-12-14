#include "Scene.h"	

void Scene::OnRender()
{
	for (auto& obj : gameObjects)
	{
		std::shared_ptr<GameObject> gameObject = obj.second;
		
	}
}

void Scene::OnUpdate(float deltaTime)
{

}