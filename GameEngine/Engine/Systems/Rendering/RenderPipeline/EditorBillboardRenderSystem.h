#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"

#include <memory>


struct EngineContext;
struct RenderContext;
struct HardwareContext;

class EditorBillboardRenderSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	HardwareContext* hc;
	RenderContext* rc;
	EngineContext* ec;
};

