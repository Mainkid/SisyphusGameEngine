#pragma once
#include "ISystem.h"
#include "../Core/ServiceLocator.h"
#include <fstream>

class SyErrorLoggingSystem : public ISystem
{
	SyResult Init();
	SyResult Run();
	SyResult Destroy();

	std::string logPath;
	std::ofstream fout;
};

