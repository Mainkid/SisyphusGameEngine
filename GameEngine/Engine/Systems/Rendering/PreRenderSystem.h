#pragma once
#include "../ISystem.h"
#include "SimpleMath.h"

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class PreRenderSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float bgEntitiesColor[4] = { -1,-1,-1,1 };
	RenderContext* rc;
	EngineContext* ec;
	HardwareContext* hc;
};

