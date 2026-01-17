#pragma once
#include <string>
#include <vector>
#include "BoneAnimation.h"
#include <map>

using namespace std;

class Animation
{
public:
	map<string, int> m_mappingBoneAnimations; 
	vector<BoneAnimation> m_boneAnimations;		
	float m_duration;							
	string m_name;								
	float m_tick;								

	void CreateBoneAnimation(aiAnimation* pAiAnimation);
	bool GetBoneAnimationByName(string boneName, BoneAnimation& out);
};

