#include "JointSystem.h"
#include <boost/functional/hash.hpp>
#include "../Components/JointComponent.h"
#include "../Components/RBodyComponent.h"
#include "extensions/PxRevoluteJoint.h"
#include "extensions/PxFixedJoint.h"

SyResult SyJointSystem::Init()
{
    return SyResult();
}

SyResult SyJointSystem::Run()
{
    SyResult result;
#pragma region Fixed Joint
    auto fjView = _ecs->view<SyFixedJointComponent>();
    for (auto& entity : fjView)
    {
        auto& fixedJointC           = _ecs->get<SyFixedJointComponent>(entity);
        auto* rigidBodyCPtr         = _ecs->try_get<SyRBodyComponent>(entity);
        if (rigidBodyCPtr == nullptr)
        {
            SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "No RigidBody Component was found on entity (%d). You have a joint component on it. The joint has been removed. ", (int) entity);
            _ecs->remove<SyFixedJointComponent>(entity);
            continue;
        }
        boost::hash<SyFixedJointComponent> hasher;
        std::size_t newHash = hasher(fixedJointC);
        if (newHash != fixedJointC._hash)
        {
            UpdateFixedJoint(entity, fixedJointC, *rigidBodyCPtr);
            fixedJointC._hash = newHash;
        }       
    }
#pragma endregion

    return SyResult();
}

SyResult SyJointSystem::Destroy()
{
    return SyResult();
}

SyResult SyJointSystem::UpdateFixedJoint(const entt::entity& entity,
                                         SyFixedJointComponent& fixedJointC,
                                         SyRBodyComponent rigidBodyС)
{
    SyResult result;
    if (fixedJointC._fixedJoint != nullptr)
        fixedJointC._fixedJoint->release();
    auto* actor = rigidBodyС._rbActor;
    auto* otherRigidBodyC = _ecs->try_get<SyRBodyComponent>(fixedJointC.OtherEntity);
    auto* otherActor = (otherRigidBodyC == nullptr) ? nullptr : otherRigidBodyC->_rbActor;
    physx::PxTransform localFrame(fixedJointC.LocalPosition, SyVector3::EulerToPxQuat(fixedJointC.LocalRotation));
    physx::PxTransform otherLocalFrame(fixedJointC.OtherLocalPosition, SyVector3::EulerToPxQuat(fixedJointC.OtherLocalRotation));
    fixedJointC._fixedJoint = physx::PxFixedJointCreate(*SyRBodyComponent::_physics,
                                                        actor,
                                                        localFrame,
                                                        otherActor,
                                                        otherLocalFrame);
    if (fixedJointC._fixedJoint == nullptr)
    {
        result.code = SY_RESCODE_ERROR;
        result.message = xstring("physx::PxFixedJointCreate failed. Could not create fixed joint in FixedJoint Component on entity(%d).", (int)entity);
        SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "physx::PxFixedJointCreate failed. Could not create fixed joint in FixedJoint Component on entity(%d).", (int)entity);
    }
    return result;
}
