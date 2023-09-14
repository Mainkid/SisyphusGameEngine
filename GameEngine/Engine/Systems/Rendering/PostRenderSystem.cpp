#include "PostRenderSystem.h"
#include "../../Core/EngineCore.h"

void PostRenderSystem::Init()
{
	engine = EngineCore::instance();
	rc = ServiceLocator::instance()->Get<RenderContext>();
}

void PostRenderSystem::Run()
{
	engine->context->ClearState();
}

void PostRenderSystem::Destroy()
{

}
