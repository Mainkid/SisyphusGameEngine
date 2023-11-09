#include "RBodyComponent.h"
using namespace physx;


SyRBodyComponent::SyRBodyComponent( const SyERBodyType&      rbType,
                                    float                   mass,
                                    unsigned flags)
{
    _rbType = rbType;
    _mass   = mass;
    _flags  = flags;
}

SyRBodyComponent::~SyRBodyComponent()
{
}

