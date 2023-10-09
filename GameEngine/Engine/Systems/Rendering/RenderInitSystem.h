#pragma once
#include "../ISystem.h"
#include <memory>

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class RenderInitSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void InitSkybox();
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
};

