#pragma once
#include "../../../Components/TransformComponent.h"
#include "../../../Core/ECS/SystemBase.h"


struct SyFixedJointComponent;
struct SyRBodyComponent;


class SyJointSystem : public SystemBase
{
public:
    SyResult Init()     override;
    SyResult Run()      override;
    SyResult Destroy()  override;

private:
    SyResult InitFixedJointComponent(const entt::entity& entity,
                              SyFixedJointComponent& fixedJointC,
                              SyRBodyComponent& rigidBodyС,
                              TransformComponent& transformC);
    SyResult UpdateFixedJointComponent(const entt::entity& entity,
                              SyFixedJointComponent& fixedJointC,
                              SyRBodyComponent& rigidBodyС,
                              TransformComponent& transformC);
};
