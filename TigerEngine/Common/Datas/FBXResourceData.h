#pragma once

#include "Datas/SkeletonInfo.h"
#include "Datas/Animation.h"
#include "Datas/Mesh.h"

/// @brief Bone offset 정보 버퍼
/// skeletalInfo 본 위치 정보
struct BoneOffsetBuffer
{
	Matrix boneOffset[256];
};

/// @brief 애니메이션 이동이 적용된 Bone 위치 정보 버퍼
struct BonePoseBuffer
{
	Matrix modelMatricies[256];
};

struct FBXResourceAsset
{
	SkeletonInfo skeletalInfo;
	std::vector<Animation> animations;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;

	std::string directory = "";
	BoneOffsetBuffer m_BoneOffsets{};
};