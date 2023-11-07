#include "MaterialUpdateSystem.h"
#include "EngineContext.h"
#include "HardwareContext.h"
#include "Rendering/RenderContext.h"
#include "../Core/ServiceLocator.h"
#include "MeshLoader.h"


SyResult MaterialUpdateSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();

	return SyResult();
}

SyResult MaterialUpdateSystem::Run()
{

	return SyResult();
}

SyResult MaterialUpdateSystem::Destroy()
{
	return SyResult();
}
