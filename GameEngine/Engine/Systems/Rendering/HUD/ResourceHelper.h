#pragma once
#include <filesystem>
#include "../../ResourceInfo.h"
#include "../../../Core/ServiceLocator.h"
#include "../../EngineContext.h"
#include "../../ResourceService.h"
#include "json.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <regex>
#include <string>

class ResourceHelper
{
public:
	static std::filesystem::path ConstructFile(std::filesystem::path path)
	{
		int fileCtr = 0;
		if (!path.has_extension())
		{
			while (std::filesystem::exists(path.string() + std::to_string(fileCtr)))
				fileCtr++;
			std::filesystem::create_directory(path.string() + std::to_string(fileCtr));
			return (path.string() + std::to_string(fileCtr));
		}

		std::ofstream file;
		
		std::filesystem::path pathCopy = path;
		std::string pathWithoutFile = path.remove_filename().string();
		path = pathCopy;
		std::string filenameWithoutExtension = path.filename().replace_extension().string();
		path = pathCopy;
		std::string extension = path.filename().extension().string();

		while (std::ifstream(pathWithoutFile+filenameWithoutExtension + std::to_string(fileCtr) + extension))
			++fileCtr;
		file.open(pathWithoutFile + filenameWithoutExtension + std::to_string(fileCtr) + extension, std::ios::out);
		file.close();
		return pathWithoutFile + filenameWithoutExtension + std::to_string(fileCtr) + extension;
	}
	
	static void RemoveFile(std::filesystem::path path)
	{
		auto ec=ServiceLocator::instance()->Get<EngineContext>();
		auto rs = ServiceLocator::instance()->Get<ResourceService>();
		if (ec->selectedContent.uuid == rs->GetUUIDFromPath(path))
		{
			ec->selectedContent.uuid = boost::uuids::nil_uuid();
			ec->selectedContent.assetType = EAssetType::ASSET_NONE;
		}
		std::filesystem::remove_all(path);

		if (!std::filesystem::is_directory(path))
			std::filesystem::remove_all(path.string() + ".meta");
	}

	static void MoveFile_(std::filesystem::path oldPath, std::filesystem::path newPath)
	{
		
		if (!std::filesystem::is_directory(newPath))
			return;

		if (std::filesystem::is_directory(oldPath) && newPath.string().find(oldPath.string()) != std::string::npos)
			return;
		
		auto filename = oldPath.filename();
		if (std::filesystem::exists(newPath.string() + "\\" + filename.string()))
			return;
		RenameFile(oldPath, newPath.string() + "\\" + filename.string());
	}

	static void CreateMetaFile_(std::filesystem::path metaPath, EAssetType assetType)
	{
		using json = nlohmann::json;

		std::ofstream file;
		file.open(metaPath.string() + ".meta");
		boost::uuids::uuid uuid = boost::uuids::random_generator()();

		json fileData = {
			{"uuid", boost::uuids::to_string(uuid)},
			{"AssetType", static_cast<int>(assetType)}
		};

		file << fileData;
		file.close();
	}

	static void RenameFile(std::filesystem::path oldPath, std::filesystem::path newPath)
	{
		auto ec = ServiceLocator::instance()->Get<EngineContext>();
		auto rs = ServiceLocator::instance()->Get<ResourceService>();
		auto uuid = rs->GetUUIDFromPath(oldPath);
		std::filesystem::rename(oldPath, newPath);
		
		if (!std::filesystem::is_directory(newPath))
			std::filesystem::rename(oldPath.string() + ".meta", newPath.string() + ".meta");

		rs->LoadResourceLibrary(".\\Game\\Assets", true);
		rs->LoadResourceLibrary(".\\Engine\\Assets\\Resources", false);
	}


	static void FillFileWithBaseData(const std::filesystem::path& path, EAssetType assetType)
	{
		using json = nlohmann::json;
		std::ofstream file;
		file.open(path);
		auto rs = ServiceLocator::instance()->Get<ResourceService>();
		switch (assetType)
		{

		case EAssetType::ASSET_MATERIAL:

			json fileData = {

				{"albedoTextureUUID",boost::lexical_cast<std::string>(rs->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Textures\\black_texture.png"))},
				{"specularTextureUUID",boost::lexical_cast<std::string>(rs->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Textures\\white_texture.png"))},
				{"roughnessTextureUUID",boost::lexical_cast<std::string>(rs->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Textures\\white_texture.png"))},
				{"metallicTextureUUID",boost::lexical_cast<std::string>(rs->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Textures\\white_texture.png"))},
				{"emissiveTextureUUID",boost::lexical_cast<std::string>(rs->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Textures\\alpha-transparent.png"))},
				{"normalmapTextureUUID",boost::lexical_cast<std::string>(rs->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Textures\\normal_map.jpg"))},
				{"opacityTextureUUID",boost::lexical_cast<std::string>(rs->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Textures\\alpha-transparent.png"))},
				{"albedoVec",std::vector<float>{0,0,0,-1}},
				{"specularVec",std::vector<float>{0,0,0,-1}},
				{"roughnessVec",std::vector<float>{-1,0,0,-1}},
				{"metallicVec",std::vector<float>{0,0,0,-1}},
				{"emissiveVec",std::vector<float>{0,0,0,-1}}
			};

			file << fileData;
			break;
		}

		file.close();
	}

	//static std::vector<const char*> GetAllResourcesOfType(EAssetType assetType, std::unordered_map<std::string, ResourceInfo>& resourceLibrary)
	//{
	//	std::vector<const char*> uuids;

	//	for (auto& asset : resourceLibrary)
	//	{
	//		if (asset.second.assetType == assetType)
	//			uuids.push_back(asset.first.c_str());
	//	}

	//	return uuids;
	//}
};

//TODO: GetAllResourcesOfType - переделать! сделать отдельную map вида <AssetType,set<uuids>>

