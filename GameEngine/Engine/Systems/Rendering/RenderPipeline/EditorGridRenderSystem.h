#pragma once
#include "../../ISystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../../Components/Mesh.h"
#include <memory>

class EngineCore;
struct EngineContext;
struct RenderContext;
struct HardwareContext;

class EditorGridRenderSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:

	std::shared_ptr<Mesh> grid;

	std::shared_ptr<Mesh> CreateGrid(float metersPerCell, int width);
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
};

