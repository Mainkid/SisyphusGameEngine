#include "ColliderComponent.h"

SyPrimitiveColliderComponent::SyPrimitiveColliderComponent(SyPrimitiveColliderType colliderType,
    const SyPrimitiveColliderShapeDesc& colliderShapeDesc, const SyColliderMaterial& material, unsigned flags) :
    _colliderType(colliderType),
    _extent(colliderShapeDesc.Extent),
    _radius(colliderShapeDesc.Radius),
    _halfHeight(colliderShapeDesc.HalfHeight),
    _material(material),
    _flags(flags){ }

SyTrimeshColliderComponent::SyTrimeshColliderComponent(const SyColliderMaterial& material, unsigned flags) :
    _material(material),
    _flags(flags){ };


