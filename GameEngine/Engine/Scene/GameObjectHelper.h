#pragma once

#include "../../../vendor/entt/entt.hpp"
#include "../Tools/Data/Vector.h"
#include "../../Components/LightComponent.h"

class GameObjectHelper
{
public:
	static entt::entity Create(entt::registry* ecs, const std::string& name);

	static void Destroy(entt::registry* ecs, entt::entity ent);

	static void SetParent(entt::registry* ecs, entt::entity child, entt::entity parent);


	static entt::entity CreateStaticBox(entt::registry* ecs,
		const SyVector3& position,
		const SyVector3& rotation = SyVector3(), 
		const SyVector3& scale = { 1.0f, 1.0f, 1.0f }
	);

	static entt::entity CreateDynamicBox(entt::registry* ecs, 
		const SyVector3& position, 
		const SyVector3& rotation = SyVector3(), 
		const SyVector3& scale = { 1.0f, 1.0f, 1.0f }
	);

	static entt::entity CreateLight(entt::registry* ecs, LightType lightType);

	static entt::entity CreateParticleSystem(entt::registry* ecs);
};