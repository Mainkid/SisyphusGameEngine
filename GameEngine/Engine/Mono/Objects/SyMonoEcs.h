#pragma once
#include "../Api/SyMonoObj.h"
#include "../Api/SyMonoMethod.h"
#include "../Ecs/ISyMonoEcsSync.h"

namespace mono
{
	class SyMonoEcs : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Ecs";
		inline static const std::string CLASS_NAME = "SyProxyEcs";

		inline static const std::string GE_CREATE_ENTITY = "GeCreateEngineEntity";
		inline static const std::string GE_DESTROY_ENTITY = "GeDestroyEngineEntity";

		inline static const std::string GE_ADD_COMP = "GeAddEngineComp";
		inline static const std::string GE_REMOVE_COMP = "GeRemoveEngineComp";

	private:
		inline static SyMonoEcs* _instance = nullptr;

		static uint32_t GeCreateEntity();
		static void GeDestroyEntity(uint32_t rawEnt);

		static void GeAddComp(uint32_t rawEnt, ECompId id);
		static void GeRemoveComp(uint32_t rawEnt, ECompId id);

	public:
		SyMonoMethod<> EgSyncEngineWithGame{ "EgSyncEngineWithGame" };

		void BindEcs(entt::registry* ecs);
		void TrySendAll();

	private:
		entt::registry* _ecs = nullptr;

		std::vector<ISyMonoEcsSync*> _syncs;
		std::unordered_map<ECompId, ISyMonoEcsSync*> _compIdToSync;

		SyMonoMethod<uint32_t> _egDestroyEntity{ "EgDestroyEntity" };


		template<typename T>
		void AddSync();
		ISyMonoEcsSync* GetSync(ECompId id);

		void DestroyEntity(entt::entity ent, bool isRecursionStep);

		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};

	template <typename T>
	void SyMonoEcs::AddSync()
	{
		auto sync = new T();
		_syncs.push_back(sync);
		_compIdToSync[sync->GetCompId()] = sync;

		sync->Bind(this, _ecs);
	}
}
