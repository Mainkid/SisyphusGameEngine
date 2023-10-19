#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "SimpleMath.h"

struct EngineContext;
struct HardwareContext;
struct RenderContext;

using namespace DirectX::SimpleMath;
class ShadowRenderSystem : public SystemBase
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