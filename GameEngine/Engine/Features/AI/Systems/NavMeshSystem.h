#pragma once
#include "../../../Core/ECS/SystemBase.h"



struct SyNavMeshComponent;

class SyNavMeshSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
    
private:
    SyResult PrepareData(const entt::entity& entity);
    SyResult BuildNavMesh(const entt::entity& entity);
};
