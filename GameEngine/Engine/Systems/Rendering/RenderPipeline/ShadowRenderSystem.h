#pragma once
#include "../../ISystem.h"
#include "SimpleMath.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

using namespace DirectX::SimpleMath;
class ShadowRenderSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;

	Matrix pointlightShadowProjectionMat;
};