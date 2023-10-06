#pragma once
#include "../Core/IService.h"
#include "../Components/Material.h"
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


	ResourceService()
	{
		for (int i=0;i<static_cast<int>(EAssetType::ASSET_TYPE_AMOUNT);i++)
		{
			resourceGroups[EAssetType(i)] = {};
		}
	}

	ResourceBase* LoadResource(const std::string& uuid, bool reloadNeeded)
	{
		if (loadedResourceDB.find(uuid) != loadedResourceDB.end())
			return loadedResourceDB.at(uuid);
		else
		{
			switch (resourceLibrary[uuid].assetType)
			{
			case EAssetType::ASSET_MATERIAL:
				//TODO: Подгрузка ресурсов
				break;
			}
		}
	}

	std::string FindFilePathByUUID(const std::string& uuid)
	{
		std::filesystem::path filePath = resourceLibrary.at(uuid).path;
		if (std::filesystem::exists(filePath))
			return filePath.string();
		else
		{
			//TODO: Добавить вывод ошибки, если файл отсутствует с ID
		}
	}

	std::string GetUUIDFromPath(std::filesystem::path path)
	{
		using json = nlohmann::json;

		if (std::filesystem::is_directory(path))
			return "";
		std::ifstream file;
		std::string uuid;

		file.open(path.string() + ".meta");
		json fileData;
		file >> fileData;
		file.close();
		uuid = fileData["uuid"];
		return uuid;


	}

	void LoadResourceLibrary(std::filesystem::path path, bool reloadNeeded = false)
	{

		using json = nlohmann::json;

		if (reloadNeeded)
			resourceLibrary.clear();


		for (auto& directoryEntry : std::filesystem::directory_iterator(path))
		{
			if (directoryEntry.is_directory())
				LoadResourceLibrary(directoryEntry);
			else
			{
				if (directoryEntry.path().filename().extension() != ".meta")
				{
					boost::uuids::uuid uuid;
					EAssetType assetType;
					int assetTypeInt;
					std::ifstream filei;
					std::ofstream fileo;
					filei.open(directoryEntry.path().string() + ".meta");
					json fileData;
					filei >> fileData;
					filei.close();

					assetType = static_cast<EAssetType>(fileData["AssetType"]);
					std::string uuidStr = fileData["uuid"];
					auto extension = directoryEntry.path().filename().extension().string();
					resourceGroups[assetType].insert(uuidStr);
					resourceLibrary[uuidStr] = { assetType,directoryEntry.path() };

				}
			}
		}
	}

	void ClearResourceGroups()
	{
		resourceGroups.clear();
	}

	void UnloadResourceDB()
	{
		loadedResourceDB.clear();
	}

	std::vector<const char*> GetAllResourcesOfType(EAssetType assetType)
	{
		std::vector<const char*> uuids;
		for (auto& uuid : resourceGroups.at(assetType))
		{
			uuids.push_back(uuid.c_str());
		}

		return uuids;
	}

};

