#include "RBodyComponent.h"
using namespace physx;


SyRigidBodyComponent::SyRigidBodyComponent( const SyERBodyType&      rbType,
                                    float                   mass,
                                    unsigned flags)
{
    RbType = rbType;
    Mass   = mass;
    Flags  = flags;
}

SyRigidBodyComponent::~SyRigidBodyComponent()
{
}

bool SyRigidBodyComponent::WasInit() const
{
    return _rbActor != nullptr;
}

std::size_t hash_value(const SyRigidBodyComponent& rigidBodyC)
{
    std::size_t hash = 0;
    boost::hash<float> massHash;
    boost::hash<unsigned> flagsHash;
    boost::hash<SyVector3> linearVelocityHash;
    boost::hash<SyVector3> angularVelocityHash;
    hash = massHash(rigidBodyC.Mass);
    boost::hash_combine(hash, flagsHash(rigidBodyC.Flags));
    boost::hash_combine(hash, linearVelocityHash(rigidBodyC.LinearVelocity));
    boost::hash_combine(hash, angularVelocityHash(rigidBodyC.AngularVelocity));
    return hash;
    
}

