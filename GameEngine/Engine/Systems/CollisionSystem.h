#pragma once

#include "../Core/ECS/SystemBase.h"

struct TransformComponent;
struct SyPrimitiveColliderComponent;
struct SyRBodyComponent;
struct SyTrimeshColliderComponent;
struct MeshComponent;

class SyCollisionSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;

private:
    SyResult InitComponentP(const entt::entity& entity,
                            SyRBodyComponent& rbComponent,
                            SyPrimitiveColliderComponent& cComponent);
    
    SyResult InitComponentTm(const entt::entity& entity,
                             SyRBodyComponent& rbComponent,
                             SyTrimeshColliderComponent& cComponent,
                             const MeshComponent& mComponent,
                             const TransformComponent& tComponent);
};

struct SyEventOnCreateCollider
{
    SyEventOnCreateCollider(entt::entity entity) : Entity(entity) {};
    entt::entity Entity; //entity that collider is attached to
};