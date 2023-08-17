#pragma once
#include <math.h>
#include "Particle.h"
#include "../Core/Graphics/Shader.h"

class ParticleComponent
{
public:
    struct CB
    {
        UINT iLevel;
        UINT iLevelMask;
        UINT iWidth;
        UINT iHeight;
    };


    ParticleComponent();
    ParticleComponent(int maxParticles, DirectX::SimpleMath::Vector3 origin, LPCWSTR texturePath);;
    void Initialize();
    void InitializeBuffers();
    void InitializeShaders();
    void LoadTexture();
    void SortGPU();
    void SetConstants(UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight);
    void NullSortList();




    Particle* particleList;
    int* indexList;
    int maxParticles;
    int numParticles;
    DirectX::SimpleMath::Vector3 origin;
    float accumulatedTime;
    DirectX::SimpleMath::Vector3 force;

    int groupSizeX;
    int groupSizeY;

    std::unique_ptr<Buffer> poolBuffer;
    std::unique_ptr<Buffer> deadListBuffer;
    //std::unique_ptr<Buffer> deadListAppendBuffer;
    std::unique_ptr<Buffer> sortListBuffer;
    //std::unique_ptr<Buffer> sortListAppendBuffer;

    std::unique_ptr<Buffer> indirectDrawBuffer;
    std::unique_ptr<Buffer> constBuffer;
    std::unique_ptr<Buffer> groupCountConstBuffer;
    std::unique_ptr<Buffer> counterBuffer;
    std::unique_ptr<Buffer> sortGPUConstBuffer;
    std::unique_ptr<Buffer> tmpGPUBuffer;
    std::unique_ptr<Buffer> indexBuffer;
    ID3D11Buffer* g_pReadBackBuffer;
    std::unique_ptr<Shader> simpleParticleShader;
    std::vector<SortListParticle> results = std::vector<SortListParticle>(512 * 512);
    std::vector<SortListParticle> data = std::vector<SortListParticle>(512 * 512);

    std::unique_ptr<Shader> particleEmitterShader;
    std::unique_ptr<Shader> particleUpdateShader;
    std::unique_ptr<Shader> bitonicSortShader;
    std::unique_ptr<Shader> matrixTransposeShader;
    std::unique_ptr<Shader> multiply6Shader;

    ID3D11Buffer* buffer = nullptr;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> poolBufferSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sortBufferSRV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> poolBufferUAV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> deadListUAV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> sortBufferUAV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> counterUAV;
    //Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> deadListAppendUAV;

    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> tmpGPUBufferUAV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tmpGPUBufferSRV;
    //Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> sortListAppendUAV;
    LPCWSTR texturePath;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;


    std::vector<int> indices;
};

