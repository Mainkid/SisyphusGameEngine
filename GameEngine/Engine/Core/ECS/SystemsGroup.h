#pragma once
#include <vector>

#include "SystemBase.h"
#include "../../Contexts/EngineContext.h"

class SystemsGroup
{
public:
	~SystemsGroup();

	void AttachEcs(entt::registry* ecs);

	template<typename T>
	void Add();

	void Init();

	void Run();

	void Destroy();

private:
	entt::registry* _ecs;
	std::vector<SystemBase*> _systems;
};

template <typename T>
void SystemsGroup::Add()
{
	T* system = new T();
	system->AttachEcs(_ecs);
	_systems.push_back(system);
}