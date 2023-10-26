#pragma once


#include "../../vendor/entt/entt.hpp"
#include "../Core/IService.h"
#include "../Mono/SyMono.h"
#include "../Serialization/Serializer.hpp"

struct EngineContext : public IService
{
	entt::registry ecs;
	ser::Serializer serializer;

	entt::entity selectedEntityID=entt::null;
	float totalTime = 0;
	unsigned int frameCount = 0;
	float deltaTime = 0;
	bool isClosed = false;
	//PlayMode states and var for it
	enum class EPlayModeState
	{
		EditorMode,
		PlayMode,
		PauseMode
	};
	EPlayModeState playModeState = EPlayModeState::EditorMode;
	
};

