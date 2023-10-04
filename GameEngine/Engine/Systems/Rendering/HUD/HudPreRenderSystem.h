#pragma once
#include "../../Core/ECS/SystemBase.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudPreRenderSystem : public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	void CleanupRenderTarget();
	void CreateRenderTarget();
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
	int windowWidth;
	int windowHeight;
};

