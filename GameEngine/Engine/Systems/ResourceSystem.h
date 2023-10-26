#pragma once
#include "../Core/ECS/SystemBase.h"
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "../../vendor/Delegates.h"

struct EngineContext;
class ResourceService;

class ResourceSystem:public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	ResourceService* rs;
	
	const std::filesystem::path _gameAssetsPath = ".\\Game\\Assets";
	const std::filesystem::path _engineAssetsPath = ".\\Engine\\Assets\\Resources";
	//void FillResourceLibrary(std::filesystem::path path);
};

