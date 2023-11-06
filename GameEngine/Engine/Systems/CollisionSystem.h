#pragma once
#include "../Core/ECS/SystemBase.h"

struct TransformComponent;
struct SyPrimitiveColliderComponent;
struct SyRBodyComponent;

class SyCollisionSystem : SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;

private:
    SyResult InitComponentP(const entt::entity& entity,
                            SyRBodyComponent& rbComponent,
                            SyPrimitiveColliderComponent& cComponent);
};
