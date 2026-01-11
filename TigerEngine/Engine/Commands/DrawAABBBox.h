#pragma once
#include "pch.h"
#include <Util/DebugDraw.h>
#include <Renderer/IRenderCommand.h>

class DrawAABBBox : public IRenderCommand
{
public:
    void Execute(ComPtr<ID3D11DeviceContext>& context) override;
private:
    
}