#pragma once


#include "../../../vendor/entt/entt.hpp"
#include "../Core/Tools/Structures/Vector.h"
#include "../Components/Components.h"
#include "../Core/Tools/ErrorLogger.h"
//#include "../Core/ECS/Event.h"
#include "../Core/Tools/Structures/Vector.h"
#include "../Features/Lighting/Components/LightComponent.h"
#include "../../Components/GameObjectComp.h"
#include "../Features/Physics/Components/RBodyComponent.h"
#include "../Features/Physics/Components/ColliderComponent.h"
#include "../Features/Mesh/Components/MeshComponent.h"
#include "../Core/Tools/ErrorLogger.h"
#include "../Features/Events/Tags/SyEventTag.h"

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
											const SyERBodyType&		rbType,
											float					mass			= 1.0,
											unsigned				flags			= 0);
	static SyResult AddPrimitiveColliderComponent(	entt::registry*						ecs,
													entt::entity						entity,
													SyEPrimitiveColliderType				colliderType,
													const SyPrimitiveColliderShapeDesc& colliderShapeDesc,
													const SyColliderMaterial& 			material = SyColliderMaterial());
	static SyResult AddTrimeshColliderComponent(	entt::registry*						ecs,
													entt::entity						entity,
													const SyColliderMaterial& 			material = SyColliderMaterial());
	
	static entt::entity CreateLight(entt::registry* ecs, ELightType lightType, Vector3 pos = Vector3::Zero);
	static entt::entity CreateSoundComponent(entt::registry* ecs);//, ESoundType type = ESoundType::Sound3D, float volume = 1.0f, bool isoplay = false, bool ispooping = false, std::string soundPath = "Engine/Assets/Audio/LookinAtIt.ogg", Vector3 pos = Vector3::Zero);
	static entt::entity CreateMesh(entt::registry* ecs, boost::uuids::uuid uuid, Vector3 pos=Vector3::Zero);
	static SyResult AddMeshComponent(entt::registry* ecs, entt::entity entity, boost::uuids::uuid uuid, unsigned flags = SyEMeshComponentFlags::MESH_RENDER);
	static SyResult AddCubeMeshComponent(entt::registry* ecs, entt::entity entity);
	static SyResult AddSphereMeshComponent(entt::registry* ecs, entt::entity entity);
	static SyResult AddAnimatorComponent(entt::registry* ecs, entt::entity entity);
	static entt::entity CreateParticleSystem(entt::registry* ecs);
	static entt::entity CreateCamera(entt::registry* ecs);

	static entt::entity CreateSkybox(entt::registry* ecs,boost::uuids::uuid uuid = boost::uuids::nil_uuid());

	template <typename T_Event, typename ... Args>
	static SyResult CallEvent(entt::registry* ecs, const std::string& name, Args... eventArgs);

	static void CopyEntity(entt::registry* ecs, std::set<entt::entity> entitiesToCopy, entt::entity parent = entt::null)
	{
		std::set<entt::entity> tmp;

		std::function<void(entt::entity)> childFinder;
		childFinder = [&](entt::entity entity)
			{
				TransformComponent& tc = ecs->get<TransformComponent>(entity);
				for (auto& child : tc.children)
				{
					tmp.insert(child);
					childFinder(child);
				}
			};

		
		for (auto& ent : entitiesToCopy)
		{
			tmp.insert(ent);
			childFinder(ent);
		}
		entitiesToCopy = tmp;

		std::unordered_map<entt::entity, entt::entity> oldToNewEnts;
		

		for (auto& ent : entitiesToCopy)
		{
			TransformComponent& tc = ecs->get<TransformComponent>(ent);
			//if (entitiesToCopy.contains(static_cast<entt::entity>(tc.parent)))
			//	continue;

			const auto newEnt=ecs->create();
			ComponentCopy(COMPONENTS_TYPELIST, ecs, ent, newEnt);
			oldToNewEnts[ent]=newEnt;
		}

		for (auto& ent : entitiesToCopy)
		{
			const auto newEnt = oldToNewEnts[ent];
			TransformComponent& tcOld = ecs->get<TransformComponent>(ent);
			TransformComponent& tcNew = ecs->get<TransformComponent>(newEnt);

			if (entitiesToCopy.contains(static_cast<entt::entity>(tcOld.parent)))
			{
				GameObjectHelper::SetParent(ecs, newEnt, oldToNewEnts[static_cast<entt::entity>(tcNew.parent)]);
			}
			else
			{
				GameObjectHelper::SetParent(ecs, newEnt, parent);
			}

		}

	}

	template<typename Head, typename... Tail>
	static void ComponentCopy(Typelist<Head, Tail...>& t,entt::registry* ecs, entt::entity from, entt::entity to)
	{
		Head* component=ecs->try_get<Head>(from);
		if (component)
		{
			ecs->emplace<Head>(to,*component);
		}
		Typelist<Tail...> remainingComps;
		ComponentCopy(remainingComps, ecs, from, to);		
	}

	template<typename Head>
	static void ComponentCopy(Typelist<Head>& t, entt::registry* ecs, entt::entity from, entt::entity to)
	{
		Head* component = ecs->try_get<Head>(from);
		if (component)
		{
			ecs->emplace<Head>(to, *component);
		}
	}

	
	
};

//Calls the event to be listened to NEXT FRAME. Events are only to be listened to in Runtime (it is possible to listen to events in Init(), but it relies on order of systems update). Use the macros from SystemBase.h to listen to event!
template <typename T_Event, typename ... Args>
SyResult GameObjectHelper::CallEvent(entt::registry* ecs, const std::string& name, Args... eventArgs)
{
	entt::entity ent = ecs->create();
	//ecs->emplace<GameObjectComp>(ent, name);
	auto c = ecs->emplace<T_Event>(ent, eventArgs...);
	ecs->emplace<SyEventTag>(ent);
	return SyResult();
}

