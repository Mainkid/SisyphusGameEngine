#pragma once
#include "../../ISystem.h"
#include "../../Core/ServiceLocator.h"

#include <memory>

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class LightRenderSystem: public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;

	void ShadowMap();
};

