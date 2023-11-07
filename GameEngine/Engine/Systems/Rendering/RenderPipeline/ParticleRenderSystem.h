#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"

struct ParticleComponent;
struct EngineContext;
struct RenderContext;
struct HardwareContext;

class ParticleRenderSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* _ec = nullptr;
	RenderContext* _rc = nullptr;
	HardwareContext* _hc = nullptr;

	void SortGpu();
	void NullSortList(ParticleComponent& pc);
};

