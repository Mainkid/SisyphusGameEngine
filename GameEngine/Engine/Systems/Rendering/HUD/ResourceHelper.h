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
			ec->selectedContent.uuid = "";
			ec->selectedContent.assetType = EAssetType::ASSET_NONE;
		}
		std::filesystem::remove_all(path);

		if (!std::filesystem::is_directory(path))
			std::filesystem::remove_all(path.string() + ".meta");
	}

	static void MoveFile_(std::filesystem::path oldPath, std::filesystem::path newPath, std::unordered_map<std::string, EAssetType>& map)
	{
		
		if (!std::filesystem::is_directory(newPath))
			return;
		
		auto filename = oldPath.filename();
		if (std::filesystem::exists(newPath.string() + "\\" + filename.string()))
			return;
		RenameFile(oldPath, newPath.string() + "\\" + filename.string(),map);
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

	static void RenameFile(std::filesystem::path oldPath, std::filesystem::path newPath, std::unordered_map<std::string, EAssetType>& map)
	{
		auto ec = ServiceLocator::instance()->Get<EngineContext>();
		auto rs = ServiceLocator::instance()->Get<ResourceService>();
		auto uuid = rs->GetUUIDFromPath(oldPath);
		auto val = map[oldPath.string()];

		map.erase(oldPath.string());
		std::filesystem::rename(oldPath, newPath);
		map[newPath.string()] = val;

		
		if (!std::filesystem::is_directory(newPath))
			std::filesystem::rename(oldPath.string() + ".meta", newPath.string() + ".meta");

		rs->LoadResourceLibrary(".\\Game\\Assets", true);
	}

	//static void LoadResourceLibrary(std::filesystem::path path, std::unordered_map<std::string, ResourceInfo>& resourceLibrary, bool reloadNeeded=false)
	//{
	//	using json = nlohmann::json;

	//	if (reloadNeeded)
	//		resourceLibrary.clear();


	//	for (auto& directoryEntry : std::filesystem::directory_iterator(path))
	//	{
	//		if (directoryEntry.is_directory())
	//			LoadResourceLibrary(directoryEntry,resourceLibrary);
	//		else
	//		{
	//			if (directoryEntry.path().filename().extension() != ".meta")
	//			{
	//				boost::uuids::uuid uuid;
	//				EAssetType assetType;
	//				int assetTypeInt;
	//				std::ifstream filei;
	//				std::ofstream fileo;
	//				filei.open(directoryEntry.path().string() + ".meta");
	//				json fileData;
	//				filei >> fileData;
	//				filei.close();

	//				assetType = static_cast<EAssetType>(fileData["AssetType"]);
	//				std::string uuidStr = fileData["uuid"];
	//				auto extension = directoryEntry.path().filename().extension().string();
	//				resourceLibrary[uuidStr] = { assetType,directoryEntry.path() };

	//			}
	//		}
	//		std::cout << directoryEntry.path() << std::endl;
	//	}
	//}

	//static std::string GetUUIDFromPath(std::filesystem::path path)
	//{
	//	using json = nlohmann::json;

	//	if (std::filesystem::is_directory(path))
	//		return "";
	//	std::ifstream file;
	//	std::string uuid;

	//	file.open(path.string() + ".meta");
	//	json fileData;
	//	file >> fileData;
	//	file.close();
	//	uuid = fileData["uuid"];
	//	return uuid;

	//	
	//}

	//static std::string FindFilePathByUUID(const std::string& uuid)
	//{
	//	std::filesystem::path filePath=ServiceLocator::instance()->Get<EngineContext>()->resourceLibrary.at(uuid).path;
	//	if (std::filesystem::exists(filePath))
	//		return filePath.string();
	//	else
	//	{
	//		//TODO: Добавить вывод ошибки, если файл отсутствует с ID
	//	}
	//}

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

				{"albedoTextureUUID",rs->GetUUIDFromPath(".\\Engine\\Assets\\Textures\\black_texture.png")},
				{"specularTextureUUID",rs->GetUUIDFromPath(".\\Engine\\Assets\\Textures\\white_texture.png")},
				{"roughnessTextureUUID",rs->GetUUIDFromPath(".\\Engine\\Assets\\Textures\\white_texture.png")},
				{"metallicTextureUUID",rs->GetUUIDFromPath(".\\Engine\\Assets\\Textures\\white_texture.png")},
				{"emissiveTextureUUID",rs->GetUUIDFromPath(".\\Engine\\Assets\\Textures\\alpha-transparent.png")},
				{"normalmapTextureUUID",rs->GetUUIDFromPath(".\\Engine\\Assets\\Textures\\default_normalmap.jpg")},
				{"opacityTextureUUID",rs->GetUUIDFromPath(".\\Engine\\Assets\\Textures\\alpha-transparent.png")},
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

