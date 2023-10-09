#pragma once
#include "../../ISystem.h"

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class ShadowMapGenerationSystem : public ISystem
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

