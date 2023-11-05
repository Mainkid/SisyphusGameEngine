#pragma once
#include "../../vendor/entt/entt.hpp"
#include "../../Tools/ErrorLogger.h"
#include "../../Components/GameObjectComp.h"
#include "Event.h"

#define SY_GET_THIS_FRAME_EVENT_VIEW(eventType_) _ecs->view<eventType_, SyThisFrameEventTag>()

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
	SyResult CallEvent(const std::string& name, Args... eventArgs_)
	{
		entt::entity ent = _ecs->create();
		_ecs->emplace<GameObjectComp>(ent, name);
		_ecs->emplace<T_Event>(ent, eventArgs_...);
		_ecs->emplace<SyEventTag>(ent);
		return SyResult();
	}

protected:
	entt::registry* _ecs;
};


