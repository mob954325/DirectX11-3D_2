#pragma once
#include <pch.h>
#include <Entity/RenderComponent.h>
#include <Components/FBXData.h>
#include <Datas/FBXResourceData.h>
#include <Datas/Bone.h>
#include <Commands/DrawFBXCommand.h>

class FBXRenderer : public RenderComponent
{
public:
	void OnInitialize() override;
	void OnStart() override;
	void OnUpdate(float delta) override;

	
private:
    void CreateBoneInfo();
	void CreateCommand();	// 매 프레임마다 어떻게 그려질지 정한다.

    std::shared_ptr<FBXData> fbxData{}; // 참조할 FBX 데이터
	std::shared_ptr<DrawFBXCommand> command;

	// 모델 인스턴스 데이터
	std::string directory{};				// 로드한 파일이 위차한 폴더명
	std::vector<Bone> bones{};			// 로드된 모델의 본 모음 -> 계층 구조에 있는 오브젝트들

	// 해당 모델의 상수 버퍼 내용
	BonePoseBuffer bonePoses{};

    // animation info
    int animationIndex = 0;             // 현재 실행 중인 애니메이션 인덱스
    int progressAnimationTime = 0.0f;   // 현재 애니메이션 시간
    bool isAnimPlay = true;             
};