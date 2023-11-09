#include "RBodyComponent.h"
using namespace physx;


SyRBodyComponent::SyRBodyComponent( const SyRBodyType&      rbType,
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

