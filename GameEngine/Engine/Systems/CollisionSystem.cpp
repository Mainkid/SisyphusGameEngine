#include "CollisionSystem.h"
#include "../Components/ColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RBodyComponent.h"
#include "../Components/MeshComponent.h"
#include "PxPhysicsAPI.h"
#include "RBSystem.h"

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
	auto toInitPView = _ecs->view<SyRBodyComponent, SyPrimitiveColliderComponent, SyColliderCreateOnNextUpdateTag>();
	for (auto& entity : toInitPView)
	{
		auto& rbComponent = _ecs->get<SyRBodyComponent>(entity);
		auto& cComponent = _ecs->get<SyPrimitiveColliderComponent>(entity);
		if (InitComponentP(entity, rbComponent, cComponent).code != SY_RESCODE_OK)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize PrimitiveCollider component on entity %d", (int)entity);
			_ecs->remove<SyPrimitiveColliderComponent>(entity);
		}
		_ecs->remove<SyColliderCreateOnNextUpdateTag>(entity);
	}
	auto toInitTmView = _ecs->view<SyRBodyComponent, SyTrimeshColliderComponent, SyColliderCreateOnNextUpdateTag>();
	for (auto& entity : toInitTmView)
	{
		auto& rbComponent = _ecs->get<SyRBodyComponent>(entity);
		auto& cComponent = _ecs->get<SyTrimeshColliderComponent>(entity);
		auto& mComponent = _ecs->get<MeshComponent>(entity);
		auto& tComponent = _ecs->get<TransformComponent>(entity);
		if (InitComponentTm(entity, rbComponent, cComponent, mComponent, tComponent).code != SY_RESCODE_OK)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize TrimeshComponent component on entity %d", (int)entity);
			_ecs->remove<SyTrimeshColliderComponent>(entity);
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
	switch (cComponent._colliderType)
    {
    case SY_COL_SHAPE_TYPE_BOX:
		
    	cComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxBoxGeometry(cComponent._extent),
																		pxMaterial);
		break;
    case SY_COL_SHAPE_TYPE_SPHERE:
    	cComponent._rbShape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxSphereGeometry(cComponent._radius),
																		pxMaterial);
		
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
	if (rbComponent._flags & SyRBodyFlags::SY_RB_USE_DENSITY && rbComponent._rbType == SY_RB_TYPE_DYNAMIC)
		updateMassResult = PxRigidBodyExt::updateMassAndInertia(	*static_cast<PxRigidBody*>(rbComponent._rbActor),
																	cComponent._material.density);
	if (updateMassResult == false)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::updateMassAndInertia returned false");
		return result;
	}
	return result;
}

SyResult SyCollisionSystem::InitComponentTm(const entt::entity& entity, SyRBodyComponent& rbComponent,
                                            SyTrimeshColliderComponent& cComponent, const MeshComponent& mComponent, const TransformComponent& tComponent)
{
	SyResult result;
	auto& pxMaterial = *SyRBodyComponent::_physics->createMaterial(	cComponent._material.staticFriction,
																	cComponent._material.dynamicFriction,
																	cComponent._material.restitution);
	 for (const auto& meshPtr : mComponent.model->meshes)
	 {
	 	const unsigned NUM_VECTORS_PER_VERTEX = 5;
	 	PxTriangleMeshDesc meshDesc;
	 	meshDesc.points.count           = meshPtr->vertices.size() / NUM_VECTORS_PER_VERTEX;
	 	meshDesc.points.stride          = NUM_VECTORS_PER_VERTEX * 4 * sizeof(float);
	 	meshDesc.points.data            = (void*)(std::addressof(*meshPtr->vertices.begin()));
	 	
	 	meshDesc.triangles.count        = meshPtr->indices.size() / 3;
	 	meshDesc.triangles.stride       = 3 * sizeof(int);
	 	meshDesc.triangles.data         = (void*)(std::addressof(*meshPtr->indices.begin()));
	
	 	PxTolerancesScale toleranceScale;
	 	PxCookingParams params(toleranceScale);
	
	 	PxDefaultMemoryOutputStream writeBuffer;
	 	PxTriangleMeshCookingResult::Enum cookingResult;
	 	bool cookingStatus = PxCookTriangleMesh(params, meshDesc, writeBuffer, &cookingResult);
	 	if(!cookingStatus)
	 	{
	 		result.code = SY_RESCODE_ERROR;
	 		result.message = xstring("PhysX failed to cook mesh in SyTrimeshCollider Component on entity %d", (int)entity);
	 		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PhysX failed to cook mesh in SyTrimeshCollider Component on entity %d", (int)entity);
	 		return result;
	 	}
		PxMeshScale scale(tComponent.localScale, PxQuat(PxIdentity));
	 	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	 	PxTriangleMesh* trimeshPtr = rbComponent._physics->createTriangleMesh(readBuffer);
		PxTriangleMeshGeometry trimeshGeometry = PxTriangleMeshGeometry(trimeshPtr, scale);
	 	cComponent._shape = PxRigidActorExt::createExclusiveShape(*(rbComponent._rbActor),
	 														trimeshGeometry,
	 														pxMaterial);
	 	if (cComponent._shape == nullptr)
	 	{
	 		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidActorExt::createExclusiveShape returned nullptr in TrimeshCollider Component on entity %d.", (int)entity);
	 		result.code = SY_RESCODE_ERROR;
	 		result.message = xstring("PxRigidActorExt::createExclusiveShape returned nullptr in TrimeshCollider Component on entity %d.", (int)entity);
	 		return result;
	 	}
	 }

	return result;
}
