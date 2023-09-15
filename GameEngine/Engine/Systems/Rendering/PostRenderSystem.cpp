#include "PostRenderSystem.h"
#include "../HardwareContext.h"


void PostRenderSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
}

void PostRenderSystem::Run()
{
	hc->context->ClearState();
}

void PostRenderSystem::Destroy()
{

}
