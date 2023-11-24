#pragma once
#include <math.h>
#include "../../../Core/Graphics/Shader.h"
#include "../../../Contexts/HardwareContext.h"
#include "../Particle.h"
#include "../../../Components/Texture.h"
#include "../../../Core/Rendering/Lights/LightData.h"

struct CB
{
	UINT iLevel;
	UINT iLevelMask;
	UINT iWidth;
	UINT iHeight;
};

struct ParticleComponent
{

	//----User vars----
	boost::uuids::uuid SharedParticlesDataUuid;					
	std::shared_ptr<SharedParticlesData> SharedParticlesDataResource;

	//----Engine vars----
	std::shared_ptr<Texture> ParticleTexture;
	std::vector<Particle> ParticlesList;
	uint32_t GroupSizeX;
	uint32_t GroupSizeY;
	uint32_t NumParticles;
	float TimeFromStart;
	float ParticlesToEmit = 0.0f;
	LightData lightObjects[10];
	bool IsInitialized = false;

	std::unique_ptr<Buffer> PoolBuffer;
	std::unique_ptr<Buffer> DeadListBuffer;
	std::unique_ptr<Buffer> SortListBuffer;
	std::unique_ptr<Buffer> IndirectDrawBuffer;
	std::unique_ptr<Buffer> ConstBuffer;
	std::unique_ptr<Buffer> GroupCountConstBuffer;
	std::unique_ptr<Buffer> CounterBuffer;
	std::unique_ptr<Buffer> SortGpuConstBuffer;
	std::unique_ptr<Buffer> TmpGpuBuffer;
	std::unique_ptr<Buffer> IndexBuffer;
	Microsoft::WRL::ComPtr< ID3D11Buffer> CounterCpuBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> PoolBufferSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SortBufferSrv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> PoolBufferUav;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> DeadListUav;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> SortBufferUav;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> CounterUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CounterSrv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> TmpGpuBufferUav;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TmpGpuBufferSrv;

	std::vector<SortListParticle> data = std::vector<SortListParticle>(1);

	std::vector<int> Indices;
	std::vector<int> IndexList;
	size_t Hash;

	SER_COMP(ParticleComponent, SharedParticlesDataUuid)

};

