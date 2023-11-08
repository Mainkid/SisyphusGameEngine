#include "ParticlesSystem.h"
#include "../Core/ServiceLocator.h"
#include "HardwareContext.h"
#include "EngineContext.h"
#include "ResourceService.h"
#include "../Core/Graphics/ConstantBuffer.h"
#include "../Tools/ImageLoader.h"
#include "Rendering/RenderContext.h"

SyResult ParticlesSystem::Init()
{
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    _rs = ServiceLocator::instance()->Get<ResourceService>();

   

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
            ps.SharedParticlesDataResource = std::static_pointer_cast<SharedParticlesData>(_rs->LoadResource(ps.SharedParticlesDataUuid));
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

    Clear(pc);

    int maxParticles = pc.SharedParticlesDataResource->MaxParticles;

    pc.ParticlesList.resize(maxParticles);

    for (int i=0;i<maxParticles;i++)
    {
        pc.ParticlesList[i].size = Vector4(pc.SharedParticlesDataResource->StartSize.Fvalue,
            pc.SharedParticlesDataResource->StartSize.Fvalue,
            pc.SharedParticlesDataResource->StartSize.Fvalue,
            1);
        pc.ParticlesList[i].color = Vector4(pc.SharedParticlesDataResource->StartColor.V4value);
        pc.ParticlesList[i].lifeTime = Vector4(0, pc.SharedParticlesDataResource->StartLifeTime.Fvalue, 1, 1);

    }

    pc.IndexList.resize(maxParticles);

    for (int i=0;i<maxParticles;i++)
    {
        pc.IndexList[i] = i;
    }

    int startIndex = 0;
    for (int i=0; i<maxParticles;i++)
    {
        pc.Indices.push_back(startIndex);
        pc.Indices.push_back(startIndex + 2);
        pc.Indices.push_back(startIndex + 1);
        pc.Indices.push_back(startIndex);
        pc.Indices.push_back(startIndex + 3);
        pc.Indices.push_back(startIndex + 2);
        startIndex += 4;
    }

    pc.data.resize(maxParticles);

    InitBuffers(pc);
    CreateSrvAndUav(pc);

}

void ParticlesSystem::Clear(ParticleComponent& pc)
{
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

}

void ParticlesSystem::InitBuffers(ParticleComponent& pc)
{
    pc.IndexBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.IndexBuffer->Initialize(pc.Indices);
    
	pc.PoolBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.PoolBuffer->Initialize(pc.ParticlesList.data(), pc.SharedParticlesDataResource->MaxParticles, sizeof(Particle), true);
    
	pc.DeadListBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.DeadListBuffer->Initialize(pc.IndexList.data(), pc.SharedParticlesDataResource->MaxParticles, sizeof(int), true);
    
	pc.SortListBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.SortListBuffer->Initialize(nullptr, pc.SharedParticlesDataResource->MaxParticles, sizeof(SortListParticle), true);
    
	pc.ConstBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.ConstBuffer->Initialize(sizeof(CB_ParticleVisualisation));
    
	pc.GroupCountConstBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.GroupCountConstBuffer->Initialize(sizeof(CB_ComputeShader));
    
	pc.SortGpuConstBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.SortGpuConstBuffer->Initialize(sizeof(CB));
    
	pc.TmpGpuBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.TmpGpuBuffer->Initialize(nullptr, pc.SharedParticlesDataResource->MaxParticles, sizeof(SortListParticle), true);
    
	pc.IndirectDrawBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.IndirectDrawBuffer->InitializeIndirect(pc.SharedParticlesDataResource->MaxParticles);
    
	pc.CounterBuffer = std::make_unique<Buffer>(_hc->device.Get());
	pc.CounterBuffer->InitializeCounterBuffer();

    D3D11_BUFFER_DESC readback_buffer_desc = {};
    readback_buffer_desc.ByteWidth = 4;
    readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
    readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    readback_buffer_desc.StructureByteStride = 4;
    _hc->device->CreateBuffer(&readback_buffer_desc, nullptr, pc.CounterCpuBuffer.GetAddressOf());

}

void ParticlesSystem::CreateSrvAndUav(ParticleComponent& pc)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
    viewDesc.Format = DXGI_FORMAT_UNKNOWN;
    viewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    viewDesc.Buffer.FirstElement = 0;
    viewDesc.Buffer.Flags = 0;
    viewDesc.Buffer.NumElements = pc.PoolBuffer->size;

    HRESULT res = _hc->device->CreateUnorderedAccessView(pc.PoolBuffer->buffer.Get(), &viewDesc, pc.PoolBufferUav.GetAddressOf());

    viewDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

    res = _hc->device->CreateUnorderedAccessView(pc.DeadListBuffer->buffer.Get(), &viewDesc, pc.DeadListUav.GetAddressOf());
    viewDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
    res = _hc->device->CreateUnorderedAccessView(pc.SortListBuffer->buffer.Get(), &viewDesc, pc.SortBufferUav.GetAddressOf());

    D3D11_BUFFER_DESC constant_buffer_desc = {};
    constant_buffer_desc.ByteWidth = sizeof(CB);
    constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constant_buffer_desc.CPUAccessFlags = 0;
    HRESULT hr = _hc->device->CreateBuffer(&constant_buffer_desc, nullptr, pc.SortGpuConstBuffer->buffer.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvbuffer_desc = {};
    srvbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvbuffer_desc.Buffer.ElementWidth = pc.SharedParticlesDataResource->MaxParticles;
    hr = _hc->device->CreateShaderResourceView(pc.SortListBuffer->buffer.Get(), &srvbuffer_desc, pc.SortBufferSrv.GetAddressOf());
    hr = _hc->device->CreateShaderResourceView(pc.PoolBuffer->buffer.Get(), &srvbuffer_desc, pc.PoolBufferSrv.GetAddressOf());
    hr = _hc->device->CreateShaderResourceView(pc.TmpGpuBuffer->buffer.Get(), &srvbuffer_desc, pc.TmpGpuBufferSrv.GetAddressOf());

    srvbuffer_desc.Format = DXGI_FORMAT_R32_UINT;
    srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvbuffer_desc.Buffer.ElementWidth = 1;

    hr = _hc->device->CreateShaderResourceView(pc.CounterBuffer->buffer.Get(), &srvbuffer_desc, pc.CounterSrv.GetAddressOf());


    D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc = {};
    uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavbuffer_desc.Buffer.NumElements = pc.SharedParticlesDataResource->MaxParticles;
    uavbuffer_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
    hr = _hc->device->CreateUnorderedAccessView(pc.TmpGpuBuffer->buffer.Get(), &uavbuffer_desc, pc.TmpGpuBufferUav.GetAddressOf());


    D3D11_UNORDERED_ACCESS_VIEW_DESC counterDesc = {};
    counterDesc.Format = DXGI_FORMAT_R32_UINT;
    counterDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    counterDesc.Buffer.NumElements = 1;
    counterDesc.Buffer.Flags = 0;
    hr = _hc->device->CreateUnorderedAccessView(pc.CounterBuffer->buffer.Get(), &counterDesc, pc.CounterUav.GetAddressOf());

    int numGroups = (pc.SharedParticlesDataResource->MaxParticles % 768 != 0) ?
        ((pc.SharedParticlesDataResource->MaxParticles / 768) + 1) : (pc.SharedParticlesDataResource->MaxParticles / 768);
    double secondRoot = sqrt(numGroups);
    secondRoot = ceilf(secondRoot);
    pc.GroupSizeX = (int)secondRoot;
    pc.GroupSizeY = (int)secondRoot;
}
