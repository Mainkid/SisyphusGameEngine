#pragma once
#include "../Core/ECS/SystemBase.h"
#include "../Core/ServiceLocator.h"
#include <fstream>

class SyErrorLoggingSystem : public SystemBase
{
	SyResult Init();
	SyResult Run();
	SyResult Destroy();

	std::string logPath;
	std::ofstream fout;
};

