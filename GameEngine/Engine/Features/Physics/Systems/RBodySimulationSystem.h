#pragma once
#include "../../../Core/ECS/SystemBase.h"
class SyRBodySimulationSystem : public SystemBase
{
public:
	SyResult Run() override;
	SyResult Init() override;
	SyResult Destroy() override;
};

