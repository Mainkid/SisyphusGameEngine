#pragma once
#include <string>

#include "SyMonoGameConfig.h"
#include "SyMonoMethod.h"
#include "SyMonoObj.h"
#include "SyMonoProxyDatas.h"

namespace mono
{
	class ISyMonoGameCallbackReceiver;

	class SyMonoGame : public SyMonoObj
	{
	private:
		inline static const std::string NAMESPACE = "SyEngine.Game";
		inline static const std::string CLASS_NAME = "SyProxyGame";

		inline static const std::string GE_CREATE_ENTITY = "GE_CreateEngineEntity";
		inline static const std::string GE_DESTROY_ENTITY = "GE_DestroyEngineEntity";

		inline static const std::string GE_ADD_TRANSFORM_COMP = "GE_AddTransformComp";
		inline static const std::string GE_UPDATE_TRANSFORM_COMP = "GE_UpdateTransformComp";
		inline static const std::string GE_REMOVE_TRANSFORM_COMP = "GE_RemoveTransformComp";

		inline static const std::string GE_ADD_MESH_COMP = "GE_AddMeshComp";
		inline static const std::string GE_UPDATE_MESH_COMP = "GE_UpdateMeshComp";
		inline static const std::string GE_REMOVE_MESH_COMP = "GE_RemoveMeshComp";

	public:
		SyMonoMethod<MonoObject*> EG_SetConfig{ "EG_SetConfig" };

		SyMonoMethod<> EG_LoopInit{ "EG_Init" };
		SyMonoMethod<ProxyTimeData> EG_LoopRun{ "EG_Run" };
		SyMonoMethod<> EG_LoopDestroy{ "EG_Destroy" };

		SyMonoMethod<uint32_t, ProxyTransformComp> EG_UpdateTransformComp{ "EG_UpdateTransformComp" };
		SyMonoMethod<uint32_t, ProxyTransformComp> EG_UpdateMeshComp{ "EG_UpdateMeshComp" };

		void SetCallbackReceiver(ISyMonoGameCallbackReceiver* receiver);

		static void GE_CreateEngineEntity();
		static void GE_DestroyEngineEntity(uint32_t rawEnt);

		static void GE_AddTransformComp(uint32_t rawEnt);
		static void GE_UpdateTransformComp(uint32_t rawEnt, ProxyTransformComp proxy);
		static void GE_RemoveTransformComp(uint32_t rawEnt);

		static void GE_AddMeshComp(uint32_t rawEnt);
		static void GE_UpdateMeshComp(uint32_t rawEnt, ProxyMeshComp proxy);
		static void GE_RemoveMeshComp(uint32_t rawEnt);

	private:
		inline static SyMonoGame* _instance = nullptr;


		SyMonoGameConfig _gameConfig;
		ISyMonoGameCallbackReceiver* _cbReceiver = nullptr;


		void OnAfterCreate() override;
		void OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}