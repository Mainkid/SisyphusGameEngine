#include "JointSystem.h"
#include <boost/functional/hash.hpp>

#include "../../../Components/TransformComponent.h"
#include "../../../Contexts/EngineContext.h"
#include "../Components/JointComponent.h"
#include "../Components/RBodyComponent.h"
#include "extensions/PxRevoluteJoint.h"
#include "extensions/PxFixedJoint.h"
#include "../../Common/Events/OnAddComponentEvent.h"
#include "../../Common/Events/OnRemoveComponentEvent.h"
#include "../../../Core/Tools/Macro.h"

SyResult SyJointSystem::Init()
{
    return SyResult();
}

SyResult SyJointSystem::Run()
{
    SyResult result;
    auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
    if (deltaTime == 0)
        return result;
    //if (ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
    //    return result;
#pragma region Fixed Joint
    auto view = _ecs->view<SyFixedJointComponent>();
    for (auto& entity : view)
    {
        auto& fixedJointC           = _ecs->get<SyFixedJointComponent>(entity);
        auto* rigidBodyCPtr         = _ecs->try_get<SyRBodyComponent>(entity);
        auto* otherRigidBodyCPtr    = _ecs->try_get<SyRBodyComponent>(fixedJointC.OtherEntity);
        if (rigidBodyCPtr == nullptr)
        {
            _ecs->emplace<SyRBodyComponent>(entity);
            CallEvent<SyOnAddComponentEvent>("AddRBody", SyEComponentTypes::RIGID_BODY, entity);
            SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
                "RigidBody Component required for RigidBody Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
                SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
            continue;
        }
        if (fixedJointC.OtherEntity != entt::null && otherRigidBodyCPtr == nullptr)
        {
            _ecs->emplace<SyRBodyComponent>(fixedJointC.OtherEntity);
            CallEvent<SyOnAddComponentEvent>("AddRBody", SyEComponentTypes::RIGID_BODY, fixedJointC.OtherEntity);
            SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
                "RigidBody Component required for RigidBody Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
                SY_GET_ENTITY_NAME_CHAR(_ecs, fixedJointC.OtherEntity));
            continue;
        }
        if (rigidBodyCPtr->WasInit() == false)
        {
            SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
               "RigidBody Component on entity (%s) has not been initialized. The corresponding JointComponent was not initialized",
               SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
            continue;
        }
        if (fixedJointC.OtherEntity != entt::null && otherRigidBodyCPtr->WasInit() == false)
        {
            SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
              "RigidBody Component on entity (%s) has not been initialized. The corresponding JointComponent was not initialized",
              SY_GET_ENTITY_NAME_CHAR(_ecs, fixedJointC.OtherEntity));
            continue;
        }
        auto& transformC = _ecs->get<TransformComponent>(entity);
        boost::hash<SyJointBase> hasher;
        std::size_t newfjHash = hasher(fixedJointC);
        if (fixedJointC._fixedJoint == nullptr)
        {
            if (InitFixedJointComponent(entity, fixedJointC, *rigidBodyCPtr, transformC).code == SY_RESCODE_ERROR)
            {
                _ecs->remove<SyFixedJointComponent>(entity);
                CallEvent<SyOnRemoveComponentEvent>("RemoveFixedJoint", SyEComponentTypes::FIXED_JOINT, entity);
                SY_LOG_PHYS(SY_LOGLEVEL_ERROR,
                    "Failed to initialize FixedJoint Componenton entity (%s). The component has been removed.",
                    SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
            }
        }
        else if (ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
        {
            UpdateFixedJointComponent(entity, fixedJointC, *rigidBodyCPtr, transformC);
        }
    }
#pragma endregion

    return SyResult();
}

SyResult SyJointSystem::Destroy()
{
    return SyResult();
}

SyResult SyJointSystem::InitFixedJointComponent(const entt::entity& entity, SyFixedJointComponent& fixedJointC,
    SyRBodyComponent& rigidBodyС, TransformComponent& transformC)
{
    SyResult result;
    if (fixedJointC._fixedJoint != nullptr)
        fixedJointC._fixedJoint->release();
    auto* actor = rigidBodyС._rbActor;
    auto* otherActor = (fixedJointC.OtherEntity == entt::null) ? nullptr :
            _ecs->get<SyRBodyComponent>(fixedJointC.OtherEntity)._rbActor;
    TransformComponent* otherTransformCPtr = (fixedJointC.OtherEntity == entt::null) ? nullptr :
            _ecs->try_get<TransformComponent>(fixedJointC.OtherEntity);
    SyVector3 position = transformC._position;
    SyVector3 rotation = transformC._rotation;
    SyVector3 otherPosition = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_position;
    SyVector3 otherRotation = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_rotation;
    physx::PxTransform localFrame(otherPosition - position, SyVector3::EulerToPxQuat(rotation - otherRotation));
    physx::PxTransform otherLocalFrame(SyVector3::ZERO, SyVector3::EulerToPxQuat(SyVector3::ZERO));
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

SyResult SyJointSystem::UpdateFixedJointComponent(const entt::entity& entity, SyFixedJointComponent& fixedJointC,
                                         SyRBodyComponent& rigidBodyС, TransformComponent& transformC)
{
    SyResult result;
    auto* otherRigidBodyCPtr = (fixedJointC.OtherEntity == entt::null) ? nullptr : _ecs->try_get<SyRBodyComponent>(fixedJointC.OtherEntity);
    if (rigidBodyС._wasActorRecreatedThisFrame || (otherRigidBodyCPtr != nullptr && otherRigidBodyCPtr->_wasActorRecreatedThisFrame))
    {
        auto* actor = rigidBodyС._rbActor;
        auto* otherActor = (fixedJointC.OtherEntity == entt::null) ? nullptr :
                _ecs->get<SyRBodyComponent>(fixedJointC.OtherEntity)._rbActor;
        fixedJointC._fixedJoint->setActors(actor, otherActor);
    }
    if (rigidBodyС._wasTransformChangedFromOutside || (otherRigidBodyCPtr != nullptr && otherRigidBodyCPtr->_wasTransformChangedFromOutside))
    {
        TransformComponent* otherTransformCPtr = (fixedJointC.OtherEntity == entt::null) ? nullptr :
                _ecs->try_get<TransformComponent>(fixedJointC.OtherEntity);
        SyVector3 position = transformC._position;
        SyVector3 rotation = transformC._rotation;
        SyVector3 otherPosition = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_position;
        SyVector3 otherRotation = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_rotation;
        physx::PxTransform localFrame(otherPosition - position, SyVector3::EulerToPxQuat(otherRotation - rotation));
        physx::PxTransform otherLocalFrame(SyVector3::ZERO, SyVector3::EulerToPxQuat(SyVector3::ZERO));
        fixedJointC._fixedJoint->setLocalPose(physx::PxJointActorIndex::eACTOR0, localFrame);
        fixedJointC._fixedJoint->setLocalPose(physx::PxJointActorIndex::eACTOR1, otherLocalFrame);
    }
    return result;
}
