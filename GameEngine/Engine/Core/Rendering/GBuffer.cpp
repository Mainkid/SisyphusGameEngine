#include "GBuffer.h"
#include "../../Core/EngineCore.h"

GBuffer::GBuffer(Microsoft::WRL::ComPtr<ID3D11Device> _device)
{
    this->device = _device;
}

void GBuffer::Initialize(int width, int height)
{
    this->t_height = height;
    this->t_width = width;
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    textureDesc.Width = width;
    textureDesc.Height =height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1; 
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT res =device->CreateTexture2D(&textureDesc, nullptr, diffuseTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, metallicTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, specularTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, roughnessTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, emissiveTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, normalTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, positionTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, depthTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, skyboxTexture.GetAddressOf());
    res = device->CreateTexture2D(&textureDesc, nullptr, HDRBufferTexture.GetAddressOf());

    textureDesc.BindFlags = 0;
    textureDesc.Usage = D3D11_USAGE_STAGING;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    res = device->CreateTexture2D(&textureDesc, nullptr, depthCpuTexture.GetAddressOf());

    
    res = device->CreateRenderTargetView(diffuseTexture.Get(), nullptr, diffuseRTV.GetAddressOf());
    res = device->CreateRenderTargetView(metallicTexture.Get(), nullptr, metallicRTV.GetAddressOf());
    res = device->CreateRenderTargetView(specularTexture.Get(), nullptr, specularRTV.GetAddressOf());
    res = device->CreateRenderTargetView(roughnessTexture.Get(), nullptr, roughnessRTV.GetAddressOf());
    res = device->CreateRenderTargetView(emissiveTexture.Get(), nullptr, emissiveRTV.GetAddressOf());
    res = device->CreateRenderTargetView(normalTexture.Get(),nullptr,normalRTV.GetAddressOf());
    res = device->CreateRenderTargetView(positionTexture.Get(), nullptr, positionRTV.GetAddressOf());
    res = device->CreateRenderTargetView(depthTexture.Get(), nullptr, depthRTV.GetAddressOf());
    res = device->CreateRenderTargetView(skyboxTexture.Get(), nullptr, skyboxRTV.GetAddressOf());
    res = device->CreateRenderTargetView(HDRBufferTexture.Get(), nullptr, HDRBufferRTV.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc1;
    ZeroMemory(&shaderResourceViewDesc1, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc1.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    shaderResourceViewDesc1.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc1.Texture2D.MostDetailedMip=0;
    shaderResourceViewDesc1.Texture2D.MipLevels = 1;

    res = device->CreateShaderResourceView(diffuseTexture.Get(), &shaderResourceViewDesc1, diffuseSRV.GetAddressOf());
    res = device->CreateShaderResourceView(metallicTexture.Get(), &shaderResourceViewDesc1, metallicSRV.GetAddressOf());
    res = device->CreateShaderResourceView(specularTexture.Get(), &shaderResourceViewDesc1, specularSRV.GetAddressOf());
    res = device->CreateShaderResourceView(roughnessTexture.Get(), &shaderResourceViewDesc1, roughnessSRV.GetAddressOf());
    res = device->CreateShaderResourceView(emissiveTexture.Get(), &shaderResourceViewDesc1, emissiveSRV.GetAddressOf());


    res = device->CreateShaderResourceView(normalTexture.Get(), &shaderResourceViewDesc1, normalSRV.GetAddressOf());
    res = device->CreateShaderResourceView(positionTexture.Get(), &shaderResourceViewDesc1, positionSRV.GetAddressOf());
    res = device->CreateShaderResourceView(depthTexture.Get(), &shaderResourceViewDesc1, depthSRV.GetAddressOf());
    res = device->CreateShaderResourceView(skyboxTexture.Get(), &shaderResourceViewDesc1, skyboxSRV.GetAddressOf());
    res = device->CreateShaderResourceView(HDRBufferTexture.Get(), &shaderResourceViewDesc1, HDRBufferSRV.GetAddressOf());






}



