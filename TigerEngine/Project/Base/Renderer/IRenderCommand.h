#pragma once
#include "../pch.h"
#include "../Renderer/IRenderer.h"

class IRenderCommand
{
public:
    virtual ~IRenderCommand() = default;

    /// @brief This function execute directx api function
    virtual void Execute(ComPtr<ID3D11DeviceContext>& renderer) = 0;
};