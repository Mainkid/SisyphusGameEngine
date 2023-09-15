#include "ParticleComponent.h"

#include "../../vendor/WICTextureLoader.h"
#include "../Core/ServiceLocator.h"
#include "../Core/Graphics/ConstantBuffer.h"

UINT NUM_ELEMENTS = 512 * 512;
UINT BITONIC_BLOCK_SIZE = 512; //Ближайшая степень двойки
const UINT TRANSPOSE_BLOCK_SIZE = 16;
const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;

ParticleComponent::ParticleComponent()
{
    this->maxParticles = 262144;
    this->origin =Vector3::Zero;
    this->texturePath = L"./Engine/Assets/DefaultTexture.png";
    Initialize();
}

ParticleComponent::ParticleComponent( int _maxParticles, DirectX::SimpleMath::Vector3 _origin, LPCWSTR _texturePath)
{
    this->maxParticles = _maxParticles;
    this->origin = _origin;
    this->texturePath = _texturePath;
    Initialize();
}

void ParticleComponent::InitializeBuffers()
{
    this->indexBuffer = std::make_unique<Buffer>(hc->device.Get());
    indexBuffer->Initialize(indices);

    this->poolBuffer = std::make_unique<Buffer>(hc->device.Get());
    poolBuffer->Initialize(particleList, maxParticles, sizeof(Particle), true);

    this->deadListBuffer = std::make_unique<Buffer>(hc->device.Get());
    deadListBuffer->Initialize(indexList, maxParticles, sizeof(int), true);

    this->sortListBuffer = std::make_unique<Buffer>(hc->device.Get());
    sortListBuffer->Initialize(nullptr, maxParticles, sizeof(SortListParticle), true);

    this->constBuffer = std::make_unique<Buffer>(hc->device.Get());
    constBuffer->Initialize(sizeof(CB_ParticleVisualisation));

    this->groupCountConstBuffer = std::make_unique<Buffer>(hc->device.Get());
    groupCountConstBuffer->Initialize(sizeof(CB_ComputeShader));

    this->sortGPUConstBuffer = std::make_unique<Buffer>(hc->device.Get());
    sortGPUConstBuffer->Initialize(sizeof(CB));

    this->tmpGPUBuffer = std::make_unique<Buffer>(hc->device.Get());
    tmpGPUBuffer->Initialize(nullptr, maxParticles, sizeof(SortListParticle), true);

    this->indirectDrawBuffer = std::make_unique<Buffer>(hc->device.Get());
    indirectDrawBuffer->InitializeIndirect(maxParticles);

    this->counterBuffer = std::make_unique<Buffer>(hc->device.Get());
    counterBuffer->InitializeCounterBuffer();


    D3D11_BUFFER_DESC readback_buffer_desc = {};
    readback_buffer_desc.ByteWidth = NUM_ELEMENTS * sizeof(SortListParticle);
    readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
    readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    readback_buffer_desc.StructureByteStride = sizeof(SortListParticle);
    hc->device->CreateBuffer(&readback_buffer_desc, nullptr, &g_pReadBackBuffer);

}

void ParticleComponent::InitializeShaders()
{
    this->simpleParticleShader = std::make_unique<Shader>();
    simpleParticleShader->Initialize(L"./Engine/Assets/Shaders/ParticleSystem/SimpleParticle.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL|COMPILE_GEOM|COMPILE_COMPUTE,USE_POSITION,"DefaultVS","DefaultPS","TriangleGS","DefaultCS");

    this->particleUpdateShader = std::make_unique<Shader>();
    particleUpdateShader->Initialize(L"./Engine/Assets/Shaders/ParticleSystem/ParticleUpdateShader.hlsl",
        COMPILE_COMPUTE,USE_NONE,"","","","DefaultCS");

    this->particleEmitterShader = std::make_unique<Shader>();
    particleEmitterShader->Initialize(L"./Engine/Assets/Shaders/ParticleSystem/ParticleEmitter.hlsl",
        COMPILE_COMPUTE, USE_NONE, "", "", "","DefaultCS");

    this->bitonicSortShader = std::make_unique<Shader>();
    bitonicSortShader->Initialize(L"./Engine/Assets/Shaders/ParticleSystem/BitonicSort.hlsl",
        COMPILE_COMPUTE, USE_NONE, "", "", "", "BitonicSort");

    this->matrixTransposeShader = std::make_unique<Shader>();
    matrixTransposeShader->Initialize(L"./Engine/Assets/Shaders/ParticleSystem/BitonicSort.hlsl",
        COMPILE_COMPUTE, USE_NONE, "", "", "","MatrixTranspose");

    this->multiply6Shader = std::make_unique<Shader>();
    multiply6Shader->Initialize(L"./Engine/Assets/Shaders/ParticleSystem/Multiply6Shader.hlsl",
        COMPILE_COMPUTE, USE_NONE, "", "", "", "DefaultCS");
}

void ParticleComponent::LoadTexture()
{
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = hc->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
    if (FAILED(hr))
    {
        std::cout << "OP" << std::endl;
    }

    hr = DirectX::CreateWICTextureFromFile(hc->device.Get(), texturePath, nullptr, textureSRV.GetAddressOf());
    int q = 0;
}


void ParticleComponent::Initialize()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();

    particleList = new Particle[maxParticles];
    indexList = new int[maxParticles];
    for (int i = 0; i < maxParticles; i++)
        indexList[i] = i;


    int startIndex = 0;
    for (int i = 0; i < maxParticles; i++)
    {
        indices.push_back(startIndex);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex);
        indices.push_back(startIndex + 3);
        indices.push_back(startIndex + 2);
        startIndex += 4;
    }


    InitializeBuffers();

    InitializeShaders();

    LoadTexture();


    D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
    viewDesc.Format = DXGI_FORMAT_UNKNOWN;
    viewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    viewDesc.Buffer.FirstElement = 0;
    viewDesc.Buffer.Flags = 0;
    viewDesc.Buffer.NumElements = poolBuffer->size;

    HRESULT res = hc->device->CreateUnorderedAccessView(poolBuffer->buffer.Get(), &viewDesc, poolBufferUAV.GetAddressOf());

    viewDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

    res = hc->device->CreateUnorderedAccessView(deadListBuffer->buffer.Get(), &viewDesc, deadListUAV.GetAddressOf());
    viewDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
    res = hc->device->CreateUnorderedAccessView(sortListBuffer->buffer.Get(), &viewDesc, sortBufferUAV.GetAddressOf());


    D3D11_BUFFER_DESC constant_buffer_desc = {};
    constant_buffer_desc.ByteWidth = sizeof(CB);
    constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constant_buffer_desc.CPUAccessFlags = 0;
    HRESULT hr = hc->device->CreateBuffer(&constant_buffer_desc, nullptr, sortGPUConstBuffer->buffer.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvbuffer_desc = {};
    srvbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvbuffer_desc.Buffer.ElementWidth = maxParticles;
    hr = hc->device->CreateShaderResourceView(sortListBuffer->buffer.Get(), &srvbuffer_desc, sortBufferSRV.GetAddressOf());
    hr = hc->device->CreateShaderResourceView(poolBuffer->buffer.Get(), &srvbuffer_desc, poolBufferSRV.GetAddressOf());
    hr = hc->device->CreateShaderResourceView(tmpGPUBuffer->buffer.Get(), &srvbuffer_desc, tmpGPUBufferSRV.GetAddressOf());

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc = {};
    uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavbuffer_desc.Buffer.NumElements = maxParticles;
    uavbuffer_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
    hr = hc->device->CreateUnorderedAccessView(tmpGPUBuffer->buffer.Get(), &uavbuffer_desc, tmpGPUBufferUAV.GetAddressOf());


    D3D11_UNORDERED_ACCESS_VIEW_DESC counterDesc = {};
    counterDesc.Format = DXGI_FORMAT_R32_UINT;
    counterDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    counterDesc.Buffer.NumElements = 1;
    counterDesc.Buffer.Flags = 0;
    hr = hc->device->CreateUnorderedAccessView(counterBuffer->buffer.Get(), &counterDesc, counterUAV.GetAddressOf());

    int numGroups = (maxParticles % 768 != 0) ? ((maxParticles / 768) + 1) : (maxParticles / 768);
    double secondRoot = sqrt(numGroups);
    secondRoot = ceilf(secondRoot);
    groupSizeX = (int)secondRoot;
    groupSizeY = (int)secondRoot;
}

void ParticleComponent::SortGPU()
{
    /*std::random_device rd;
    std::mt19937 mt(rd());
    //std::generate( data.begin(), data.end(), [&] { return SortListParticle(mt(), static_cast <float> (rand())*4.0f / static_cast <float> (RAND_MAX)); });
    //game->context->UpdateSubresource( sortListBuffer->buffer.Get(), 0, nullptr, &data[0], 0, 0 );

    D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
    game->context->CopyResource(g_pReadBackBuffer, sortListBuffer->buffer.Get());
    if (SUCCEEDED(game->context->Map(g_pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource)))
    {
        _Analysis_assume_(MappedResource.pData);
        assert(MappedResource.pData);
        //memcpy(&results[0], MappedResource.pData, NUM_ELEMENTS * sizeof(SortListParticle));
        memcpy(&data[0], MappedResource.pData, NUM_ELEMENTS * sizeof(SortListParticle));
        game->context->Unmap(g_pReadBackBuffer, 0);
    }*/

    /*
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        /*if (i<NUM_ELEMENTS/2)
            data[i].distance = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        else
            data[i].distance = -static_cast <float> (rand()) / static_cast <float> (RAND_MAX);#1#
        if (i < NUM_ELEMENTS)
        {
            float k = data[i].distance;
            if (k != k)
                data[i].distance = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                //std::cout << "IS NAN" << std::endl;
        }
        //data[i].distance = k*10;
    }
    game->context->UpdateSubresource(sortListBuffer->buffer.Get(), 0, nullptr, &data[0], 0, 0);
    */



    //g_pBuffer1 - sortBuffer
    //g_pBuffer1UAV - sortBufferUAV
    for (UINT level = 2; level <= BITONIC_BLOCK_SIZE; level = level * 2)
    {
        SetConstants(level, level, MATRIX_HEIGHT, MATRIX_WIDTH);

        // Sort the row data
        hc->context->CSSetUnorderedAccessViews(0, 1, sortBufferUAV.GetAddressOf(), nullptr);
        hc->context->CSSetShader(bitonicSortShader->computeShader.Get(), nullptr, 0);
        hc->context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
        //std::cout << NUM_ELEMENTS / BITONIC_BLOCK_SIZE << std::endl;
    }

    // Then sort the rows and columns for the levels > than the block size
    // Transpose. Sort the Columns. Transpose. Sort the Rows.
    for (UINT level = (BITONIC_BLOCK_SIZE * 2); level <= NUM_ELEMENTS; level = level * 2)
    {
        SetConstants((level / BITONIC_BLOCK_SIZE), (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT);

        // Transpose the data from buffer 1 into buffer 2
        ID3D11ShaderResourceView* pViewnullptr = nullptr;
        hc->context->CSSetShaderResources(0, 1, &pViewnullptr);
        hc->context->CSSetUnorderedAccessViews(0, 1, tmpGPUBufferUAV.GetAddressOf(), nullptr);
        hc->context->CSSetShaderResources(0, 1, sortBufferSRV.GetAddressOf());
        hc->context->CSSetShader(matrixTransposeShader->computeShader.Get(), nullptr, 0);
        hc->context->Dispatch(MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);

        //std::cout << MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE << " " << MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE << std::endl;
        // Sort the transposed column data
        hc->context->CSSetShader(bitonicSortShader->computeShader.Get(), nullptr, 0);
        hc->context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);

        //std::cout << NUM_ELEMENTS / BITONIC_BLOCK_SIZE << std::endl;

        SetConstants(BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH);

        // Transpose the data from buffer 2 back into buffer 1
        hc->context->CSSetShaderResources(0, 1, &pViewnullptr);
        hc->context->CSSetUnorderedAccessViews(0, 1, sortBufferUAV.GetAddressOf(), nullptr);
        hc->context->CSSetShaderResources(0, 1, tmpGPUBufferSRV.GetAddressOf());
        hc->context->CSSetShader(matrixTransposeShader->computeShader.Get(), nullptr, 0);
        hc->context->Dispatch(MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);
        //std::cout << MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE<<" "<< MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE << std::endl;

        // Sort the row data
        hc->context->CSSetShader(bitonicSortShader->computeShader.Get(), nullptr, 0);
        hc->context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);

        //std::cout << NUM_ELEMENTS / BITONIC_BLOCK_SIZE << std::endl;
    }

    // Download the data
    /*MappedResource = {0};
    game->context->CopyResource( g_pReadBackBuffer, sortListBuffer->buffer.Get() );
    if( SUCCEEDED( game->context->Map( g_pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource ) ) )
    {
        _Analysis_assume_( MappedResource.pData);
        assert( MappedResource.pData );
        memcpy( &results[0], MappedResource.pData, NUM_ELEMENTS * sizeof(SortListParticle) );
        //memcpy(&data[0], MappedResource.pData, NUM_ELEMENTS * sizeof(SortListParticle));
        game->context->Unmap( g_pReadBackBuffer, 0 );
    }
    std::sort(data.begin(), data.end(), [](SortListParticle& a, SortListParticle& b) {return a.distance > b.distance; });
    bool bComparisonSucceeded = true;
    for (UINT i = 0; i < NUM_ELEMENTS; ++i)
    {
        if (data[i].distance != results[i].distance)
        {
            bComparisonSucceeded = false;
            std::cout << i << std::endl;
            break;
        }
    }

    std::cout << bComparisonSucceeded << std::endl;*/
}

void ParticleComponent::SetConstants(UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight)
{

    CB cb = { iLevel, iLevelMask, iWidth, iHeight };
    hc->context->UpdateSubresource(sortGPUConstBuffer->buffer.Get(), 0, nullptr, &cb, 0, 0);
    hc->context->CSSetConstantBuffers(0, 1, sortGPUConstBuffer->buffer.GetAddressOf());
}

void ParticleComponent::NullSortList()
{
    std::generate(data.begin(), data.end(), [&] { return SortListParticle(0, 0); });
    hc->context->UpdateSubresource(sortListBuffer->buffer.Get(), 0, nullptr, &data[0], 0, 0);
}








