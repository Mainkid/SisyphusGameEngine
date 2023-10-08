#pragma once
#include "../Core/IService.h"
#include "../Components/Material.h"
#include "../Components/Model.h"
#include "SimpleMath.h"
#include "MeshLoader.h"

#include "ResourceInfo.h"
#include <string>
#include <set>
#include <unordered_map>
#include <queue>
#include <fstream>
#include "json.hpp"
#include <uuid.hpp>

class ResourceService : public IService
{
public:
	std::unordered_map<std::string, ResourceInfo> resourceLibrary;           // uuid: assetType, path, resourceBase
	std::unordered_map<EAssetType, std::set<std::string>> resourceGroups;    // assetType: set<uuid>
	std::unordered_map<std::string, ResourceBase*> loadedResourceDB;          //uuid : Resources*
	std::unordered_map<EAssetType, ResourceBase*> baseResourceDB;

	ResourceService();
	void LoadBaseAssets();
	ResourceBase* LoadResource(const std::string& uuid, bool reloadNeeded = false);
	std::string FindFilePathByUUID(const std::string& uuid, bool getFileName = false);
	std::string GetUUIDFromPath(const std::filesystem::path& path);
	void LoadResourceLibrary(std::filesystem::path path, bool reloadNeeded = false);
	void ClearResourceGroups();
	void UnloadResourceDB();
	std::vector<const char*> GetAllResourcesOfType(EAssetType assetType);
	std::vector<std::string> GetAllResourcesFilePaths(EAssetType assetType, bool findFullPath = false);

	

private:
	const std::string baseTexture = ".\\Engine\\Assets\\Resources\\Textures\\default_albedo.png";
	const std::string baseMaterial = ".\\Engine\\Assets\\Resources\\Materials\\baseMaterial.mat";
	const std::string baseModel = ".\\Engine\\Assets\\Resources\\Cube.fbx";

	
};

