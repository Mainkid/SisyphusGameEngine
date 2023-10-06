#pragma once
#include <map>
#include <filesystem>
#include "../../ResourceInfo.h"





static const std::map<EAssetType, std::filesystem::path> cb_iconsPathsDB{
	{EAssetType::ASSET_NONE,"Engine/Assets/HUD/ContentBrowser/meshIcon.png"},
	{EAssetType::ASSET_FOLDER, "Engine/Assets/HUD/ContentBrowser/folderIcon.png"},
	{EAssetType::ASSET_TEXTURE, "Engine/Assets/HUD/ContentBrowser/imageIcon.png"},
	{EAssetType::ASSET_MESH, "Engine/Assets/HUD/ContentBrowser/meshIcon.png"},
	{EAssetType::ASSET_MATERIAL, "Engine/Assets/HUD/ContentBrowser/materialIcon.png"}
};

//static std::map<std::string, EAsset> cb_extensionsDB{
//
//	{"",EContent::Folder},
//	{".obj",EContent::Mesh},
//	{".fbx",EContent::Mesh},
//	{".png",EContent::Image},
//	{".jpeg",EContent::Image},
//};