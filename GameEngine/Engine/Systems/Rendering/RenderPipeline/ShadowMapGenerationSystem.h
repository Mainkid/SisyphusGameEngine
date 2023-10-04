#pragma once
#include "../../Core/ECS/SystemBase.h"

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class ShadowMapGenerationSystem : public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
};

