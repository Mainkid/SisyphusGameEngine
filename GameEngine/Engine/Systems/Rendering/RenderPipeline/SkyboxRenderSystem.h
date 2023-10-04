#pragma once
#include "../../Core/ECS/SystemBase.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class SkyboxRenderSystem
	: public SystemBase
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;


};

