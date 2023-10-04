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
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	EngineContext* ec;
	RenderContext* rc;
	HardwareContext* hc;

	Matrix pointlightShadowProjectionMat;
};