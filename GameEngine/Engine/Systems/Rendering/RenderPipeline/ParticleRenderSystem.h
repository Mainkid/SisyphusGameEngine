#pragma once
#include <wrl/client.h>

#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include "../../Tools/ImageLoader.h"

struct ParticleComponent;
struct EngineContext;
struct RenderContext;
struct HardwareContext;

class ParticleRenderSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	EngineContext* _ec = nullptr;
	RenderContext* _rc = nullptr;
	HardwareContext* _hc = nullptr;


	Microsoft::WRL::ComPtr<ID3D11Texture2D> _noiseTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _noiseTextureSrv;

	void SortGpu();
	void NullSortList(ParticleComponent& pc);
};

