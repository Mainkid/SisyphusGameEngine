#pragma once
#include "ISystem.h"
struct EngineContext;

class UpdateTimingSystem : public ISystem
{
	EngineContext* ec;

public:
	SyResult Init();
	SyResult Run();
	SyResult Destroy();
};

