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
               //bool useRelativePose = false,
                const entt::entity& otherEntity = entt::null,
                const SyVector3& otherLocalPosition = {0.0f, 0.0f, 0.0f},
                const SyVector3& otherLocalRotation = {0.0f, 0.0f, 0.0f}) :
                    LocalPosition(localPosition),
                    LocalRotation(localRotation),
                    //UseRelativePose(useRelativePose),
                    OtherEntity(otherEntity),
                    OtherLocalPosition(otherLocalPosition),
                    OtherLocalRotation(otherLocalRotation) {}
    
    entt::entity OtherEntity;
    //bool UseRelativePose;
    SyVector3 LocalPosition;
    SyVector3 OtherLocalPosition;
    SyVector3 LocalRotation;        //euler angles
    SyVector3 OtherLocalRotation;   //euler angles

private:
    std::size_t _hash = 0;
    friend SyJointSystem;
};

inline std::size_t hash_value(const SyJointBase& joint)
{
    std::size_t hash = 0;
    boost::hash<SyVector3> vecHasher;
    //boost::hash<bool> boolHasher;
    boost::hash<entt::entity> entHasher;
    hash = entHasher(joint.OtherEntity);
    boost::hash_combine(hash, vecHasher(joint.LocalPosition));
    boost::hash_combine(hash, vecHasher(joint.LocalRotation));
    //boost::hash_combine(hash, boolHasher(joint.UseRelativePose));
    boost::hash_combine(hash, vecHasher(joint.OtherLocalPosition));
    boost::hash_combine(hash, vecHasher(joint.OtherLocalRotation));
    return hash;
}

struct SyFixedJointComponent : SyJointBase
{
    SyFixedJointComponent(  const entt::entity& otherEntity = entt::null) :
                                SyJointBase(SyVector3::ZERO,
                                            SyVector3::ZERO,
                                            otherEntity,
                                            SyVector3::ZERO,
                                            SyVector3::ZERO) {} 
private:
   physx::PxFixedJoint* _fixedJoint = nullptr;

    friend SyJointSystem;
    //friend std::size_t hash_value(const SyFixedJointComponent& joint);
};

struct SyFixedJointComponentHelper
{
    entt::entity fixedJointHolder = entt::null;
    SyFixedJointComponent* fixedJointCPtr;
};

struct SyEngineFixedJointComponent
{
    SyEngineFixedJointComponent(  const entt::entity& otherEntity = entt::null) :
    _fixedJointC(SyFixedJointComponent(otherEntity)) {};

private:
    SyFixedJointComponent _fixedJointC;

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