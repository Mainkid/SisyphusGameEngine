#pragma once
#include <map>
#include <filesystem>
#include "../../Resources/ResourceInfo.h"


static const std::map<EAssetType, std::filesystem::path> cb_iconsPathsDB{
	{EAssetType::ASSET_NONE,"Engine/Assets/HUD/ContentBrowser/meshIcon.png"},
	{EAssetType::ASSET_FOLDER, "Engine/Assets/HUD/ContentBrowser/folderIcon.png"},
	{EAssetType::ASSET_TEXTURE, "Engine/Assets/HUD/ContentBrowser/imageIcon.png"},
	{EAssetType::ASSET_MESH, "Engine/Assets/HUD/ContentBrowser/meshIcon.png"},
	{EAssetType::ASSET_MATERIAL, "Engine/Assets/HUD/ContentBrowser/materialIcon.png"},
	{EAssetType::ASSET_CUBEMAP, "Engine/Assets/HUD/ContentBrowser/cubeMap.png"},
	{EAssetType::ASSET_SCENE, "Engine/Assets/HUD/ContentBrowser/scene.png"},
	{EAssetType::ASSET_PREFAB, "Engine/Assets/HUD/ContentBrowser/prefab.png"},
	{EAssetType::ASSET_SOUND, "Engine/Assets/HUD/ContentBrowser/audio.png"},
	{EAssetType::ASSET_ANIMATION, "Engine/Assets/HUD/ContentBrowser/Animation.png"}
};

//static std::map<std::string, EAsset> cb_extensionsDB{
//
//	{"",EContent::Folder},
//	{".obj",EContent::Mesh},
//	{".fbx",EContent::Mesh},
//	{".png",EContent::Image},
//	{".jpeg",EContent::Image},
//};