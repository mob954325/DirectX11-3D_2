#pragma once
#include "pch.h"
#include "Renderer/IRenderCommand.h"
#include "Datas/FBXResourceData.h"
#include "Entity/Transform.h"

class DrawFBXCommand : public IRenderCommand
{
public:
    void CreateCommand(std::weak_ptr<FBXResourceAsset> fbxData, BonePoseBuffer& bonePoses, Transform* transform);
    void Execute(ComPtr<ID3D11DeviceContext>& context) override;

	float GetRoughness() { return roughnessFactor; }
	void SetRoughness(float value) { roughnessFactor = value; }

	float GetMetalic() { return metalicFactor;  }
	void SetMetalic(float value) { metalicFactor = value; }

	Color GetColor() { return colorFactor; }
	void SetColor(Color color) { colorFactor = color; }

private:	
    std::weak_ptr<FBXResourceAsset> fbxData{};	
	BonePoseBuffer bonePoses{};
	Transform transform{};

	ComPtr<ID3D11Buffer> bonePoseBuffer{};
	ComPtr<ID3D11Buffer> boneOffsetBuffer{};
	ComPtr<ID3D11Buffer> transformBuffer{};
	ComPtr<ID3D11Buffer> materialBuffer{};

	float roughnessFactor = 0.0f;
	float metalicFactor = 0.0f;
	Color colorFactor{};
};