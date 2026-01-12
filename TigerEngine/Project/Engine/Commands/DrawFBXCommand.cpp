#include "DrawFBXCommand.h"
#include "../Manager/ShaderManager.h"
#include "Datas/TransformData.h"
#include "Datas/MaterialData.h"

void DrawFBXCommand::CreateCommand(std::weak_ptr<FBXResourceAsset> fbxData, BonePoseBuffer& bonePoses, Transform* transform)
{
    this->fbxData = fbxData;
    this->bonePoses = bonePoses;
    this->transform = *transform;

    boneOffsetBuffer = ShaderManager::Instance().GetBoneOffsetCB();
    bonePoseBuffer = ShaderManager::Instance().GetBonePoseCB();
    transformBuffer = ShaderManager::Instance().GetTransformCB();
    materialBuffer = ShaderManager::Instance().GetMaterialCB();
}

void DrawFBXCommand::Execute(ComPtr<ID3D11DeviceContext> &context)
{
    if(fbxData.expired()) return; // 그릴 메쉬가 없음 -> data 없음
	auto lockedFbxData = fbxData.lock();

    context->UpdateSubresource(bonePoseBuffer.Get(), 0, nullptr, &bonePoses, 0, 0);
	context->UpdateSubresource(boneOffsetBuffer.Get(), 0, nullptr, &lockedFbxData->m_BoneOffsets, 0, 0);
	context->VSSetConstantBuffers(3, 1, bonePoseBuffer.GetAddressOf());
	context->VSSetConstantBuffers(4, 1, boneOffsetBuffer.GetAddressOf());

	TransformData tb = {};
	tb.isRigid = lockedFbxData->skeletalInfo.IsRigid();
	tb.world = XMMatrixTranspose(transform.GetWorldTransform());
	int size = lockedFbxData->meshes.size();
	for (size_t i = 0; i < size; i++)
	{
		MaterialData meshMaterial = lockedFbxData->meshes[i].GetMaterial();
		context->UpdateSubresource(materialBuffer.Get(), 0, nullptr, &meshMaterial, 0, 0);		
		tb.refBoneIndex = lockedFbxData->meshes[i].refBoneIndex;
    
		context->UpdateSubresource(transformBuffer.Get(), 0, nullptr, &tb, 0, 0);
		context->VSSetConstantBuffers(2, 1, transformBuffer.GetAddressOf());
		context->PSSetConstantBuffers(1, 1, materialBuffer.GetAddressOf());
		lockedFbxData->meshes[i].Draw(context);
	}
}
