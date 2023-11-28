#include "RBodyComponent.h"
using namespace physx;


SyRBodyComponent::SyRBodyComponent( const SyERBodyType&      rbType,
                                    float                   mass,
                                    unsigned flags)
{
    RbType = rbType;
    Mass   = mass;
    Flags  = flags;
}

SyRBodyComponent::~SyRBodyComponent()
{
}

bool SyRBodyComponent::WasInit() const
{
    return _rbActor != nullptr;
}

std::size_t hash_value(const SyRBodyComponent& rigidBodyC)
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

