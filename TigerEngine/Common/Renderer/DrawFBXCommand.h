#pragma once
#include "pch.h"
#include <Renderer/IRenderCommand.h>
#include <Datas/FBXResourceData.h>

class DrawFBXCommand : public IRenderCommand
{
private:
    FBXResourceAsset refFBXData{};
public:
    void CreateCommand(FBXResourceAsset& data); // TODO 상수 버퍼 어떻게 만들지?

    void Execute(ComPtr<ID3D11DeviceContext> context) override
    {
        if(refFBXData.meshes.empty()) return; // 그릴 메쉬가 없음 -> FBXData가 없음

        

    }
};