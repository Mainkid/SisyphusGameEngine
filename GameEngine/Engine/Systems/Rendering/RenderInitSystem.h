#pragma once
#include "../../Core/ECS/SystemBase.h"
#include <memory>

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class RenderInitSystem : public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	void InitSkybox();
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;
};

