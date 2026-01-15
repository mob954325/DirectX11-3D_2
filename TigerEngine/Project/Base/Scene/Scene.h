#pragma once
#include "../pch.h"
#include "../Renderer/RenderQueue.h"
#include "../Entity/RenderComponent.h"
#include <map>

class GameObject;

struct GameObjectEntity
{
	GameObject* objPtr;
	Handle handle;
};

class Scene
{
public:
	void OnRender(std::unique_ptr<RenderQueue>& renderQueue);
	void OnUpdate(float deltaTime);	
	void CheckDestroy();

	void ForEachGameObject(std::function<void(GameObject*)> fn);

	GameObject* AddGameObjectByName(std::string name); // add empty gameObject to Scene
	GameObject* GetGameObjectByName(std::string name);

	/// @brief 모든 씬 오브젝트들을 제거하는 함수
	void ClearScene();

	/// @brief json으로 scene정보를 저장하는 함수
	/// @param filename 저장할 파일 이름
	bool SaveToJson(const std::string& filename) const;

	/// @brief json 파일을 읽어서 scene에 로드하는 함수
	/// @param 불러오는 파일 이름
	bool LoadToJson(const std::string& filename);
	
	int GetObjectCount() { return gameObjects.size(); }
	GameObject* GetGameobjectFromScene(std::string name);

	GameObject* RayCastGameObject(const Ray& ray, float* outDistance);

protected:
	std::multimap<std::string, GameObjectEntity> gameObjects;
};