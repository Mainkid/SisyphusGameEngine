#pragma once
#include <memory>
#include <wrl/client.h>

#include "DDSTextureLoader.h"

class Game;

class GBuffer
{
public:
    friend class RenderPipeline;
    GBuffer(Microsoft::WRL::ComPtr<ID3D11Device> _device);
    void Initialize(int height, int width);
    

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularSRV;
    

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> diffuseRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> normalRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> positionRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> depthRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> specularRTV;

private:
    int t_height;
    int t_width;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> diffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> normalTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> positionTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> specularTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthCpuTexture;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthCpuSRV;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    
};
