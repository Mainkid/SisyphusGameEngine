#pragma once
#include "../Core/ECS/SystemBase.h"
#include "../Components/Material.h"

struct EngineContext;
struct RenderContext;
struct HardwareContext;

class MaterialUpdateSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;

	std::hash<Material> hasher;
};

