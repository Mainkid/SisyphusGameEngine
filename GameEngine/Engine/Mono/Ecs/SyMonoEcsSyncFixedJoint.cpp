#include "SyMonoEcsSyncFixedJoint.h"

#include "../../Features/Common/Events/CompAddedEv.h"
#include "../../Features/Common/Events/CompRemovedEv.h"
#include "../../Scene/GameObjectHelper.h"

using namespace mono;

void SyMonoEcsSyncFixedJoint::AddComp(entt::entity ent)
{
	_ecs->emplace<SyFixedJointComponent>(ent);
	GameObjectHelper::CallEvent<CompAddedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncFixedJoint::RemoveComp(entt::entity ent)
{
	_ecs->remove<SyFixedJointComponent>(ent);
	GameObjectHelper::CallEvent<CompRemovedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncFixedJoint::FillProxyByComp(const SyFixedJointComponent& comp)
{
	_proxy.IsAttachedToEnt = comp.OtherEntity != entt::null;
	_proxy.AttachedToEnt = static_cast<uint32_t>(comp.OtherEntity);
}

void SyMonoEcsSyncFixedJoint::FillCompByProxy(
	const ProxyFixedJointComp& proxy, 
	entt::entity ent,
	SyFixedJointComponent& comp)
{
	comp.OtherEntity = proxy.IsAttachedToEnt
		? static_cast<entt::entity>(proxy.AttachedToEnt)
		: entt::null;
}

size_t SyMonoEcsSyncFixedJoint::GetHash(const SyFixedJointComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.OtherEntity);
	return hash;
}
