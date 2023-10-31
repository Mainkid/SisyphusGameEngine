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
    
    //Diffuse color (RGBA)
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DiffuseSrv;

    //Metallic (RGB) + Roughness (A)
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> MetallicSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SpecularSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> HbaoSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> EmissiveSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> NormalSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> PositionSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> IdSrv;
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> SkyboxSrv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> HdrBufferSrv;
    
   

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> DiffuseRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> MetallicRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> SpecularRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> HbaoRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> EmissiveRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> NormalRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> PositionRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> IdRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> SkyboxRtv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> HdrBufferRtv;
private:
    int _height;
    int _width;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _diffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _metallicTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _specularTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _hbaoTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _emissiveTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _normalTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _positionTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _idTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _idCpuTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _skyboxTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _hdrBufferTexture;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _idCpuSrv;

    Microsoft::WRL::ComPtr<ID3D11Device> _device;
    
};
