#pragma once
#include "../../vendor/entt/entt.hpp"

class SystemBase
{
public:
	void AttachEcs(entt::registry* ecs)
	{
		_ecs = ecs;
	}

	virtual ~SystemBase() {}

	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void Destroy() = 0;

protected:
	entt::registry* _ecs;
};

