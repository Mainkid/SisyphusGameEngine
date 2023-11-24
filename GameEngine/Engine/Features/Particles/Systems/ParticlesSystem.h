#pragma once
#include <boost/container_hash/hash.hpp>

#include "../Core/ECS/SystemBase.h"
#include "../Components/ParticleComponent.h"
#include "SimpleMath.h"

class ResourceService;
struct EngineContext;
struct HardwareContext;
struct RenderContext;


class ParticlesSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;

private:
	boost::hash<boost::uuids::uuid> _hasher;
	EngineContext* _ec = nullptr;
	HardwareContext* _hc = nullptr;
	RenderContext* _rc = nullptr;
	ResourceService* _rs = nullptr;

	UINT NUM_ELEMENTS = 512 * 512;
	UINT BITONIC_BLOCK_SIZE = 512; //Ближайшая степень двойки
	const UINT TRANSPOSE_BLOCK_SIZE = 16;
	const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
	const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;


	void InitRenderResources(ParticleComponent& pc);
	void Clear(ParticleComponent& pc);
	void InitBuffers(ParticleComponent& pc);
	void CreateSrvAndUav(ParticleComponent& pc);
};

