#pragma once

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <map>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"
#include "TextureLoader.h"
#include "SkeletonInfo.h"
#include "Animation.h"
#include "Bone.h"
#include "FBXResourceManager.h"

struct BonePoseBuffer
{
	Matrix modelMatricies[256];
};

/// <summary>
/// 모델에서 사용할 트랜스폼 상수 버퍼 구조체
/// </summary>
struct TransformBuffer
{
	Matrix world;

	UINT isRigid;		// 1 : rigid, 0 : skinned
	UINT refBoneIndex;	// 리지드일 때 참조하는 본 인덱스
	FLOAT pad1;
	FLOAT pad2;
};

class SkeletalModel
{
public:
	SkeletalModel();
	~SkeletalModel();

	Matrix world{};
	Vector3 position{ 0.0f, 0.0f, 10.0f };
	Vector3 rotation{};
	Vector3 scale{ 1.0f, 1.0f, 1.0f };
	bool isRemoved = false;

	bool Load(HWND hwnd, ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pDeviceContext, std::string filename);
	void Draw(ComPtr<ID3D11DeviceContext>& pDeviceContext, ComPtr<ID3D11Buffer>& pMatBuffer);
	void Update();

	void Close();

	// 애니메이션 관련 내용 - 디버그를 위해 public으로 옮김
	int animationIndex = 0;				// 실행 중인 애니메이션 인덱스
	float progressAnimationTime = 0.0f;		// 현재 애니메이션 시간 

	bool isAnimPlay = true;

	// 리소스 데이터
	shared_ptr<FBXResourceAsset> modelAsset{};

	void GetBuffer(ComPtr<ID3D11Buffer>& pTransform, ComPtr<ID3D11Buffer>& pBonePose, ComPtr<ID3D11Buffer>& pBoneOffset);

private:
	ComPtr<ID3D11Device> device = nullptr;
	ComPtr<ID3D11DeviceContext> deviceContext = nullptr;
	HWND hwnd{};

	// 인스턴스 데이터
	std::string directory{};				// 로드한 파일이 위차한 폴더명
	std::vector<Bone> bones{};				// 로드된 모델의 본 모음 -> 계층 구조에 있는 오브젝트들

	// 해당 모델의 상수 버퍼 내용
	BonePoseBuffer bonePoses{};

	// 버퍼들 -> App에서 참조
	ComPtr<ID3D11Buffer> transformBuffer{};
	ComPtr<ID3D11Buffer> bonePoseBuffer{};
	ComPtr<ID3D11Buffer> boneOffsetBuffer{};

	// 기능 함수
	void CreateBoneInfos();
};

