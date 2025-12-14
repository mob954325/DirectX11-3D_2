#pragma once
#include "IComponent.h"

class IRenderComponent : public IComponent
{
public:
	virtual void OnRender() {};
};
