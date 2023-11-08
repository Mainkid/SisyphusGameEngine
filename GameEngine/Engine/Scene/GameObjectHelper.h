#pragma once

#include "../../../vendor/entt/entt.hpp"
#include "../Tools/Data/Vector.h"
#include "../../Components/LightComponent.h"
#include "../Components/RBodyComponent.h"
#include "../Components/ColliderComponent.h"
#include "../Tools/ErrorLogger.h"

class GameObjectHelper
{
public:
	static entt::entity Create(entt::registry* ecs, const std::string& name, SyVector3 pos);

	static void Destroy(entt::registry* ecs, entt::entity ent);

	static void SetParent(entt::registry* ecs, entt::entity child, entt::entity parent);

	static void AddChild(entt::registry* ecs, entt::entity child, entt::entity parent);

	static void RemoveChild(entt::registry* ecs, entt::entity child, entt::entity parent);

	static SyResult AddRigidBodyComponent(	entt::registry*			ecs,
											entt::entity			entity,
											const SyRBodyType&		rbType,
											float					mass			= 1.0);
	static SyResult AddPrimitiveColliderComponent(	entt::registry*						ecs,
													entt::entity						entity,
													SyPrimitiveColliderType				colliderType,
													const SyPrimitiveColliderShapeDesc& colliderShapeDesc,
													const SyColliderMaterial& 			material = SyColliderMaterial(),
													unsigned							flags = 0);
	static SyResult AddTrimeshColliderComponent(	entt::registry*						ecs,
													entt::entity						entity,
													const SyColliderMaterial& 			material = SyColliderMaterial(),
													unsigned							flags = 0);
	
	static entt::entity CreateLight(entt::registry* ecs, ELightType lightType, Vector3 pos = Vector3::Zero);

	static entt::entity CreateMesh(entt::registry* ecs, boost::uuids::uuid uuid, Vector3 pos=Vector3::Zero);
	static SyResult AddMeshComponent(entt::registry* ecs, entt::entity entity, boost::uuids::uuid uuid); 
	static SyResult AddCubeMeshComponent(entt::registry* ecs, entt::entity entity);
	static entt::entity CreateParticleSystem(entt::registry* ecs);

	static entt::entity CreateSkybox(entt::registry* ecs,boost::uuids::uuid uuid = boost::uuids::nil_uuid());

	
};
