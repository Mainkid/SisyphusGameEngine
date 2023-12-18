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

    SyResult ValidateJointComponent(const entt::entity& entity,
                            const entt::entity& otherEntity,
                            const std::string& jointCName);
    SyResult InitFixedJointComponent(const entt::entity& entity);
};
