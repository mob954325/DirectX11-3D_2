#pragma once
#include "pch.h"
#include <Renderer/IRenderCommand.h>
#include <Datas/FBXResourceData.h>
#include <Entity/Transform.h>

class DrawFBXCommand : public IRenderCommand
{
public:
    void CreateCommand(std::weak_ptr<FBXResourceAsset> fbxData, BonePoseBuffer& bonePoses, std::weak_ptr<Transform>& transform); 
    void Execute(ComPtr<ID3D11DeviceContext>& context) override;

private:	
    std::weak_ptr<FBXResourceAsset> fbxData{};	
	BonePoseBuffer bonePoses{};
	Transform transform{};

	ComPtr<ID3D11Buffer> bonePoseBuffer{};
	ComPtr<ID3D11Buffer> boneOffsetBuffer{};
	ComPtr<ID3D11Buffer> transformBuffer{};
	ComPtr<ID3D11Buffer> materialBuffer{};
};