#include "SyMonoEcsSyncMesh.h"

#include "../../Features/Resources/ResourceService.h"
#include "../Api/SyMonoStr.h"

#include "../../Features/Common/Events/CompAddedEv.h"
#include "../../Features/Common/Events/CompRemovedEv.h"
#include "../../Scene/GameObjectHelper.h"

using namespace mono;

void SyMonoEcsSyncMesh::AddComp(entt::entity ent)
{
	auto resService = ServiceLocator::instance()->Get<ResourceService>();
	auto uuid = resService->baseResourceDB[EAssetType::ASSET_MESH].uuid;
	_ecs->emplace<MeshComponent>(ent).modelUUID = uuid;

	GameObjectHelper::CallEvent<CompAddedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncMesh::RemoveComp(entt::entity ent)
{
	_ecs->remove<MeshComponent>(ent);
	GameObjectHelper::CallEvent<CompRemovedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncMesh::FillProxyByComp(const MeshComponent& comp)
{
	if (comp.modelUUID.is_nil())
	{
		_proxy.ModelUuid = nullptr;
	}
	else
	{
		auto strModelUuid = boost::lexical_cast<std::string>(comp.modelUUID);
		_proxy.ModelUuid = mono_string_new(_monoDomain, strModelUuid.c_str());
	}

	if (comp.materialUUIDs.size() == 0)
	{
		_proxy.MaterialsUuids = nullptr;
	}
	else
	{
		_proxy.MaterialsUuids = mono_array_new(_monoDomain, mono_get_string_class(), comp.materialUUIDs.size());
		for (int i = 0; i < comp.materialUUIDs.size(); i++)
		{
			MonoString* rawMaterialUuid = nullptr;
			if (!comp.materialUUIDs[i].is_nil())
			{
				auto strMaterialUuid = boost::lexical_cast<std::string>(comp.materialUUIDs[i]);
				rawMaterialUuid = mono_string_new(_monoDomain, strMaterialUuid.c_str());
			}
			mono_array_set(_proxy.MaterialsUuids, MonoString*, i, rawMaterialUuid);
		}
	}
}

void SyMonoEcsSyncMesh::FillCompByProxy(const ProxyMeshComp& proxy, entt::entity ent, MeshComponent& comp)
{
	if (proxy.ModelUuid == nullptr)
	{
		auto resService = ServiceLocator::instance()->Get<ResourceService>();
		auto uuid = resService->baseResourceDB[EAssetType::ASSET_MESH].uuid;
		comp.modelUUID = uuid;
	}
	else
	{
		SyMonoStr strModelUuid{ proxy.ModelUuid };
		comp.modelUUID = strModelUuid.ToUuid();
	}

	comp.materialUUIDs.clear();
	if (proxy.MaterialsUuids != nullptr)
	{
		int length = mono_array_length(proxy.MaterialsUuids);
		for (int i = 0; i < length; i++)
		{
			MonoString* rawMaterialUuid = mono_array_get(proxy.MaterialsUuids, MonoString*, i);
			if (rawMaterialUuid != nullptr)
			{
				SyMonoStr strMaterialUuid{ rawMaterialUuid };
				comp.materialUUIDs.push_back(strMaterialUuid.ToUuid());
			}
		}
	}
}

size_t SyMonoEcsSyncMesh::GetHash(const MeshComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.modelUUID);
	boost::hash_combine(hash, comp.materialUUIDs);
	return hash;
}