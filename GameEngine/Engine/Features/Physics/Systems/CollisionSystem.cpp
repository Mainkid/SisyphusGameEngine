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
#pragma region Primitive Colliders -- Update Geometry
	{
		auto view = _ecs->view<SyPrimitiveColliderComponent>();
		for (auto& entity : view)
		{
			auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
			auto& pColC = _ecs->get<SyPrimitiveColliderComponent>(entity);
			size_t hash = 0;
			boost::hash_combine(hash, pColC);
			if (pColC._hash != hash)
			{
				if (UpdatePrimitiveColliderGeometry(entity, pColC).code == SY_RESCODE_ERROR)
					SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "An error occured during PrimitiveCollider Component update on entity (%s). ", entityName);
				if (ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
					pColC._hash = hash;
			}
		}
	}
#pragma endregion
	if (ServiceLocator::instance()->Get<EngineContext>()->playModeState != EngineContext::EPlayModeState::PlayMode)
		return SyResult();
#pragma region Primitive Colliders -- Update Collider
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
			size_t hash = 0;
			boost::hash_combine(hash, pColC);
			if (pColC._hash != hash || pColC._wasInit == false)
			{
				pColC._hash = hash;
				pColC._wasInit = true;
				if (UpdatePrimitiveCollider(entity, *rigidBCPtr, pColC).code == SY_RESCODE_ERROR)
					SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "An error occured during PrimitiveCollider Component update on entity (%s). ", entityName);
			}
		}
	}
	#pragma endregion
#pragma region Trimesh Colliders -- Update Collider
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
			size_t hash = 0;
			boost::hash_combine(hash, tmColC);
			if (tmColC._hash != hash)
			{
				tmColC._hash = hash;
				if (UpdateTrimeshCollider(entity, *rigidBCPtr, tmColC, *meshCPtr, transformC).code == SY_RESCODE_ERROR)
					SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "An error occured during TrimeshCollider Component update on entity (%s). ", entityName);
			}
				
		}
		
	}
#pragma endregion 
	return result;
}

SyResult SyCollisionSystem::Destroy()
{
	return SyResult();
}

SyResult SyCollisionSystem::UpdatePrimitiveColliderGeometry(const entt::entity& entity, SyPrimitiveColliderComponent& pColC)
{
	SyResult result;
	auto* entityName = SY_GET_ENTITY_NAME_CHAR(_ecs, entity);
	switch (pColC.ColliderType)
	{
	case BOX:
		pColC._colliderGeometry.MakeBox(pColC.Extent);
		break;
	case SPHERE:
		pColC._colliderGeometry.MakeSphere(pColC.Radius, 16 * SyMathHelper::Max(1.0f, pColC.Radius));
		break;
	case CAPSULE:
		pColC._colliderGeometry.MakeCapsule(pColC.Radius, pColC.HalfHeight, 16 * SyMathHelper::Max(1.0f, pColC.Radius));
		break;
	default:
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = xstring("Something was wrong during Collider Component initialization on entity (%s)", entityName);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Unknown shape type detected in Collider Component on entity (%s)", entityName);
		break;
	}
	pColC._colliderGeometry.ResetBuffers();
	return result;
}

SyResult SyCollisionSystem::UpdatePrimitiveCollider(const entt::entity& entity, SyRigidBodyComponent& rigidBC,
                                                    SyPrimitiveColliderComponent& pColC)
{
	SyResult result;
	if (pColC._shape != nullptr)
		pColC._shape->release();
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
		break;
    case SPHERE:
    	pColC._shape = PxRigidActorExt::createExclusiveShape(	*(rigidBC._rbActor),
																		PxSphereGeometry(pColC.Radius),
																		pxMaterial);
		break;
    case CAPSULE:
	    {
		    pColC._shape = PxRigidActorExt::createExclusiveShape(	*(rigidBC._rbActor),
																			PxCapsuleGeometry(pColC.Radius, pColC.HalfHeight),
																			pxMaterial);
    		PxTransform rot;
			rot.p = { 0.0f, 0.0f, 0.0f };
    		rot.q = SyVector3::EulerToPxQuat({0.0f, 0.0f, SyMathHelper::PI / 2});
    		pColC._shape->setLocalPose(rot);
    		break;
	    }
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

SyResult SyCollisionSystem::UpdateTrimeshCollider(const entt::entity& entity, SyRigidBodyComponent& rigidBC,
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
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count           = meshPtr->vertices.size() / Mesh::VECTORS4_PER_VERTEX;
	meshDesc.points.stride          = Mesh::VECTORS4_PER_VERTEX * 4 * sizeof(float);
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
