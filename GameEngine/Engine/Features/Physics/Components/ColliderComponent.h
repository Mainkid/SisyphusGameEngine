#pragma once
#include "../../vendor/entt/entt.hpp"
#include "../../../Core/Tools/Structures/Vector.h"
#include "../../../Core/Tools/Structures/Geometry.h"
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
    SyVector3   Extent      = SyVector3::ONE;   //for box
    float       Radius      = 1.0f;             //for sphere and capsule
    float       HalfHeight  = 1.0f;             //for capsule

};




struct SyPrimitiveColliderComponent
{

    SyPrimitiveColliderComponent(   SyEPrimitiveColliderType colliderType                   = SyEPrimitiveColliderType::BOX,
                                    const SyPrimitiveColliderShapeDesc& colliderShapeDesc   = SyPrimitiveColliderShapeDesc(),
                                    const SyColliderMaterial& material                      = SyColliderMaterial());

    SyPrimitiveColliderComponent(const SyPrimitiveColliderComponent& other)
    {
        this->ColliderType = other.ColliderType;
        this->Extent = other.Extent;
        this->Radius = other.Radius;
        this->HalfHeight = other.HalfHeight;
        this->Material = other.Material;
    }
    
    //members initialized in constructor
    
    SyEPrimitiveColliderType ColliderType;  //runtime modification is not supported, but will be added in future. The member has to be added to properties
    SyVector3               Extent;         //for box //runtime modification is not supported, but will be added in future. The member has to be added to properties
    float                   Radius;         //for sphere and capsule //runtime modification is not supported, but will be added in future. The member has to be added to properties
    float                   HalfHeight;     //for capsule //runtime modification is not supported, but will be added in future. The member has to be added to properties
    SyColliderMaterial      Material;       //runtime modification is not supported, but will be added in future. The member has to be added to properties
    
    //members initialized in CollisionSystem::InitComponentP

    size_t MonoHash = 0; // read/write only by mono sync system
private:
    physx::PxShape*			_shape = nullptr;
    SyGeometry      _colliderGeometry;
    
    friend class SyCollisionPreSystem;
    friend class SyCollisionSystem;
    friend class EditorColliderRenderSystem;
};

struct SyTrimeshColliderComponent
{
    SyTrimeshColliderComponent(  const SyColliderMaterial& material = SyColliderMaterial());
    //members initialized in constructor

    SyTrimeshColliderComponent(const SyTrimeshColliderComponent& other)
    {
        this->Material = other.Material;
    }

    SyColliderMaterial      Material; //runtime modification is not supported, but will be added in future. The member has to be added to properties

    //members initialized in CollisionSystem::InitComponentTm

private:
    physx::PxShape* _shape;
    
    friend class SyCollisionSystem;
};
