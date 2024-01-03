#pragma once
#include "ISyMonoEcsSync.h"
#include "../../Components/GameObjectComp.h"
#include "../API/SyMonoMethod.h"
#include "../Objects/SyMonoEcs.h"

namespace mono
{
	template<typename TSync, typename TComp, typename TProxyComp>
	class SyMonoEcsSyncBase : public ISyMonoEcsSync
	{
	public:
		virtual ~SyMonoEcsSyncBase()
		{
			_instance = nullptr;
		}

		SyResult Bind(SyMonoEcs* monoEcs, entt::registry* ecs) final
		{
			_instance = (TSync*)this;

			_ecs = ecs;
			_monoDomain = mono_object_get_domain(monoEcs->GetInstance());

			_egUpdate = { GetEgMethodName()};
			SY_RESULT_CHECK(_egUpdate.Bind(monoEcs));

			monoEcs->BindCallback(GetGeMethodName(), &OnStaticGeUpdate);

			return {};
		}

		void TrySendAll()
		{
			if constexpr (std::is_same_v<GameObjectComp, TComp>)
			{
				auto view = _ecs->view<TComp>();
				for (auto ent : view)
				{
					auto& comp = view.get<TComp>(ent);
					size_t hash = GetHash(comp);
					if (comp.MonoHash == hash)
						continue;
					comp.MonoHash = hash;

					FillProxyByComp(comp);
					_egUpdate.Invoke(static_cast<uint32_t>(ent), _proxy);
					ReleaseProxy();
				}
			}
			else
			{
				auto view = _ecs->view<GameObjectComp, TComp>();
				for (auto ent : view)
				{
					auto& comp = view.get<TComp>(ent);
					size_t hash = GetHash(comp);
					if (comp.MonoHash == hash)
						continue;
					comp.MonoHash = hash;

					FillProxyByComp(comp);
					_egUpdate.Invoke(static_cast<uint32_t>(ent), _proxy);
					ReleaseProxy();
				}
			}
		}

		void SendDirectly(entt::entity ent) final
		{
			auto& comp = _ecs->get<TComp>(ent);

			FillProxyByComp(comp);
			_egUpdate.Invoke(static_cast<uint32_t>(ent), _proxy);
			ReleaseProxy();

			comp.MonoHash = GetHash(comp);
		}
		
	protected:
		entt::registry* _ecs = nullptr;
		MonoDomain* _monoDomain = nullptr;

		TProxyComp _proxy;


		virtual std::string GetEgMethodName() const = 0;
		virtual std::string GetGeMethodName() const = 0;

		virtual void FillProxyByComp(const TComp& comp) = 0;

		virtual void ReleaseProxy() {}

		virtual void FillCompByProxy(const TProxyComp& proxy, entt::entity ent, TComp& comp) = 0;

		virtual size_t GetHash(const TComp& comp) = 0;

	private:
		inline static TSync* _instance = nullptr;

		SyMonoMethod<uint32_t, TProxyComp> _egUpdate{ "none" };

		static void OnStaticGeUpdate(uint32_t rawEnt, TProxyComp proxy)
		{
			if (_instance != nullptr)
				_instance->OnGeUpdate(rawEnt, proxy);
		}

		void OnGeUpdate(uint32_t rawEnt, TProxyComp& proxy)
		{
			auto ent = static_cast<entt::entity>(rawEnt);
			TComp* comp = _ecs->try_get<TComp>(ent);
			if (comp == nullptr)
			{
				std::cout << "[mono] e" << rawEnt << "does not have " << GetCompId() << std::endl;
				return;
			}
			FillCompByProxy(proxy, ent, *comp);
			comp->MonoHash = GetHash(*comp);
		}
	};
}
