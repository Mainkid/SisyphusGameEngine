#include "RBodyComponent.h"
#include "PxConfig.h"
#include "PxPhysicsAPI.h"
#include "../Tools/ErrorLogger.h"
#include <iostream>
using namespace physx;


SyRBodyComponent::SyRBodyComponent( const SyRBodyType&      rbType,
                                    const SyRbTransform&    rbTransform,
                                    const SyRBodyMaterial&  rbMaterial,
                                    bool                    manuallySetMass,
                                    float                   mass)
{
    _rbType          = rbType;
    _rbMaterial      = rbMaterial;
    _manuallySetMass = manuallySetMass;
    _mass            = mass;
    _origin          = rbTransform.Origin;
    _rotation        = rbTransform.Rotation;
}

SyRBodyComponent::~SyRBodyComponent()
{
}

