#include "ParticlesSystem.h"
#include "../Core/ServiceLocator.h"
#include "HardwareContext.h"
#include "EngineContext.h"
#include "Rendering/RenderContext.h"

SyResult ParticlesSystem::Init()
{
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();

    SY_LOG_CORE(SY_LOGLEVEL_INFO, "Particles system initialization successful. ");
    return SyResult();
}

SyResult ParticlesSystem::Run()
{
    auto viewParticlesSystem = _ecs->view<ParticleComponent>();

    for (auto& ent : viewParticlesSystem)
    {
        ParticleComponent& ps = viewParticlesSystem.get<ParticleComponent>(ent);
        size_t hash = _hasher(ps.SharedParticlesDataUuid);
        if (hash != ps.Hash)
        {
            ps.Hash = hash;
            InitRenderResources(ps);
        }
    }
}

SyResult ParticlesSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "Particles system destruction successful. ");
    return SyResult();
}

void ParticlesSystem::InitRenderResources(ParticleComponent& pc)
{
    //Clear all views and resources;
}
