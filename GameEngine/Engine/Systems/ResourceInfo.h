#pragma once
#include <filesystem>
#include <unordered_map>
#include "../Components/ResourceBase.h"

enum class EAssetType
{
	ASSET_NONE,
	ASSET_FOLDER,
	ASSET_TEXTURE,
	ASSET_MESH,
	ASSET_SOUND,
	ASSET_MATERIAL,

	ASSET_TYPE_AMOUNT
};

struct ResourceInfo
{
	EAssetType assetType;
	std::filesystem::path path;
	std::string filename;
};

const std::unordered_map<std::string, EAssetType> extensionToAssetTypeMap =
{
	{".obj",EAssetType::ASSET_MESH},
	{".fbx",EAssetType::ASSET_MESH},
	{".mat",EAssetType::ASSET_MATERIAL},
	{".wav",EAssetType::ASSET_SOUND},
	{".png",EAssetType::ASSET_TEXTURE},
	{".bmp",EAssetType::ASSET_TEXTURE},
	{".hdr",EAssetType::ASSET_TEXTURE},
	{".jpg",EAssetType::ASSET_TEXTURE}
};

//class ResourceLibrary
//{
//	std::unordered_map<
//};