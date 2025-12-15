#pragma once
#include "IComponent.h"
#include "../Renderer/RenderQueue.h"

class IRenderComponent : public IComponent
{
public:
	virtual void OnRender(std::unique_ptr<RenderQueue>& queue) {};
};
