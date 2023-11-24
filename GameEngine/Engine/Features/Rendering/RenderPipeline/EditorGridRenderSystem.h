#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include "../../Mesh/Mesh.h"
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
	const int _minGridSize = 64;
	const int _maxGridSize = 128;
	std::shared_ptr<Mesh> _grid1M;
	std::shared_ptr<Mesh> _grid10M;
	EngineContext* _ec = nullptr;
	RenderContext* _rc = nullptr;
	HardwareContext* _hc = nullptr;

	std::shared_ptr<Mesh> CreateGrid(float metersPerCell, int width);
};

