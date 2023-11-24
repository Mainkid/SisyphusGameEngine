#pragma once
#include <wrl/client.h>

#include "../../Core/ECS/SystemBase.h"
#include "../../Core/ServiceLocator.h"
#include "../../../Core/Tools/ImageLoader.h"

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

	UINT NUM_ELEMENTS = 512 * 512;
	UINT BITONIC_BLOCK_SIZE = 512; //Ближайшая степень двойки
	UINT TRANSPOSE_BLOCK_SIZE = 1;
	UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
	UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;


	Microsoft::WRL::ComPtr<ID3D11Texture2D> _noiseTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _noiseTextureSrv;

	void SortGpu(ParticleComponent& pc);
	void NullSortList(ParticleComponent& pc);
	void SetConstants(ParticleComponent& pc,UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight);
};

