#pragma once
#include "../ISystem.h"
#include "SimpleMath.h"

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class PreRenderSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float bgEntitiesColor[4] = { -1,-1,-1,1 };
	RenderContext* rc;
	EngineContext* ec;
	HardwareContext* hc;
};

