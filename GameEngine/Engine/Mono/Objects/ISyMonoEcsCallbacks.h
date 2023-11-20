#pragma once
#include <cstdint>
#include "../SyMonoProxyDatas.h"

namespace mono
{
	class ISyMonoEcsCallbacks
	{
	public:
		virtual ~ISyMonoEcsCallbacks() = default;

		virtual uint32_t OnCreateEntity() = 0;
		virtual void OnDestroyEntity(uint32_t rawEnt) = 0;

		virtual void OnAddComp(uint32_t rawEnt, EProxyCompId id) = 0;
		virtual void OnRemoveComp(uint32_t rawEnt, EProxyCompId id) = 0;

		virtual void OnUpdateTransformComp(uint32_t rawEnt, const ProxyTransformComp& proxy) = 0;
		virtual void OnUpdateMeshComp(uint32_t rawEnt, const ProxyMeshComp& proxy) = 0;
		virtual void OnUpdateLightComp(uint32_t rawEnt, const ProxyLightComp& proxy) = 0;
	};
}