#pragma once
#include <memory>
#include <wrl/client.h>

#include "DDSTextureLoader.h"

class Game;

class GBuffer
{
public:
    friend class RenderHelper;
    friend class RenderPipeline;
    GBuffer(Microsoft::WRL::ComPtr<ID3D11Device> _device);
    void Initialize(int height, int width);
    

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metallicSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissiveSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthSRV;
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> skyboxSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> HDRBufferSRV;
    
   

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> diffuseRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> metallicRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> specularRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> roughnessRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> emissiveRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> normalRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> positionRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> depthRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> skyboxRTV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> HDRBufferRTV;
private:
    int t_height;
    int t_width;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> diffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> metallicTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> specularTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> roughnessTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> emissiveTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> normalTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> positionTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthCpuTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> skyboxTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> HDRBufferTexture;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthCpuSRV;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    
};
