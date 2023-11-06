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
    return SyResult();
}

SyResult ParticlesSystem::Destroy()
{
    SY_LOG_CORE(SY_LOGLEVEL_INFO, "Particles system destruction successful. ");
    return SyResult();
}

void ParticlesSystem::InitRenderResources(ParticleComponent& pc)
{
    //Clear all views and resources;
    //std::unique<...> should be released;
    if (pc.PoolBufferSrv.Get() != nullptr)
        pc.PoolBufferSrv.Get()->Release();

    if (pc.SortBufferSrv.Get() != nullptr)
        pc.SortBufferSrv.Get()->Release();

    if (pc.PoolBufferUav.Get() != nullptr)
        pc.PoolBufferUav.Get()->Release();

    if (pc.DeadListUav.Get() != nullptr)
        pc.DeadListUav.Get()->Release();

    if (pc.SortBufferUav.Get() != nullptr)
        pc.SortBufferUav.Get()->Release();

    if (pc.CounterUav.Get() != nullptr)
        pc.CounterUav.Get()->Release();

    if (pc.TmpGpuBufferUav.Get() != nullptr)
        pc.TmpGpuBufferUav.Get()->Release();

    if (pc.TmpGpuBufferSrv.Get() != nullptr)
        pc.TmpGpuBufferSrv.Get()->Release();

    pc.Indices.clear();

    InitBuffers(pc);
}

void ParticlesSystem::InitBuffers(ParticleComponent& pc)
{
    pc.IndexBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.IndexBuffer->Initialize(pc.Indices);
    
	pc.PoolBuffer = std::make_unique<Buffer>(hc->device.Get());
	pc.PoolBuffer->Initialize(particleList, maxParticles, sizeof(Particle), true);
    
	pc.DeadListBuffer = std::make_unique<Buffer>(hc->device.Get());
	pc.DeadListBuffer->Initialize(indexList, maxParticles, sizeof(int), true);
    
	pc.SortListBuffer = std::make_unique<Buffer>(hc->device.Get());
	sortListBuffer->Initialize(nullptr, maxParticles, sizeof(SortListParticle), true);
    
	this->constBuffer = std::make_unique<Buffer>(hc->device.Get());
	constBuffer->Initialize(sizeof(CB_ParticleVisualisation));
    
	this->groupCountConstBuffer = std::make_unique<Buffer>(hc->device.Get());
	groupCountConstBuffer->Initialize(sizeof(CB_ComputeShader));
    
	this->sortGPUConstBuffer = std::make_unique<Buffer>(hc->device.Get());
	sortGPUConstBuffer->Initialize(sizeof(CB));
    
	this->tmpGPUBuffer = std::make_unique<Buffer>(hc->device.Get());
	tmpGPUBuffer->Initialize(nullptr, maxParticles, sizeof(SortListParticle), true);
    
	this->indirectDrawBuffer = std::make_unique<Buffer>(hc->device.Get());
	indirectDrawBuffer->InitializeIndirect(maxParticles);
    
	this->counterBuffer = std::make_unique<Buffer>(hc->device.Get());
	counterBuffer->InitializeCounterBuffer();
}
