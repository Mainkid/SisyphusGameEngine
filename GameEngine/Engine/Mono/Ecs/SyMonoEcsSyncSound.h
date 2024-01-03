#pragma once
#include "SyMonoEcsSyncBase.h"
#include "../../Features/Sounds/Components/FSoundComponent.h"

namespace mono
{
	struct ProxySoundComp
	{
		bool  IsPlaying;
		bool  IsLooping;
		float Volume;
		bool  Is3d;
		MonoString* SoundUuid;
	};


	class SyMonoEcsSyncSound :
		public SyMonoEcsSyncBase<SyMonoEcsSyncSound, FSoundComponent, ProxySoundComp>
	{
	public:
		ECompId GetCompId() const override { return ECompId::Sound; }

		void AddComp(entt::entity ent) override;
		void RemoveComp(entt::entity ent) override;

	protected:
		std::string GetEgMethodName() const override { return "EgUpdateSoundComp"; }
		std::string GetGeMethodName() const override { return "GeUpdateSoundComp"; }

		void FillProxyByComp(const FSoundComponent& comp) override;
		void FillCompByProxy(const ProxySoundComp& proxy, entt::entity ent, FSoundComponent& comp) override;
		size_t GetHash(const FSoundComponent& comp) override;
	};
}
