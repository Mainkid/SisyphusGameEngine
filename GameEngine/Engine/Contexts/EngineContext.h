#pragma once


#include "../../vendor/entt/entt.hpp"
#include "../Core/IService.h"
#include "../Features/Resources/ResourceInfo.h"
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <set>
#include "../Core/Tools/FlagBitmask.h"
#include "../Serialization/Serializer.hpp"



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


enum SyEEnginePropertiesFlags
{
	SHOW_ALL_COLLIDERS = 1,
	SHOW_COLLIDERS_IN_PLAYMODE = 1 << 1
};
DEFINE_BITWISE_OPERATORS(SyEEnginePropertiesFlags);

struct EngineContext : public IService
{
	entt::registry ecs;
	ser::Serializer serializer;

	entt::entity selectedEntityID=entt::null;
	HudData hudData;
	float totalTime = 0;
	unsigned int frameCount = 0;
	float deltaTime = 0;
	bool isClosed = false;
	std::unordered_map<entt::entity, entt::entity> serEnttToSceneEnttMap;
	unsigned propertyFlags = 0;

	//PlayMode states and var for it
	enum class EPlayModeState
	{
		EditorMode,
		PlayMode,
		PauseMode
	};
	EPlayModeState playModeState = EPlayModeState::EditorMode;
	
};

//TODO: ��������� resourceLibrary � ResourceService;