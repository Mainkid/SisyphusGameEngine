#include "SyMonoEcsSyncTransform.h"
#include "../../Engine/Scene/GameObjectHelper.h"
#include "../../Systems/TransformHelper.h"

using namespace mono;


void SyMonoEcsSyncTransform::AddComp(entt::entity ent)
{
	//impossible
}

void SyMonoEcsSyncTransform::RemoveComp(entt::entity ent)
{
	//impossible
}

void SyMonoEcsSyncTransform::FillProxyByComp(const TransformComponent& comp)
{
	_proxy.Position = comp._position;
	_proxy.Rotation = TransformHelper::RadToDegrees(comp._rotation);
	_proxy.Scale = comp.scale;
	_proxy.LocalPosition = comp.localPosition;
	_proxy.LocalRotation = TransformHelper::RadToDegrees(comp.localRotation);
	_proxy.LocalScale = comp.localScale;
	_proxy.HasParent = comp.parent != entt::null;
	_proxy.ParentEngineEnt = comp.parent;
}


void SyMonoEcsSyncTransform::FillCompByProxy(const ProxyTransformComp& proxy, entt::entity ent, TransformComponent& comp)
{
	if (proxy.HasParent)
	{
		if (comp.parent != proxy.ParentEngineEnt)
			GameObjectHelper::SetParent(_ecs, ent, static_cast<entt::entity>(proxy.ParentEngineEnt));
	}
	else
	{
		if (comp.parent != entt::null)
			GameObjectHelper::SetParent(_ecs, ent, entt::null);
	}

	comp._position = proxy.Position;
	comp._rotation = TransformHelper::DegreesToRad(proxy.Rotation);
	comp.scale = proxy.Scale;
	comp.localPosition = proxy.LocalPosition;
	comp.localRotation = TransformHelper::DegreesToRad(proxy.LocalRotation);
	comp.localScale = proxy.LocalScale;
}


size_t SyMonoEcsSyncTransform::GetHash(const TransformComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp._position);
	boost::hash_combine(hash, comp._rotation);
	boost::hash_combine(hash, comp.scale);
	boost::hash_combine(hash, comp.parent);
	boost::hash_combine(hash, comp.localPosition);
	boost::hash_combine(hash, comp.localRotation);
	boost::hash_combine(hash, comp.localScale);
	return hash;
}