#include "SyMonoEcsSyncParticles.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../Api/SyMonoStr.h"

using namespace mono;

void SyMonoEcsSyncParticles::AddComp(entt::entity ent)
{
	auto& particles = _ecs->emplace<ParticleComponent>(ent);
	particles.IsMonoDirty = true;
}

void SyMonoEcsSyncParticles::RemoveComp(entt::entity ent)
{
	_ecs->remove<ParticleComponent>(ent);
}

void SyMonoEcsSyncParticles::FillProxyByComp(const ParticleComponent& comp)
{
	_proxy.Duration = comp.Duration;
	_proxy.IsLooping = comp.IsLooping;
	_proxy.StartDelayTime = comp.StartDelayTime;
	_proxy.StartLifeTime = comp.StartLifeTime;
	_proxy.StartSpeed = comp.StartSpeed;
	_proxy.StartSize = comp.StartSize;
	_proxy.StartColor = comp.StartColor;
	_proxy.StartRotation = comp.StartRotation;
	_proxy.SizeOverLifetime = comp.SizeOverLifetime.Curve;
	_proxy.SpeedOverLifetime = comp.SpeedOverLifetime.Curve;
	_proxy.RotationOverLifetime = comp.RotationOverLifetime;
	_proxy.MaxParticles = comp.MaxParticles;
	_proxy.IsLit = comp.IsLit;
	_proxy.AmbientAmount = comp.AmbientAmount;

	_proxy.RateOverTime = comp.RateOverTime;

	if (comp.ParticleBursts.size() == 0)
	{
		_proxy.BurstsCount = 0;
		_proxy.Bursts = nullptr;
	}
	else
	{
		_proxy.BurstsCount = (int)comp.ParticleBursts.size();
		_proxy.Bursts = new mono::ProxyParticlesComp::BurstData[_proxy.BurstsCount];
		for (int i = 0; i < _proxy.BurstsCount; i++)
			_proxy.Bursts[i] = comp.ParticleBursts[i];
	}

	_proxy.ParticleEmitShape = comp.ParticleEmitShape;
	_proxy.Angle = comp.Angle;
	_proxy.Radius = comp.Radius;

	auto rawTextureUuid = boost::lexical_cast<std::string>(comp.TextureUuid);
	_proxy.TextureUuid = mono_string_new(_monoDomain, rawTextureUuid.data());
}

void SyMonoEcsSyncParticles::ReleaseProxy()
{
	delete[] _proxy.Bursts;
}

void SyMonoEcsSyncParticles::FillCompByProxy(
	const ProxyParticlesComp& proxy, 
	entt::entity ent, 
	ParticleComponent& comp)
{
	comp.Duration = proxy.Duration;
	comp.IsLooping = proxy.IsLooping;
	comp.StartDelayTime = proxy.StartDelayTime;
	comp.StartLifeTime = proxy.StartLifeTime;
	comp.StartSpeed = proxy.StartSpeed;
	comp.StartSize = proxy.StartSize;
	comp.StartColor = proxy.StartColor;
	comp.StartRotation = proxy.StartRotation;
	comp.SizeOverLifetime.Curve = proxy.SizeOverLifetime;
	comp.SpeedOverLifetime.Curve = proxy.SpeedOverLifetime;
	comp.RotationOverLifetime = proxy.RotationOverLifetime;
	comp.MaxParticles = proxy.MaxParticles;
	comp.IsLit = proxy.IsLit;
	comp.AmbientAmount = proxy.AmbientAmount;

	comp.RateOverTime = proxy.RateOverTime;

	comp.ParticleBursts.clear();
	for (int i = 0; i < proxy.BurstsCount; i++)
		comp.ParticleBursts.push_back(proxy.Bursts[i]);

	comp.ParticleEmitShape = proxy.ParticleEmitShape;
	comp.Angle = proxy.Angle;
	comp.Radius = proxy.Radius;

	SyMonoStr rawTextureUuid{ proxy.TextureUuid };
	comp.TextureUuid = rawTextureUuid.ToUuid();

	comp.IsMonoDirty = false;
}

size_t SyMonoEcsSyncParticles::GetHash(const ParticleComponent& comp)
{
	return comp.IsMonoDirty ? 1 : 0;
}
