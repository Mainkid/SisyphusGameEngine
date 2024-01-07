#pragma once
#include "../../../Core/ECS/SystemBase.h"

struct SyNavMeshComponent;
class EngineCore;
struct EngineContext;
struct RenderContext;
struct HardwareContext;

class SyNavMeshDrawSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;

private:
    SyResult PrepareRenderDataAabb(const entt::entity& entity, SyNavMeshComponent& navMeshC);
    SyResult PrepareRenderDataMesh(const entt::entity& entity, SyNavMeshComponent& navMeshC);
    SyResult PrepareRenderDataInternalEdges(const entt::entity& entity, SyNavMeshComponent& navMeshC);
    SyResult PrepareRenderDataExternalEdges(const entt::entity& entity, SyNavMeshComponent& navMeshC);

    SyVector3 IntToColor(int number);

    EngineContext* _ec = nullptr;
    RenderContext* _rc = nullptr;
    HardwareContext* _hc = nullptr;
};