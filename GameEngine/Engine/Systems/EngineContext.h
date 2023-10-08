#pragma once
#include "../Core/Scene/Scene.h"
#include "../Core/IService.h"
#include "../Systems/ResourceInfo.h"
#include "ResourceInfo.h"
#include <string>


struct SelectedContent
{
	std::string uuid;
	EAssetType assetType;
};

struct EngineContext : public IService
{
	std::unique_ptr<Scene> scene;
	entt::entity selectedEntityID=entt::null;
	SelectedContent selectedContent = {"",EAssetType::ASSET_NONE};
	uint32_t selectedContentID = 0;
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

//TODO: Перенести resourceLibrary в ResourceService;