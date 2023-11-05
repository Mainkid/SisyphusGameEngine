#include "GameObjectHelper.h"

#include "../../../vendor/entt/entt.hpp"
#include "../../Components/GameObjectComp.h"
#include "../../Components/MeshComponent.h"
#include "../../Components/LightComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/ParticleComponent.h"
#include "../../Components/EditorBillboardComponent.h"
#include "../../Components/RBodyComponent.h"
#include "../Systems/TransformHelper.h"
#include "../../Tools/Data/Vector.h"


entt::entity GameObjectHelper::Create(entt::registry* ecs, const std::string& name, Vector3 pos)
{
	auto ent = ecs->create();
	ecs->emplace<GameObjectComp>(ent, name);
	TransformComponent& tc=ecs->emplace<TransformComponent>(ent);
	tc.localPosition = pos;
	return ent;
}


void GameObjectHelper::Destroy(entt::registry* ecs, entt::entity ent)
{
	auto tf = ecs->try_get<TransformComponent>(ent);
	if (tf != nullptr)
	{
		if (tf->children.size() > 0)
		{
			for (auto childEnt : tf->children)
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

SyResult GameObjectHelper::AddRigidBodyComponent(entt::registry* ecs, entt::entity entity, const SyRBodyType& rbType,
	const SyRbTransform& rbLocalTransform, const SyRBodyMaterial& rbMaterial, bool manuallySetMass, float mass)
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
	SyRbTransform rbGlobalTransform;
	rbGlobalTransform.Origin = transformComponent->position + rbLocalTransform.Origin;
	rbGlobalTransform.Rotation = transformComponent->rotation + rbLocalTransform.Rotation;
	ecs->emplace<SyRBodyComponent>(	entity,
									rbType,
									rbGlobalTransform,
									rbMaterial,
									manuallySetMass,
									mass);
	ecs->emplace<SyRbCreateOnNextUpdateTag>(entity);
}


entt::entity GameObjectHelper::CreateStaticBox(	entt::registry* ecs, 
                                                const SyVector3& position, 
                                                const SyVector3& rotation, 
                                                const SyVector3& scale,
                                                const SyRBodyMaterial& material)
{
	auto ent = Create(ecs, "StaticBox");

	ecs->emplace<MeshComponent>(ent);

	auto& tf = ecs->get<TransformComponent>(ent);
	tf.localPosition = position;
	tf.localRotation = rotation;
	tf.localScale = scale;

	SyRbTransform boxDesc;
	boxDesc.RbType = SY_RB_TYPE_STATIC;
	boxDesc.rbShapeType = SY_RB_SHAPE_TYPE_BOX;
	boxDesc.Origin = position;
	boxDesc.Rotation = rotation;
	boxDesc.HalfExtent = scale;
	ecs->emplace<SyRBodyComponent>(ent, boxDesc, material);

	return ent;
}

entt::entity GameObjectHelper::CreateDynamicBox(entt::registry* ecs, 
												const SyVector3& position, 
												const SyVector3& rotation, 
												const SyVector3& scale,
												const SyRBodyMaterial& material)
{
	auto ent = Create(ecs, "DynamicBox");

	ecs->emplace<MeshComponent>(ent);

	auto& tc = ecs->get<TransformComponent>(ent);
	tc.localPosition = position;
	tc.localRotation = rotation;
	tc.localScale = scale;

	SyRbTransform boxDesc;
	boxDesc.RbType = SY_RB_TYPE_DYNAMIC;
	boxDesc.rbShapeType = SY_RB_SHAPE_TYPE_BOX;
	boxDesc.Origin = position;
	boxDesc.Rotation = rotation;
	boxDesc.HalfExtent = scale;
	ecs->emplace<SyRBodyComponent>(ent, boxDesc, material);
	return ent;
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

entt::entity GameObjectHelper::CreateParticleSystem(entt::registry* ecs)
{
	auto ent = Create(ecs, "ParticleObject");
	ecs->emplace<ParticleComponent>(ent);
	return ent;
}