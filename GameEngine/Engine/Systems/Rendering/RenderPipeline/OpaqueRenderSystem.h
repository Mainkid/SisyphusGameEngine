#pragma once
#include "../../ISystem.h"
#include "../../Core/ServiceLocator.h"
#include "../RenderContext.h"
#include <memory>

class EngineCore;
struct EngineContext;
struct RenderContext;
struct HardwareContext;

class OpaqueRenderSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
};

