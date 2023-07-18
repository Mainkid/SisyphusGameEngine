#include "RenderSystem.h"
#include "../../Core/EngineCore.h"

RenderSystem::RenderSystem()
{

}
    
void RenderSystem::Update(float deltaSec)
{
	EngineCore::instance()->renderPipeline->Render();
}

//TODO: Перенести renderPipeline в рендерсистему