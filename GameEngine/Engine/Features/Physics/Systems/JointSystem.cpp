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
    if (deltaTime == 0 || ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
        return result;
    
#pragma region Fixed Joint
    {
        auto view = _ecs->view<SyFixedJointComponent>();
        for (auto& entity : view)
        {
            auto& fixedJointC = _ecs->get<SyFixedJointComponent>(entity);
            bool validationSuccessful = ValidateJointComponent(entity, fixedJointC.OtherEntity, "FixedJoint").code == SY_RESCODE_OK;
            if (validationSuccessful == false)
                continue;
            static boost::hash<SyFixedJointComponent> jointHasher;
            std::size_t newHash = jointHasher(fixedJointC);
            if (fixedJointC._fixedJointPtr == nullptr || fixedJointC._hash != newHash)
            {
                fixedJointC._hash = newHash;
                if (InitFixedJointComponent(entity).code == SY_RESCODE_ERROR)
                {
                    _ecs->remove<SyFixedJointComponent>(entity);
                    CallEvent<SyOnRemoveComponentEvent>("RemoveJoint", SyEComponentType::FIXED_JOINT, entity);
                    SY_LOG_PHYS(SY_LOGLEVEL_ERROR,
                        "Failed to initialize FixedJoint Component on entity (%s). The component has been removed.",
                        SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
                }
            }
        }
    }
#pragma endregion
#pragma region Hinge Joint
{
    auto view = _ecs->view<SyHingeJointComponent>();
    for (auto& entity : view)
    {
        auto& hingeJointC = _ecs->get<SyHingeJointComponent>(entity);
        bool validationSuccessful = ValidateJointComponent(entity, hingeJointC.OtherEntity, "HingeJoint").code == SY_RESCODE_OK;
        if (validationSuccessful == false)
            continue;
        static boost::hash<SyHingeJointComponent> jointHasher;
        std::size_t newHash = jointHasher(hingeJointC);
        if (hingeJointC._hingeJointPtr == nullptr || hingeJointC._hash != newHash)
        {
            hingeJointC._hash = newHash;
            if (InitHingeJointComponent(entity).code == SY_RESCODE_ERROR)
            {
                _ecs->remove<SyHingeJointComponent>(entity);
                CallEvent<SyOnRemoveComponentEvent>("RemoveJoint", SyEComponentType::HINGE_JOINT, entity);
                SY_LOG_PHYS(SY_LOGLEVEL_ERROR,
                    "Failed to initialize HingeJoint Component on entity (%s). The component has been removed.",
                    SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
            }
        }
    }
}
#pragma endregion

    return SyResult();
}

SyResult SyJointSystem::Destroy()
{
    return SyResult();
}

SyResult SyJointSystem::ValidateJointComponent(const entt::entity& entity, const entt::entity& otherEntity, const std::string& jointCName)
{
    SyResult result;
    auto* rigidBodyCPtr = _ecs->try_get<SyRBodyComponent>(entity);
    auto* otherRigidBodyCPtr = _ecs->try_get<SyRBodyComponent>(otherEntity);
    if (rigidBodyCPtr == nullptr)
    {
        _ecs->emplace<SyRBodyComponent>(entity);
        CallEvent<SyOnAddComponentEvent>("AddRBody", SyEComponentType::RIGID_BODY, entity);
        SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
        "RigidBody Component required for %s Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
        jointCName.c_str(),
        SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        result.code = SY_RESCODE_UNEXPECTED;
        result.message = xstring("RigidBody Component required for %s Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
                jointCName.c_str(),
                SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
        return result;
    }
    if (otherEntity != entt::null && otherRigidBodyCPtr == nullptr)
    {
        _ecs->emplace<SyRBodyComponent>(otherEntity);
        CallEvent<SyOnAddComponentEvent>("AddRBody", SyEComponentType::RIGID_BODY, otherEntity);
        SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
               "RigidBody Component required for %s Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
               jointCName.c_str(),
               SY_GET_ENTITY_NAME_CHAR(_ecs, otherEntity));
        result.code = SY_RESCODE_UNEXPECTED;
        result.message = xstring(   "RigidBody Component required for %s Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
                                    jointCName.c_str(),
                                    SY_GET_ENTITY_NAME_CHAR(_ecs, otherEntity));
        return result;
    }
    if (rigidBodyCPtr->WasInit() == false)
    {
        result.code = SY_RESCODE_UNEXPECTED;
        result.message = xstring(   "RigidBody Component on entity (%s) has not been initialized. The corresponding %s Component was not initialized",
                                    SY_GET_ENTITY_NAME_CHAR(_ecs, entity),
                                    jointCName.c_str());
        return result;
    }
    if (otherEntity != entt::null && otherRigidBodyCPtr->WasInit() == false)
    {
        result.code = SY_RESCODE_UNEXPECTED;
        result.message = xstring(   "RigidBody Component on entity (%s) has not been initialized. The corresponding %s Component was not initialized",
                                    SY_GET_ENTITY_NAME_CHAR(_ecs, otherEntity),
                                    jointCName.c_str());
        return result;
    }
    return result;
}

SyResult SyJointSystem::InitFixedJointComponent(const entt::entity& entity)
{
    SyResult result;
    auto& fixedJointC = _ecs->get<SyFixedJointComponent>(entity);
    auto& rigidBodyС =  _ecs->get<SyRBodyComponent>(entity);
    auto& transformC =  _ecs->get<TransformComponent>(entity);
    if (fixedJointC._fixedJointPtr != nullptr)
        fixedJointC._fixedJointPtr->release();
    if (fixedJointC.OtherEntity != entt::null)
        _ecs->emplace_or_replace<SyJointComponentHelper>(fixedJointC.OtherEntity, entity);
    //physx::PxFixedJoint keeps localFrame and otherLocalFrame same in global frame. To avoid snapping entities together
    //we have to make localFrame the transform of OtherEntity in entity's local frame, and otherLocalFrame a zero transform

    auto* actor = rigidBodyС._rbActor;
    auto* otherActor = (fixedJointC.OtherEntity == entt::null) ? nullptr :
            _ecs->get<SyRBodyComponent>(fixedJointC.OtherEntity)._rbActor;
    TransformComponent* otherTransformCPtr = (fixedJointC.OtherEntity == entt::null) ? nullptr :
            _ecs->try_get<TransformComponent>(fixedJointC.OtherEntity);
    SyVector3 position = transformC._position;
    SyVector3 rotation = transformC._rotation;
    SyVector3 otherPosition = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_position;
    SyVector3 otherRotation = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_rotation;
    physx::PxTransform localFrame(otherPosition - position, SyVector3::EulerToPxQuat(otherRotation - rotation));
    physx::PxTransform otherLocalFrame(SyVector3::ZERO, SyVector3::EulerToPxQuat(SyVector3::ZERO));
    fixedJointC._fixedJointPtr = physx::PxFixedJointCreate(*SyRBodyComponent::_physics,
                                                        actor,
                                                        localFrame,
                                                        otherActor,
                                                        otherLocalFrame);
    if (fixedJointC._fixedJointPtr == nullptr)
    {
        result.code = SY_RESCODE_ERROR;
        result.message = xstring("Failed to create FixedJoint Component on entity(%d) due to internal PhysX error.",
            (int)entity);
        SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to create %s Component on entity(%d) due to internal PhysX error.",
            (int)entity);
    }
    return result;
}

SyResult SyJointSystem::InitHingeJointComponent(const entt::entity& entity)
{
    SyResult result;
    auto& hingeJoinC = _ecs->get<SyHingeJointComponent>(entity);
    auto& rigidBodyС =  _ecs->get<SyRBodyComponent>(entity);
    auto& transformC =  _ecs->get<TransformComponent>(entity);
    if (hingeJoinC._hingeJointPtr != nullptr)
        hingeJoinC._hingeJointPtr->release();
    if (hingeJoinC.OtherEntity != entt::null)
        _ecs->emplace_or_replace<SyJointComponentHelper>(hingeJoinC.OtherEntity, entity);
    //physx::PxHingeJoint keeps localFrame and otherLocalFrame at the same global point and both x-axes are aligned. To avoid snapping entities together
    //we have to make localFrame the transform of OtherEntity in entity's local frame, and otherLocalFrame a zero transform

    auto* actor = rigidBodyС._rbActor;
    auto* otherActor = (hingeJoinC.OtherEntity == entt::null) ? nullptr :
            _ecs->get<SyRBodyComponent>(hingeJoinC.OtherEntity)._rbActor;
    TransformComponent* otherTransformCPtr = (hingeJoinC.OtherEntity == entt::null) ? nullptr :
            _ecs->try_get<TransformComponent>(hingeJoinC.OtherEntity);
    SyVector3 position = transformC._position + hingeJoinC.LocalPosition;
    SyVector3 rotation = transformC._rotation + hingeJoinC.LocalRotation;
    SyVector3 otherPosition = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_position; 
    SyVector3 otherRotation = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_rotation;
    physx::PxTransform localFrame(hingeJoinC.LocalPosition, SyVector3::EulerToPxQuat(hingeJoinC.LocalRotation));
    physx::PxTransform otherLocalFrame(position - otherPosition, SyVector3::EulerToPxQuat(rotation - otherRotation));
    hingeJoinC._hingeJointPtr = physx::PxRevoluteJointCreate(   *SyRBodyComponent::_physics,
                                                                actor,
                                                                localFrame,
                                                                otherActor,
                                                                otherLocalFrame);
    if (hingeJoinC._hingeJointPtr == nullptr)
    {
        result.code = SY_RESCODE_ERROR;
        result.message = xstring("Failed to create FixedJoint Component on entity(%d) due to internal PhysX error.",
            (int)entity);
        SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to create %s Component on entity(%d) due to internal PhysX error.",
            (int)entity);
    }
    return result;
}

