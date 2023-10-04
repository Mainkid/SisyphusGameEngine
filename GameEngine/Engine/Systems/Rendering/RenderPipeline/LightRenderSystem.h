#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"

#include <memory>

struct RenderContext;
struct EngineContext;
struct HardwareContext;

class LightRenderSystem: public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;

	void ShadowMap();
};

