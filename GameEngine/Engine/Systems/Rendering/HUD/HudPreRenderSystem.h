#pragma once
#include "../../Core/ECS/SystemBase.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudPreRenderSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void CleanupRenderTarget();
	void CreateRenderTarget();
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
	int windowWidth;
	int windowHeight;
};

