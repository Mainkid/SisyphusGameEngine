#pragma once
#include <vector>

#include "Shaders/AmbientLightShader.h"
#include "Shaders/DirLightShader.h"
#include "Shaders/OpaqueShader.h"
#include "Shaders/PointLightShader.h"
#include "Shaders/SpotLightShader.h"
#include "Shaders/StencilPassShader.h"
#include "Shaders/PointLightShader.h"
#include "Shaders/ShadowShader.h"

#include "../../Components/LightComponent.h"
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



    //std::vector<BaseLight*> baseLights;

    ID3D11RenderTargetView* rtvs[5];

    std::shared_ptr<OpaqueShader> opaqueShader;
    std::shared_ptr<DirLightShader> dirLightShader;
    std::shared_ptr<AmbientLightShader> ambientLightShader;
    std::shared_ptr<PointLightShader> pointLightShader;
    std::shared_ptr<StencilPassShader> stencilPassShader;
    std::shared_ptr<SpotLightShader> spotLightShader;
    std::shared_ptr<ShadowShader> shadowShader;


    std::shared_ptr<GBuffer> gBuffer;
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


    EngineCore* engine;

    D3D11_VIEWPORT viewport = {};

    bool helper = false;

    /*
     * Screen Space Quad
     *
     */


    std::shared_ptr<Buffer> indexQuadBuffer;
    std::shared_ptr<Buffer> vertexQuadBuffer;
    std::shared_ptr<Buffer> opaqueConstBuffer;
    std::shared_ptr<Buffer> lightConstBuffer;

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

    std::shared_ptr<Buffer> shadowConstBuffer;

    //DebugWindow* debugWindow;
    //TextureShaderClass* textureShader;


    /*
     *
     * Particles
     *
     */

};

