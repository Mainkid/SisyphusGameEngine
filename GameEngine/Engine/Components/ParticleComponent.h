#pragma once
#include <math.h>
#include "Particle.h"
#include "../Core/Graphics/Shader.h"
#include "../Systems/HardwareContext.h"
#include "../Components/Particle.h"
#include "../Components/Texture.h"

struct CB
{
	UINT iLevel;
	UINT iLevelMask;
	UINT iWidth;
	UINT iHeight;
};

struct ParticleComponent
{
	


	boost::uuids::uuid SharedParticlesDataUuid;					//Particles Data Resource;
	std::shared_ptr<SharedParticlesData> SharedParticlesDataResource;
	std::shared_ptr<Texture> ParticleTexture;
	std::vector<Particle> ParticlesList;


	uint32_t GroupSizeX;
	uint32_t GroupSizeY;
	uint32_t NumParticles;
	float TimeFromStart;
	float ParticlesToEmit = 0.0f;

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

	SER_COMP(ParticleComponent, SharedParticlesDataUuid
		//IsLooping, StartDelayTime, StartLifeTime,
		//StartSpeed, StartSize, StartColor, MaxParticles, RateOverTime, ParticleBursts
	)

//public:
//    struct CB
//    {
//        UINT iLevel;
//        UINT iLevelMask;
//        UINT iWidth;
//        UINT iHeight;
//    };
//
//
//    ParticleComponent();
//    ParticleComponent(int maxParticles, DirectX::SimpleMath::Vector3 origin, LPCWSTR texturePath);;
//    void Initialize();
//    void InitializeBuffers();
//    void InitializeShaders();
//    void LoadTexture();
//    void SortGPU();
//    void SetConstants(UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight);
//    void NullSortList();
//
//
//
//
//    Particle* particleList;
//    int* indexList;
//    int maxParticles;
//    int numParticles;
//    DirectX::SimpleMath::Vector3 origin;
//    float accumulatedTime;
//    DirectX::SimpleMath::Vector3 force;
//
//    int groupSizeX;
//    int groupSizeY;
//
//    std::unique_ptr<Buffer> poolBuffer;
//    std::unique_ptr<Buffer> deadListBuffer;
//    //std::unique_ptr<Buffer> deadListAppendBuffer;
//    std::unique_ptr<Buffer> sortListBuffer;
//    //std::unique_ptr<Buffer> sortListAppendBuffer;
//
//    std::unique_ptr<Buffer> indirectDrawBuffer;
//    std::unique_ptr<Buffer> constBuffer;
//    std::unique_ptr<Buffer> groupCountConstBuffer;
//    std::unique_ptr<Buffer> counterBuffer;
//    std::unique_ptr<Buffer> sortGPUConstBuffer;
//    std::unique_ptr<Buffer> tmpGPUBuffer;
//    std::unique_ptr<Buffer> indexBuffer;
//    ID3D11Buffer* g_pReadBackBuffer;
//    std::unique_ptr<Shader> simpleParticleShader;
//    std::vector<SortListParticle> results = std::vector<SortListParticle>(512 * 512);
//    std::vector<SortListParticle> data = std::vector<SortListParticle>(512 * 512);
//
//    std::unique_ptr<Shader> particleEmitterShader;
//    std::unique_ptr<Shader> particleUpdateShader;
//    std::unique_ptr<Shader> bitonicSortShader;
//    std::unique_ptr<Shader> matrixTransposeShader;
//    std::unique_ptr<Shader> multiply6Shader;
//
//    ID3D11Buffer* buffer = nullptr;
//
//    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> poolBufferSRV;
//    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sortBufferSRV;
//    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> poolBufferUAV;
//    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> deadListUAV;
//    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> sortBufferUAV;
//    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> counterUAV;
//    //Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> deadListAppendUAV;
//
//    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> tmpGPUBufferUAV;
//    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tmpGPUBufferSRV;
//    //Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> sortListAppendUAV;
//    LPCWSTR texturePath;
//    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
//    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;
//    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
//
//
//    std::vector<int> indices;
//private:
//    HardwareContext* hc;
};

