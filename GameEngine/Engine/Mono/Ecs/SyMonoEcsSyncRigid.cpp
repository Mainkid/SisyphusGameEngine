#include "SyMonoEcsSyncRigid.h"

using namespace mono;

void SyMonoEcsSyncRigid::AddComp(entt::entity ent)
{
	_ecs->emplace<SyRigidBodyComponent>(ent);
}

void SyMonoEcsSyncRigid::RemoveComp(entt::entity ent)
{
	_ecs->remove<SyRigidBodyComponent>(ent);
}

void SyMonoEcsSyncRigid::FillProxyByComp(const SyRigidBodyComponent& comp)
{
	_proxy.Type = comp.RbType;
	_proxy.Mass = comp.Mass;
	_proxy.IsAutoMass = comp.Flags & SyERBodyFlags::USE_DENSITY;
	_proxy.IsKinematic = comp.Flags & SyERBodyFlags::KINEMATIC;
	_proxy.IsGravityOn = (comp.Flags & SyERBodyFlags::DISABLE_GRAVITY) == 0;
	_proxy.LinearVelocity = comp.LinearVelocity;
	_proxy.AngularVelocity = comp.AngularVelocity;
}

void SyMonoEcsSyncRigid::FillCompByProxy(const ProxyRigidComp& proxy, entt::entity ent, SyRigidBodyComponent& comp)
{
	comp.RbType = proxy.Type;
	comp.Mass = proxy.Mass;
	comp.Flags = 0;
	comp.Flags |= proxy.IsAutoMass ? SyERBodyFlags::USE_DENSITY : 0;
	comp.Flags |= proxy.IsKinematic ? SyERBodyFlags::KINEMATIC : 0;
	comp.Flags |= proxy.IsGravityOn ? 0 : SyERBodyFlags::DISABLE_GRAVITY;
	comp.LinearVelocity = proxy.LinearVelocity;
	comp.AngularVelocity = proxy.AngularVelocity;
}

size_t SyMonoEcsSyncRigid::GetHash(const SyRigidBodyComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.RbType);
	boost::hash_combine(hash, comp.Mass);
	boost::hash_combine(hash, comp.Flags);
	boost::hash_combine(hash, comp.LinearVelocity);
	boost::hash_combine(hash, comp.AngularVelocity);
	return hash;
}