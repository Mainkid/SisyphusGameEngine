#pragma once
#include "../../../Components/TransformComponent.h"
#include "../../../Core/ECS/SystemBase.h"

struct SyJointComponent;
struct SyFixedJointComponent;
struct SyRigidBodyComponent;


class SyJointSystem : public SystemBase
{
public:
    SyResult Init()     override;
    SyResult Run()      override;
    SyResult Destroy()  override;

private:
//check that all required components are attached to the entity and the other entity, if not attach them and return SY_RESCODE_UNEXPECTED
    SyResult ValidateJointComponent(const entt::entity& entity,
                            const entt::entity& otherEntity,
                            const std::string& jointCName);
    SyResult InitFixedJointComponent(const entt::entity& entity);
    SyResult InitHingeJointComponent(const entt::entity& entity);
};
