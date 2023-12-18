#pragma once
#include "../../../Components/TransformComponent.h"
#include "../../../Core/ECS/SystemBase.h"

struct SyJointComponent;
struct SyFixedJointComponent;
struct SyRBodyComponent;


class SyJointSystem : public SystemBase
{
public:
    SyResult Init()     override;
    SyResult Run()      override;
    SyResult Destroy()  override;

private:
    SyResult InitJointComponent(const entt::entity& entity,
                                SyJointComponent& jointC,
                                SyRBodyComponent& rigidBodyС,
                                TransformComponent& transformC);
    std::string GetJointComponentName(const SyJointComponent& jointComponent);
};
