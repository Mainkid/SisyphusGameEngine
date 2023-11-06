#pragma once
#include "../Tools/Data/Vector.h"

enum SyPrimitiveColliderType
{
    SY_RB_SHAPE_TYPE_BOX = 0,
    SY_RB_SHAPE_TYPE_SPHERE = 1,
    SY_RB_SHAPE_TYPE_CAPSULE = 2
};

struct mitiveColliderShapeDesc
{
    SyVector3   HalfExtent; //for box
    float       Radius; //for sphere and capsule
    float       HalfHeight; //for capsule
};



struct SyPrimitiveColliderComponent
{

private:
    SyPrimitiveColliderType _colliderType;
    SyVector3               _halfExtent; //for box
    float                   _radius; //for sphere and capsule
    float                   _halfHeight; //for capsule
    
};