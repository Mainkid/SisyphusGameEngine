#include "ColliderComponent.h"

SyPrimitiveColliderComponent::SyPrimitiveColliderComponent(SyEPrimitiveColliderType colliderType,
    const SyPrimitiveColliderShapeDesc& colliderShapeDesc, const SyColliderMaterial& material) :
    ColliderType(colliderType),
    Extent(colliderShapeDesc.Extent),
    Radius(colliderShapeDesc.Radius),
    HalfHeight(colliderShapeDesc.HalfHeight),
    Material(material){ }

SyTrimeshColliderComponent::SyTrimeshColliderComponent(const SyColliderMaterial& material) :
    Material(material){ };


