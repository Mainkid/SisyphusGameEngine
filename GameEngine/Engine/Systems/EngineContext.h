#pragma once
#include "../Core/Scene/Scene.h"
#include "../Core/IService.h"

struct EngineContext : public IService
{
	std::unique_ptr<Scene> scene;
	entt::entity selectedEntityID=entt::null;
	bool isClosed = false;
	//PlayMode states and var for it
	enum class EPlayModeState
	{
		EditorMode,
		PlayMode,
		PauseMode
	};

	struct TimerData
	{
		float totalTime = 0;
		unsigned int frameCount = 0;
		float deltaTime = 0;
	} timerData;
	EPlayModeState playModeState = EPlayModeState::EditorMode;
	
};

