#pragma once
#include "../../ISystem.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class HudPreRenderSystem : public ISystem
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

