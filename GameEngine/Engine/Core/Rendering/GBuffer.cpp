#include "GBuffer.h"
#include "../../Core/EngineCore.h"

GBuffer::GBuffer(Microsoft::WRL::ComPtr<ID3D11Device> _device)
{
    this->_device = _device;
}

void GBuffer::Initialize(int width, int height)
{
    this->_height = height;
    this->_width = width;
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

    HRESULT res =_device->CreateTexture2D(&textureDesc, nullptr, _diffuseTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _metallicTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _specularTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _depthTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _emissiveTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _normalAndDepthTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _positionTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _idTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _skyboxTexture.GetAddressOf());
    res = _device->CreateTexture2D(&textureDesc, nullptr, _hdrBufferTexture.GetAddressOf());

    textureDesc.BindFlags = 0;
    textureDesc.Usage = D3D11_USAGE_STAGING;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    res = _device->CreateTexture2D(&textureDesc, nullptr, _idCpuTexture.GetAddressOf());

    
    res = _device->CreateRenderTargetView(_diffuseTexture.Get(), nullptr, DiffuseRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_metallicTexture.Get(), nullptr, MetallicRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_specularTexture.Get(), nullptr, SpecularRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_depthTexture.Get(), nullptr, DepthRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_emissiveTexture.Get(), nullptr, EmissiveRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_normalAndDepthTexture.Get(),nullptr,NormalAndDepthRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_positionTexture.Get(), nullptr, PositionRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_idTexture.Get(), nullptr, IdRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_skyboxTexture.Get(), nullptr, SkyboxRtv.GetAddressOf());
    res = _device->CreateRenderTargetView(_hdrBufferTexture.Get(), nullptr, HdrBufferRtv.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc1;
    ZeroMemory(&shaderResourceViewDesc1, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc1.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    shaderResourceViewDesc1.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc1.Texture2D.MostDetailedMip=0;
    shaderResourceViewDesc1.Texture2D.MipLevels = 1;

    res = _device->CreateShaderResourceView(_diffuseTexture.Get(), &shaderResourceViewDesc1, DiffuseSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_metallicTexture.Get(), &shaderResourceViewDesc1, MetallicSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_specularTexture.Get(), &shaderResourceViewDesc1, DepthSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_depthTexture.Get(), &shaderResourceViewDesc1, DepthSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_emissiveTexture.Get(), &shaderResourceViewDesc1, EmissiveSrv.GetAddressOf());


    res = _device->CreateShaderResourceView(_normalAndDepthTexture.Get(), &shaderResourceViewDesc1, NormalAndDepthSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_positionTexture.Get(), &shaderResourceViewDesc1, PositionSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_idTexture.Get(), &shaderResourceViewDesc1, IdSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_skyboxTexture.Get(), &shaderResourceViewDesc1, SkyboxSrv.GetAddressOf());
    res = _device->CreateShaderResourceView(_hdrBufferTexture.Get(), &shaderResourceViewDesc1, HdrBufferSrv.GetAddressOf());






}



