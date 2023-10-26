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


entt::entity GameObjectHelper::CreateStaticBox(entt::registry* ecs, 
                                               const SyVector3& position, 
                                               const SyVector3& rotation, 
                                               const SyVector3& scale)
{
	auto ent = Create(ecs, "StaticBox");

	ecs->emplace<MeshComponent>(ent);

	auto& tf = ecs->get<TransformComponent>(ent);
	tf.localPosition = position;
	tf.localRotation = rotation;
	tf.localScale = scale;

	SyRBodyBoxShapeDesc boxDesc;
	boxDesc.origin = position;
	boxDesc.rotation = rotation;
	boxDesc.halfExt = scale;
	ecs->emplace<SyRBodyComponent>(ent, RB_TYPE_STATIC, RB_SHAPE_TYPE_BOX, boxDesc);

	return ent;
}

entt::entity GameObjectHelper::CreateDynamicBox(entt::registry* ecs, 
	const SyVector3& position, 
	const SyVector3& rotation, 
	const SyVector3& scale)
{
	auto ent = Create(ecs, "DynamicBox");

	ecs->emplace<MeshComponent>(ent);

	auto& tc = ecs->get<TransformComponent>(ent);
	tc.localPosition = position;
	tc.localRotation = rotation;
	tc.localScale = scale;

	SyRBodyBoxShapeDesc boxDesc;
	boxDesc.origin = position;
	boxDesc.rotation = rotation;
	boxDesc.halfExt = scale;
	ecs->emplace<SyRBodyComponent>(ent, RB_TYPE_DYNAMIC, RB_SHAPE_TYPE_BOX, boxDesc);
	return ent;
}

entt::entity GameObjectHelper::CreateLight(entt::registry* ecs, LightType lightType,Vector3 pos)
{
	entt::entity ent;

	if (lightType == LightType::PointLight)
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

	ecs->get<LightComponent>(ent).lightBehavior = LightBehavior::Static;
	ecs->get<LightComponent>(ent).paramsRadiusAndAttenuation = Vector4(1, 0, 0, 1);
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