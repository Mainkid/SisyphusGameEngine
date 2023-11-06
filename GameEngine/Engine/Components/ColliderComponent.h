#pragma once
#include "../Tools/Data/Vector.h"
#include "../Tools/Data/FlagBitmask.h"

namespace physx
{
    class PxShape;
}

struct SyColliderMaterial
{
    float staticFriction	= 1.0f;
    float dynamicFriction	= 1.0f;
    float restitution		= 0.3f;
    float density			= 0.0001f;
};

enum SyPrimitiveColliderType
{
    SY_RB_SHAPE_TYPE_BOX = 0,
    SY_RB_SHAPE_TYPE_SPHERE = 1,
    SY_RB_SHAPE_TYPE_CAPSULE = 2
};

struct SyPrimitiveColliderShapeDesc
{
    SyVector3   HalfExtent; //for box
    float       Radius; //for sphere and capsule
    float       HalfHeight; //for capsule
};

enum SyColliderFlags
{
    SY_COL_SET_MASS_MANUALLY = 1
};
DEFINE_BITWISE_OPERATORS(SyColliderFlags);

struct SyColliderCreateOnNextUpdateTag
{
    
};

struct SyPrimitiveColliderComponent
{

    SyPrimitiveColliderComponent(   SyPrimitiveColliderType colliderType,
                                    const SyPrimitiveColliderShapeDesc& colliderShapeDesc,
                                    const SyColliderMaterial& material,
                                    unsigned flags = 0);
private:
    //members initialized in constructor
    
    SyPrimitiveColliderType _colliderType;
    SyVector3               _extent;    //for box
    float                   _radius;        //for sphere and capsule
    float                   _halfHeight;    //for capsule
    SyColliderMaterial      _material;
    unsigned                _flags;
    
    friend class SyCollisionSystem;
};