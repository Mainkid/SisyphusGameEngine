#pragma once
#include <cstdint>
#include "SyMonoProxyDatas.h"

namespace mono
{
	class ISyMonoGameCallbackReceiver
	{
	public:
		virtual ~ISyMonoGameCallbackReceiver() = default;

		virtual uint32_t CreateEngineEntity() = 0;
		virtual void DestroyEngineEntity(uint32_t rawEnt) = 0;

		virtual void AddTransformComp(uint32_t rawEnt) = 0;
		virtual void RemoveTransformComp(uint32_t rawEnt) = 0;
		virtual void UpdateTransformComp(uint32_t rawEnt, const ProxyTransformComp& proxy) = 0;

		virtual void AddMeshComp(uint32_t rawEnt) = 0;
		virtual void RemoveMeshComp(uint32_t rawEnt) = 0;
		virtual void UpdateMeshComp(uint32_t rawEnt, const ProxyMeshComp& proxy) = 0;

	};
}