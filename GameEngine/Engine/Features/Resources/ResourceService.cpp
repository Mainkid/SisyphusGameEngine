#include "ResourceService.h"

#include "../../Contexts/HardwareContext.h"
#include "../Particles/Particle.h"
#include "../../Components/SkyboxResource.h"
#include "../../Events/SySceneLoadEvent.h"
#include "../../Core/Tools/ImageLoader.h"
#include "../../Scene/Prefab.h"
#include "../../Scene/Scene.h"

ResourceService::ResourceService()
{
	for (int i = 0; i < static_cast<int>(EAssetType::ASSET_TYPE_AMOUNT); i++)
	{
		resourceGroups[EAssetType(i)] = {};
	}
}

void ResourceService::LoadBaseAssets()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Loading base assets...");

	baseResourceDB[EAssetType::ASSET_TEXTURE].uuid = GetUUIDFromPath(baseTexture);
	baseResourceDB[EAssetType::ASSET_TEXTURE].resource = LoadResource(GetUUIDFromPath(baseTexture));
	resourceLibrary[GetUUIDFromPath(baseTexture)].resource = baseResourceDB[EAssetType::ASSET_TEXTURE].resource;

	baseResourceDB[EAssetType::ASSET_MATERIAL].uuid = GetUUIDFromPath(baseMaterial);
	baseResourceDB[EAssetType::ASSET_MATERIAL].resource = LoadResource(GetUUIDFromPath(baseMaterial));
	resourceLibrary[GetUUIDFromPath(baseMaterial)].resource = baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

	baseResourceDB[EAssetType::ASSET_MESH].uuid =  GetUUIDFromPath(baseModel);
	baseResourceDB[EAssetType::ASSET_MESH].resource = LoadResource(GetUUIDFromPath(baseModel));
	resourceLibrary[GetUUIDFromPath(baseModel)].resource = baseResourceDB[EAssetType::ASSET_MESH].resource;

	baseResourceDB[EAssetType::ASSET_CUBEMAP].uuid = GetUUIDFromPath(baseCubemap);
	baseResourceDB[EAssetType::ASSET_CUBEMAP].resource = LoadResource(GetUUIDFromPath(baseCubemap));
	resourceLibrary[GetUUIDFromPath(baseCubemap)].resource = baseResourceDB[EAssetType::ASSET_CUBEMAP].resource;

	baseResourceDB[EAssetType::ASSET_PARTICLESYS].uuid = GetUUIDFromPath(baseParticle);
	baseResourceDB[EAssetType::ASSET_PARTICLESYS].resource = LoadResource(GetUUIDFromPath(baseParticle));
	resourceLibrary[GetUUIDFromPath(baseParticle)].resource = baseResourceDB[EAssetType::ASSET_PARTICLESYS].resource;

	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Base assets loaded successfuly!");
}

std::shared_ptr<ResourceBase> ResourceService::LoadResource(const boost::uuids::uuid& uuid, bool reloadNeeded )
{

	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Loading resource with %s uuid and %s refs",
		boost::lexical_cast<std::string>(uuid).c_str(), std::to_string(resourceLibrary[uuid].resource.use_count()).c_str());

	std::shared_ptr<std::string> strPtr;

	using json = nlohmann::json;
	if (resourceLibrary[uuid].resource.lock() != nullptr && !reloadNeeded)
	{
		return resourceLibrary[uuid].resource.lock();
	}
	else
	{
		if (resourceLibrary[uuid].assetType == EAssetType::ASSET_MATERIAL)
		{
			std::shared_ptr<Material> mat = nullptr;
			if (!reloadNeeded || resourceLibrary[uuid].resource.lock() == nullptr)
				mat = std::make_shared<Material>();
			else
				mat = std::static_pointer_cast<Material>(resourceLibrary[uuid].resource.lock());

			std::string filePath = FindFilePathByUUID(uuid);
			if (filePath == "")
			{

				SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Can't load resource with %s uuid! Loading base asset...",
					boost::lexical_cast<std::string>(uuid).c_str());
				filePath = FindFilePathByUUID(baseResourceDB[EAssetType::ASSET_MATERIAL].uuid);
			}

			std::ifstream file;
			file.open(filePath);
			json fileData;
			file >> fileData;
			file.close();

			mat->albedoTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["albedoTextureUUID"]);
			mat->albedoSRV = std::static_pointer_cast<Texture>(LoadResource(mat->albedoTextureUUID));
			if (!mat->albedoSRV)
				return baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

			mat->emissiveTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["emissiveTextureUUID"]);
			mat->emissiveSRV = std::static_pointer_cast<Texture>(LoadResource(mat->emissiveTextureUUID));
			if (!mat->emissiveSRV)
				return baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

			mat->metallicTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["metallicTextureUUID"]);
			mat->metallicSRV = std::static_pointer_cast<Texture>(LoadResource(mat->metallicTextureUUID));
			if (!mat->metallicSRV)
				return baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

			mat->opacityTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["opacityTextureUUID"]);
			mat->opacitySRV = std::static_pointer_cast<Texture>(LoadResource(mat->opacityTextureUUID));
			if (!mat->opacitySRV)
				return baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

			mat->roughnessTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["roughnessTextureUUID"]);
			mat->roughnessSRV = std::static_pointer_cast<Texture>(LoadResource(mat->roughnessTextureUUID));
			if (!mat->roughnessSRV)
				return baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

			mat->specularTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["specularTextureUUID"]);
			mat->specularSRV = std::static_pointer_cast<Texture>(LoadResource(mat->specularTextureUUID));
			if (!mat->specularSRV)
				return baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

			mat->normalmapTextureUUID = boost::lexical_cast<boost::uuids::uuid>((std::string)fileData["normalmapTextureUUID"]);
			mat->normalmapSRV = std::static_pointer_cast<Texture>(LoadResource(mat->normalmapTextureUUID));
			if (!mat->normalmapSRV)
				return baseResourceDB[EAssetType::ASSET_MATERIAL].resource;

			mat->albedoValue = Vector4(fileData["albedoVec"][0], fileData["albedoVec"][1], fileData["albedoVec"][2], fileData["albedoVec"][3]);
			mat->emissiveValue = Vector4(fileData["emissiveVec"][0], fileData["emissiveVec"][1], fileData["emissiveVec"][2], fileData["emissiveVec"][3]);
			mat->metallicValue = Vector4(fileData["metallicVec"][0], fileData["metallicVec"][1], fileData["metallicVec"][2], fileData["metallicVec"][3]);
			mat->roughnessValue = Vector4(fileData["roughnessVec"][0], fileData["roughnessVec"][1], fileData["roughnessVec"][2], fileData["roughnessVec"][3]);
			mat->specularValue = Vector4(fileData["specularVec"][0], fileData["specularVec"][1], fileData["specularVec"][2], fileData["specularVec"][3]);

			mat->resources[0] = mat->albedoSRV->textureSRV.Get();
			mat->resources[1] = mat->roughnessSRV->textureSRV.Get();
			mat->resources[2] = mat->metallicSRV->textureSRV.Get();
			mat->resources[3] = mat->emissiveSRV->textureSRV.Get();
			mat->resources[4] = mat->normalmapSRV->textureSRV.Get();
			mat->resources[5] = mat->opacitySRV->textureSRV.Get();


			resourceLibrary[uuid].resource = std::static_pointer_cast<ResourceBase>(mat);
			return mat;
		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_TEXTURE)
		{
			auto texture = std::make_shared<Texture>();
			std::string filePath = FindFilePathByUUID(uuid);
			if (filePath == "")
			{
				SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Can't load resource with %s uuid! Loading base asset...",
					boost::lexical_cast<std::string>(uuid).c_str());
				filePath = FindFilePathByUUID(baseResourceDB[EAssetType::ASSET_TEXTURE].uuid);
			}
			SyResult hr = MeshLoader::LoadTexture(filePath, texture->textureSamplerState.GetAddressOf(), texture->textureSRV.GetAddressOf());

			if (hr.code != SY_RESCODE_OK)
			{
				SY_LOG_REND(SY_LOGLEVEL_ERROR, hr.message.ToString());
			}
			resourceLibrary[uuid].resource = std::static_pointer_cast<ResourceBase>(texture);
			return texture;
		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_MESH)
		{
			auto model = std::make_shared<Model>();
			std::string filePath = FindFilePathByUUID(uuid);
			if (filePath == "")
			{
				SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Can't load resource with %s uuid! Loading base asset...",
					boost::lexical_cast<std::string>(uuid).c_str());
				filePath = FindFilePathByUUID(baseResourceDB[EAssetType::ASSET_MESH].uuid);
			}
			MeshLoader::LoadModel(filePath, model->meshes);

			resourceLibrary[uuid].resource = std::static_pointer_cast<ResourceBase>( model);
			return model;
		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_CUBEMAP)
		{
			auto _hc = ServiceLocator::instance()->Get<HardwareContext>();
			auto skyboxResource = std::make_shared<SkyboxResource>();

			std::string filePath = FindFilePathByUUID(uuid);
			int width = 0;
			int height = 0;
			auto cubeMapArray = ImageLoader::LoadSkyboxFromFile(filePath.c_str(), &width, &height);
			skyboxResource->Resolution = width;
			D3D11_TEXTURE2D_DESC textureDesc_ = {};
			textureDesc_.Width = width;
			textureDesc_.Height = width;
			textureDesc_.MipLevels = 1;
			textureDesc_.ArraySize = 6;

			textureDesc_.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			textureDesc_.SampleDesc.Count = 1;
			textureDesc_.SampleDesc.Quality = 0;
			textureDesc_.Usage = D3D11_USAGE_DEFAULT;
			textureDesc_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc_.CPUAccessFlags = 0;
			textureDesc_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


			D3D11_SUBRESOURCE_DATA data[6];
			for (int i = 0; i < 6; i++)
			{

				data[i].pSysMem = cubeMapArray[i];
				data[i].SysMemPitch = sizeof(float) * width * 4;
				data[i].SysMemSlicePitch = 0;
			}
			HRESULT result = _hc->device->CreateTexture2D(&textureDesc_, data, skyboxResource->cubemapTexture.GetAddressOf());
			
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc_.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
			result = _hc->device->CreateShaderResourceView(skyboxResource->cubemapTexture.Get(), &srvDesc, skyboxResource->textureSRV.GetAddressOf());
			resourceLibrary[uuid].resource = std::static_pointer_cast<ResourceBase>(skyboxResource);
			return skyboxResource;

		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_PARTICLESYS)
		{
			std::shared_ptr<SharedParticlesData> spd = std::make_shared<SharedParticlesData>();
			ser::Serializer& ser = ServiceLocator::instance()->Get<EngineContext>()->serializer;
			std::string filePath = FindFilePathByUUID(uuid);
			if (filePath == "")
			{
				SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Can't load resource with %s uuid! Loading base asset...",
					boost::lexical_cast<std::string>(uuid).c_str());
				filePath = FindFilePathByUUID(baseResourceDB[EAssetType::ASSET_PARTICLESYS].uuid);
			}

			std::ifstream file;
			nlohmann::json data;
			file.open(filePath);
			file >> data;
		
			ser.Deserialize(data, *spd.get());
			if (spd->TextureUuid == boost::uuids::nil_uuid())
				spd->TextureUuid = baseResourceDB[EAssetType::ASSET_TEXTURE].uuid;
			resourceLibrary[uuid].resource = std::static_pointer_cast<ResourceBase>(spd);
			return spd;

		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_PREFAB)
		{
			
			auto prefab = std::make_shared<Prefab>();
			std::string filePath = FindFilePathByUUID(uuid);

			std::ifstream file;
			nlohmann::json fileData;
			file.open(filePath);
			file >> fileData;

			prefab->Data = fileData.dump();

			resourceLibrary[uuid].resource = std::static_pointer_cast<ResourceBase>(prefab);
			return prefab;

		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_SCENE)
		{
			auto scene = std::make_shared<Scene>();
			std::string filePath = FindFilePathByUUID(uuid);

			std::ifstream file;
			nlohmann::json fileData;
			file.open(filePath);
			file >> fileData;

			scene->Data = fileData.dump();

			resourceLibrary[uuid].resource = std::static_pointer_cast<ResourceBase>(scene);
			return scene;
		}
		else if (resourceLibrary[uuid].assetType == EAssetType::ASSET_NONE)
		{
			SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Can't load resource with %s uuid! Loading base asset...",
				boost::lexical_cast<std::string>(uuid).c_str());
			return resourceLibrary[uuid].resource.lock();
		}
	}

	return resourceLibrary[uuid].resource.lock();
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
		SY_LOG_CORE(SY_LOGLEVEL_WARNING, "Can't find file path with current UUID. Returning NONE");
		return "";
	}
}

boost::uuids::uuid ResourceService::GetUUIDFromPath(const std::filesystem::path& path)
{
	using json = nlohmann::json;

	if (std::filesystem::is_directory(path))
	{
		return boost::uuids::nil_uuid();
	}
	std::ifstream file;
	std::string uuid;

	if (!std::filesystem::exists(path.string() + ".meta"))
	{
		SY_LOG_CORE(SY_LOGLEVEL_WARNING, "File with current path doesn't have generated metafile. Returning uuid_null");
		return boost::uuids::nil_uuid();
	}

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
	std::vector<boost::uuids::uuid> uuidsToDeleteFromResourceGroups;
	for (auto& uuid : resourceGroups.at(assetType))
	{
		std::filesystem::path filePath;
		std::string result = FindFilePathByUUID(uuid, !findFullPath);
		if (result != "")
			filePaths.push_back(result);
		else
			uuidsToDeleteFromResourceGroups.push_back(uuid);
	}

	for (auto& uuid : uuidsToDeleteFromResourceGroups)
	{
		resourceGroups.at(assetType).erase(uuid);
	}

	return filePaths;
}

void ResourceService::LoadSceneFromFile(std::filesystem::path filePath, entt::registry* ecs)
{
	ser::Serializer& ser = ServiceLocator::instance()->Get<EngineContext>()->serializer;
	std::ifstream file;
	nlohmann::json json;
	file.open(filePath);
	file >> json;
	file.close();

	auto view = ecs->view<GameObjectComp>();
	for (auto ent : view)
		ecs->destroy(ent);
	ser.Deserialize(json, *ecs);

	auto& hudData = ServiceLocator::instance()->Get<EngineContext>()->hudData;
	hudData.copyBufferEntityIDs.clear();
	hudData.selectedContent.assetType = EAssetType::ASSET_NONE;
	hudData.selectedContent.uuid = boost::uuids::nil_uuid();
	hudData.selectedEntityIDs.clear();

}

void ResourceService::SaveSceneToFile(std::filesystem::path filePath, entt::registry* ecs)
{
	ser::Serializer& ser = ServiceLocator::instance()->Get<EngineContext>()->serializer;
	auto json = ser.Serialize<GameObjectComp>(*ecs);
	std::ofstream file;
	file.open(filePath, std::ios::trunc);
	file << std::setw(1) << json;
	file.close();
}

void ResourceService::GenerateMetaFiles(std::filesystem::path currentDirectory)
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Generating metafiles in %s folder",currentDirectory.string().c_str());

	using json = nlohmann::json;
	for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory))
	{
		if (directoryEntry.is_directory())
			GenerateMetaFiles(directoryEntry);
		else
		{
			if (directoryEntry.path().filename().extension() != ".meta" && !std::ifstream(directoryEntry.path().string() + ".meta"))
			{
				boost::uuids::uuid uuid = boost::uuids::random_generator()();
				std::string extension = directoryEntry.path().filename().extension().string();
				std::ofstream file;
				json fileData = {
					{"uuid",boost::uuids::to_string(uuid)}
				};

				file.open(directoryEntry.path().string() + ".meta");

				if (extensionToAssetTypeMap.count(extension) > 0)
					fileData["AssetType"] = static_cast<int>(extensionToAssetTypeMap.at(extension));
				else
					fileData["AssetType"] = static_cast<int>(EAssetType::ASSET_NONE);

				file << fileData;
				file.close();
			}
		}
		
	}
}