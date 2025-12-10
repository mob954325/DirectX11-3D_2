#pragma once
#include <string>
#include <vector>
#include "BoneAnimation.h"
#include <map>

using namespace std;

class Animation
{
public:
	map<string, int> mappingBoneAnimations; 
	vector<BoneAnimation> boneAnimations;		// 키 프레임에 등록된 본 위치와 시간 정보들
	float duration;							// 최종 시간 ( 초 단위 )
	string name;								// 애니메이션 이름
	float tickPerSec;								// 애니메이션 틱

	void CreateBoneAnimation(aiAnimation* pAiAnimation);
	bool GetBoneAnimationByName(string boneName, BoneAnimation& out);
};

