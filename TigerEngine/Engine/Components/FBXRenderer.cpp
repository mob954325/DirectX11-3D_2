#include "FBXRenderer.h"
#include "Renderer/DrawMeshCommand.h"
#include "Manager/ComponentFactory.h"
#include "Scene/Scene.h"
#include <Components/FBXData.h>

REGISTER_COMPONENT(FBXRenderer);

void FBXRenderer::OnInitialize()
{
    fbxData = owner->GetComponent<FBXData>();
}

void FBXRenderer::OnStart()
{
}

void FBXRenderer::OnUpdate(float delta)
{
    auto modelAsset = fbxData->GetFBXInfo();
    if (!modelAsset->animations.empty() && isAnimPlay)
	{
		//progressAnimationTime += GameTimer::m_Instance->DeltaTime();
		progressAnimationTime = fmod(progressAnimationTime, modelAsset->animations[animationIndex].m_duration);
	}

	// pose 본 갱신
	for (auto& bone : bones)
	{
		// 애니메이션 업데이트
		if (bone.m_boneAnimation.m_boneName != "")
		{
			Vector3 positionVec = Vector3::Zero;
			Vector3 scaleVec = Vector3::Zero;
			Quaternion rotationQuat = Quaternion::Identity;
			bone.m_boneAnimation.Evaluate(progressAnimationTime, positionVec, rotationQuat, scaleVec);

			if (positionVec != Vector3::Zero || rotationQuat != Quaternion::Identity || scaleVec != Vector3::Zero) // 움직이지 않는 본들은 갱신 안함
			{
				Matrix mat = Matrix::CreateScale(scaleVec) * Matrix::CreateFromQuaternion(rotationQuat) * Matrix::CreateTranslation(positionVec);
				bone.m_localTransform = mat.Transpose();
			}
		}

		// 위치 갱신
		if (bone.m_parentIndex != -1)
		{
			bone.m_worldTransform = bones[bone.m_parentIndex].m_worldTransform * bone.m_localTransform;
		}
		else
		{
			bone.m_worldTransform = bone.m_localTransform;
		}

		bonePoses.modelMatricies[bone.m_index] = bone.m_worldTransform;
	}	
}

void FBXRenderer::SetData(std::shared_ptr<FBXData> data)
{
    fbxData = data;
    CreateBoneInfo();

	// TODO 상수 버퍼는 어디서 만들지, 업데이트는 
	// m_deviceContext->UpdateSubresource(m_bonePoseBuffer.Get(), 0, nullptr, &m_BonePoses, 0, 0);
	// m_deviceContext->UpdateSubresource(m_boneOffsetBuffer.Get(), 0, nullptr, &modelAsset->m_BoneOffsets, 0, 0);
	// 
	// m_deviceContext->VSSetConstantBuffers(3, 1, m_bonePoseBuffer.GetAddressOf());
	// m_deviceContext->VSSetConstantBuffers(4, 1, m_boneOffsetBuffer.GetAddressOf());
	
	// 커멘드 구성 어떻게 할지
	auto command = std::make_shared<DrawMeshCommand>();
	SetCommand(); //
}

void FBXRenderer::CreateBoneInfo()
{
    auto modelAsset = fbxData->GetFBXInfo();
    int size = modelAsset->skeletalInfo.m_bones.size();
	for (int i = 0; i < size; i++)
	{
		string boneName = modelAsset->skeletalInfo.m_bones[i].name;
		BoneInfo boneInfo = modelAsset->skeletalInfo.GetBoneInfoByName(boneName);
		int boneIndex = modelAsset->skeletalInfo.GetBoneIndexByName(boneName);

		string parentBoneName = boneInfo.parentBoneName;
		BoneInfo parentBoneInfo;
		int parentBoneIndex = -1;
		if (parentBoneName != "")
		{
			parentBoneInfo = modelAsset->skeletalInfo.GetBoneInfoByName(parentBoneName);
			parentBoneIndex = modelAsset->skeletalInfo.GetBoneIndexByName(parentBoneName);
		}

		Matrix localMat = boneInfo.relativeTransform;
		Matrix worldMat = parentBoneIndex > 0 ? bones[parentBoneIndex].m_worldTransform * localMat : localMat;

		// Bone 정보 생성
		Bone bone;
		bone.CreateBone(boneName, parentBoneIndex, boneIndex, worldMat, localMat);	//...

		BoneAnimation boneAnim;
		bool hasAnim = !modelAsset->animations.empty();
		if (parentBoneIndex != -1 && hasAnim)
		{
			modelAsset->animations[animationIndex].GetBoneAnimationByName(boneName, boneAnim);
			bone.m_boneAnimation = boneAnim;	// 임시 -> 0번째 애니메이션 받기
		}

		bones.push_back(bone);
	}
}
