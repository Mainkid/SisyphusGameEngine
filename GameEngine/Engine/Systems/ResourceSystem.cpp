#include "ResourceSystem.h"
#include "ResourceInfo.h"
#include "EngineContext.h"
#include "Rendering/HUD/ResourceHelper.h"
#include "ResourceService.h"
#include "../Core/ServiceLocator.h"
#include "json.hpp"
#include <uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>


void ResourceSystem::Init()
{
	ec=ServiceLocator::instance()->Get<EngineContext>();
	rs = ServiceLocator::instance()->Get<ResourceService>();
	GenerateMetaFiles("./Game/Assets");
	GenerateMetaFiles("./Engine/Assets/Resources");
	rs->LoadResourceLibrary("./Game/Assets");
	rs->LoadResourceLibrary("./Engine/Assets/Resources",false,true);
	rs->LoadBaseAssets();
}

void ResourceSystem::Run()
{

}

void ResourceSystem::Destroy()
{
}

void ResourceSystem::GenerateMetaFiles(std::filesystem::path currentDirectory)
{
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
					fileData["AssetType"]=static_cast<int>(extensionToAssetTypeMap.at(extension));
				else
					fileData["AssetType"]= static_cast<int>(EAssetType::ASSET_NONE);

				file << fileData;
				file.close();
			}
		}
		std::cout << directoryEntry.path() << std::endl;
	}
}



//TODO: Привести в порядок ассеты из движка!