#pragma once

#include "../../../vendor/entt/entt.hpp"
#include "../Tools/Data/Vector.h"
#include "../../Components/LightComponent.h"
#include "../Components/RBodyComponent.h"
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
											const SyRbTransform&	rbLocalTransform = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },
											float					mass			= 1.0);
	
	static entt::entity CreateStaticBox(entt::registry* ecs,
										const SyVector3& position,
										const SyVector3& rotation = SyVector3(), 
										const SyVector3& scale = { 1.0f, 1.0f, 1.0f });

	static entt::entity CreateDynamicBox(	entt::registry* ecs, 
											const SyVector3& position, 
											const SyVector3& rotation = SyVector3(), 
											const SyVector3& scale = { 1.0f, 1.0f, 1.0f });

	static entt::entity CreateLight(entt::registry* ecs, ELightType lightType, Vector3 pos = Vector3::Zero);

	static entt::entity CreateMesh(entt::registry* ecs, boost::uuids::uuid uuid, Vector3 pos=Vector3::Zero);

	static entt::entity CreateParticleSystem(entt::registry* ecs);
};
