#include "MaterialUpdateSystem.h"
#include "../Contexts/EngineContext.h"
#include "../Contexts/HardwareContext.h"
#include "../Contexts/RenderContext.h"
#include "../Core/ServiceLocator.h"
#include "../Features/Mesh/MeshLoader.h"


SyResult MaterialUpdateSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();

	return SyResult();
}

SyResult MaterialUpdateSystem::Run()
{
	OPTICK_EVENT();
	return SyResult();
}

SyResult MaterialUpdateSystem::Destroy()
{
	return SyResult();
}
