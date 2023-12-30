#include "MonoSyncEgSystem.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>

#include "../SyMono.h"
#include "../../Components/SkyboxComponent.h"
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
	SendSkyboxes();
	SendParticles();

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

void MonoSyncEgSystem::SendSkyboxes()
{
	MonoDomain* monoDomain = mono_object_get_domain(_monoEcs->GetInstance());

	mono::ProxySkyboxComp proxy;

	auto view = _ecs->view<GameObjectComp, SkyboxComponent>();
	for (auto ent : view)
	{
		auto& skybox = view.get<SkyboxComponent>(ent);

		size_t hash = mono::SyMonoHashHelper::Hash(skybox);
		if (hash == skybox.MonoHash)
			continue;
		skybox.MonoHash = hash;

		if (skybox.uuid.is_nil())
		{
			proxy.CubemapUuid = nullptr;
		}
		else
		{
			auto strModelUuid = boost::lexical_cast<std::string>(skybox.uuid);
			proxy.CubemapUuid = mono_string_new(monoDomain, strModelUuid.c_str());
		}

		_monoEcs->EgUpdateSkyboxComp.Invoke(static_cast<uint32_t>(ent), proxy);
	}
}

void MonoSyncEgSystem::SendParticles()
{
	MonoDomain* monoDomain = mono_object_get_domain(_monoEcs->GetInstance());

	mono::ProxyParticlesComp proxy;
	
	auto view = _ecs->view<GameObjectComp, ParticleComponent>();
	for (auto ent : view)
	{
		auto& particles = view.get<ParticleComponent>(ent);
		if (!particles.IsMonoDirty)
			continue;
		particles.IsMonoDirty = false;
		
		proxy.Duration = particles.Duration;
		proxy.IsLooping = particles.IsLooping;
		proxy.StartDelayTime = particles.StartDelayTime;
		proxy.StartLifeTime = particles.StartLifeTime;
		proxy.StartSpeed = particles.StartSpeed;
		proxy.StartSize = particles.StartSize;
		proxy.StartColor = particles.StartColor;
		proxy.StartRotation = particles.StartRotation;
		proxy.SizeOverLifetime = particles.SizeOverLifetime;
		proxy.SpeedOverLifetime = particles.SpeedOverLifetime;
		proxy.RotationOverLifetime = particles.RotationOverLifetime;
		proxy.MaxParticles = particles.MaxParticles;
		proxy.IsLit = particles.IsLit;
		proxy.AmbientAmount = particles.AmbientAmount;
		
		proxy.RateOverTime = particles.RateOverTime;

		if (particles.ParticleBursts.size() == 0)
		{
			proxy.BurstsCount = 0;
			proxy.Bursts = nullptr;
		}
		else
		{
			proxy.BurstsCount = (int)particles.ParticleBursts.size();
			proxy.Bursts = new mono::ProxyParticlesComp::BurstData[proxy.BurstsCount];
			for (int i = 0; i < proxy.BurstsCount; i++)
				proxy.Bursts[i] = particles.ParticleBursts[i];
		}
		
		proxy.ParticleEmitShape = particles.ParticleEmitShape;
		proxy.Angle = particles.Angle;
		proxy.Radius = particles.Radius;

		auto rawTextureUuid = boost::lexical_cast<std::string>(particles.TextureUuid);
		proxy.TextureUuid = mono_string_new(monoDomain, rawTextureUuid.data());

		proxy.IsDirty = false;

		_monoEcs->EgUpdateParticlesComp.Invoke(static_cast<uint32_t>(ent), proxy);

		delete[] proxy.Bursts;
	}
}
