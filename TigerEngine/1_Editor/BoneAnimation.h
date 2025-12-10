#pragma once
#include <vector>
#include <algorithm>
#include <assimp\scene.h>
#include "AnimationKey.h"


using namespace std;

class BoneAnimation
{
public:
	string boneName;				// 사용하는 본 이름
	vector<AnimationKey> animKeys;	// 채널(mChaneels)에 저장되어 있는 키 값들
	// string interpolationType		

	void CreateKeys(aiNodeAnim* pAiNodeAnim, float perTick)
	{
		// 일단 키 값들 개수와 각 키에 대한 시간이 모두 동일하다고 가정하고 작성
		int keyNum = pAiNodeAnim->mNumPositionKeys;
		for (int i = 0; i < keyNum; i++)
		{
			AnimationKey key;
			key.time = pAiNodeAnim->mPositionKeys[i].mTime / perTick;
			key.position =
			{
				pAiNodeAnim->mPositionKeys[i].mValue.x,
				pAiNodeAnim->mPositionKeys[i].mValue.y,
				pAiNodeAnim->mPositionKeys[i].mValue.z
			};

			key.quat =
			{
				pAiNodeAnim->mRotationKeys[i].mValue.x,
				pAiNodeAnim->mRotationKeys[i].mValue.y,
				pAiNodeAnim->mRotationKeys[i].mValue.z,
				pAiNodeAnim->mRotationKeys[i].mValue.w
			};

			key.scale =
			{
				pAiNodeAnim->mScalingKeys[i].mValue.x,
				pAiNodeAnim->mScalingKeys[i].mValue.y,
				pAiNodeAnim->mScalingKeys[i].mValue.z
			};

			animKeys.push_back(key);
		}
	}

	template <typename T>
	T Clamp(T value, T min, T max)
	{
		return (value < min) ? min : (value > max) ? max : value;
	}

	void Evaluate(float time, Vector3& outPosition, Quaternion& outRotation, Vector3& outScale)
	{
		if (animKeys.size() < 2) // check has anim
			return;

		// 키프레임 쌍 탐색
		size_t index = 1;
		while (index + 1 < animKeys.size() && time >= animKeys[index + 1].time) // 보간 종료 지점 찾기 ( index + 1 )
		{
			index++;
		}

		// 인덱스 범위 확인
		if (index + 1 >= animKeys.size())
		{
			index = animKeys.size() - 2;
		}

		const auto& keyA = animKeys[index];			// 보간 시작 지점
		const auto& keyB = animKeys[index + 1];		// 보간 종료 지점

		float duration = keyB.time - keyA.time;	// 유효한 지점인지 검사
		if (duration <= 0.0f) return;

		float t = (time - keyA.time) / duration;
		t = Clamp(t, 0.0f, 1.0f);

		// calcuate out datas
		outPosition = Vector3::Lerp(keyA.position, keyB.position, t);
		outRotation = Quaternion::Slerp(keyA.quat, keyB.quat, t);
		outScale = Vector3::Lerp(keyA.scale, keyB.scale, t);
	};
};