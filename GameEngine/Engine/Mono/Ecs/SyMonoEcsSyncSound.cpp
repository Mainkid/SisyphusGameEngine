#include "SyMonoEcsSyncSound.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../Api/SyMonoStr.h"

#include "../../Features/Common/Events/CompAddedEv.h"
#include "../../Features/Common/Events/CompRemovedEv.h"
#include "../../Scene/GameObjectHelper.h"


using namespace mono;

void SyMonoEcsSyncSound::AddComp(entt::entity ent)
{
	_ecs->emplace<FSoundComponent>(ent);
	GameObjectHelper::CallEvent<CompAddedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncSound::RemoveComp(entt::entity ent)
{
	_ecs->remove<FSoundComponent>(ent);
	GameObjectHelper::CallEvent<CompRemovedEv>(_ecs, GetCompId(), ent, true);
}

void SyMonoEcsSyncSound::FillProxyByComp(const FSoundComponent& comp)
{
	_proxy.IsPlaying = comp.IsSoundPlaying;
	_proxy.IsLooping = comp.LoopedSound;
	_proxy.Is3d = comp.SoundType3D;
	_proxy.Volume = comp.SoundVolume;

	if (comp.SoundUuid.is_nil())
	{
		_proxy.SoundUuid = nullptr;
	}
	else
	{
		auto rawSoundUuid = boost::lexical_cast<std::string>(comp.SoundUuid);
		_proxy.SoundUuid = mono_string_new(_monoDomain, rawSoundUuid.c_str());
	}
}

void SyMonoEcsSyncSound::FillCompByProxy(const ProxySoundComp& proxy, entt::entity ent, FSoundComponent& comp)
{
	comp.IsSoundPlaying = proxy.IsPlaying;
	comp.LoopedSound = proxy.IsLooping;
	comp.SoundType3D = proxy.Is3d;
	comp.SoundVolume = proxy.Volume;

	if (proxy.SoundUuid == nullptr)
	{
		comp.SoundUuid = boost::uuids::nil_uuid();
	}
	else
	{
		SyMonoStr rawSoundUuid { proxy.SoundUuid};
		comp.SoundUuid = rawSoundUuid.ToUuid();
	}
}

size_t SyMonoEcsSyncSound::GetHash(const FSoundComponent& comp)
{
	size_t hash = 0;
	boost::hash_combine(hash, comp.IsSoundPlaying);
	boost::hash_combine(hash, comp.LoopedSound);
	boost::hash_combine(hash, comp.SoundType3D);
	boost::hash_combine(hash, comp.SoundVolume);
	boost::hash_combine(hash, comp.SoundUuid);
	return hash;
}
