#include "MaterialUpdateSystem.h"
#include "EngineContext.h"
#include "HardwareContext.h"
#include "Rendering/RenderContext.h"
#include "../Core/ServiceLocator.h"
#include "MeshLoader.h"


void MaterialUpdateSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();

	//rc->materialSet.insert(std::make_unique<Material>());
}

void MaterialUpdateSystem::Run()
{

}

void MaterialUpdateSystem::Destroy()
{
}
