#pragma once
#include <vector>

#include "../Graphics/Shader.h"
#include "../../Components/LightComponent.h"
#include "../../Components/ParticleComponent.h"
#include "../Graphics/ConstantBuffer.h"
#include "GBuffer.h"

class EngineCore;

class RenderPipeline
{
public:
    RenderPipeline(EngineCore* _game);
    void Initialize();
    void CompileShaders();

    void Render();
    void ShadowPass();
    void OpaquePass();
    void LightPass();
    void ParticlePass();
    //void GetBoundingBoxForLight(BaseLight* light);
    float GetPixelValue(int clickX, int clickY);
    DirectX::SimpleMath::Vector2 GetRtvResolution();


    //std::vector<BaseLight*> baseLights;

    ID3D11RenderTargetView* rtvs[5];

    std::unique_ptr<Shader> opaqueShader;
    std::unique_ptr<Shader> dirLightShader;
    std::unique_ptr<Shader> ambientLightShader;
    std::unique_ptr<Shader> pointLightShader;
    std::unique_ptr<Shader> stencilPassShader;
    std::unique_ptr<Shader> spotLightShader;
    std::unique_ptr<Shader> shadowShader;


    std::unique_ptr<GBuffer> gBuffer;
    Microsoft::WRL::ComPtr<ID3D11BlendState> lightBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> particlesBlendState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerDepthState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> offStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> backFaceStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> frontFaceStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> finalPassStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> cullFrontRS;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> cullBackRS;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> entityIdTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> entityIdSRV;


    EngineCore* engine;

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
    ID3D11ShaderResourceView* shadowResourceView;
    ID3D11Texture2D* texture_;
    ID3D11Texture2D* m_renderTargetTexture;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11ShaderResourceView* m_shaderResourceView;

    

    std::unique_ptr<Buffer> shadowConstBuffer;

    //DebugWindow* debugWindow;
    //TextureShaderClass* textureShader;


    /*
     *
     * Particles
     *
     */

};

