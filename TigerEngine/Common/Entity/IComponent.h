#pragma once

class IComponent
{
	/// <summary>
	/// IComponent가 처음 실행될 때 실행됩니다.
	/// </summary>
	virtual void OnInitialize() {};

	/// <summary>
	/// OnUpdate()를 실행하기 전 ***한 번*** 실행됩니다.
	/// </summary>
	virtual void OnStart() {};

	/// <summary>
	/// GameEngine에 매 프레임마다 OnUpdate()내 에서 호출됩니다.
	/// </summary>
	virtual void OnUpdate() {};
};
