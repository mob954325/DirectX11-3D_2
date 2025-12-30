#pragma once
#include <Entity/RenderComponent.h>
#include <Components/FBXData.h>

class MeshRenderer : public RenderComponent
{
private:
	std::shared_ptr<FBXData> targetMeshData; // mesh data for rendering
	
public:
	MeshRenderer();
	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;
};