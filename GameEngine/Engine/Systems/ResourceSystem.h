#pragma once
#include "ISystem.h"
#include <filesystem>
#include <fstream>

struct EngineContext;
class ResourceService;

class ResourceSystem:public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	ResourceService* rs;
	void GenerateMetaFiles(std::filesystem::path currentDirectory);
	//void FillResourceLibrary(std::filesystem::path path);
};

