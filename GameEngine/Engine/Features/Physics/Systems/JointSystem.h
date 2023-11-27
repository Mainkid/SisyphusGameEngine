#pragma once
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
    SyResult UpdateFixedJoint(const entt::entity& entity,
                              SyFixedJointComponent& fixedJointC,
                              SyRBodyComponent rigidBodyС);
};
