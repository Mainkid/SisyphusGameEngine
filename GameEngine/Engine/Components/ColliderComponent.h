﻿#pragma once
#include "../../vendor/entt/entt.hpp"
#include "../Tools/Data/Vector.h"
#include "../Serialization/Serializable.h"
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

    SER_DATA(   SyColliderMaterial,
                staticFriction,
                dynamicFriction,
                restitution,
                density)
};

enum SyEPrimitiveColliderType
{
    BOX = 0,
    SPHERE = 1,
    CAPSULE = 2
};

struct SyPrimitiveColliderShapeDesc
{
    SyVector3   Extent      = SyVector3::ONE;   //for box
    float       Radius      = 1.0f;             //for sphere and capsule
    float       HalfHeight  = 1.0f;             //for capsule

    SER_DATA(   SyPrimitiveColliderShapeDesc,
                Extent,
                Radius,
                HalfHeight)
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

    SyPrimitiveColliderComponent(   SyEPrimitiveColliderType colliderType                   = SyEPrimitiveColliderType::BOX,
                                    const SyPrimitiveColliderShapeDesc& colliderShapeDesc   = SyPrimitiveColliderShapeDesc(),
                                    const SyColliderMaterial& material                      = SyColliderMaterial());
    
    //members initialized in constructor
    
    SyEPrimitiveColliderType ColliderType;  //runtime modification is not supported, but will be added in future. The member has to be added to properties
    SyVector3               Extent;         //for box //runtime modification is not supported, but will be added in future. The member has to be added to properties
    float                   Radius;         //for sphere and capsule //runtime modification is not supported, but will be added in future. The member has to be added to properties
    float                   HalfHeight;     //for capsule //runtime modification is not supported, but will be added in future. The member has to be added to properties
    SyColliderMaterial      Material;       //runtime modification is not supported, but will be added in future. The member has to be added to properties
    
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
    
    SyColliderMaterial      Material; //runtime modification is not supported, but will be added in future. The member has to be added to properties

    //members initialized in CollisionSystem::InitComponentTm

private:
    physx::PxShape* _shape;
    
    friend class SyCollisionSystem;
};