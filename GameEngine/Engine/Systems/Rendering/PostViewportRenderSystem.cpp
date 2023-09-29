#include "PostViewportRenderSystem.h"
#include "../HardwareContext.h"


SyResult PostViewportRenderSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, L"PostViewportRender system initialization successful.");
	return SyResult();
}

SyResult PostViewportRenderSystem::Run()
{
	hc->context->ClearState();
	return SyResult();
}

SyResult PostViewportRenderSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, L"PostViewportRender system destruction successful.");
	return SyResult();
}
