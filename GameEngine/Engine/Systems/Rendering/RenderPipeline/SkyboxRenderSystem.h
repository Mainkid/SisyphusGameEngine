#pragma once
#include "../../Core/ECS/SystemBase.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class SkyboxRenderSystem
	: public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* _ec;
	RenderContext* _rc;
	HardwareContext* _hc;


};

