#pragma once
#include "SyMonoEcsSyncBase.h"

namespace mono
{
	struct ProxyParticlesComp
	{
		struct InputFloat
		{
			bool IsRange;
			float Value;
			float From;
			float To;

			InputFloat() {}
			InputFloat(const ParticleInputDataF& input) :
				IsRange(input.InputType == RandomBetweenFloats),
				Value(input.Fvalue),
				From(input.RandomBetweenConstsF.f1),
				To(input.RandomBetweenConstsF.f2) {}
			operator ParticleInputDataF() const
			{
				ParticleInputDataF data;
				data.InputType = IsRange ? RandomBetweenFloats : SimpleFloat;
				data.Fvalue = Value;
				data.RandomBetweenConstsF.f1 = From;
				data.RandomBetweenConstsF.f2 = To;
				return data;
			}
		};

		struct InputColor
		{
			bool IsRange;
			ProxyVector4 Value;
			ProxyVector4 From;
			ProxyVector4 To;

			InputColor() {}
			InputColor(const ParticleInputDataF& input) :
				IsRange(input.InputType == RandomBetweenVectors),
				Value(input.V4value),
				From(input.RandomBetweenConstsV.vec1),
				To(input.RandomBetweenConstsV.vec2) {}
			operator ParticleInputDataF() const
			{
				ParticleInputDataF data;
				data.InputType = IsRange ? RandomBetweenVectors : SimpleVector;
				data.V4value = Value;
				data.RandomBetweenConstsV.vec1 = From;
				data.RandomBetweenConstsV.vec2 = To;
				return data;
			}
		};

		struct BurstData
		{
			float Time;
			float TimeSinceLastBurst;
			InputFloat Count;
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
		InputFloat StartDelayTime;
		InputFloat StartLifeTime;
		InputFloat StartSpeed;
		InputFloat StartSize;
		InputColor StartColor;
		InputFloat StartRotation;
		ProxyCurve SizeOverLifetime;
		ProxyCurve SpeedOverLifetime;
		InputFloat RotationOverLifetime;
		uint32_t MaxParticles;
		bool IsLit;
		float AmbientAmount;

		InputFloat RateOverTime;
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
		ECompId GetCompId() const override { return ECompId::Particles; }

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
