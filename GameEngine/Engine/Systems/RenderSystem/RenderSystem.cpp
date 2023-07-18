#include "RenderSystem.h"
#include "../../Core/EngineCore.h"

RenderSystem::RenderSystem(EngineCore* _engine)
{
	this->engine = _engine;
}
    
void RenderSystem::Update(float deltaSec)
{
	engine->renderPipeline->Render();
}

//TODO: Перенести renderPipeline в рендерсистему