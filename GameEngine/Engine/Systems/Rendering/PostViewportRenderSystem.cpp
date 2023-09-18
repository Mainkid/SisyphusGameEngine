#include "PostViewportRenderSystem.h"
#include "../HardwareContext.h"


void PostViewportRenderSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
}

void PostViewportRenderSystem::Run()
{
	hc->context->ClearState();
}

void PostViewportRenderSystem::Destroy()
{

}
