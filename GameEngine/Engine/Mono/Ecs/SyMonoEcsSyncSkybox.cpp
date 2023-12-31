#include "SyMonoEcsSyncSkybox.h"

#include "../Api/SyMonoStr.h"
#include "../../Components/ImageBasedLightingComponent.h"
#include "../../Features/Resources/ResourceService.h"

using namespace mono;

void SyMonoEcsSyncSkybox::AddComp(entt::entity ent)
{
	auto resService = ServiceLocator::instance()->Get<ResourceService>();
	_ecs->emplace<SkyboxComponent>(ent).uuid = resService->baseResourceDB[EAssetType::ASSET_CUBEMAP].uuid;
	_ecs->emplace<ImageBasedLightingComponent>(ent);
}

void SyMonoEcsSyncSkybox::RemoveComp(entt::entity ent)
{
	_ecs->remove<SkyboxComponent>(ent);
	_ecs->remove<ImageBasedLightingComponent>(ent);
}

void SyMonoEcsSyncSkybox::FillProxyByComp(const SkyboxComponent& comp)
{
	if (comp.uuid.is_nil())
	{
		_proxy.CubemapUuid = nullptr;
	}
	else
	{
		auto strModelUuid = boost::lexical_cast<std::string>(comp.uuid);
		_proxy.CubemapUuid = mono_string_new(_monoDomain, strModelUuid.c_str());
	}
}

void SyMonoEcsSyncSkybox::FillCompByProxy(const ProxySkyboxComp& proxy, entt::entity ent, SkyboxComponent& comp)
{
	if (proxy.CubemapUuid == nullptr)
	{
		auto resService = ServiceLocator::instance()->Get<ResourceService>();
		comp.uuid = resService->baseResourceDB[EAssetType::ASSET_CUBEMAP].uuid;
	}
	else
	{
		SyMonoStr strUuid{ proxy.CubemapUuid };
		comp.uuid = strUuid.ToUuid();
	}
}

size_t SyMonoEcsSyncSkybox::GetHash(const SkyboxComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.uuid);
	return hash;
}