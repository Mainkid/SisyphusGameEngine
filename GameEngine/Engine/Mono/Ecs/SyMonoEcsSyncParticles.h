#pragma once
#include "SyMonoEcsSyncBase.h"

namespace mono
{
	struct ProxyParticlesComp
	{
		struct InputData
		{
			float F;
			ProxyVector2 RandomBetweenConstsF;
			ProxyCurve Curve;
			ProxyVector4 V4;
			ProxyVector4Pair RandomBetweenConstsV;
			EInputType InputType;

			InputData() {}
			InputData(const ParticleInputDataF& data) :
				F(data.Fvalue), RandomBetweenConstsF(data.RandomBetweenConstsF),
				Curve(data.Curve),
				V4(data.V4value), RandomBetweenConstsV(data.RandomBetweenConstsV),
				InputType(data.InputType) {}

			operator ParticleInputDataF() const
			{
				return
				{
					F,
					RandomBetweenConstsF,
					Curve,
					V4,
					RandomBetweenConstsV,
					InputType
				};
			}
		};

		struct BurstData
		{
			float Time;
			float TimeSinceLastBurst;
			InputData Count;
			bool IsLooping;
			float Probability;

			BurstData() {}
			BurstData(const ParticleBurst& data) :
				Time(data.Time), TimeSinceLastBurst(data.TimeSinceLastBurst),
				Count(data.Count), IsLooping(data.IsLooping), Probability(data.Probability) {}

			operator ParticleBurst() const
			{
				return
				{
					Time,
					TimeSinceLastBurst,
					Count,
					IsLooping,
					Probability
				};
			}
		};

		float Duration;
		bool IsLooping;
		InputData StartDelayTime;
		InputData StartLifeTime;
		InputData StartSpeed;
		InputData StartSize;
		InputData StartColor;
		InputData StartRotation;
		InputData SizeOverLifetime;
		InputData SpeedOverLifetime;
		InputData RotationOverLifetime;
		uint32_t MaxParticles;
		bool IsLit;
		float AmbientAmount;

		InputData RateOverTime;
		BurstData* Bursts;
		int BurstsCount;

		EParticleEmitShape ParticleEmitShape;
		float Angle;
		float Radius;

		MonoString* TextureUuid;
	};


	class SyMonoEcsSyncParticles :
		public SyMonoEcsSyncBase<SyMonoEcsSyncParticles, ParticleComponent, ProxyParticlesComp>
	{
	public:
		EProxyCompId GetCompId() const override { return EProxyCompId::Particles; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateParticlesComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateParticlesComp"; }

		void FillProxyByComp(const ParticleComponent& comp) override;

		void ReleaseProxy() override;

		void FillCompByProxy(const ProxyParticlesComp& proxy, entt::entity ent, ParticleComponent& comp) override;

		size_t GetHash(const ParticleComponent& comp) override;
	};
}
