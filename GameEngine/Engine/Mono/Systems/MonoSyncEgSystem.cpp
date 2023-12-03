#include "MonoSyncEgSystem.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>

#include "../SyMono.h"
#include "../../Features/Mesh/Components/MeshComponent.h"
#include "../../Components/TransformComponent.h"
#include "../Helpers/SyMonoHashHelper.h"


SyResult MonoSyncEgSystem::Init()
{
	auto mono = ServiceLocator::instance()->Get<mono::SyMono>();
	_monoEcs = mono->GetEcs();

	return {};
}

SyResult MonoSyncEgSystem::Run()
{
	if (!_monoEcs->IsValid())
		return {};

	SendSceneObjects();
	SendTransforms();
	SendMeshes();
	SendLights();
	SendColliders();
	SendRigids();

	return {};
}

SyResult MonoSyncEgSystem::Destroy()
{
	return {};
}

void MonoSyncEgSystem::SendSceneObjects()
{
	MonoDomain* monoDomain = mono_object_get_domain(_monoEcs->GetInstance());

	mono::ProxySceneObjectComp proxy;

	auto view = _ecs->view<GameObjectComp>();
	for (auto ent : view)
	{
		auto& go = view.get<GameObjectComp>(ent);
		size_t hash = mono::SyMonoHashHelper::Hash(go);
		if (hash == go.MonoHash)
			continue;
		go.MonoHash = hash;

		proxy.Name = mono_string_new(monoDomain, go.Name.c_str());
		proxy.IsActive = true;

		_monoEcs->EgUpdateSceneObjectComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}
}

void MonoSyncEgSystem::SendTransforms()
{
	mono::ProxyTransformComp proxy;

	auto view = _ecs->view<GameObjectComp, TransformComponent>();
	for (auto ent : view)
	{
		auto& tf = view.get<TransformComponent>(ent);
		size_t hash = mono::SyMonoHashHelper::Hash(tf);
		if (hash == tf.MonoHash)
			continue;
		tf.MonoHash = hash;

		proxy.Position = tf._position;
		proxy.Rotation = tf._rotation;
		proxy.Scale = tf.scale;
		proxy.LocalPosition = tf.localPosition;
		proxy.LocalRotation = tf.localRotation;
		proxy.LocalScale = tf.localScale;
		proxy.HasParent = tf.parent != entt::null;
		proxy.ParentEngineEnt = tf.parent;
		_monoEcs->EgUpdateTransformComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}
}

void MonoSyncEgSystem::SendMeshes()
{
	MonoDomain* monoDomain = mono_object_get_domain(_monoEcs->GetInstance());

	mono::ProxyMeshComp proxy;

	auto view = _ecs->view<GameObjectComp, MeshComponent>();
	for (auto ent : view)
	{
		auto& mesh = view.get<MeshComponent>(ent);

		size_t hash = mono::SyMonoHashHelper::Hash(mesh);
		if (hash == mesh.MonoHash)
			continue;
		mesh.MonoHash = hash;

		if (mesh.modelUUID.is_nil())
		{
			proxy.ModelUuid = nullptr;
		}
		else
		{
			auto strModelUuid = boost::lexical_cast<std::string>(mesh.modelUUID);
			proxy.ModelUuid = mono_string_new(monoDomain, strModelUuid.c_str());
		}

		if (mesh.materialUUIDs.size() == 0)
		{
			proxy.MaterialsUuids = nullptr;
		}
		else
		{
			proxy.MaterialsUuids = mono_array_new(monoDomain, mono_get_string_class(), mesh.materialUUIDs.size());
			for (int i = 0; i < mesh.materialUUIDs.size(); i++)
			{
				MonoString* rawMaterialUuid = nullptr;
				if (!mesh.materialUUIDs[i].is_nil())
				{
					auto strMaterialUuid = boost::lexical_cast<std::string>(mesh.materialUUIDs[i]);
					rawMaterialUuid = mono_string_new(monoDomain, strMaterialUuid.c_str());
				}
				mono_array_set(proxy.MaterialsUuids, MonoString*, i, rawMaterialUuid);
			}
		}

		_monoEcs->EgUpdateMeshComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}
}

void MonoSyncEgSystem::SendLights()
{
	mono::ProxyLightComp proxy;

	auto view = _ecs->view<GameObjectComp, LightComponent>();
	for (auto ent : view)
	{
		auto& light = view.get<LightComponent>(ent);

		size_t hash = mono::SyMonoHashHelper::Hash(light);
		if (hash == light.MonoHash)
			continue;
		light.MonoHash = hash;

		proxy.LightType = light.LightType;
		proxy.LightBehavior = light.LightBehavior;
		proxy.Color = light.Color;
		proxy.PointLightRadius = light.ParamsRadiusAndAttenuation.x;
		proxy.ShouldCastShadows = light.CastShadows;

		_monoEcs->EgUpdateLightComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}
}

void MonoSyncEgSystem::SendColliders()
{
	mono::ProxyColliderComp proxy;

	auto view = _ecs->view<GameObjectComp, SyPrimitiveColliderComponent>();
	for (auto ent : view)
	{
		auto& collider = view.get<SyPrimitiveColliderComponent>(ent);

		size_t hash = mono::SyMonoHashHelper::Hash(collider);
		if (hash == collider.MonoHash)
			continue;
		collider.MonoHash = hash;

		proxy.Type = collider.ColliderType;
		proxy.Extent = collider.Extent;
		proxy.Radius = collider.Radius;
		proxy.HalfHeight = collider.HalfHeight;

		_monoEcs->EgUpdateColliderComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}
}

void MonoSyncEgSystem::SendRigids()
{
	mono::ProxyRigidComp proxy;

	auto view = _ecs->view<GameObjectComp, SyRBodyComponent>();
	for (auto ent : view)
	{
		auto& rigid = view.get<SyRBodyComponent>(ent);

		size_t hash = mono::SyMonoHashHelper::Hash(rigid);
		if (hash == rigid.MonoHash)
			continue;
		rigid.MonoHash = hash;

		proxy.Type = rigid.RbType;
		proxy.Mass = rigid.Mass;
		proxy.IsAutoMass = rigid.Flags & SyERBodyFlags::USE_DENSITY;
		proxy.IsKinematic = rigid.Flags & SyERBodyFlags::KINEMATIC;
		proxy.IsGravityOn = (rigid.Flags & SyERBodyFlags::DISABLE_GRAVITY) == 0;
		proxy.LinearVelocity = rigid.LinearVelocity;
		proxy.AngularVelocity = rigid.AngularVelocity;

		_monoEcs->EgUpdateRigidComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}
}