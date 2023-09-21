#include "RenderInitSystem.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "RenderContext.h"
#include "../../../vendor/WICTextureLoader.h"
#include "../EngineContext.h"
#include "../HardwareContext.h"
#include "../../Core/ServiceLocator.h"
#include "../../Tools/ImageLoader.h"
#include "../MeshLoader.h"

void RenderInitSystem::Init()
{
	ServiceLocator::instance()->Register<RenderContext>();
    
    hc = ServiceLocator::instance()->Get<HardwareContext>();
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();

    ec->scene=std::make_unique<Scene>();

    rc->cubeMesh = MeshLoader::LoadSimpleMesh("./Engine/Assets/Cube.fbx");

    using namespace DirectX::SimpleMath;
    {
        std::vector<int> quadIndex = { 3,1,0,2,1,3 };
        std::vector<Vector4> quadCoords = {
            Vector4(-1.0f, -1.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f,0.0f,1.0f),
        Vector4(1.0f, -1.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f,0.0f,1.0f),
        Vector4(1.0f,  1.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f,0.0f,1.0f),
        Vector4(-1.0f,  1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f,0.0f,1.0f)
        };

        rc->indexQuadBuffer = std::make_unique<Buffer>(hc->device.Get());
        rc->indexQuadBuffer->Initialize(quadIndex);
        rc->vertexQuadBuffer = std::make_unique<Buffer>(hc->device.Get());
        rc->vertexQuadBuffer->Initialize(quadCoords);
    }


    rc->opaqueConstBuffer = std::make_unique<Buffer>(hc->device.Get());
    rc->opaqueConstBuffer->Initialize(sizeof(CB_BaseEditorBuffer));

    rc->lightConstBuffer = std::make_unique<Buffer>(hc->device.Get());
    rc->lightConstBuffer->Initialize(sizeof(CB_LightBuffer));

    rc->shadowConstBuffer = std::make_unique<Buffer>(hc->device.Get());
    rc->shadowConstBuffer->Initialize(sizeof(CB_ShadowBuffer));

    rc->shadowPointlightConstBuffer = std::make_unique<Buffer>(hc->device.Get());
    rc->shadowPointlightConstBuffer->Initialize(sizeof(CB_PointlightShadowBuffer));

    rc->gBuffer = std::make_unique<GBuffer>(hc->device);
    rc->gBuffer->Initialize(hc->window->GetWidth(), hc->window->GetHeight());

    rc->rtvs[0] = rc->gBuffer->normalRTV.Get();
    rc->rtvs[1] = rc->gBuffer->diffuseRTV.Get();
    rc->rtvs[2] = rc->gBuffer->positionRTV.Get();
    rc->rtvs[3] = rc->gBuffer->depthRTV.Get();
    rc->rtvs[4] = rc->gBuffer->specularRTV.Get();

    rc->editorBillboardRtvs[0] = hc->renderTarget->renderTargetView.Get();
    rc->editorBillboardRtvs[1] = rc->gBuffer->depthRTV.Get();


    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = hc->device->CreateSamplerState(&sampDesc, rc->samplerState.GetAddressOf()); //Create sampler state


    D3D11_DEPTH_STENCIL_DESC depthstencildesc;
    ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthstencildesc.DepthEnable = true;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
    depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

    hr = hc->device->CreateDepthStencilState(&depthstencildesc, rc->offStencilState.GetAddressOf());

    depthstencildesc.DepthEnable = true;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
    depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL; //yup

    depthstencildesc.StencilEnable = true;
    depthstencildesc.StencilReadMask = 0xFF;
    depthstencildesc.StencilWriteMask = 0xFF;
    depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
    depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
    depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    hr = hc->device->CreateDepthStencilState(&depthstencildesc, rc->backFaceStencilState.GetAddressOf());

    depthstencildesc.DepthEnable = true;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;

    depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;

    depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
    depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
    depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    hr = hc->device->CreateDepthStencilState(&depthstencildesc, rc->frontFaceStencilState.GetAddressOf());


    depthstencildesc.DepthEnable = true;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;

    depthstencildesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
    depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
    depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    hr = hc->device->CreateDepthStencilState(&depthstencildesc, rc->finalPassStencilState.GetAddressOf());


    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    blendDesc.IndependentBlendEnable = false;

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));

    rtbd.BlendEnable = true;

    rtbd.SrcBlend = D3D11_BLEND_ONE;
    rtbd.DestBlend = D3D11_BLEND_ONE;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ZERO;
    //!!!! ???
    rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    blendDesc.RenderTarget[0] = rtbd;

    hr = hc->device->CreateBlendState(&blendDesc, rc->lightBlendState.GetAddressOf());


    D3D11_BLEND_DESC blendDesc2;
    ZeroMemory(&blendDesc2, sizeof(blendDesc2));

    blendDesc2.IndependentBlendEnable = false;
    blendDesc2.AlphaToCoverageEnable = false;

    D3D11_RENDER_TARGET_BLEND_DESC rtbd2;
    ZeroMemory(&rtbd2, sizeof(rtbd2));

    rtbd2.BlendEnable = true;

    rtbd2.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rtbd2.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd2.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd2.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd2.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtbd2.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd2.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    blendDesc2.RenderTarget[0] = rtbd2;

    hr = hc->device->CreateBlendState(&blendDesc2, rc->particlesBlendState.GetAddressOf());



    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_FRONT;

    hc->device->CreateRasterizerState(&desc, rc->cullFrontRS.GetAddressOf());

    desc.CullMode = D3D11_CULL_BACK;
    hc->device->CreateRasterizerState(&desc, rc->cullBackRS.GetAddressOf());

    //**Shadows**

    D3D11_TEXTURE2D_DESC shadowMapDesc;
    ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
    shadowMapDesc.Width = rc->SHADOWMAP_WIDTH;
    shadowMapDesc.Height = rc->SHADOWMAP_HEIGHT;
    shadowMapDesc.MipLevels = 1;
    shadowMapDesc.ArraySize = 4;
    shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    shadowMapDesc.SampleDesc.Count = 1;
    shadowMapDesc.SampleDesc.Quality = 0;
    shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
    shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    shadowMapDesc.CPUAccessFlags = 0;
    shadowMapDesc.MiscFlags = 0;

    hr = hc->device->CreateTexture2D(&shadowMapDesc, nullptr, &rc->texture_);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewDesc.Texture2DArray.ArraySize = 4;

    hr = hc->device->CreateDepthStencilView(rc->texture_, &depthStencilViewDesc, &rc->shadowStencilView);

    

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
    shaderResourceViewDesc.Texture2DArray.ArraySize = 4;


    hr = hc->device->CreateShaderResourceView(rc->texture_, &shaderResourceViewDesc, &rc->shadowResourceView);

    //Shadow maps -->

    D3D11_TEXTURE2D_DESC shadowMapDesc_;
    ZeroMemory(&shadowMapDesc_, sizeof(D3D11_TEXTURE2D_DESC));
    shadowMapDesc_.Width = hc->window->GetWidth();
    shadowMapDesc_.Height = hc->window->GetHeight();
    shadowMapDesc_.MipLevels = 1;
    shadowMapDesc_.Format = DXGI_FORMAT_R32_TYPELESS;
    shadowMapDesc_.SampleDesc.Count = 1;
    shadowMapDesc_.ArraySize = 1;
    shadowMapDesc_.SampleDesc.Quality = 0;
    shadowMapDesc_.Usage = D3D11_USAGE_DEFAULT;
    shadowMapDesc_.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    shadowMapDesc_.CPUAccessFlags = 0;
    shadowMapDesc_.MiscFlags = 0;

    hr = hc->device->CreateTexture2D(&shadowMapDesc_, nullptr, &rc->texturePCF);

    /*D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewDesc.Texture2DArray.ArraySize = 4;

    hr = hc->device->CreateDepthStencilView(rc->texture_, &depthStencilViewDesc, &rc->shadowStencilView);*/

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc_;
    ZeroMemory(&shaderResourceViewDesc_, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc_.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc_.Texture2D.MipLevels = 1;


    hr = hc->device->CreateShaderResourceView(rc->texturePCF, &shaderResourceViewDesc_, &rc->shadowMapResourceView);

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target view.
    hr = hc->device->CreateRenderTargetView(rc->texturePCF, &renderTargetViewDesc, &rc->shadowMapRTV);

    //<---


    D3D11_DEPTH_STENCIL_DESC depthstencildesc2;
    ZeroMemory(&depthstencildesc2, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthstencildesc2.DepthEnable = true;
    depthstencildesc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthstencildesc2.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hr = hc->device->CreateDepthStencilState(&depthstencildesc2, rc->shadowStencilState.GetAddressOf());

    //**End


    rc->viewport.Width = static_cast<float>(hc->window->GetWidth());
    rc->viewport.Height = static_cast<float>(hc->window->GetHeight());
    rc->viewport.TopLeftX = 0;
    rc->viewport.TopLeftY = 0;
    rc->viewport.MinDepth = 0;
    rc->viewport.MaxDepth = 1.0f;

    hc->context->RSSetViewports(1, &rc->viewport);


    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc2;


    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = rc->SHADOWMAP_WIDTH;
    textureDesc.Height = rc->SHADOWMAP_HEIGHT;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 4;
    textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    result = hc->device->CreateTexture2D(&textureDesc, NULL, &rc->m_renderTargetTexture);
    // Setup the description of the render target view.

    renderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    renderTargetViewDesc.Texture2DArray.MipSlice = 0;
    renderTargetViewDesc.Texture2DArray.ArraySize = 4;
    renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;

    // Create the render target view.
    result = hc->device->CreateRenderTargetView(rc->m_renderTargetTexture, &renderTargetViewDesc, &rc->m_renderTargetView);

    //result=engine->device->CreateRenderTargetView(m_renderTargetTexture, nullptr, &m_renderTargetView);
    // Setup the description of the shader resource view.
    shaderResourceViewDesc2.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc2.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc2.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc2.Texture2DArray.ArraySize = 4;
    shaderResourceViewDesc2.Texture2DArray.FirstArraySlice = 0;


    // Create the shader resource view.
    result = hc->device->CreateShaderResourceView(rc->m_renderTargetTexture, &shaderResourceViewDesc2, &rc->m_shaderResourceView);


    //Creating instance ID Texture
    D3D11_TEXTURE2D_DESC desc2;
    ZeroMemory(&desc2, sizeof(desc2));
    desc2.Width = hc->window->GetWidth();
    desc2.Height = hc->window->GetHeight();
    desc2.MipLevels = 1;
    desc2.ArraySize = 1;
    desc2.Format = DXGI_FORMAT_R32_FLOAT;
    desc2.SampleDesc.Count = 1;
    desc2.Usage = D3D11_USAGE_DEFAULT;
    desc2.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc2.CPUAccessFlags = 0;

    result = hc->device->CreateTexture2D(&desc2, NULL, rc->entityIdTexture.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    result = hc->device->CreateShaderResourceView(rc->entityIdTexture.Get(), &srvDesc, rc->entityIdSRV.GetAddressOf());


    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    // Create the texture sampler state.
    auto res2 = hc->device->CreateSamplerState(&samplerDesc, rc->samplerDepthState.GetAddressOf());


    InitSkybox();



    rc->opaqueShader = std::make_unique<Shader>();
    rc->opaqueShader->Initialize(L"./Engine/Assets/Shaders/OpaqueShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_NORMAL | USE_COLOR);

    rc->dirLightShader = std::make_unique<Shader>();
    rc->dirLightShader->Initialize(L"./Engine/Assets/Shaders/LightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PS_Directional");

    rc->ambientLightShader = std::make_unique<Shader>();
    rc->ambientLightShader->Initialize(L"./Engine/Assets/Shaders/LightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PS_Ambient");

    rc->pointLightShader = std::make_unique<Shader>();
    rc->pointLightShader->Initialize(L"./Engine/Assets/Shaders/PointLightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PS_PointLight");

    rc->spotLightShader = std::make_unique<Shader>();
    rc->spotLightShader->Initialize(L"./Engine/Assets/Shaders/SpotLightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PS_SpotLight");

    rc->stencilPassShader = std::make_unique<Shader>();
    rc->stencilPassShader->Initialize(L"./Engine/Assets/Shaders/StencilPassShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR | USE_NORMAL);

    rc->shadowShader = std::make_unique<Shader>();
    rc->shadowShader->Initialize(L"./Engine/Assets/Shaders/ShadowShader.hlsl",
        COMPILE_VERTEX | COMPILE_GEOM, USE_POSITION | USE_COLOR | USE_NORMAL, "DepthVertexShader");

    rc->shadowPointLightShader = std::make_unique<Shader>();
    rc->shadowPointLightShader->Initialize(L"./Engine/Assets/Shaders/ShadowPointlightShader.hlsl",
        COMPILE_VERTEX | COMPILE_GEOM | COMPILE_PIXEL, USE_POSITION | USE_COLOR | USE_NORMAL, "DepthVertexShader","PSMain");

    rc->billboardShader = std::make_unique<Shader>();
    rc->billboardShader->Initialize(L"./Engine/Assets/Shaders/SpriteBillboardShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR | USE_NORMAL);

    rc->shadowMapGenerator = std::make_unique<Shader>();
    rc->shadowMapGenerator->Initialize(L"./Engine/Assets/Shaders/ShadowMapGenerator.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR,"VSMain","PS_ShadowMapGenerator");

    rc->shadowMapPointLightGenerator = std::make_unique<Shader>();
    rc->shadowMapPointLightGenerator->Initialize(L"./Engine/Assets/Shaders/ShadowPointLightGenerator.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VS", "PS");

    rc->shadowMapFinal= std::make_unique<Shader>();
    rc->shadowMapFinal->Initialize(L"./Engine/Assets/Shaders/ShadowFinal.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VS", "PS");


    rc->skyBoxShader = std::make_unique<Shader>();
    rc->skyBoxShader->Initialize(L"./Engine/Assets/Shaders/Skybox.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VS", "PS");

}

void RenderInitSystem::Run()
{
}

void RenderInitSystem::Destroy()
{
}

void RenderInitSystem::InitSkybox()
{

    int resolution = 512;
    D3D11_TEXTURE2D_DESC textureDesc_ = {};
    textureDesc_.Width = resolution;
    textureDesc_.Height = resolution;
    textureDesc_.MipLevels = 1;
    textureDesc_.ArraySize = 6;
    textureDesc_.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc_.SampleDesc.Count = 1;
    textureDesc_.SampleDesc.Quality = 0;
    textureDesc_.Usage = D3D11_USAGE_DEFAULT;
    textureDesc_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc_.CPUAccessFlags = 0;
    textureDesc_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


    D3D11_SUBRESOURCE_DATA data[6];
    for (int i = 0; i < 6; i++)
    {
        std::string file = "./Engine/Assets/SkyBox/skybox_";
        file += std::to_string(i + 1);
        file += ".png";
        int width = 0;
        int height = 0;
        data[i].pSysMem = ImageLoader::LoadImageFromFile(file.c_str(), &width, &height);
        data[i].SysMemPitch = sizeof(char) * width * 4;
        data[i].SysMemSlicePitch = 0;
    }
    HRESULT result = hc->device->CreateTexture2D(&textureDesc_, data, rc->skyboxTexture.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc_.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    result = hc->device->CreateShaderResourceView(rc->skyboxTexture.Get(), &srvDesc, rc->skyboxSRV.GetAddressOf());
}
