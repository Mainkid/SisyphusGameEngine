#pragma once
#include "../../../Core/Tools/Structures/Vector.h"
#include "../../vendor/entt/entt.hpp"
#include <boost/functional/hash.hpp>
namespace  physx
{
    class PxJoint;
    class PxFixedJoint;
}


struct SyJointComponentHelper
{
    entt::entity jointHolder = entt::null;
};


struct SyJointComponent
{

    SyJointComponent(   const entt::entity& otherEntity = entt::null,
                        const SyVector3& localPosition = {0.0f, 0.0f, 0.0f},
                        const SyVector3& localRotation = {0.0f, 0.0f, 0.0f},
                        const SyVector3& otherLocalPosition = {0.0f, 0.0f, 0.0f},
                        const SyVector3& otherLocalRotation = {0.0f, 0.0f, 0.0f}) :
                        OtherEntity(otherEntity),
                        LocalPosition(localPosition),
                        LocalRotation(localRotation),
                        OtherLocalPosition(otherLocalPosition),
                        OtherLocalRotation(otherLocalRotation) {}
    entt::entity OtherEntity;
    SyVector3 LocalPosition;
    SyVector3 OtherLocalPosition;
    SyVector3 LocalRotation;        //euler angles
    SyVector3 OtherLocalRotation;   //euler angles
private:
    physx::PxJoint* _jointPtr = nullptr;
    size_t _hash;

    friend class SyJointSystem;
};
inline std::size_t hash_value(const SyJointComponent& joint)
{
    std::size_t hash = 0;
    boost::hash<int> intHasher;
    boost::hash<SyVector3> vecHasher;
    boost::hash<entt::entity> entHasher;
    boost::hash_combine(hash, entHasher(joint.OtherEntity));
    boost::hash_combine(hash, vecHasher(joint.LocalPosition));
    boost::hash_combine(hash, vecHasher(joint.LocalRotation));
    boost::hash_combine(hash, vecHasher(joint.OtherLocalPosition));
    boost::hash_combine(hash, vecHasher(joint.OtherLocalRotation));
    return hash;    
}

struct SyFixedJointComponent
{
    SyFixedJointComponent(const entt::entity& otherEntity = entt::null) : OtherEntity(otherEntity) {};
    
    entt::entity OtherEntity;
private:
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