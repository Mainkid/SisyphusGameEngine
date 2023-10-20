#pragma once
#include "../../vendor/entt/entt.hpp"
#include "../../Tools/ErrorLogger.h"

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

protected:
	entt::registry* _ecs;
};

