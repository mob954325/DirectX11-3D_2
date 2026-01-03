#include "DrawFBXCommand.h"
#include <Manager/ShaderManager.h>
#include <Datas/TransformData.h>
#include <Datas/MaterialData.h>

void DrawFBXCommand::CreateCommand(std::shared_ptr<FBXResourceAsset> data, BonePoseBuffer& bonePoses, std::shared_ptr<Transform>& transform)
{
    this->fbxData = data;
    this->bonePoses = bonePoses;
    this->transform = *transform;

    boneOffsetBuffer = ShaderManager::Instance().GetBoneOffsetCB();
    bonePoseBuffer = ShaderManager::Instance().GetBonePoseCB();
    transformBuffer = ShaderManager::Instance().GetTransformCB();
    materialBuffer = ShaderManager::Instance().GetMaterialCB();
}

void DrawFBXCommand::Execute(ComPtr<ID3D11DeviceContext> &context)
{
    if(fbxData->meshes.empty()) return; // 그릴 메쉬가 없음 -> FBXData가 없음

    context->UpdateSubresource(bonePoseBuffer.Get(), 0, nullptr, &bonePoses, 0, 0);
	context->UpdateSubresource(boneOffsetBuffer.Get(), 0, nullptr, &fbxData->m_BoneOffsets, 0, 0);
	context->VSSetConstantBuffers(3, 1, bonePoseBuffer.GetAddressOf());
	context->VSSetConstantBuffers(4, 1, boneOffsetBuffer.GetAddressOf());

	TransformData tb = {};
	tb.isRigid = fbxData->skeletalInfo.IsRigid();
	tb.world = XMMatrixTranspose(transform.GetWorldTransform());
	int size = fbxData->meshes.size();
	for (size_t i = 0; i < size; i++)
	{
		MaterialData meshMaterial = fbxData->meshes[i].GetMaterial();
		context->UpdateSubresource(materialBuffer.Get(), 0, nullptr, &meshMaterial, 0, 0);		
		tb.refBoneIndex = fbxData->meshes[i].refBoneIndex;
    
		context->UpdateSubresource(transformBuffer.Get(), 0, nullptr, &tb, 0, 0);
		context->VSSetConstantBuffers(2, 1, transformBuffer.GetAddressOf());
		context->PSSetConstantBuffers(1, 1, materialBuffer.GetAddressOf());
		fbxData->meshes[i].Draw(context);
	}
}
