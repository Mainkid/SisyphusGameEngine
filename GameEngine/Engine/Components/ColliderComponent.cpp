#include "ColliderComponent.h"

SyPrimitiveColliderComponent::SyPrimitiveColliderComponent(SyEPrimitiveColliderType colliderType,
    const SyPrimitiveColliderShapeDesc& colliderShapeDesc, const SyColliderMaterial& material) :
    _colliderType(colliderType),
    _extent(colliderShapeDesc.Extent),
    _radius(colliderShapeDesc.Radius),
    _halfHeight(colliderShapeDesc.HalfHeight),
    _material(material){ }

SyTrimeshColliderComponent::SyTrimeshColliderComponent(const SyColliderMaterial& material) :
    _material(material){ };


