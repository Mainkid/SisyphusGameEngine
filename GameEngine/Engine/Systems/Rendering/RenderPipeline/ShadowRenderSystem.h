#pragma once
#include "../../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "../../../Components/LightComponent.h"

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
	void BlurShadowMap(LightComponent& lc);
	EngineContext* _ec = nullptr;
	RenderContext* _rc = nullptr;
	HardwareContext* _hc = nullptr;
	Matrix pointlightShadowProjectionMat;
};