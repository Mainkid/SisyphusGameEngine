#include "ResourceSystem.h"
#include "EngineContext.h"
#include "../Core/ServiceLocator.h"


void ResourceSystem::Init()
{
	ec=ServiceLocator::instance()->Get<EngineContext>();
	
	GenerateMetaFiles("./Game/Assets");

}

void ResourceSystem::Run()
{

}

void ResourceSystem::Destroy()
{
}

void ResourceSystem::GenerateMetaFiles(std::filesystem::path currentDirectory)
{
	for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory))
	{
		if (directoryEntry.is_directory())
			GenerateMetaFiles(directoryEntry);
		else
		{
			if (directoryEntry.path().filename().extension() != ".meta" && !std::ifstream(directoryEntry.path().string() + ".meta"))
			{
				GUID guid;
				CoCreateGuid(&guid);
				std::ofstream file;
				file.open(directoryEntry.path().string() + ".meta");
				file << directoryEntry.path().string();
				file.close();
			}
		}
		std::cout << directoryEntry.path() << std::endl;
	}
}
