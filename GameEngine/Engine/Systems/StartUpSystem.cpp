#include "StartUpSystem.h"

SyResult StartUpSystem::Init()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "StartUp system initialization successful. ");
	return SyResult();
}

SyResult StartUpSystem::Run()
{
	return SyResult();
}

SyResult StartUpSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "StartUp system destruction successful. ");
	return SyResult();
}
