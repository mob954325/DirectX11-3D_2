#include "Animation.h"
#include <stdexcept>

void Animation::CreateBoneAnimation(aiAnimation* pAiAnimation)
{
	name = pAiAnimation->mName.C_Str();
	tickPerSec = pAiAnimation->mTicksPerSecond;
	duration = pAiAnimation->mDuration / tickPerSec;

	// 본에 대한 키 애니메이션 저장
	for (int i = 0; i < pAiAnimation->mNumChannels; i++)
	{
		aiNodeAnim* pAiNodeAnim = pAiAnimation->mChannels[i];

		BoneAnimation boneAnim;
		boneAnim.boneName = pAiNodeAnim->mNodeName.C_Str();
		boneAnim.CreateKeys(pAiNodeAnim, tickPerSec);
		boneAnimations.push_back(boneAnim);
		mappingBoneAnimations.insert({ boneAnim.boneName, i });
	}

}

bool Animation::GetBoneAnimationByName(string boneName, BoneAnimation& out)
{
	auto anim = mappingBoneAnimations.find(boneName);
	if (anim == mappingBoneAnimations.end())
	{
		return false;
	}

	int index = anim->second;
	out = boneAnimations[index];

	return true;
}
