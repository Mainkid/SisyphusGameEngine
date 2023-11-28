#pragma once
#include "../../Core/Ecs/SystemBase.h"
#include "../../Core/ServiceLocator.h"


struct EngineContext;
struct RenderContext;
struct HardwareContext;

class ToneMappingRenderSystem : public SystemBase
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

