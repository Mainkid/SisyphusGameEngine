#pragma once
#include "../../vendor/entt/entt.hpp"
#include "../../Tools/ErrorLogger.h"
#include "../EventBase.h"
#include "../../Components/GameObjectComp.h"

template<typename T_DerivedSystem>
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
		const T_Event defaultEvent;
		const SyEventBase* iEvent = dynamic_cast<const SyEventBase*>(&defaultEvent);
		if (iEvent == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = xstring("The type of object passed to function does not implement IEvent interface.");
			SY_LOG_EVSY(SY_LOGLEVEL_ERROR,"The type of object passed to function is not of SyEventBase type.");
			return result;
		}
		auto ent = _ecs->create();
		_ecs->emplace<GameObjectComp>(ent, iEvent->name);
		_ecs->emplace<T_Event>(ent, eventArgs_...);
		return SyResult();
	}
	template <typename ...Args>
	SyResult ListenToEvents()
	{
		std::vector<SyResult> results;
		results = {DispatchEvents<Args>(_ecs->view<Args>())...};
		for (auto& result : results)
			if (result.code == SY_RESCODE_ERROR)
				return result;
		return SyResult();
	}
	template <typename T_Event>
	SyResult DispatchEvents(decltype(entt::registry::view<T_Event>)& view_)
	{
		SyResult result;
		auto* derivedSystemPtr = static_cast<T_DerivedSystem*>(this);
		for (auto& entity : view_)
			if(derivedSystemPtr->ProcessEvent(view_.get<T_Event>(entity)) == SY_RESCODE_ERROR)
			{
				result.code = SY_RESCODE_UNEXPECTED;
				result.message = xstring("Failed to process one or more events.");;
			}
		return result;
	}
	template <typename T_Event>
	SyResult ProcessEvent(const T_Event& event_)
	{
		std::cout << xstring("Could not find proper ProcessEvent function for event %s.\n", event_.name).ToString();
		SyResult result;
		result.code = SY_RESCODE_ERROR;
		result.message = xstring("Could not find proper ProcessEvent function for event %s.", event_.name);
		SY_LOG_EVSY(SY_LOGLEVEL_ERROR, "Could not find proper ProcessEvent function for event %s.", event_.name);
		return result;
	}
protected:
	entt::registry* _ecs;
};

