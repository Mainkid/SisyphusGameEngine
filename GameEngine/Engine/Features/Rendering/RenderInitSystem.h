#pragma once
#include "../../Core/ECS/SystemBase.h"
#include <memory>

struct EngineContext;
struct HardwareContext;
struct RenderContext;

class RenderInitSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	void InitSkybox() const;
	void CreateTextures() const;
	void CompileShaders() const;
	void CreateBlendStates() const;
	void CreateBuffers() const;
	void CreateSamplers() const;
	void CreateDepthAndStencils() const;
	void CreateShaderResourceViewsAndRenderTargets() const;
	void CreateRasterizerStates() const;
	void CreateHbao() const;
	EngineContext* _ec = nullptr;
	RenderContext* _rc = nullptr;
	HardwareContext* _hc = nullptr;
};

