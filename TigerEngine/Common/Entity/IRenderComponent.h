#pragma once
#include "IComponent.h"

class IRenderComponent : public IComponent
{
	virtual void OnRender() {};
};
