#pragma once
#include <string>
#include <vector>
#include <map>

#include <assimp/scene.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;
using namespace std;

/// <summary>
/// ���� ���� ���� ������
/// </summary>
class BoneInfo
{
public:
	string name{};				// �� �̸�
	string parentBoneName{};	// �ش� ���� �θ� �̸� ������ ""
	Matrix relativeTransform{};	// �ش� �� ��Ʈ���� ��
};

/// <summary>
/// boneInfo ���� �����͸� ������ �ִ� Ŭ����
/// </summary>
class SkeletonInfo
{
private:
	map<string, int> m_boneMappingTable;
	map<string, Matrix> m_bonesOffset;
	bool isRigid = true;

	void CreateBoneInfoFromNode(const aiNode* pAiNode);

public:
	vector<BoneInfo> m_bones;

	BoneInfo GetBoneInfoByIndex(int index);
	BoneInfo GetBoneInfoByName(const string& boneName);
	int GetBoneIndexByName(const string& boneName);
	Matrix GetBoneOffsetByName(const string& boneName);
	bool CreateFromAiScene(const aiScene* pAiScene);

	bool IsRigid() { return isRigid; };
};