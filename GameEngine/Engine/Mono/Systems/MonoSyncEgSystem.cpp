#include "MonoSyncEgSystem.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>

#include "../SyMono.h"
#include "../../Features/Mesh/Components/MeshComponent.h"
#include "../Components/MonoSyncComp.h"
#include "../../Components/TransformComponent.h"


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

	SendTransforms();
	SendMeshes();

	return {};
}

SyResult MonoSyncEgSystem::Destroy()
{
	return {};
}

void MonoSyncEgSystem::SendTransforms()
{
	mono::ProxyTransformComp proxy;

	auto view = _ecs->view<MonoSyncComp, TransformComponent>();
	for (auto ent : view)
	{
		auto& tf = view.get<TransformComponent>(ent);
		size_t hash = hash_value(tf);
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

	auto view = _ecs->view<MonoSyncComp, MeshComponent>();
	for (auto ent : view)
	{
		auto& mesh = view.get<MeshComponent>(ent);

		size_t hash = hash_value(mesh);
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
