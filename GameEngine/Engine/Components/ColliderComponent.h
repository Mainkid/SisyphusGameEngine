#pragma once
#include "../../vendor/entt/entt.hpp"
#include "../Tools/Data/Vector.h"

namespace physx
{
    class PxShape;
}

struct SyColliderMaterial
{
    float staticFriction	= 1.0f;
    float dynamicFriction	= 1.0f;
    float restitution		= 0.3f;
    float density			= 0.1f;
};

enum SyEPrimitiveColliderType
{
    BOX = 0,
    SPHERE = 1,
    CAPSULE = 2
};

struct SyPrimitiveColliderShapeDesc
{
    SyVector3   Extent;     //for box
    float       Radius;     //for sphere and capsule
    float       HalfHeight; //for capsule
};

struct SyEventOnCreateCollider
{
    SyEventOnCreateCollider(entt::entity entity) : Entity(entity) {};
    entt::entity Entity; //entity that collider is attached to
};

struct SyTagColliderVisualizationEnabled
{
    int PlaceHolder = 0; //needed for try_get working correctly
};

struct SyPrimitiveColliderComponent
{



    SyPrimitiveColliderComponent(   SyEPrimitiveColliderType colliderType,
                                    const SyPrimitiveColliderShapeDesc& colliderShapeDesc,
                                    const SyColliderMaterial& material);
private:
    //members initialized in constructor
    
    SyEPrimitiveColliderType _colliderType;
    SyVector3               _extent;        //for box
    float                   _radius;        //for sphere and capsule
    float                   _halfHeight;    //for capsule
    SyColliderMaterial      _material;
    
    //members initialized in CollisionSystem::InitComponentP
private:
    physx::PxShape*			_shape = nullptr;

    friend class SyCollisionPreSystem;
    friend class SyCollisionSystem;
};

struct SyTrimeshColliderComponent
{
    SyTrimeshColliderComponent(  const SyColliderMaterial& material = SyColliderMaterial());
    //members initialized in constructor
    
    SyColliderMaterial      _material;
    //members initialized in CollisionSystem::InitComponentTm
    
    physx::PxShape* _shape;
    
    friend class SyCollisionSystem;
};