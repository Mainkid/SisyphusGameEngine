#pragma once
#include "../SyMonoObj.h"
#include "../SyMonoMethod.h"

namespace mono
{
	class ISyMonoEcsCallbackReceiver;
	struct ProxyTransformComp;
	struct ProxyMeshComp;

	class SyMonoEcs : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Core";
		inline static const std::string CLASS_NAME = "SyProxyEcs";

		inline static const std::string GE_CREATE_ENTITY = "GeCreateEngineEntity";
		inline static const std::string GE_DESTROY_ENTITY = "GeDestroyEngineEntity";

		inline static const std::string GE_ADD_TRANSFORM_COMP = "GeAddTransformComp";
		inline static const std::string GE_UPDATE_TRANSFORM_COMP = "GeUpdateTransformComp";
		inline static const std::string GE_REMOVE_TRANSFORM_COMP = "GeRemoveTransformComp";

		inline static const std::string GE_ADD_MESH_COMP = "GeAddMeshComp";
		inline static const std::string GE_UPDATE_MESH_COMP = "GeUpdateMeshComp";
		inline static const std::string GE_REMOVE_MESH_COMP = "GeRemoveMeshComp";

	private:
		inline static SyMonoEcs* _instance = nullptr;

		static void GeCreateEngineEntity();
		static void GeDestroyEngineEntity(uint32_t rawEnt);

		static void GeAddTransformComp(uint32_t rawEnt);
		static void GeUpdateTransformComp(uint32_t rawEnt, ProxyTransformComp proxy);
		static void GeRemoveTransformComp(uint32_t rawEnt);

		static void GeAddMeshComp(uint32_t rawEnt);
		static void GeUpdateMeshComp(uint32_t rawEnt, ProxyMeshComp proxy);
		static void GeRemoveMeshComp(uint32_t rawEnt);

	public:
		SyMonoMethod<uint32_t> EgContinueEntityDestroyCascade { "EgContinueEntityDestroyCascade" };

		SyMonoMethod<uint32_t, ProxyTransformComp> EgUpdateTransformComp{ "EgUpdateTransformComp" };
		SyMonoMethod<uint32_t, ProxyTransformComp> EgUpdateMeshComp{ "EgUpdateMeshComp" };

		void SetCallbackReceiver(ISyMonoEcsCallbackReceiver* receiver);
	private:


		ISyMonoEcsCallbackReceiver* _cbReceiver = nullptr;

		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}