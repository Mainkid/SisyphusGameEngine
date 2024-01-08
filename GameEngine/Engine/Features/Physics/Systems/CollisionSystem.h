#pragma once

#include "../../../Core/ECS/SystemBase.h"

struct TransformComponent;
struct SyPrimitiveColliderComponent;
struct SyRigidBodyComponent;
struct SyTrimeshColliderComponent;
struct MeshComponent;

class SyCollisionSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;

private:
    SyResult UpdatePrimitiveColliderGeometry(const entt::entity& entity, SyPrimitiveColliderComponent& pColC);
    SyResult UpdatePrimitiveCollider(const entt::entity& entity,
                            SyRigidBodyComponent& rigidBC,
                            SyPrimitiveColliderComponent& pColC);
    
    SyResult UpdateTrimeshCollider(const entt::entity& entity,
                             SyRigidBodyComponent& rigidBC,
                             SyTrimeshColliderComponent& tmColC,
                             const MeshComponent& meshC,
                             const TransformComponent& transformC);
};
