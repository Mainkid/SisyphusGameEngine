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

