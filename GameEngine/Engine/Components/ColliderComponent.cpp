#include "ColliderComponent.h"

SyPrimitiveColliderComponent::SyPrimitiveColliderComponent(SyPrimitiveColliderType colliderType,
    const SyPrimitiveColliderShapeDesc& colliderShapeDesc, const SyColliderMaterial& material, unsigned flags) :
    _colliderType(colliderType),
    _extent(colliderShapeDesc.HalfExtent),
    _radius(colliderShapeDesc.Radius),
    _halfHeight(colliderShapeDesc.HalfHeight),
    _material(material),
    _flags(flags){ };


