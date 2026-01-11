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

void Scene::ClearScene()
{
	for(auto& it : gameObjects)
	{
		auto obj = it.second;
		obj->Destory();	
	}
	
	gameObjects.clear();
}

bool Scene::SaveToJson(const std::string &filename) const
{
	nlohmann::json root;

	root["objects"] = nlohmann::json::array();
	for(auto& obj : gameObjects)
	{
		if(!obj.second) continue;
		
		nlohmann::json objData = obj.second->Serialize();
		root["objects"].push_back(objData);
	}

	std::ofstream file(filename);
	if(!file.is_open()) return false;

	file << root.dump(2); // ??
	file.close();

	return true;
}

bool Scene::LoadToJson(const std::string &filename)
{
	std::ifstream file(filename);
	if(!file.is_open()) return false;

	nlohmann::json root;

	try
	{
		file >> root;
	}
	catch(const nlohmann::json::exception& e)
	{	// 파일 열기 실패
		MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
		file.close();
		return false;
	}
	file.close();	

	ClearScene();
	
	// json 데이터에 objects 객체이나 배열이 없음
	if(!root.contains("objects") || !root["objects"].is_array()) return false;

	for(const auto& objData : root["objects"])
	{
		if(!objData.contains("type")) continue;

		std::string typeName = objData["type"]; // 클래스 타입 : GameObject
		
		// 이름 찾기
		std::string objectName = "";
		if(objData.contains("properties")) objectName = objData["properties"]["Name"];

		auto instance = AddGameObjectByName(objectName);
		if(!instance) continue; 

		instance->Deserialize(objData["properties"]);
	}

    return true;
}

std::weak_ptr<GameObject> Scene::GetGameobjectFromScene(std::string name)
{
	if(auto it = gameObjects.find(name); it != gameObjects.end())
	{
		return it->second;
	}
	else
	{
		return std::shared_ptr<GameObject>(); // 빈 객체 보내기
	}
}

std::weak_ptr<GameObject> Scene::RayCastGameObject(const Ray &ray, float *outDistance)
{
	std::shared_ptr<GameObject> hitObject{};
	float minDistant = FLT_MAX;

	for(auto& [name, obj]: gameObjects)
	{
		if(!obj) continue;

		float outDist = 0.0f;
		if(ray.Intersects(obj->GetAABB(), outDist))
		{
			if(outDist < minDistant)
			{
				minDistant = outDist;
				hitObject = obj;
			}
		}
	}

	if(outDistance != nullptr && hitObject)
	{
		*outDistance = minDistant;
	}

    return hitObject;
}
