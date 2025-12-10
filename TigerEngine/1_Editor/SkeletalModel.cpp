#include "SkeletalModel.h"
#include "../Common/Helper.h"
#include "../Common/System/TimeSystem.h"

SkeletalModel::SkeletalModel()
{
}

SkeletalModel::~SkeletalModel()
{
}

bool SkeletalModel::Load(HWND hwnd, ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pDeviceContext, std::string filename)
{
	this->directory = filename.substr(0, filename.find_last_of("/\\"));
	this->device = pDevice;
	this->deviceContext = pDeviceContext;
	this->hwnd = hwnd;

	// 리소스 매니저에서 FBX 정보 가져오기
	modelAsset = FBXResourceManager::Instance().LoadFBXByPath(pDevice, pDeviceContext, filename);

	// 인스턴스 데이터 생성하기 ( 본 데이터 )
	CreateBoneInfos();

	return true;
}

void SkeletalModel::Draw(ComPtr<ID3D11DeviceContext>& pDeviceContext, ComPtr<ID3D11Buffer>& pMatBuffer)
{
	deviceContext->UpdateSubresource(bonePoseBuffer.Get(), 0, nullptr, &bonePoses, 0, 0);
	deviceContext->UpdateSubresource(boneOffsetBuffer.Get(), 0, nullptr, &modelAsset->boneOffsets, 0, 0);

	deviceContext->VSSetConstantBuffers(3, 1, bonePoseBuffer.GetAddressOf());
	deviceContext->VSSetConstantBuffers(4, 1, boneOffsetBuffer.GetAddressOf());

	TransformBuffer tb = {};

	tb.isRigid = modelAsset->skeletalInfo.IsRigid();
	world = world.CreateScale(scale) *
			world.CreateFromYawPitchRoll(rotation) *
			world.CreateTranslation(position);
	tb.world = XMMatrixTranspose(world);

	int size = modelAsset->meshes.size();
	for (size_t i = 0; i < size; i++)
	{
		Material meshMaterial = modelAsset->meshes[i].GetMaterial();
		deviceContext->UpdateSubresource(pMatBuffer.Get(), 0, nullptr, &meshMaterial, 0, 0);		

		tb.refBoneIndex = modelAsset->meshes[i].refBoneIndex;
		
		deviceContext->UpdateSubresource(transformBuffer.Get(), 0, nullptr, &tb, 0, 0);
		deviceContext->VSSetConstantBuffers(2, 1, transformBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(1, 1, pMatBuffer.GetAddressOf());

		modelAsset->meshes[i].Draw(pDeviceContext);
	}
}

void SkeletalModel::Update()
{
	if (!modelAsset->animations.empty() && isAnimPlay)
	{
		progressAnimationTime += GameTimer::m_Instance->DeltaTime();
		progressAnimationTime = fmod(progressAnimationTime, modelAsset->animations[animationIndex].duration);
	}

	// pose 본 갱신
	for (auto& bone : bones)
	{
		// 애니메이션 업데이트
		if (bone.boneAnimation.boneName != "")
		{
			Vector3 positionVec = Vector3::Zero;
			Vector3 scaleVec = Vector3::Zero;
			Quaternion rotationQuat = Quaternion::Identity;
			bone.boneAnimation.Evaluate(progressAnimationTime, positionVec, rotationQuat, scaleVec);

			if (positionVec != Vector3::Zero || rotationQuat != Quaternion::Identity || scaleVec != Vector3::Zero) // 움직이지 않는 본들은 갱신 안함
			{
				Matrix mat = Matrix::CreateScale(scaleVec) * Matrix::CreateFromQuaternion(rotationQuat) * Matrix::CreateTranslation(positionVec);
				bone.localTransform = mat.Transpose();
			}
		}

		// 위치 갱신
		if (bone.parentIndex != -1)
		{
			bone.worldTransform = bones[bone.parentIndex].worldTransform * bone.localTransform;
		}
		else
		{
			bone.worldTransform = bone.localTransform;
		}

		bonePoses.modelMatricies[bone.index] = bone.worldTransform;
	}	
}

void SkeletalModel::Close()
{
}

void SkeletalModel::GetBuffer(ComPtr<ID3D11Buffer>& pTransform, ComPtr<ID3D11Buffer>& pBonePose, ComPtr<ID3D11Buffer>& pBoneOffset)
{
	transformBuffer = pTransform;
	bonePoseBuffer = pBonePose;
	boneOffsetBuffer = pBoneOffset;
}

void SkeletalModel::CreateBoneInfos()
{
	int size = modelAsset->skeletalInfo.bones.size();
	for (int i = 0; i < size; i++)
	{
		string boneName = modelAsset->skeletalInfo.bones[i].name;
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
		Matrix worldMat = parentBoneIndex > 0 ? bones[parentBoneIndex].worldTransform * localMat : localMat;

		// Bone 정보 생성
		Bone bone;
		bone.CreateBone(boneName, parentBoneIndex, boneIndex, worldMat, localMat);	//...

		BoneAnimation boneAnim;
		bool hasAnim = !modelAsset->animations.empty();
		if (parentBoneIndex != -1 && hasAnim)
		{
			modelAsset->animations[animationIndex].GetBoneAnimationByName(boneName, boneAnim);
			bone.boneAnimation = boneAnim;	// 임시 -> 0번째 애니메이션 받기
		}

		bones.push_back(bone); // -> 할당 겁나됨
	}
}