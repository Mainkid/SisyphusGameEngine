#pragma once
#include "../../ISystem.h"
#include "../../Core/ServiceLocator.h"


struct EngineContext;
struct RenderContext;
struct HardwareContext;

class ToneMappingRenderSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
};

