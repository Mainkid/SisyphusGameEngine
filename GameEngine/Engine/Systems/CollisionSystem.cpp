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

SyResult SyCollisionSystem::Run()
{
	SyResult result;
	if(ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
		return SyResult();
	auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
	if (deltaTime == 0)
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = "EngineContext.deltaTime == 0";
		return result;
	}
	auto toInitView = _ecs->view<SyRBodyComponent, SyPrimitiveColliderComponent, SyColliderCreateOnNextUpdateTag>();

	for (auto& entity : toInitView)
	{
		auto& rbComponent = _ecs->get<SyRBodyComponent>(entity);
		auto& cComponent = _ecs->get<SyPrimitiveColliderComponent>(entity);
		if (InitComponentP(entity, rbComponent, cComponent).code != SY_RESCODE_OK)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize PrimitiveCollider component on entity %d", (int)entity);
			_ecs->remove<SyRBodyComponent>(entity);
		}
		_ecs->remove<SyColliderCreateOnNextUpdateTag>(entity);
	}
	return result;
}

SyResult SyCollisionSystem::Destroy()
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
	//rbComponent._rbActor->detachShape(*rbComponent._rbShape);
	switch (cComponent._colliderType)
    {
    case SY_COL_SHAPE_TYPE_BOX:
		
    	cComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxBoxGeometry(cComponent._extent),
																		pxMaterial);
		shapeVolume = SyMathHelper::CalculateBoxVolume(cComponent._extent);
		break;
    case SY_COL_SHAPE_TYPE_SPHERE:
    	cComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxSphereGeometry(cComponent._radius),
																		pxMaterial);
		shapeVolume = SyMathHelper::CalculateSphereVolume(cComponent._radius);
		break;
    case SY_COL_SHAPE_TYPE_CAPSULE:
    	cComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxCapsuleGeometry(cComponent._radius, cComponent._halfHeight),
																		pxMaterial);
		break;
    default:
    		result.code = SY_RESCODE_ERROR;
    		result.message = xstring("Unknown shape type detected in Collider Component on entity %d", (int)entity);
    		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown shape type detected in Collider Component on entity %d", (int)entity);
    	break;
    }
	bool updateMassResult = true;
	if (!(cComponent._flags & SyColliderFlags::SY_COL_SET_MASS_MANUALLY) && rbComponent._rbType == SY_RB_TYPE_DYNAMIC)
		updateMassResult = PxRigidBodyExt::updateMassAndInertia(	*static_cast<PxRigidBody*>(rbComponent._rbActor),
																	cComponent._material.density);
	if (updateMassResult == false)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::updateMassAndInertia returned false");
		return result;
	}
	return result;
}
