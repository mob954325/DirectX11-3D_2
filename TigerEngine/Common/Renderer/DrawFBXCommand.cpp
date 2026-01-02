#include "DrawFBXCommand.h"

void DrawFBXCommand::CreateCommand(FBXResourceAsset &data)
{
    m_deviceContext->UpdateSubresource(m_bonePoseBuffer.Get(), 0, nullptr, &m_BonePoses, 0, 0);
	m_deviceContext->UpdateSubresource(m_boneOffsetBuffer.Get(), 0, nullptr, &modelAsset->m_BoneOffsets, 0, 0);

	m_deviceContext->VSSetConstantBuffers(3, 1, m_bonePoseBuffer.GetAddressOf());
	m_deviceContext->VSSetConstantBuffers(4, 1, m_boneOffsetBuffer.GetAddressOf());

	TransformBuffer tb = {};

	tb.isRigid = modelAsset->skeletalInfo.IsRigid();
	m_world = m_world.CreateScale(m_Scale) *
			  m_world.CreateFromYawPitchRoll(m_Rotation) *
			  m_world.CreateTranslation(m_Position);
	tb.world = XMMatrixTranspose(m_world);

	int size = modelAsset->meshes.size();
	for (size_t i = 0; i < size; i++)
	{
		Material meshMaterial = modelAsset->meshes[i].GetMaterial();
		m_deviceContext->UpdateSubresource(pMatBuffer.Get(), 0, nullptr, &meshMaterial, 0, 0);		

		tb.refBoneIndex = modelAsset->meshes[i].refBoneIndex;
		
		m_deviceContext->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &tb, 0, 0);
		m_deviceContext->VSSetConstantBuffers(2, 1, m_transformBuffer.GetAddressOf());
		m_deviceContext->PSSetConstantBuffers(1, 1, pMatBuffer.GetAddressOf());

		modelAsset->meshes[i].Draw(pDeviceContext);
	}
}