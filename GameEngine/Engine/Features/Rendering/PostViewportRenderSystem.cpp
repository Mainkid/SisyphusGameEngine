#include "PostViewportRenderSystem.h"
#include "../../Contexts/HardwareContext.h"


SyResult PostViewportRenderSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	return SyResult();
}

SyResult PostViewportRenderSystem::Run()
{
	hc->context->ClearState();
	return SyResult();
}

SyResult PostViewportRenderSystem::Destroy()
{
	return SyResult();
}
