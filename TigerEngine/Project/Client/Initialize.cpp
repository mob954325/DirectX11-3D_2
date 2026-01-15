#include "ClientGlobal.h"
#include "Player/Player.h"

// dll 내부에서 사용할 진짜 팩토리 주소 
ComponentFactory* ENGINE_FACTORY = nullptr;

// dll 로드 직후 엔진이 호출해줄 초기화 함수
extern "C" __declspec(dllexport) void InitializeClient(ComponentFactory* engineFactory)
{
    ENGINE_FACTORY = engineFactory;
}