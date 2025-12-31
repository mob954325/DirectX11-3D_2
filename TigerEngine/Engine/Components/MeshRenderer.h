#pragma once
#include <Manager/FBXResourceManager.h>
#include <Entity/RenderComponent.h>
#include "Renderer/DrawMeshCommand.h"

class MeshRenderer : public RenderComponent
{
private:
	std::shared_ptr<FBXResourceAsset> targetMeshData; // mesh data for rendering
	std::shared_ptr<DrawMeshCommand> command;		// original command

public:
	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;

	void SetData(std::shared_ptr<FBXResourceAsset> data);
};