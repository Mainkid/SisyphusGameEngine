#include "GameObjectHelper.h"

#include "../../../vendor/entt/entt.hpp"
#include "../Features/Physics/Components/RBodyComponent.h"
#include "../Features/Physics/Events/SyOnCreateRBodyEvent.h"
#include "../Features/Physics/Events/SyOnCreateColliderEvent.h"
#include "../Features/Mesh/Components/MeshComponent.h"
#include "../Features/Lighting/Components/LightComponent.h"
#include "../../Components/TransformComponent.h"
#include "../Features/Particles/Components/ParticleComponent.h"
#include "../../Components/EditorBillboardComponent.h"
#include "../Systems/TransformHelper.h"
#include "../Core/Tools/Structures/Vector.h"
#include "../Components/ImageBasedLightingComponent.h"
#include "../Components/SkyboxComponent.h"
#include "../Features/Resources/ResourceService.h"


entt::entity GameObjectHelper::Create(entt::registry* ecs, const std::string& name, SyVector3 pos = Vector3::Zero)
{
	auto ent = ecs->create();
	ecs->emplace<GameObjectComp>(ent, name);
	TransformComponent& tc=ecs->emplace<TransformComponent>(ent);
	tc._position = pos;
	return ent;
}


void GameObjectHelper::Destroy(entt::registry* ecs, entt::entity ent)
{
	auto tf = ecs->try_get<TransformComponent>(ent);
	if (tf != nullptr)
	{
		if (tf->children.size() > 0)
		{
			std::set<entt::entity> tmpSet = tf->children;
			for (auto childEnt : tmpSet)
				Destroy(ecs, childEnt);
		}
		SetParent(ecs, ent, entt::null);
	}
	ecs->destroy(ent);
}

void GameObjectHelper::SetParent(entt::registry* ecs, entt::entity child, entt::entity parent)
{
	/*auto childTf = ecs->try_get<TransformComponent>(child);
	if (childTf == nullptr)
		return;

	if (childTf->parent != entt::null)
	{
		auto prevParentTf = ecs->try_get<TransformComponent>(static_cast<entt::entity>(childTf->parent));
		if (prevParentTf != nullptr)
			prevParentTf->children.erase(child);
		childTf->parent = entt::null;
	}

	if (parent == entt::null)
		return;

	auto parentTf = ecs->try_get<TransformComponent>(parent);
	if (parentTf == nullptr)
		return;
	parentTf->children.insert(child);

	childTf->parent = static_cast<uint32_t>(parent);*/


	if (parent == entt::null)
	{
		if (ecs->get<TransformComponent>(child).parent != entt::null) 
		{

			RemoveChild(ecs,static_cast<entt::entity>(ecs->get<TransformComponent>(child).parent), child);
		}
		
		TransformHelper::UpdateRelativeToParent(nullptr, ecs->get<TransformComponent>(child));
		ecs->get<TransformComponent>(child).parent = entt::null;
	}
	else if (!TransformHelper::HasHierarchyCycles(ecs,child, parent))
	{

		if (ecs->get<TransformComponent>(child).parent != entt::null)
		{

			RemoveChild(ecs,static_cast<entt::entity>(ecs->get<TransformComponent>(child).parent), child);

		}
		TransformHelper::UpdateRelativeToParent(nullptr,ecs->get<TransformComponent>(child));
		ecs->get<TransformComponent>(child).parent = static_cast<uint32_t>(parent);
		ecs->get<TransformComponent>(parent).children.insert(child);
		TransformHelper::UpdateRelativeToParent(ecs->try_get<TransformComponent>(parent),
			ecs->get<TransformComponent>(child));
	}
	/*TransformHelper::UpdateRelativeToParent(registry.try_get<TransformComponent>(parentGameObject),
		registry.get<TransformComponent>(sourceGameObject));*/
}

void GameObjectHelper::AddChild(entt::registry* ecs, entt::entity parent, entt::entity child)
{
	SetParent(ecs,child, parent);
}

void GameObjectHelper::RemoveChild(entt::registry* ecs, entt::entity parent, entt::entity child)
{
	ecs->get<TransformComponent>(parent).children.erase(child);
}

SyResult GameObjectHelper::AddRigidBodyComponent(entt::registry* ecs, entt::entity entity, const SyERBodyType& rbType, float mass, unsigned flags)
{
	SyResult result;
	auto* transformComponent = ecs->try_get<TransformComponent>(entity);
	if (transformComponent == nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Entity %d lacks Transform Component. You can't attach RigidBody Component to it.", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Entity %d lacks Transform Component. You can't attach RigidBody Component to it.", (int)entity);
		return result;
	}
	ecs->emplace<SyRBodyComponent>(	entity,
									rbType,
									mass,
									flags);
	CallEvent<SyOnCreateRBodyEvent>(ecs, "OnCreateRBody", entity);
	//ecs->emplace<SyRbCreateOnNextUpdateTag>(entity);
	return result;
}

SyResult GameObjectHelper::AddPrimitiveColliderComponent(entt::registry* ecs, entt::entity entity,
	SyEPrimitiveColliderType colliderType, const SyPrimitiveColliderShapeDesc& colliderShapeDesc,
	const SyColliderMaterial& material)
{
	SyResult result;
	auto* rbComponent = ecs->try_get<SyRBodyComponent>(entity);
	if (rbComponent == nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Could not find RigidBody Component on entity (%d). Hence, you can't attach a collider component to it. ", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Could not find RigidBody Component on entity (%d). Hence, you can't attach a collider component to it. ", (int)entity);
		return result;
	}
	auto* pComponent = ecs->try_get<SyPrimitiveColliderComponent>(entity);
	auto* tmComponent = ecs->try_get<SyTrimeshColliderComponent>(entity);
	if (pComponent != nullptr || tmComponent != nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Could not find RigidBody Component on entity (%d). Hence, you can't attach a collider component to it. ", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Could not find RigidBody Component on entity (%d). Hence, you can't attach a collider component to it. ", (int)entity);
		return result;
	}
	ecs->emplace<SyPrimitiveColliderComponent>(entity, colliderType, colliderShapeDesc,  material);
	CallEvent<SyOnCreateColliderEvent>(ecs, "OnCreateCollider", entity);
	return result;
}

SyResult GameObjectHelper::AddTrimeshColliderComponent(entt::registry* ecs, entt::entity entity,
	const SyColliderMaterial& material)
{
	SyResult result;
	auto* rbComponent = ecs->try_get<SyRBodyComponent>(entity);
	if (rbComponent == nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Could not find RigidBody Component on entity (%d). Hence, you can't attach a collider component to it. ", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Could not find RigidBody Component on entity (%d). Hence, you can't attach a collider component to it. ", (int)entity);
		return result;
	}
	auto* pComponent = ecs->try_get<SyPrimitiveColliderComponent>(entity);
	auto* tmComponent = ecs->try_get<SyTrimeshColliderComponent>(entity);
	if (pComponent != nullptr || tmComponent != nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("A collider component is already attached to the entity (%d). You can't attach more than one collider to an entity. ", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "A collider component is already attached to the entity (%d). You can't attach more than one collider to an entity. ", (int)entity);
		return result;
	}
	auto* mComponent = ecs->try_get<MeshComponent>(entity);
	if (mComponent == nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Could not find Mesh Component on entity (%d). Hence, you can't attach TrimeshCollider Component to it. ", (int)entity);
		SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Could not find Mesh Component on entity (%d). Hence, you can't attach TrimeshCollider Component to it. ", (int)entity);
		return result;
	}
	ecs->emplace<SyTrimeshColliderComponent>(entity, material);
	CallEvent<SyOnCreateColliderEvent>(ecs, "OnCreateCollider", entity);
	return result;
}


entt::entity GameObjectHelper::CreateLight(entt::registry* ecs, ELightType lightType,Vector3 pos)
{
	entt::entity ent;

	if (lightType == ELightType::PointLight)
	{
		ent = Create(ecs, "PointLight",pos);
		ecs->emplace<EditorBillboardComponent>(ent, "Engine/Assets/Sprites/PointLightSprite.png");
		
	}
	else
	{
		ent = Create(ecs, "DirectionalLight",pos);
		ecs->emplace<EditorBillboardComponent>(ent, "Engine/Assets/Sprites/DirLightSprite.png");
	}
	ecs->emplace<LightComponent>(ent, lightType);

	ecs->get<LightComponent>(ent).LightBehavior = LightBehavior::Static;
	ecs->get<LightComponent>(ent).ParamsRadiusAndAttenuation = Vector4(1, 0, 0, 1);
	return ent;
}

entt::entity GameObjectHelper::CreateMesh(entt::registry* ecs, boost::uuids::uuid uuid, Vector3 pos)
{
	auto entt = Create(ecs,"Mesh",pos);
	MeshComponent& mesh = ecs->emplace<MeshComponent>(entt);
	mesh.modelUUID = uuid;
	return entt::entity();
}

SyResult GameObjectHelper::AddMeshComponent(entt::registry* ecs, entt::entity entity, boost::uuids::uuid uuid, unsigned flags)
{
	MeshComponent& mesh = ecs->emplace<MeshComponent>(entity, uuid, flags);
	return SyResult();
}

SyResult GameObjectHelper::AddCubeMeshComponent(entt::registry* ecs, entt::entity entity)
{
	return AddMeshComponent(ecs, entity, ServiceLocator::instance()->Get<ResourceService>()->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\Cube.fbx"));
}

SyResult GameObjectHelper::AddSphereMeshComponent(entt::registry* ecs, entt::entity entity)
{
	return AddMeshComponent(ecs, entity, ServiceLocator::instance()->Get<ResourceService>()->GetUUIDFromPath(".\\Engine\\Assets\\Resources\\sphere.fbx"));
}

entt::entity GameObjectHelper::CreateParticleSystem(entt::registry* ecs)
{
	//TODO: Translate to resource service!
	auto ent = Create(ecs, "ParticleSystem");
	ParticleComponent& pc = ecs->emplace<ParticleComponent>(ent);
	pc.SharedParticlesDataUuid = ServiceLocator::instance()->Get<ResourceService>()->baseResourceDB[EAssetType::ASSET_PARTICLESYS].uuid;
	return ent;
}

entt::entity GameObjectHelper::CreateSkybox(entt::registry* ecs, boost::uuids::uuid uuid)
{
	auto ent = ecs->create();
	ecs->emplace<GameObjectComp>(ent);
	ecs->emplace<SkyboxComponent>(ent);
	ecs->emplace<ImageBasedLightingComponent>(ent);
	
	if (uuid == boost::uuids::nil_uuid())
		uuid = ServiceLocator::instance()->Get<ResourceService>()->baseResourceDB[EAssetType::ASSET_CUBEMAP].uuid;
	ecs->get<SkyboxComponent>(ent).uuid = uuid;

	return ent;
}
