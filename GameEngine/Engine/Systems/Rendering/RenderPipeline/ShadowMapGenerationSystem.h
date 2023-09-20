#pragma once
#include "../../ISystem.h"

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class ShadowMapGenerationSystem : public ISystem
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

