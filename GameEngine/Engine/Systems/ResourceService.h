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
#include <unordered_set>
#include <queue>
#include <fstream>
#include "json.hpp"
#include <uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include "../../vendor/Delegates.h"
#include "../Tools/ErrorLogger.h"
#include "../../vendor/entt/entt.hpp"
#include "../Serialization/Serializer.hpp"
#include "EngineContext.h"
#include "../Components/GameObjectComp.h"

struct BaseResourceInfo
{
	using uuid_hash = boost::hash<boost::uuids::uuid>;
	boost::uuids::uuid uuid;
	std::shared_ptr<ResourceBase> resource;

};


class ResourceService : public IService
{
	using uuid_hash = boost::hash<boost::uuids::uuid>;
public:
	std::unordered_map<boost::uuids::uuid, ResourceInfo, uuid_hash> resourceLibrary;           // uuid: assetType, path, resourceBase
	std::unordered_map<EAssetType, std::set<boost::uuids::uuid>> resourceGroups;               // assetType: set<uuid>
	std::unordered_map<EAssetType, BaseResourceInfo> baseResourceDB;                         //assetType : Resource*  
	

	ResourceService();
	void LoadBaseAssets();
	std::shared_ptr<ResourceBase> LoadResource(const boost::uuids::uuid& uuid, bool reloadNeeded = false);
	/*void UnloadResource(const boost::uuids::uuid& uuid);*/
	std::string FindFilePathByUUID(const boost::uuids::uuid& uuid, bool getFileName = false);
	boost::uuids::uuid GetUUIDFromPath(const std::filesystem::path& path);
	void LoadResourceLibrary(std::filesystem::path path, bool reloadNeeded = false, bool isLoadingBaseResources=false);
	void GenerateMetaFiles(std::filesystem::path currentDirectory);
	std::vector<boost::uuids::uuid> GetAllResourcesOfType(EAssetType assetType);
	std::vector<std::string> GetAllResourcesFilePaths(EAssetType assetType, bool findFullPath = false);
	SyDelegates::MulticastDelegate<bool> updateContentBrowser;
	
	void LoadSceneFromFile(std::filesystem::path file, entt::registry* ecs);
	void SaveSceneToFile(std::filesystem::path file, entt::registry* ecs);

private:
	const std::string baseTexture = ".\\Engine\\Assets\\Resources\\Textures\\default_albedo.png";
	const std::string baseCubemap = ".\\Engine\\Assets\\Resources\\Cubemaps\\cubemap.hdr";
	const std::string baseMaterial = ".\\Engine\\Assets\\Resources\\Materials\\baseMaterial.mat";
	const std::string baseModel = ".\\Engine\\Assets\\Resources\\Cube.fbx";
	
	
};

