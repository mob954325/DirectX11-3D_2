#include "FBXRenderer.h"
#include "System/ComponentFactory.h"
#include "Scene/Scene.h"
#include <Components/FBXData.h>
#include <Manager/ShaderManager.h>

REGISTER_COMPONENT(FBXRenderer);

void FBXRenderer::OnInitialize()
{
    fbxData = owner->GetComponent<FBXData>().lock();
	if(!fbxData.expired()) CreateBoneInfo(); // 임시
}

void FBXRenderer::OnStart()
{
}

void FBXRenderer::OnUpdate(float delta)
{
	if(fbxData.expired()) return;

    auto modelAsset = fbxData.lock()->GetFBXInfo();
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

	CreateCommand();
}

void FBXRenderer::CreateBoneInfo()
{
    auto modelAsset = fbxData.lock()->GetFBXInfo();
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

void FBXRenderer::CreateCommand()
{	
	auto command = std::make_shared<DrawFBXCommand>();
	command->CreateCommand(fbxData.lock()->GetFBXInfo(), bonePoses, owner->GetTransform());
	SetCommand(command); //
}
