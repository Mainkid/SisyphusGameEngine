#include "ResourceService.h"


ResourceService::ResourceService()
{
	for (int i = 0; i < static_cast<int>(EAssetType::ASSET_TYPE_AMOUNT); i++)
	{
		resourceGroups[EAssetType(i)] = {};
	}
}

void ResourceService::LoadBaseAssets()
{
	baseResourceDB[EAssetType::ASSET_TEXTURE] = GetUUIDFromPath(baseTexture);
	resourceLibrary[GetUUIDFromPath(baseTexture)].resource = LoadResource(GetUUIDFromPath(baseTexture));

	baseResourceDB[EAssetType::ASSET_MATERIAL] = GetUUIDFromPath(baseMaterial);
	resourceLibrary[GetUUIDFromPath(baseMaterial)].resource = LoadResource(GetUUIDFromPath(baseMaterial));

	baseResourceDB[EAssetType::ASSET_MESH] =  GetUUIDFromPath(baseModel);
	resourceLibrary[GetUUIDFromPath(baseModel)].resource = LoadResource(GetUUIDFromPath(baseModel));
}

std::shared_ptr<ResourceBase> ResourceService::LoadResource(const boost::uuids::uuid& uuid, bool reloadNeeded )
{
	std::shared_ptr<std::string> strPtr;

	using json = nlohmann::json;
	if (resourceLibrary[uuid].resource.lock() != nullptr && !reloadNeeded)
	{
		resourceLibrary[uuid].referenceCounter += 1;
		return resourceLibrary[uuid].resource.lock();
	}
	else
	{
		if (resourceLibrary[uuid].assetType == EAssetType::ASSET_MATERIAL)
		{
			std::shared_ptr<Material> mat;
			if (!reloadNeeded || resourceLibrary[uuid].resource.lock() == nullptr)
				mat = std::make_shared<Material>();
			else
				mat = std::static_pointer_cast<std::shared_ptr<Material>>(resourceLibrary[uuid].resource.lock());
			std::ifstream file;
			file.open(FindFilePathByUUID(uuid));
			json fileData;
			file >> fileData;
			file.close();
			mat->albedoTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["albedoTextureUUID"]);
			if (resourceLibrary[mat->albedoTextureUUID].resource == nullptr)
				LoadResource(mat->albedoTextureUUID);

			mat->albedoSRV = static_cast<Texture*>(resourceLibrary[mat->albedoTextureUUID].resource);

			mat->emissiveTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["emissiveTextureUUID"]);
			if (resourceLibrary[mat->emissiveTextureUUID].resource == nullptr)
				LoadResource(mat->emissiveTextureUUID);

			mat->emissiveSRV = static_cast<Texture*>(resourceLibrary[mat->emissiveTextureUUID].resource);

			mat->metallicTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["metallicTextureUUID"]);
			if (resourceLibrary[mat->metallicTextureUUID].resource == nullptr)
				LoadResource(mat->metallicTextureUUID);

			mat->metallicSRV = static_cast<Texture*>(resourceLibrary[mat->metallicTextureUUID].resource);

			mat->opacityTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["opacityTextureUUID"]);
			if (resourceLibrary[mat->opacityTextureUUID].resource == nullptr)
				LoadResource(mat->opacityTextureUUID);

			mat->opacitySRV = static_cast<Texture*>(resourceLibrary[mat->opacityTextureUUID].resource);

			mat->roughnessTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["roughnessTextureUUID"]);
			if (resourceLibrary[mat->roughnessTextureUUID].resource == nullptr)
				LoadResource(mat->roughnessTextureUUID);

			mat->roughnessSRV = static_cast<Texture*>(resourceLibrary[mat->roughnessTextureUUID].resource);

			mat->specularTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["specularTextureUUID"]);
			if (resourceLibrary[mat->specularTextureUUID].resource == nullptr)
				LoadResource(mat->specularTextureUUID);

			mat->specularSRV = static_cast<Texture*>(resourceLibrary[mat->specularTextureUUID].resource);

			mat->normalmapTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["normalmapTextureUUID"]);
			if (resourceLibrary[mat->normalmapTextureUUID].resource == nullptr)
				LoadResource(mat->normalmapTextureUUID);

			mat->normalmapSRV = static_cast<Texture*>(resourceLibrary[mat->normalmapTextureUUID].resource);

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


			resourceLibrary[uuid].resource = mat;
		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_TEXTURE)
		{
			//TODO: Поддержка sRGB;
			auto texture = new Texture();
			MeshLoader::LoadTexture(FindFilePathByUUID(uuid), texture->textureSamplerState.GetAddressOf(), texture->textureSRV.GetAddressOf());
			resourceLibrary[uuid].resource = texture;
		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_MESH)
		{
			auto model = new Model();
			MeshLoader::LoadModel(FindFilePathByUUID(uuid), model->meshes);
			resourceLibrary[uuid].resource = model;
		}

		resourceLibrary[uuid].referenceCounter = 1;
	}

	return resourceLibrary[uuid].resource;
}

void ResourceService::UnloadResource(const boost::uuids::uuid& uuid) 
{ 
	if (resourceLibrary[uuid].resource != nullptr)
	{
		auto resource = resourceLibrary[uuid].resource;
		//Check if unloading base resource;
		if (resourceLibrary[uuid].isBaseResource)
			return;

		resourceLibrary[uuid].referenceCounter--;
		if (resourceLibrary[uuid].referenceCounter == 0)
		{
			resourceLibrary[uuid].resource = nullptr;
			delete resource;
		}
	}
	else
	{
		//TODO: Ошибка, ресурсов не осталось для выгрузки
	}
}

std::string ResourceService::FindFilePathByUUID(const boost::uuids::uuid& uuid, bool getFileName)
{
	std::filesystem::path filePath = resourceLibrary.at(uuid).path;
	std::filesystem::path fileName = resourceLibrary.at(uuid).filename;
	if (std::filesystem::exists(filePath))
	{
		if (!getFileName)
			return filePath.string();
		else
			return fileName.string();
	}
	else
	{
		//TODO: Добавить вывод ошибки, если файл отсутствует с ID
	}
}

boost::uuids::uuid ResourceService::GetUUIDFromPath(const std::filesystem::path& path)
{
	using json = nlohmann::json;

	if (std::filesystem::is_directory(path))
		return boost::uuids::nil_uuid();
	std::ifstream file;
	std::string uuid;

	file.open(path.string() + ".meta");
	json fileData;
	file >> fileData;
	file.close();
	uuid = fileData["uuid"];
	return boost::lexical_cast<boost::uuids::uuid>(uuid);
}

void ResourceService::LoadResourceLibrary(std::filesystem::path path, bool isReloading, bool isLoadingBaseResources)
{
	using json = nlohmann::json;

	if (isReloading)
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
				auto uuidValue = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["uuid"]);
				auto extension = directoryEntry.path().filename().extension().string();
				resourceGroups[assetType].insert(uuidValue);
				resourceLibrary[uuidValue] = { assetType,directoryEntry.path(),directoryEntry.path().filename().string(),isLoadingBaseResources };
				
			}
		}
	}
}

void ResourceService::ClearResourceGroups()
{
	resourceGroups.clear();
}

void ResourceService::UnloadResourceDB()
{
	/*loadedResourceDB.clear();*/
}

std::vector<boost::uuids::uuid> ResourceService::GetAllResourcesOfType(EAssetType assetType)
{
	std::vector<boost::uuids::uuid> uuids;
	for (auto& uuid : resourceGroups.at(assetType))
	{
		uuids.push_back(uuid);
	}
	return uuids;
}

std::vector<std::string> ResourceService::GetAllResourcesFilePaths(EAssetType assetType, bool findFullPath)
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