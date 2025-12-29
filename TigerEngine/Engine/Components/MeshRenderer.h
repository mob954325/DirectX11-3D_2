#pragma once
#include <Entity/IRenderComponent.h>
#include <Components/FBXData.h>
#include <Renderer/DrawMeshCommand.h>

class MeshRenderer : public IRenderComponent
{
private:
	std::shared_ptr<FBXData> targetMeshData; // mesh data for rendering
    DrawMeshCommand command;
	
public:
	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;
    void OnRender(std::unique_ptr<RenderQueue>& queue) override;
};