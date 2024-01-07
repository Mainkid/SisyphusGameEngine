#include "CollisionSystem.h"
#include "../Components/ColliderComponent.h"
#include "../../../Components/TransformComponent.h"
#include "../Components/RBodyComponent.h"
#include "../../Mesh/Components/MeshComponent.h"
#include "..\Events\SyOnCreateCollider.h"
#include "PxPhysicsAPI.h"
#include "RBSystem.h"

using namespace physx;

SyResult SyCollisionSystem::Init()
{
	return SyResult();
}

SyResult SyCollisionSystem::Run()
{
	OPTICK_EVENT();
	SyResult result;
	auto deltaTime = ServiceLocator::instance()->Get<EngineContext>()->deltaTime;
	if (deltaTime == 0)
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = "EngineContext.deltaTime == 0";
		return result;
	}
	auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyOnCreateColliderEvent);
	for (auto& eventEntity : eventView)
	{
		auto& entity = _ecs->get<SyOnCreateColliderEvent>(eventEntity).Entity;
		auto* rbComponent = _ecs->try_get<SyRigidBodyComponent>(entity);
		if (rbComponent == nullptr)
		{
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity (%d) is missing the RigidBody Component. Hence, you can't attach a collider component to it. The collider component has been removed. ", (int)entity);
			_ecs->remove<SyTrimeshColliderComponent>(entity);
		}
		auto* pComponent = _ecs->try_get<SyPrimitiveColliderComponent>(entity);
		auto* tmComponent = _ecs->try_get<SyTrimeshColliderComponent>(entity);
		if (pComponent != nullptr)
		{
			if (tmComponent != nullptr)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "A collider component is already attached to the entity (%d). You can't attach more than one collider to an entity. ", (int)entity);
				_ecs->remove<SyTrimeshColliderComponent>(entity);
				continue;
			}
			if (InitComponentP(entity, *rbComponent, *pComponent).code != SY_RESCODE_OK)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize PrimitiveCollider Component on entity (%d). The PrimitiveCollider Component has been removed.", (int)entity);
				_ecs->remove<SyPrimitiveColliderComponent>(entity);
			}
		}
		else if (tmComponent != nullptr)
		{
			auto* mComponent = _ecs->try_get<MeshComponent>(entity);
			if (mComponent == nullptr)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity (%d) is missing the Mesh Component. Hence, you can't attach a collider component to it. The collider component has been removed. ", (int)entity);
				_ecs->remove<SyTrimeshColliderComponent>(entity);
				continue;
			}
			auto* tComponent = _ecs->try_get<TransformComponent>(entity);
			if (tComponent == nullptr)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity (%d) is missing the Transform Component. Hence, you can't attach a collider component to it. The collider component has been removed. ", (int)entity);
				_ecs->remove<SyTrimeshColliderComponent>(entity);
				continue;
			}
			if (InitComponentTm(entity, *rbComponent, *tmComponent, *mComponent, *tComponent).code != SY_RESCODE_OK)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to initialize TrimeshComponent Component on entity (%d). The TrimeshComponent Component has been removed.", (int)entity);
				_ecs->remove<SyTrimeshColliderComponent>(entity);
			}
		}
		else
			SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "No collider components have been detected on entity (%d). Something went wrong. ", (int)entity);
	}
	return result;
}

SyResult SyCollisionSystem::Destroy()
{
	return SyResult();
}

SyResult SyCollisionSystem::InitComponentP(const entt::entity& entity, SyRigidBodyComponent& rbComponent,
                                           SyPrimitiveColliderComponent& cComponent)
{
	SyResult result;
	auto& pxMaterial = *GET_PHYSICS_CONTEXT->Physics->createMaterial(	cComponent.Material.staticFriction,
																	cComponent.Material.dynamicFriction,
																	cComponent.Material.restitution);
	switch (cComponent.ColliderType)
    {
    case BOX:
		
    	cComponent._shape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxBoxGeometry(cComponent.Extent),
																		pxMaterial);
		cComponent._colliderGeometry.MakeBox(cComponent.Extent);
		break;
    case SPHERE:
    	cComponent._shape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxSphereGeometry(cComponent.Radius),
																		pxMaterial);
		cComponent._colliderGeometry.MakeSphere(cComponent.Radius, 16 * SyMathHelper::Max(1.0f, cComponent.Radius));
		break;
    case CAPSULE:
    	cComponent._shape = PxRigidActorExt::createExclusiveShape(	*(rbComponent._rbActor),
																		PxCapsuleGeometry(cComponent.Radius, cComponent.HalfHeight),
																		pxMaterial);
		cComponent._colliderGeometry.MakeCapsule(cComponent.Radius, cComponent.HalfHeight, 16 * SyMathHelper::Max(1.0f, cComponent.Radius));
		break;
    default:
    		result.code = SY_RESCODE_ERROR;
    		result.message = xstring("Unknown shape type detected in Collider Component on entity %d", (int)entity);
    		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown shape type detected in Collider Component on entity %d", (int)entity);
    	break;
    }
	bool updateMassResult = true;
	if (rbComponent.Flags & SyERBodyFlags::USE_DENSITY && rbComponent.RbType == DYNAMIC)
		updateMassResult = PxRigidBodyExt::updateMassAndInertia(	*static_cast<PxRigidBody*>(rbComponent._rbActor),
																	cComponent.Material.density);
	if (updateMassResult == false)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::updateMassAndInertia returned false");
		return result;
	}
	return result;
}

SyResult SyCollisionSystem::InitComponentTm(const entt::entity& entity, SyRigidBodyComponent& rbComponent,
                                            SyTrimeshColliderComponent& cComponent, const MeshComponent& mComponent, const TransformComponent& tComponent)
{
	SyResult result;
	if (!(mComponent.flags & SyEMeshComponentFlags::MESH_COLLIDER))
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("You can't use mesh from Mesh Component (entity %d) unless it's marked with flag MESH_COLLIDER. ", (int) entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "You can't use mesh from Mesh Component (entity %d) unless it's marked with flag MESH_COLLIDER. ", (int)entity);
		return result;
	}
	if (!(rbComponent.Flags & SyERBodyFlags::KINEMATIC))
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("You can't create TrimeshCollider Component on entity (%d) unless it's marked with flag KINEMATIC. ", (int) entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "You can't create TrimeshCollider Component on entity (%d) unless it's marked with flag KINEMATIC. ", (int)entity);
		return result;
	}
	auto& pxMaterial = *GET_PHYSICS_CONTEXT->Physics->createMaterial(	cComponent.Material.staticFriction,
																	cComponent.Material.dynamicFriction,
																	cComponent.Material.restitution);
	auto meshPtr = mComponent.model->meshes[0];
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
 	PxTriangleMesh* trimeshPtr = GET_PHYSICS_CONTEXT->Physics->createTriangleMesh(readBuffer);
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
	return result;
}
