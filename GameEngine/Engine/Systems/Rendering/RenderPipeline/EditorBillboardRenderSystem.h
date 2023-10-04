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
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	HardwareContext* hc;
	RenderContext* rc;
	EngineContext* ec;
};

