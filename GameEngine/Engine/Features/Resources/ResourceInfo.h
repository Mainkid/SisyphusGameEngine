#pragma once
#include <filesystem>
#include <unordered_map>
#include "../Resources/ResourceBase.h"
#include <memory>

enum class EAssetType
{
	ASSET_NONE,
	ASSET_FOLDER,
	ASSET_TEXTURE,
	ASSET_MESH,
	ASSET_SOUND,
	ASSET_MATERIAL,
	ASSET_CUBEMAP,
	ASSET_PARTICLESYS,
	ASSET_SCENE,
	ASSET_PREFAB,

	ASSET_TYPE_AMOUNT
};

enum class EWrapMode
{
	WRAP_REPEAT,
	WRAP_CLAMP,
	WRAP_MIRROR
};

enum class EFilterMode
{
	FILTER_BILINEAR,
	FILTER_ANISOTROPIC,
	FILTER_POINT
};

enum class ETextureType
{
	TEXTURE_2D,
	TEXTURE_CUBEMAP
};

struct ResourceInfo
{
	EAssetType assetType;
	std::filesystem::path path;
	std::string filename;
	bool isBaseResource;
	std::weak_ptr<ResourceBase> resource;
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
	{".jpg",EAssetType::ASSET_TEXTURE},
	{".ps",EAssetType::ASSET_PARTICLESYS},
	{".scene", EAssetType::ASSET_SCENE},
	{".prefab", EAssetType::ASSET_PREFAB}
};
