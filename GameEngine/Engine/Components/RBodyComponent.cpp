#include "RBodyComponent.h"
using namespace physx;


SyRBodyComponent::SyRBodyComponent( const SyRBodyType&      rbType,
                                    float                   mass)
{
    _rbType          = rbType;
    _mass            = mass;
}

SyRBodyComponent::~SyRBodyComponent()
{
}

