#pragma once
#include <cstdint>
#include "SyMonoProxyDatas.h"

namespace mono
{
	class ISyMonoEcsCallbackReceiver
	{
	public:
		virtual ~ISyMonoEcsCallbackReceiver() = default;

		virtual uint32_t OnCreateEngineEntity() = 0;
		virtual void OnDestroyEngineEntity(uint32_t rawEnt) = 0;

		virtual void OnAddTransformComp(uint32_t rawEnt) = 0;
		virtual void OnRemoveTransformComp(uint32_t rawEnt) = 0;
		virtual void OnUpdateTransformComp(uint32_t rawEnt, const ProxyTransformComp& proxy) = 0;

		virtual void OnAddMeshComp(uint32_t rawEnt) = 0;
		virtual void OnRemoveMeshComp(uint32_t rawEnt) = 0;
		virtual void OnUpdateMeshComp(uint32_t rawEnt, const ProxyMeshComp& proxy) = 0;

	};
}