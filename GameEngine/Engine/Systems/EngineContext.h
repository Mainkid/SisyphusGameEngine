#pragma once
#include "../Core/Scene/Scene.h"
#include "../Core/IService.h"
#include "../Systems/ResourceInfo.h"
#include "ResourceInfo.h"
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <set>




struct SelectedContent
{
	boost::uuids::uuid uuid;
	EAssetType assetType;
};

struct HudData
{
	std::set<entt::entity> selectedEntityIDs;
	std::set<entt::entity> copyBufferEntityIDs;
	SelectedContent selectedContent = { boost::uuids::nil_uuid(),EAssetType::ASSET_NONE };
};

struct EngineContext : public IService
{
	std::unique_ptr<Scene> scene;
	HudData hudData;
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