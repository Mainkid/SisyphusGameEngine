#pragma once
#include "../Core/ECS/SystemBase.h"
#include <fstream>

class SyErrorLoggingSystem : public SystemBase
{
	SyResult Init();
	SyResult Run();
	SyResult Destroy();

	//Returns maxZeros-NumDigits(number) for constant time. ONLY for number < 1000 and maxZeros <= 3
	std::string GetZeros(const unsigned maxZeros_, unsigned number_);

	const std::string logDir = "logs";
	std::string logPath;
	std::ofstream fout;
	const unsigned logDirCapacity = 10;

};

