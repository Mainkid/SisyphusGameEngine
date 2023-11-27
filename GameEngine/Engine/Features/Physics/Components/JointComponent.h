#pragma once
#include "../../../Core/Tools/Structures/Vector.h"
#include "../../vendor/entt/entt.hpp"
#include <boost/functional/hash.hpp>
namespace  physx
{
    class PxFixedJoint;
}
struct SyJointBase
{
    SyJointBase(const SyVector3& localPosition = {0.0f, 0.0f, 0.0f},
                const SyVector3& localRotation = {0.0f, 0.0f, 0.0f},
                const entt::entity& otherEntity = entt::null,
                const SyVector3& otherLocalPosition = {0.0f, 0.0f, 0.0f},
                const SyVector3& otherLocalRotation = {0.0f, 0.0f, 0.0f}) :
                    LocalPosition(localPosition),
                    LocalRotation(localRotation),
                    OtherEntity(otherEntity),
                    OtherLocalPosition(otherLocalPosition),
                    OtherLocalRotation(otherLocalRotation) {}
    
    entt::entity OtherEntity;
    SyVector3 LocalPosition;
    SyVector3 OtherLocalPosition;
    SyVector3 LocalRotation;        //euler angles
    SyVector3 OtherLocalRotation;   //euler angles

private:
    //bool _wasInit = false;
    std::size_t _hash = 0;
    friend SyJointSystem;
};

inline std::size_t hash_value(const SyJointBase& joint)
{
    std::size_t hash = 0;
    boost::hash<SyVector3> vecHasher;
    boost::hash<entt::entity> entHasher;
    hash = entHasher(joint.OtherEntity);
    boost::hash_combine(hash, vecHasher(joint.LocalPosition));
    boost::hash_combine(hash, vecHasher(joint.LocalRotation));
    boost::hash_combine(hash, vecHasher(joint.OtherLocalPosition));
    boost::hash_combine(hash, vecHasher(joint.OtherLocalRotation));
    return hash;
}

struct SyFixedJointComponent : SyJointBase
{
    SyFixedJointComponent(  const SyVector3& localPosition = {0.0f, 0.0f, 0.0f},
                            const SyVector3& localRotation = {0.0f, 0.0f, 0.0f},
                            const entt::entity& otherEntity = entt::null,
                            const SyVector3& otherLocalPosition = {0.0f, 0.0f, 0.0f},
                            const SyVector3& otherLocalRotation = {0.0f, 0.0f, 0.0f}) :
                                SyJointBase(localPosition,
                                            localRotation,
                                            otherEntity,
                                            otherLocalPosition,
                                            otherLocalRotation) {} 
private:
   physx::PxFixedJoint* _fixedJoint = nullptr;

    friend SyJointSystem;
    //friend std::size_t hash_value(const SyFixedJointComponent& joint);
};

// inline std::size_t hash_value(const SyFixedJointComponent& joint)
// {
//     std::size_t hash = 0;
//     boost::hash<SyJointBase> jointBaseHasher;
//     boost::hash<physx::PxFixedJoint*> fixedJointHasher;
//     hash = jointBaseHasher(joint);
//     boost::hash_combine(hash, fixedJointHasher(joint._fixedJoint));
//     return hash;
//     
// }