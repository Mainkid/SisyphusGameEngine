#include "RBodyComponent.h"
using namespace physx;


SyRBodyComponent::SyRBodyComponent( const SyRBodyType&      rbType,
                                    const SyRbTransform&    rbTransform,
                                    float                   mass)
{
    _rbType          = rbType;
    _mass            = mass;
    _origin          = rbTransform.Origin;
    _rotation        = rbTransform.Rotation;
}

SyRBodyComponent::~SyRBodyComponent()
{
}

