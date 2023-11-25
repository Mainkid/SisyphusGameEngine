#pragma once
#include "../SyMonoObj.h"
#include "../SyMonoMethod.h"
#include "../SyMonoProxyDatas.h"

namespace mono
{
	class ISyMonoEcsCallbacks;
	struct ProxyTransformComp;
	struct ProxyMeshComp;

	class SyMonoEcs : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Core";
		inline static const std::string CLASS_NAME = "SyProxyEcs";

		inline static const std::string GE_CREATE_ENTITY = "GeCreateEngineEntity";
		inline static const std::string GE_DESTROY_ENTITY = "GeDestroyEngineEntity";

		inline static const std::string GE_ADD_COMP = "GeAddEngineComp";
		inline static const std::string GE_REMOVE_COMP = "GeRemoveEngineComp";

		inline static const std::string GE_UPDATE_TRANSFORM_COMP = "GeUpdateTransformComp";
		inline static const std::string GE_UPDATE_MESH_COMP = "GeUpdateMeshComp";
		inline static const std::string GE_UPDATE_LIGHT_COMP = "GeUpdateLightComp";
		inline static const std::string GE_UPDATE_COLLIDER_COMP = "GeUpdateColliderComp";
		inline static const std::string GE_UPDATE_RIGID_COMP = "GeUpdateRigidComp";

	private:
		inline static SyMonoEcs* _instance = nullptr;

		static void GeCreateEntity();
		static void GeDestroyEntity(uint32_t rawEnt);

		static void GeAddComp(uint32_t rawEnt, EProxyCompId id);
		static void GeRemoveComp(uint32_t rawEnt, EProxyCompId id);

		static void GeUpdateTransformComp(uint32_t rawEnt, ProxyTransformComp proxy);
		static void GeUpdateMeshComp(uint32_t rawEnt, ProxyMeshComp proxy);
		static void GeUpdateLightComp(uint32_t rawEnt, ProxyLightComp proxy);
		static void GeUpdateColliderComp(uint32_t rawEnt, ProxyColliderComp proxy);
		static void GeUpdateRigidComp(uint32_t rawEnt, ProxyRigidComp proxy);

	public:
		SyMonoMethod<uint32_t> EgContinueEntityDestroyCascade { "EgContinueEntityDestroyCascade" };

		SyMonoMethod<uint32_t, ProxyTransformComp> EgUpdateTransformComp{ "EgUpdateTransformComp" };
		SyMonoMethod<uint32_t, ProxyMeshComp> EgUpdateMeshComp{ "EgUpdateMeshComp" };
		SyMonoMethod<uint32_t, ProxyLightComp> EgUpdateLightComp{ "EgUpdateLightComp" };
		SyMonoMethod<uint32_t, ProxyColliderComp> EgUpdateColliderComp{ "EgUpdateColliderComp" };
		SyMonoMethod<uint32_t, ProxyRigidComp> EgUpdateRigidComp{ "EgUpdateRigidComp" };

		void SetCallbackReceiver(ISyMonoEcsCallbacks* receiver);
	private:
		ISyMonoEcsCallbacks* _cbReceiver = nullptr;

		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}
