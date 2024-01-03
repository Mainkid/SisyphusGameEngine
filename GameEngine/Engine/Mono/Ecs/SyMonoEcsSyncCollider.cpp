#include "SyMonoEcsSyncCollider.h"

using namespace mono;


void SyMonoEcsSyncCollider::AddComp(entt::entity ent)
{
	_ecs->emplace<SyPrimitiveColliderComponent>(ent);
}

void SyMonoEcsSyncCollider::RemoveComp(entt::entity ent)
{
	_ecs->remove<SyPrimitiveColliderComponent>(ent);
}

void SyMonoEcsSyncCollider::FillProxyByComp(const SyPrimitiveColliderComponent& comp)
{
	_proxy.Type = comp.ColliderType;
	_proxy.Extent = comp.Extent;
	_proxy.Radius = comp.Radius;
	_proxy.HalfHeight = comp.HalfHeight;
}

void SyMonoEcsSyncCollider::FillCompByProxy(
	const ProxyColliderComp& proxy, entt::entity ent,
	SyPrimitiveColliderComponent& comp)
{
	comp.ColliderType = proxy.Type;
	comp.Extent = proxy.Extent;
	comp.Radius = proxy.Radius;
	comp.HalfHeight = proxy.HalfHeight;
}

size_t SyMonoEcsSyncCollider::GetHash(const SyPrimitiveColliderComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.ColliderType);
	boost::hash_combine(hash, comp.Extent);
	boost::hash_combine(hash, comp.Radius);
	boost::hash_combine(hash, comp.HalfHeight);
	return hash;
}