#pragma once
#include "SyMonoProxyDatas.h"
#include "../../../vendor/entt/entity/registry.hpp"
#include "../../Core/Tools/ErrorLogger.h"
#include "../../Features/Common/ECompId.h"

namespace mono
{
	class SyMonoEcs;

	class ISyMonoEcsSync
	{
	public:
		virtual ~ISyMonoEcsSync() = default;

		virtual SyResult Bind(mono::SyMonoEcs* monoEcs, entt::registry* ecs) = 0;

		virtual ECompId GetCompId() const = 0;
		virtual void AddComp(entt::entity ent) = 0;
		virtual void RemoveComp(entt::entity ent) = 0;

		virtual void TrySendAll() = 0;
		virtual void SendDirectly(entt::entity ent) = 0;
	};
}
