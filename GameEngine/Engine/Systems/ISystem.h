#pragma once
#include "../Tools/ErrorLogger.h"

class ISystem
{
public:
	virtual SyResult Init() = 0;
	virtual SyResult Run() = 0;
	virtual SyResult Destroy() = 0;
};

