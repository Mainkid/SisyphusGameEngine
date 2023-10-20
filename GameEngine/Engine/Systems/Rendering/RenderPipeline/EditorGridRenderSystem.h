#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include "../../../Components/Mesh.h"
#include <memory>

class EngineCore;
struct EngineContext;
struct RenderContext;
struct HardwareContext;

class EditorGridRenderSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:

	const int minGridSize = 64;
	const int maxGridSize = 128;
	std::shared_ptr<Mesh> grid1M;
	std::shared_ptr<Mesh> grid10M;
	std::shared_ptr<Mesh> CreateGrid(float metersPerCell, int width);
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
};

