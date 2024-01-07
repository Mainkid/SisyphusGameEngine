#include "SyMonoEcsSyncAnimator.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../../Events/SyAnimatorComponentAdded.h"
#include "../../Features/Common/Events/CompAddedEv.h"
#include "../../Features/Common/Events/CompRemovedEv.h"
#include "../../Scene/GameObjectHelper.h"
#include "../Api/SyMonoStr.h"

using namespace mono;

void SyMonoEcsSyncAnimator::AddComp(entt::entity ent)
{
	_ecs->emplace<AnimatorComponent>(ent);
	GameObjectHelper::CallEvent<CompAddedEv>(_ecs, ECompId::Animator, ent, true);
	GameObjectHelper::CallEvent<SyAnimatorComponentAdded>(_ecs, ent);
}

void SyMonoEcsSyncAnimator::RemoveComp(entt::entity ent)
{
	_ecs->remove<AnimatorComponent>(ent);
	GameObjectHelper::CallEvent<CompRemovedEv>(_ecs, ECompId::Animator, ent, true);
}

void SyMonoEcsSyncAnimator::FillProxyByComp(const AnimatorComponent& comp)
{
	if (comp.animationUUID.is_nil())
	{
		_proxy.AnimationUuid = nullptr;
	}
	else
	{
		auto rawAnimationUuid = boost::lexical_cast<std::string>(comp.animationUUID);
		_proxy.AnimationUuid = mono_string_new(_monoDomain, rawAnimationUuid.c_str());
	}
	_proxy.IsLooping = comp.IsLooping;
}

void SyMonoEcsSyncAnimator::FillCompByProxy(
	const ProxyAnimatorComp& proxy,
	entt::entity ent,
	AnimatorComponent& comp)
{
	if (proxy.AnimationUuid == nullptr)
	{
		comp.animationUUID = boost::uuids::nil_uuid();
	}
	else
	{
		SyMonoStr rawAnimationUuid{ proxy.AnimationUuid };
		comp.animationUUID = rawAnimationUuid.ToUuid();
	}
	comp.IsLooping = proxy.IsLooping;
}

size_t SyMonoEcsSyncAnimator::GetHash(const AnimatorComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.animationUUID);
	boost::hash_combine(hash, comp.IsLooping);
	return hash;
}
