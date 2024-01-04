#include "SyMonoEcsSyncLight.h"

#include "../../Features/Common/Events/CompAddedEv.h"
#include "../../Features/Common/Events/CompRemovedEv.h"
#include "../../Scene/GameObjectHelper.h"

using namespace mono;

void SyMonoEcsSyncLight::AddComp(entt::entity ent)
{
	auto& light = _ecs->emplace<LightComponent>(ent);
	light.ParamsRadiusAndAttenuation = Vector4{ 1, 0, 0, 1 };

	GameObjectHelper::CallEvent<CompAddedEv>(_ecs, "Mono", GetCompId(), ent, true);
}

void SyMonoEcsSyncLight::RemoveComp(entt::entity ent)
{
	_ecs->remove<LightComponent>(ent);
	GameObjectHelper::CallEvent<CompRemovedEv>(_ecs, "Mono", GetCompId(), ent, true);
}

void SyMonoEcsSyncLight::FillProxyByComp(const LightComponent& comp)
{
	_proxy.LightType = comp.LightType;
	_proxy.LightBehavior = comp.LightBehavior;
	_proxy.Color = comp.Color;
	_proxy.PointLightRadius = comp.ParamsRadiusAndAttenuation.x;
	_proxy.ShouldCastShadows = comp.CastShadows;
}

void SyMonoEcsSyncLight::FillCompByProxy(const ProxyLightComp& proxy, entt::entity ent, LightComponent& comp)
{
	comp.LightType = proxy.LightType;
	comp.LightBehavior = proxy.LightBehavior;
	comp.Color = proxy.Color;
	comp.ParamsRadiusAndAttenuation.x = proxy.PointLightRadius;
	comp.CastShadows = proxy.ShouldCastShadows;
}

size_t SyMonoEcsSyncLight::GetHash(const LightComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.LightType);
	boost::hash_combine(hash, comp.LightBehavior);
	boost::hash_combine(hash, comp.Color.x);
	boost::hash_combine(hash, comp.Color.y);
	boost::hash_combine(hash, comp.Color.z);
	boost::hash_combine(hash, comp.Color.w);
	boost::hash_combine(hash, comp.ParamsRadiusAndAttenuation.x);
	boost::hash_combine(hash, comp.ParamsRadiusAndAttenuation.y);
	boost::hash_combine(hash, comp.ParamsRadiusAndAttenuation.z);
	boost::hash_combine(hash, comp.ParamsRadiusAndAttenuation.w);
	boost::hash_combine(hash, comp.CastShadows);
	return hash;
}