#include "CollisionSystem.h"
#include "../Components/ColliderComponent.h"
#include "../../../Components/TransformComponent.h"
#include "../Components/RBodyComponent.h"
#include "../../Mesh/Components/MeshComponent.h"
#include "PxPhysicsAPI.h"
#include "RBSystem.h"
#include "../../../Core/Tools/Macro.h"
#include "../../Common/Events/CompRemovedEv.h"
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
	#pragma region Primitive Colliders
	{
		auto view = _ecs->view<SyPrimitiveColliderComponent>();
		for (auto& entity : view)
		{
			auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
			auto* rigidBCPtr = _ecs->try_get<SyRigidBodyComponent>(entity);
			if (rigidBCPtr == nullptr)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity (%s) is missing the RigidBody Component. RigidBody Component has been added. ", entityName);
				_ecs->emplace<SyRigidBodyComponent>(entity);
				continue;
			}
			auto& pColC = _ecs->get<SyPrimitiveColliderComponent>(entity);
			if (pColC._wasInit == false)
				InitComponentP(entity, *rigidBCPtr, pColC);
		}
	}
	#pragma endregion
#pragma region Trimesh Colliders
	{
		auto view = _ecs->view<SyTrimeshColliderComponent>();
		for (auto& entity : view)
		{
			auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
			auto* meshCPtr = _ecs->try_get<MeshComponent>(entity);
			if (meshCPtr == nullptr)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity (%s) is missing the Mesh Component. Hence, you can't attach a collider component to it. The collider component has been removed. ", entityName);
				_ecs->remove<SyTrimeshColliderComponent>(entity);
				CallEvent<CompRemovedEv>(ECompId::TriMeshCollider, entity, false);
				continue;
			}
			auto* rigidBCPtr = _ecs->try_get<SyRigidBodyComponent>(entity);
			if (rigidBCPtr == nullptr)
			{
				SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity (%s) is missing the RigidBody Component. RigidBody Component has been added. ", entityName);
				_ecs->emplace<SyRigidBodyComponent>(entity);
				continue;
			}
			auto& tmColC = _ecs->get<SyTrimeshColliderComponent>(entity);
			auto& transformC = _ecs->get<TransformComponent>(entity);
			if (tmColC._wasInit == false)
				InitComponentTm(entity, *rigidBCPtr, tmColC, *meshCPtr, transformC);
		}
		
	}
	auto view = _ecs->view<SyPrimitiveColliderComponent>();
	for (auto& entity : view)
	{
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

SyResult SyCollisionSystem::InitComponentP(const entt::entity& entity, SyRigidBodyComponent& rigidBC,
                                           SyPrimitiveColliderComponent& pColC)
{
	SyResult result;
	auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
	auto& pxMaterial = *GET_PHYSICS_CONTEXT->Physics->createMaterial(	pColC.Material.staticFriction,
																	pColC.Material.dynamicFriction,
																	pColC.Material.restitution);
	switch (pColC.ColliderType)
    {
    case BOX:
		
    	pColC._shape = PxRigidActorExt::createExclusiveShape(	*(rigidBC._rbActor),
																		PxBoxGeometry(pColC.Extent),
																		pxMaterial);
		pColC._colliderGeometry.MakeBox(pColC.Extent);
		break;
    case SPHERE:
    	pColC._shape = PxRigidActorExt::createExclusiveShape(	*(rigidBC._rbActor),
																		PxSphereGeometry(pColC.Radius),
																		pxMaterial);
		pColC._colliderGeometry.MakeSphere(pColC.Radius, 16 * SyMathHelper::Max(1.0f, pColC.Radius));
		break;
    case CAPSULE:
    	pColC._shape = PxRigidActorExt::createExclusiveShape(	*(rigidBC._rbActor),
																		PxCapsuleGeometry(pColC.Radius, pColC.HalfHeight),
																		pxMaterial);
		pColC._colliderGeometry.MakeCapsule(pColC.Radius, pColC.HalfHeight, 16 * SyMathHelper::Max(1.0f, pColC.Radius));
		break;
    default:
    		result.code = SY_RESCODE_UNEXPECTED;
    		result.message = xstring("Something was wrong during Collider Component initialization on entity (%s)", entityName);
    		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown shape type detected in Collider Component on entity (%s)", entityName);
    	break;
    }
	if (pColC._shape == nullptr)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Failed to create shape on entity (%s). Used default shape instead.", entityName);
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = xstring("Something was wrong during Collider Component initialization on entity (%s)", entityName);
		auto& defaultPxMaterial = *GET_PHYSICS_CONTEXT->Physics->createMaterial(	SyColliderMaterial().staticFriction,
																					SyColliderMaterial().dynamicFriction,
																					SyColliderMaterial().restitution);
		pColC._shape = pColC._shape = PxRigidActorExt::createExclusiveShape(	*(rigidBC._rbActor),
																				PxBoxGeometry(SyVector3::ONE),
																				defaultPxMaterial);
		pColC._colliderGeometry.MakeBox(SyVector3::ONE);
	}
	bool updateMassResult = true;
	if (rigidBC.Flags & SyERBodyFlags::USE_DENSITY && rigidBC.RbType == DYNAMIC)
		updateMassResult = PxRigidBodyExt::updateMassAndInertia(	*static_cast<PxRigidBody*>(rigidBC._rbActor),
																	pColC.Material.density);
	if (updateMassResult == false)
	{
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidBodyExt::updateMassAndInertia returned false for PrimitiveCollider Component on entity (%s). Perhaps, you used invalid density value. Default density value was used instead.",
			entityName);
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = xstring("Something was wrong during Collider Component initialization on entity (%s)", entityName);
		PxRigidBodyExt::updateMassAndInertia(	*static_cast<PxRigidBody*>(rigidBC._rbActor),
																	SyColliderMaterial().density);
	}
	return result;
}

SyResult SyCollisionSystem::InitComponentTm(const entt::entity& entity, SyRigidBodyComponent& rigidBC,
                                            SyTrimeshColliderComponent& tmColC, const MeshComponent& meshC, const TransformComponent& transformC)
{
	SyResult result;
	auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
	if (!(meshC.flags & SyEMeshComponentFlags::MESH_COLLIDER))
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = xstring("You can't use mesh from Mesh Component on entity (%s) unless it's marked with flag MESH_COLLIDER. ", entityName);
		SY_LOG_PHYS(SY_LOGLEVEL_WARNING, "You can't use mesh from Mesh Component on entity (%s) unless it's marked with flag MESH_COLLIDER. ", entityName);
		return result;
	}
	if (!(rigidBC.Flags & SyERBodyFlags::KINEMATIC))
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = xstring("You can't create TrimeshCollider Component on entity (%s) unless it's marked with flag KINEMATIC. ", entityName);
		SY_LOG_PHYS(SY_LOGLEVEL_WARNING, "You can't create TrimeshCollider Component on entity (%s) unless it's marked with flag KINEMATIC. ", entityName);
		return result;
	}
	auto& pxMaterial = *GET_PHYSICS_CONTEXT->Physics->createMaterial(	tmColC.Material.staticFriction,
																	tmColC.Material.dynamicFriction,
																	tmColC.Material.restitution);
	auto meshPtr = meshC.model->meshes[0];
	const unsigned NUM_VECTORS_PER_VERTEX = 5;
X:	PxTriangleMeshDesc meshDesc;
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
 		result.message = xstring("PhysX failed to cook mesh in SyTrimeshCollider Component on entity (%s)", entityName);
 		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PhysX failed to cook mesh in SyTrimeshCollider Component on entity (%s)", entityName);
 		return result;
 	}
	PxMeshScale scale(transformC.localScale, PxQuat(PxIdentity));
 	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
 	PxTriangleMesh* trimeshPtr = GET_PHYSICS_CONTEXT->Physics->createTriangleMesh(readBuffer);
	PxTriangleMeshGeometry trimeshGeometry = PxTriangleMeshGeometry(trimeshPtr, scale);
 	tmColC._shape = PxRigidActorExt::createExclusiveShape(*(rigidBC._rbActor),
 														trimeshGeometry,
 														pxMaterial);
 	if (tmColC._shape == nullptr)
 	{
 		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "PxRigidActorExt::createExclusiveShape returned nullptr in TrimeshCollider Component on entity %d.", (int)entity);
 		result.code = SY_RESCODE_ERROR;
 		result.message = xstring("PxRigidActorExt::createExclusiveShape returned nullptr in TrimeshCollider Component on entity %d.", (int)entity);
 		return result;
 	}
	return result;
}
