#pragma once
#include "../pch.h"
#include "../Entity/GameObject.h"
#include <map>

class Scene
{
public:
	void OnRender();
	void OnUpdate();

protected:
	std::map<std::string, std::shared_ptr<GameObject>> gameObjects; // 이름, 게임 오브젝트
};