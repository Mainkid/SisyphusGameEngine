#pragma once
#include <boost/container_hash/hash.hpp>

#include "../Core/ECS/SystemBase.h"
#include "../Components/ParticleComponent.h"
#include "SimpleMath.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;


class ParticlesSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	boost::hash<boost::uuids::uuid> _hasher;
	EngineContext* _ec = nullptr;
	HardwareContext* _hc = nullptr;
	RenderContext* _rc = nullptr;

	void InitRenderResources(ParticleComponent& pc);
};

