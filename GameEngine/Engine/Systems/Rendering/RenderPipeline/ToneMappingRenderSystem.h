#pragma once
#include "../../ISystem.h"
#include "../../Core/ServiceLocator.h"


struct EngineContext;
struct RenderContext;
struct HardwareContext;

class ToneMappingRenderSystem : public ISystem
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

