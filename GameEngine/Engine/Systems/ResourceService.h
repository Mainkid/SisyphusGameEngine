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

	ResourceService()
	{
		for (int i = 0; i < static_cast<int>(EAssetType::ASSET_TYPE_AMOUNT); i++)
		{
			resourceGroups[EAssetType(i)] = {};
		}
	}

	void LoadBaseAssets()
	{
		baseResourceDB[EAssetType::ASSET_TEXTURE]= LoadResource(GetUUIDFromPath(baseTexture));
		baseResourceDB[EAssetType::ASSET_MATERIAL] = LoadResource(GetUUIDFromPath(baseMaterial));
		baseResourceDB[EAssetType::ASSET_MESH] = LoadResource(GetUUIDFromPath(baseModel));
	}

	ResourceBase* LoadResource(const std::string& uuid, bool reloadNeeded = false)
	{
		using json = nlohmann::json;

		if (loadedResourceDB.find(uuid) != loadedResourceDB.end() && !reloadNeeded)
			return loadedResourceDB.at(uuid);
		else
		{
			if (resourceLibrary[uuid].assetType == EAssetType::ASSET_MATERIAL)
			{
				Material* mat;
				if (!reloadNeeded || loadedResourceDB.find(uuid) == loadedResourceDB.end())
					mat = new Material();
				else
					mat =static_cast<Material*>(loadedResourceDB.at(uuid));
				std::ifstream file;
				file.open(FindFilePathByUUID(uuid));
				json fileData;
				file >> fileData;
				file.close();
				mat->albedoTextureUUID = fileData["albedoTextureUUID"];
				if (loadedResourceDB.find(mat->albedoTextureUUID) == loadedResourceDB.end())
					LoadResource(mat->albedoTextureUUID);

				mat->albedoSRV = static_cast<Texture*>( loadedResourceDB[mat->albedoTextureUUID]);

				mat->emissiveTextureUUID = fileData["emissiveTextureUUID"];
				if (loadedResourceDB.find(mat->emissiveTextureUUID) == loadedResourceDB.end())
					LoadResource(mat->emissiveTextureUUID);

				mat->emissiveSRV = static_cast<Texture*>(loadedResourceDB[mat->emissiveTextureUUID]);

				mat->metallicTextureUUID = fileData["metallicTextureUUID"];
				if (loadedResourceDB.find(mat->metallicTextureUUID) == loadedResourceDB.end())
					LoadResource(mat->metallicTextureUUID);

				mat->metallicSRV = static_cast<Texture*>(loadedResourceDB[mat->metallicTextureUUID]);

				mat->opacityTextureUUID = fileData["opacityTextureUUID"];
				if (loadedResourceDB.find(mat->opacityTextureUUID) == loadedResourceDB.end())
					LoadResource(mat->opacityTextureUUID);

				mat->opacitySRV = static_cast<Texture*>(loadedResourceDB[mat->opacityTextureUUID]);

				mat->roughnessTextureUUID = fileData["roughnessTextureUUID"];
				if (loadedResourceDB.find(mat->roughnessTextureUUID) == loadedResourceDB.end())
					LoadResource(mat->roughnessTextureUUID);

				mat->roughnessSRV = static_cast<Texture*>(loadedResourceDB[mat->roughnessTextureUUID]);

				mat->specularTextureUUID = fileData["specularTextureUUID"];
				if (loadedResourceDB.find(mat->specularTextureUUID) == loadedResourceDB.end())
					LoadResource(mat->specularTextureUUID);

				mat->specularSRV = static_cast<Texture*>(loadedResourceDB[mat->specularTextureUUID]);

				mat->normalmapTextureUUID = fileData["normalmapTextureUUID"];
				if (loadedResourceDB.find(mat->normalmapTextureUUID) == loadedResourceDB.end())
					LoadResource(mat->normalmapTextureUUID);

				mat->normalmapSRV = static_cast<Texture*>(loadedResourceDB[mat->normalmapTextureUUID]);

				mat->albedoValue = Vector4(fileData["albedoVec"][0], fileData["albedoVec"][1], fileData["albedoVec"][2], fileData["albedoVec"][3]);
				mat->emissiveValue = Vector4(fileData["emissiveVec"][0], fileData["emissiveVec"][1], fileData["emissiveVec"][2], fileData["emissiveVec"][3]);
				mat->metallicValue = Vector4(fileData["metallicVec"][0], fileData["metallicVec"][1], fileData["metallicVec"][2], fileData["metallicVec"][3]);
				mat->roughnessValue = Vector4(fileData["roughnessVec"][0], fileData["roughnessVec"][1], fileData["roughnessVec"][2], fileData["roughnessVec"][3]);
				mat->specularValue = Vector4(fileData["specularVec"][0], fileData["specularVec"][1], fileData["specularVec"][2], fileData["specularVec"][3]);

				mat->resources[0] = mat->albedoSRV->textureSRV.Get();
				mat->resources[1] = mat->specularSRV->textureSRV.Get();
				mat->resources[2] = mat->roughnessSRV->textureSRV.Get();
				mat->resources[3] = mat->metallicSRV->textureSRV.Get();
				mat->resources[4] = mat->emissiveSRV->textureSRV.Get();
				mat->resources[5] = mat->normalmapSRV->textureSRV.Get();
				mat->resources[6] = mat->opacitySRV->textureSRV.Get();


				loadedResourceDB[uuid] = mat;
			}
			else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_TEXTURE)
			{
				//TODO: Поддержка sRGB;
				auto texture = new Texture();
				MeshLoader::LoadTexture(FindFilePathByUUID(uuid), texture->textureSamplerState.GetAddressOf(), texture->textureSRV.GetAddressOf());
				loadedResourceDB[uuid] = texture;
			}
			else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_MESH)
			{
				auto model = new Model();
				MeshLoader::LoadModel(FindFilePathByUUID(uuid), model->meshes);
				loadedResourceDB[uuid] = model;
			}
		}

		return loadedResourceDB.at(uuid);

	}

	std::string FindFilePathByUUID(const std::string& uuid, bool getFileName=false)
	{
		std::filesystem::path filePath = resourceLibrary.at(uuid).path;
		std::filesystem::path fileName = resourceLibrary.at(uuid).filename;
		if (std::filesystem::exists(filePath))
		{
			if (!getFileName)
				return filePath.string();
			else
				return resourceLibrary.at(uuid).filename;
		}
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
		{
			resourceLibrary.clear();
			resourceGroups.clear();
		}


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
					resourceLibrary[uuidStr] = { assetType,directoryEntry.path(),directoryEntry.path().filename().string()};
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

	std::vector<std::string> GetAllResourcesFilePaths(EAssetType assetType, bool findFullPath=false)
	{
		std::vector<std::string> filePaths;
		for (auto& uuid : resourceGroups.at(assetType))
		{
			std::filesystem::path filePath;
			std::string result;
			filePaths.push_back(FindFilePathByUUID(uuid, !findFullPath));

		}
		return filePaths;

	}

	

private:
	const std::string baseTexture = ".\\Engine\\Assets\\Resources\\Textures\\default_albedo.png";
	const std::string baseMaterial = ".\\Engine\\Assets\\Resources\\Materials\\baseMaterial.mat";
	const std::string baseModel = ".\\Engine\\Assets\\Resources\\Cube.fbx";
};

