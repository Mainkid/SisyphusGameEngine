#pragma once
#include "ISystem.h"
#include <filesystem>
#include <fstream>

struct EngineContext;

class ResourceSystem:public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	void GenerateMetaFiles(std::filesystem::path currentDirectory);
};

