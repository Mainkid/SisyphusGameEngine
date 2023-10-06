#pragma once
#include "ISystem.h"
#include <filesystem>
#include <fstream>

struct EngineContext;
class ResourceService;

class ResourceSystem:public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	ResourceService* rs;
	void GenerateMetaFiles(std::filesystem::path currentDirectory);
	void FillResourceLibrary(std::filesystem::path path);
};

