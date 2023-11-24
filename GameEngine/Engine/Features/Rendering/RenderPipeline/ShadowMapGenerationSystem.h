#pragma once
#include "../../Core/ECS/SystemBase.h"

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class ShadowMapGenerationSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* _ec = nullptr;
	RenderContext* _rc = nullptr;
	HardwareContext* _hc = nullptr;
};

