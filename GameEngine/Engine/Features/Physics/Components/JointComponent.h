#pragma once
#include "../../../Core/Tools/Structures/Vector.h"
#include "../../vendor/entt/entt.hpp"
#include <boost/functional/hash.hpp>
namespace  physx
{
    class PxJoint;
    class PxFixedJoint;
    class PxRevoluteJoint;
}

//An internal component attached to second entity, when creating a joint component.
struct SyJointComponentHelper
{
    entt::entity jointHolder = entt::null;
};

//if you want to add a new joint, do not forget to include it in SyRigidBodySystem::GetJointOtherEntities!!!

struct SyFixedJointComponent
{
    SyFixedJointComponent(const entt::entity& otherEntity = entt::null) : OtherEntity(otherEntity) {};
    //Set by user. Can be updated in runtime. The entity which the joint attaches the owning entity to.
    //Can be set to entt::null, meaning that the owning entity is attached to a fixed point in global space.
    entt::entity OtherEntity;
private:
    //Internal members
    
    physx::PxFixedJoint* _fixedJointPtr = nullptr;
    std::size_t _hash;

    friend class SyJointSystem;
};

inline std::size_t hash_value(const SyFixedJointComponent& joint)
{
    std::size_t hash = 0;
    boost::hash<entt::entity> entHasher;
    hash = entHasher(joint.OtherEntity);
    return hash;    
}

struct SyHingeJointComponent
{
    SyHingeJointComponent(  const SyVector3& localPosition = {0.0f, 0.0f, 0.0f},
                            const SyVector3& localRotation = {0.0f, 0.0f, 0.0f},
                            const entt::entity& otherEntity = entt::null) :
                            LocalPosition(localPosition),
                            LocalRotation(localRotation),
                            OtherEntity(otherEntity) {} 
    //Parameters set by user. Update is supported in runtime.
    
    //The pivot point in entity local space.  
    SyVector3 LocalPosition;
    //The rotation axis relative to global (?) X-axis. Rotation is set in euler angles.
    SyVector3 LocalRotation;
    //Sets the entity which the pivot point and axis are attached to.
    //If set to entt::null, they are attached to a fixed point in global space.
    entt::entity OtherEntity;
private:
    //Internal component
    
    physx::PxRevoluteJoint* _hingeJointPtr = nullptr;
    std::size_t _hash;

    friend class SyJointSystem;
};

inline std::size_t hash_value(const SyHingeJointComponent& joint)
{
    std::size_t hash = 0;
    boost::hash<entt::entity> entHasher;
    boost::hash<SyVector3> vecHasher;
    hash = entHasher(joint.OtherEntity);
    boost::hash_combine(hash, vecHasher(joint.LocalPosition));
    boost::hash_combine(hash, vecHasher(joint.LocalRotation));
    return hash;    
}