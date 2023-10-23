#pragma once
#include "../../vendor/entt/entt.hpp"
#include "../../Tools/ErrorLogger.h"
#include "../../Event/IEvent.h"
#include "../../Components/GameObjectComp.h"

class SystemBase
{
public:
	void AttachEcs(entt::registry* ecs)
	{
		_ecs = ecs;
	}

	virtual ~SystemBase() {}

	virtual SyResult Init() = 0;
	virtual SyResult Run() = 0;
	virtual SyResult Destroy() = 0;

	template <typename T_Event, typename ... Args>
	SyResult CallEvent(Args... eventArgs_)
	{
		SyResult result;
		IEvent* iEvent = dynamic_cast<const IEvent*>(T_Event());
		if (iEvent == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = xstring("The type of object passed to function does not implement IEvent interface.");
			return result;
		}
		else
		{
			auto ent = _ecs->create();
			_ecs->emplace<GameObjectComp>(ent, "Event" + iEvent->name);
			_ecs->emplace<T_Event>(ent, eventArgs_);
			;
		}
		return SyResult();
	}

protected:
	entt::registry* _ecs;
};

