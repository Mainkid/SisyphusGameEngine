#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include "../../../Contexts/RenderContext.h"
#include <memory>

class EngineCore;
struct EngineContext;
struct RenderContext;
struct HardwareContext;

class OpaqueRenderSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* _ec = nullptr;
	RenderContext* _rc = nullptr;
	HardwareContext* _hc = nullptr;
};

