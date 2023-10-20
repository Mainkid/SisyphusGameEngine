#include "SystemsGroup.h"

SystemsGroup::~SystemsGroup()
{
	for (auto system : _systems)
		delete system;
}

void SystemsGroup::AttachEcs(entt::registry* ecs)
{
	_ecs = ecs;
}

void SystemsGroup::Init()
{
	for (int i = 0; i < _systems.size(); i++)
		_systems[i]->Init();
}

void SystemsGroup::Run()
{
	for (int i = 0; i < _systems.size(); i++)
		_systems[i]->Run();
}

void SystemsGroup::Destroy()
{
	for (int i = 0; i < _systems.size(); i++)
		_systems[i]->Destroy();
}