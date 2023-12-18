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
    if (deltaTime == 0 || ServiceLocator::instance()->Get<EngineContext>()->playModeState == EngineContext::EPlayModeState::PlayMode)
        return result;
    
#pragma region Fixed Joint
    {
        auto view = _ecs->view<SyJointComponent>();
        for (auto& entity : view)
        {
            auto& jointC                = _ecs->get<SyJointComponent>(entity);
            auto* rigidBodyCPtr         = _ecs->try_get<SyRBodyComponent>(entity);
            auto* otherRigidBodyCPtr    = _ecs->try_get<SyRBodyComponent>(jointC.OtherEntity);
            if (rigidBodyCPtr == nullptr)
            {
                _ecs->emplace<SyRBodyComponent>(entity);
                CallEvent<SyOnAddComponentEvent>("AddRBody", SyEComponentType::RIGID_BODY, entity);
                SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
                "RigidBody Component required for %s Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
                GetJointComponentName(jointC).c_str(),
                    SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
                continue;
            }
            if (jointC.OtherEntity != entt::null && otherRigidBodyCPtr == nullptr)
            {
                _ecs->emplace<SyRBodyComponent>(jointC.OtherEntity);
                CallEvent<SyOnAddComponentEvent>("AddRBody", SyEComponentType::RIGID_BODY, jointC.OtherEntity);
                SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
                    "RigidBody Component required for %s Component is missing on entity (%s). The RigidBody Component has been added in the next frame.",
                    GetJointComponentName(jointC).c_str(),
                    SY_GET_ENTITY_NAME_CHAR(_ecs, jointC.OtherEntity));
                continue;
            }
            if (rigidBodyCPtr->WasInit() == false)
            {
                SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
                   "RigidBody Component on entity (%s) has not been initialized. The corresponding JointComponent was not initialized",
                   SY_GET_ENTITY_NAME_CHAR(_ecs, entity));
                continue;
            }
            if (jointC.OtherEntity != entt::null && otherRigidBodyCPtr->WasInit() == false)
            {
                SY_LOG_PHYS(SY_LOGLEVEL_WARNING,
                  "RigidBody Component on entity (%s) has not been initialized. The corresponding JointComponent was not initialized",
                  SY_GET_ENTITY_NAME_CHAR(_ecs, jointC.OtherEntity));
                continue;
            }
            auto& transformC = _ecs->get<TransformComponent>(entity);
            static boost::hash<SyJointComponent> jointHasher;
            std::size_t newHash = jointHasher(jointC);
            if (jointC._jointPtr == nullptr || jointC._hash != newHash)
            {
                jointC._hash = newHash;
                if (InitJointComponent(entity, jointC, *rigidBodyCPtr, transformC).code == SY_RESCODE_ERROR)
                {
                    _ecs->remove<SyFixedJointComponent>(entity);
                    CallEvent<SyOnRemoveComponentEvent>("RemoveFixedJoint", SyEComponentType::JOINT, entity);
                    SY_LOG_PHYS(SY_LOGLEVEL_ERROR,
                        "Failed to initialize %s Component on entity (%s). The component has been removed.",
                        GetJointComponentName(jointC).c_str(),
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

SyResult SyJointSystem::InitJointComponent(const entt::entity& entity, SyJointComponent& jointC,
                                           SyRBodyComponent& rigidBodyС, TransformComponent& transformC)
{
    SyResult result;
    if (jointC._jointPtr != nullptr)
        jointC._jointPtr->release();
    if (jointC.OtherEntity != entt::null)
        _ecs->emplace_or_replace<SyComponentHelper>(jointC.OtherEntity, entity, &jointC);
    auto* actor = rigidBodyС._rbActor;
    auto* otherActor = (jointC.OtherEntity == entt::null) ? nullptr :
            _ecs->get<SyRBodyComponent>(jointC.OtherEntity)._rbActor;
    TransformComponent* otherTransformCPtr = (jointC.OtherEntity == entt::null) ? nullptr :
            _ecs->try_get<TransformComponent>(jointC.OtherEntity);
    SyVector3 position = transformC._position;
    SyVector3 rotation = transformC._rotation;
    SyVector3 otherPosition = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_position;
    SyVector3 otherRotation = (otherTransformCPtr == nullptr) ? SyVector3::ZERO : otherTransformCPtr->_rotation;
    physx::PxTransform localFrame(otherPosition - position, SyVector3::EulerToPxQuat(otherRotation - rotation));
    physx::PxTransform otherLocalFrame(SyVector3::ZERO, SyVector3::EulerToPxQuat(SyVector3::ZERO));
    switch (jointC.JointType)
    {
    case (SyEJointType::FIXED_JOINT) :
        jointC._jointPtr = physx::PxFixedJointCreate(*SyRBodyComponent::_physics,
                                                        actor,
                                                        localFrame,
                                                        otherActor,
                                                        otherLocalFrame);
        break;
    }
    if (jointC._jointPtr == nullptr)
    {
        result.code = SY_RESCODE_ERROR;
        result.message = xstring("Failed to create %s Component on entity(%d) due to internal PhysX error.",
            GetJointComponentName(jointC).c_str(),
            (int)entity);
        SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to create %s Component on entity(%d) due to internal PhysX error.",
            GetJointComponentName(jointC).c_str(),
            (int)entity);
    }
    return result;
}


std::string SyJointSystem::GetJointComponentName(const SyJointComponent& jointComponent)
{
    switch (jointComponent.JointType)
    {
    case (SyEJointType::FIXED_JOINT):
            return std::string("FixedJoint");
    case (SyEJointType::HINGE_JOINT) :
        return std::string("HingeJoint");
    }
}
