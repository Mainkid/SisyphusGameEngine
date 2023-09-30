#include "UpdateTimingSystem.h"
#include "EngineContext.h"

SyResult UpdateTimingSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	return SyResult();
}
