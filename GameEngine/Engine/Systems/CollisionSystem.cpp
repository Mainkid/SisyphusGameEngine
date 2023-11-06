#include "CollisionSystem.h"
#include "../Components/ColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RBodyComponent.h"
#include "PxPhysicsAPI.h"
#include "RBSystem.h"
#include "../Tools/MathHelper.h"

using namespace physx;

SyResult SyCollisionSystem::Init()
{
	return SyResult();
}

SyResult SyCollisionSystem::InitComponentP(const entt::entity& entity, SyRBodyComponent& rbComponent,
                                           SyPrimitiveColliderComponent& cComponent)
{
	SyResult result;
	auto& pxMaterial = *SyRBodyComponent::_physics->createMaterial(	cComponent._material.staticFriction,
																	cComponent._material.dynamicFriction,
																	cComponent._material.restitution);
	float shapeVolume = 0.0f;
	rbComponent._rbActor->detachShape(*rbComponent._rbShape);
	rbComponent._rbShape->release();
	switch (cComponent._colliderType)
    {
    case SY_RB_SHAPE_TYPE_BOX:
    	rbComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxBoxGeometry(cComponent._extent),
																		pxMaterial);
		shapeVolume = SyMathHelper::CalculateBoxVolume(cComponent._extent);
		break;
    case SY_RB_SHAPE_TYPE_SPHERE:
    	rbComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxSphereGeometry(cComponent._radius),
																		pxMaterial);
		shapeVolume = SyMathHelper::CalculateSphereVolume(cComponent._radius);
		break;
    case SY_RB_SHAPE_TYPE_CAPSULE:
    	rbComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxCapsuleGeometry(cComponent._radius, cComponent._halfHeight),
																		pxMaterial);
		shapeVolume = SyMathHelper::CalculateCapsuleVolume(cComponent._radius, cComponent._halfHeight);
		break;
    default:
    		result.code = SY_RESCODE_ERROR;
    		result.message = xstring("Unknown shape type detected in Collider Component on entity %d", (int)entity);
    		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown shape type detected in Collider Component on entity %d", (int)entity);
    	break;
    }
	if (cComponent._flags & SyColliderFlags::SY_COL_SET_MASS_MANUALLY)
		cComponent._material.density = shapeVolume / rbComponent._mass;
	if (rbComponent._rbType == SY_RB_TYPE_DYNAMIC)
	{
		bool updateMassResult = PxRigidBodyExt::updateMassAndInertia(	*static_cast<PxRigidBody*>(rbComponent._rbActor),
																		cComponent._material.density);
		if (updateMassResult == false)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::updateMassAndInertia returned false");
			return result;
		}
	}
	return result;
}
