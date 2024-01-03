#include "SyMonoEcsSyncHingeJoint.h"

using namespace mono;

void SyMonoEcsSyncHingeJoint::AddComp(entt::entity ent)
{
	_ecs->emplace<SyHingeJointComponent>(ent);
}

void SyMonoEcsSyncHingeJoint::RemoveComp(entt::entity ent)
{
	_ecs->remove<SyHingeJointComponent>(ent);
}

void SyMonoEcsSyncHingeJoint::FillProxyByComp(const SyHingeJointComponent& comp)
{
	_proxy.IsAttachedToEnt = comp.OtherEntity != entt::null;
	_proxy.AttachedToEngineEnt = static_cast<uint32_t>(comp.OtherEntity);
	_proxy.PivotPos = comp.LocalPosition;
	_proxy.PivotRot = comp.LocalRotation;
}

void SyMonoEcsSyncHingeJoint::FillCompByProxy(
	const ProxyHingeJointComp& proxy, 
	entt::entity ent,
	SyHingeJointComponent& comp)
{
	comp.OtherEntity = proxy.IsAttachedToEnt
		? static_cast<entt::entity>(proxy.AttachedToEngineEnt)
		: entt::null;
	comp.LocalPosition = proxy.PivotPos;
	comp.LocalRotation = proxy.PivotRot;
}

size_t SyMonoEcsSyncHingeJoint::GetHash(const SyHingeJointComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.OtherEntity);
	boost::hash_combine(hash, comp.LocalPosition);
	boost::hash_combine(hash, comp.LocalRotation);
	return hash;
}
