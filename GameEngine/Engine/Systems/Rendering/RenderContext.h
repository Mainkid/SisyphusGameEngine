#pragma once
#include <memory>
#include <vector>
#include <set>
#include "../../Components/Material.h"
#include "../../Core/Rendering/GBuffer.h"
#include "../../Core/Graphics/Shader.h"
#include "../../Core/IService.h"
#include "../../Components/Mesh.h"

struct RenderContext : public IService
{
    ID3D11RenderTargetView* rtvs[8];
    ID3D11RenderTargetView* editorBillboardRtvs[2];

    std::unique_ptr<Shader> opaqueShader;
    std::unique_ptr<Shader> dirLightShader;
    std::unique_ptr<Shader> ambientLightShader;
    std::unique_ptr<Shader> pointLightShader;
    std::unique_ptr<Shader> stencilPassShader;
    std::unique_ptr<Shader> spotLightShader;
    std::unique_ptr<Shader> shadowShader;
    std::unique_ptr<Shader> shadowPointLightShader;
    std::unique_ptr<Shader> billboardShader;
    std::unique_ptr<Shader> shadowMapGenerator;
    std::unique_ptr<Shader> shadowMapPointLightGenerator;
    std::unique_ptr<Shader> skyBoxShader;
    std::unique_ptr<Shader> toneMapper;
    std::unique_ptr<Shader> editorGridRenderer;
    std::unique_ptr<Shader> gaussianBlurX;
    std::unique_ptr<Shader> gaussianBlurY;
    

    std::shared_ptr<Mesh> cubeMesh;

    std::unique_ptr<GBuffer> gBuffer;
    Microsoft::WRL::ComPtr<ID3D11BlendState> lightBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> particlesBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> gridBlendState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerDepthState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> pointSampler;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> offStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> backFaceStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> frontFaceStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> finalPassStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> writeOnlyStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> cullFrontRS;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> cullBackRS;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> cullNoneRS;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> entityIdTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> entityIdSRV;



    D3D11_VIEWPORT viewport = {};

    bool helper = false;

    /*
     * Screen Space Quad
     *
     */


    std::unique_ptr<Buffer> indexQuadBuffer;
    std::unique_ptr<Buffer> vertexQuadBuffer;
    std::unique_ptr<Buffer> opaqueConstBuffer;
    std::unique_ptr<Buffer> lightConstBuffer;

    /*
     * Light AABB
     *
     */

    std::shared_ptr<Buffer> indexLightAABBBuffer;
    std::shared_ptr<Buffer> vertexLightAABBBuffer;

    /*
     * Shadows
     *
     */
    const int SHADOWMAP_WIDTH = 1024;
    const int SHADOWMAP_HEIGHT = 1024;
    Microsoft::WRL::ComPtr <ID3D11DepthStencilState> shadowStencilState;
    Microsoft::WRL::ComPtr <ID3D11DepthStencilView> shadowStencilView;
    Microsoft::WRL::ComPtr <ID3D11DepthStencilView> shadowPointLightStencilView;
    ID3D11ShaderResourceView* shadowResourceView;
    Microsoft::WRL::ComPtr <ID3D11SamplerState> shadowMapSampler;
    
    ID3D11Texture2D* texture_;
    
    ID3D11Texture2D* m_renderTargetTexture;
    ID3D11Texture2D* m_bluredShadowTexture;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11ShaderResourceView* m_shaderResourceView;
    ID3D11UnorderedAccessView* m_shadowUAV;
    ID3D11ShaderResourceView* m_bluredShadowSRV;
    ID3D11RenderTargetView* m_bluredShadowRTV;

    /*
        PCF
    */

    ID3D11ShaderResourceView* shadowMapResourceView;
    ID3D11Texture2D* texturePCF;
    ID3D11RenderTargetView* shadowMapRTV;

    /*
        SkyBox
    */

    Microsoft::WRL::ComPtr < ID3D11Texture2D> skyboxTexture;
    Microsoft::WRL::ComPtr < ID3D11ShaderResourceView> skyboxSRV;

    std::unique_ptr<Buffer> shadowConstBuffer;
    std::unique_ptr<Buffer> shadowPointlightConstBuffer;

    std::set<std::unique_ptr<Material>> materialSet;
};

