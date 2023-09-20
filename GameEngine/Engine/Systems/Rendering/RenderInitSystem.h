#pragma once
#include "../ISystem.h"
#include <memory>

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class RenderInitSystem : public ISystem
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

