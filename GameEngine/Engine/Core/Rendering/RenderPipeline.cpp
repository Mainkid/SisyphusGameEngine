#include "RenderPipeline.h"
#include "../EngineCore.h"



RenderPipeline::RenderPipeline(EngineCore* _engine)
{
    this->engine = _engine;
}

//void RenderPipeline::GetBoundingBoxForLight(BaseLight* light)
//{
//
//    //Cube.fbx
//}


void RenderPipeline::Initialize()
{
    using namespace DirectX::SimpleMath;
    {
        std::vector<int> quadIndex = { 3,1,0,2,1,3 };
        std::vector<Vector4> quadCoords = {
            Vector4(-1.0f, -1.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f,0.0f,1.0f),
        Vector4(1.0f, -1.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f,0.0f,1.0f),
        Vector4(1.0f,  1.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f,0.0f,1.0f),
        Vector4(-1.0f,  1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f,0.0f,1.0f)
        };

        this->indexQuadBuffer = std::make_unique<Buffer>(engine->device.Get());
        indexQuadBuffer->Initialize(quadIndex);

        this->vertexQuadBuffer = std::make_unique<Buffer>(engine->device.Get());
        vertexQuadBuffer->Initialize(quadCoords);
    }


    opaqueConstBuffer = std::make_unique<Buffer>(engine->device.Get());
    opaqueConstBuffer->Initialize(sizeof(CB_BaseEditorBuffer));

    lightConstBuffer = std::make_unique<Buffer>(engine->device.Get());
    lightConstBuffer->Initialize(sizeof(CB_LightBuffer));

    shadowConstBuffer = std::make_unique<Buffer>(engine->device.Get());
    shadowConstBuffer->Initialize(sizeof(CB_ShadowBuffer));

    gBuffer = std::make_unique<GBuffer>(engine->device);
    gBuffer->Initialize(engine->window->GetWidth(), engine->window->GetHeight());

    rtvs[0] = gBuffer->normalRTV.Get();
    rtvs[1] = gBuffer->diffuseRTV.Get();
    rtvs[2] = gBuffer->positionRTV.Get();
    rtvs[3] = gBuffer->depthRTV.Get();
    rtvs[4] = gBuffer->specularRTV.Get();

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = engine->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state


    D3D11_DEPTH_STENCIL_DESC depthstencildesc;
    ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthstencildesc.DepthEnable = true;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
    depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

    hr = engine->device->CreateDepthStencilState(&depthstencildesc, this->offStencilState.GetAddressOf());

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

    hr = engine->device->CreateDepthStencilState(&depthstencildesc, this->backFaceStencilState.GetAddressOf());

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

    hr = engine->device->CreateDepthStencilState(&depthstencildesc, this->frontFaceStencilState.GetAddressOf());


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

    hr = engine->device->CreateDepthStencilState(&depthstencildesc, this->finalPassStencilState.GetAddressOf());


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

    hr = engine->device->CreateBlendState(&blendDesc, lightBlendState.GetAddressOf());


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

    hr = engine->device->CreateBlendState(&blendDesc2, particlesBlendState.GetAddressOf());



    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_FRONT;

    engine->device->CreateRasterizerState(&desc, cullFrontRS.GetAddressOf());

    desc.CullMode = D3D11_CULL_BACK;
    engine->device->CreateRasterizerState(&desc, cullBackRS.GetAddressOf());

    //**Shadows**

    D3D11_TEXTURE2D_DESC shadowMapDesc;
    ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
    shadowMapDesc.Width = SHADOWMAP_WIDTH;
    shadowMapDesc.Height = SHADOWMAP_HEIGHT;
    shadowMapDesc.MipLevels = 1;
    shadowMapDesc.ArraySize = 4;
    shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    shadowMapDesc.SampleDesc.Count = 1;
    shadowMapDesc.SampleDesc.Quality = 0;
    shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
    shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    shadowMapDesc.CPUAccessFlags = 0;
    shadowMapDesc.MiscFlags = 0;

    hr = engine->device->CreateTexture2D(&shadowMapDesc, nullptr, &texture_);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewDesc.Texture2DArray.ArraySize = 4;

    hr = engine->device->CreateDepthStencilView(texture_, &depthStencilViewDesc, &shadowStencilView);

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
    shaderResourceViewDesc.Texture2DArray.ArraySize = 4;


    hr = engine->device->CreateShaderResourceView(texture_, &shaderResourceViewDesc, &shadowResourceView);


    D3D11_DEPTH_STENCIL_DESC depthstencildesc2;
    ZeroMemory(&depthstencildesc2, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthstencildesc2.DepthEnable = true;
    depthstencildesc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthstencildesc2.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hr = engine->device->CreateDepthStencilState(&depthstencildesc2, this->shadowStencilState.GetAddressOf());

    //**End


    viewport.Width = static_cast<float>(engine->window->GetWidth());
    viewport.Height = static_cast<float>(engine->window->GetHeight());
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    engine->context->RSSetViewports(1, &viewport);


    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc2;


    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = SHADOWMAP_WIDTH;
    textureDesc.Height = SHADOWMAP_HEIGHT;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 4;
    textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    result = engine->device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
    // Setup the description of the render target view.

    renderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    renderTargetViewDesc.Texture2DArray.MipSlice = 0;
    renderTargetViewDesc.Texture2DArray.ArraySize = 4;
    renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;

    // Create the render target view.
    result = engine->device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);

    //result=engine->device->CreateRenderTargetView(m_renderTargetTexture, nullptr, &m_renderTargetView);
    // Setup the description of the shader resource view.
    shaderResourceViewDesc2.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc2.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc2.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc2.Texture2DArray.ArraySize = 4;
    shaderResourceViewDesc2.Texture2DArray.FirstArraySlice = 0;


    // Create the shader resource view.
    result = engine->device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc2, &m_shaderResourceView);


    //Creating instance ID Texture

    D3D11_TEXTURE2D_DESC desc2;
    ZeroMemory(&desc2, sizeof(desc2));
    desc2.Width = engine->window->GetWidth();
    desc2.Height = engine->window->GetHeight();
    desc2.MipLevels = 1;
    desc2.ArraySize = 1;
    desc2.Format = DXGI_FORMAT_R32_FLOAT;
    desc2.SampleDesc.Count = 1;
    desc2.Usage = D3D11_USAGE_DEFAULT;
    desc2.BindFlags = D3D11_BIND_SHADER_RESOURCE| D3D11_BIND_RENDER_TARGET;
    desc2.CPUAccessFlags = 0;

    result = engine->device->CreateTexture2D(&desc2, NULL, entityIdTexture.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    result = engine->device->CreateShaderResourceView(entityIdTexture.Get(), &srvDesc, entityIdSRV.GetAddressOf());


    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    // Create the texture sampler state.
    auto res2 = engine->device->CreateSamplerState(&samplerDesc, samplerDepthState.GetAddressOf());


    CompileShaders();

}

void RenderPipeline::CompileShaders()
{
    opaqueShader = std::make_unique<Shader>();
    opaqueShader->Initialize(L"./Engine/Core/Rendering/Shaders/OpaqueShader.hlsl",
        COMPILE_VERTEX|COMPILE_PIXEL,USE_POSITION|USE_NORMAL|USE_COLOR);

    dirLightShader = std::make_unique<Shader>();
    dirLightShader->Initialize(L"./Engine/Core/Rendering/Shaders/LightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL,USE_POSITION|USE_COLOR,"VSMain","PS_Directional");

    ambientLightShader = std::make_unique<Shader>();
    ambientLightShader->Initialize(L"./Engine/Core/Rendering/Shaders/LightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PS_Ambient");

    pointLightShader = std::make_unique<Shader>();
    pointLightShader->Initialize(L"./Engine/Core/Rendering/Shaders/PointLightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PS_PointLight");

    spotLightShader = std::make_unique<Shader>();
    spotLightShader->Initialize(L"./Engine/Core/Rendering/Shaders/SpotLightShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PS_SpotLight");

    stencilPassShader = std::make_unique<Shader>();
    stencilPassShader->Initialize(L"./Engine/Core/Rendering/Shaders/StencilPassShader.hlsl",
        COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR|USE_NORMAL);

    shadowShader = std::make_unique<Shader>();
    shadowShader->Initialize(L"./Engine/Core/Rendering/Shaders/ShadowShader.hlsl",
        COMPILE_VERTEX | COMPILE_GEOM, USE_POSITION | USE_COLOR | USE_NORMAL,"DepthVertexShader");
}

void RenderPipeline::Render()
{
    float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float bgEntitiesColor[] = { -1,-1,-1,1 };
    engine->context->ClearRenderTargetView(engine->rtv.Get(), bgColor);
    engine->context->ClearRenderTargetView(rtvs[0], bgColor);
    engine->context->ClearRenderTargetView(rtvs[1], bgColor);
    engine->context->ClearRenderTargetView(rtvs[2], bgColor);
    engine->context->ClearRenderTargetView(rtvs[3], bgEntitiesColor);
    engine->context->ClearRenderTargetView(rtvs[4], bgColor);
    engine->context->ClearDepthStencilView(engine->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    engine->context->RSSetViewports(1, &viewport);

    ShadowPass();
    OpaquePass();
    LightPass();
    ParticlePass();

    
    
    engine->context->ClearState();
    engine->context->OMSetRenderTargets(1, engine->rtv.GetAddressOf(), engine->depthStencilView.Get());
}

void RenderPipeline::ShadowPass()
{
    engine->context->RSSetState(cullBackRS.Get());

    engine->context->ClearDepthStencilView(this->shadowStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    for (auto& entity : engine->scene->registry.view<LightComponent>())
    {
        LightComponent& light = engine->scene->registry.get<LightComponent>(entity);
        if (light.lightType == LightType::Directional)
        {
            light.GenerateViewMatrix(Vector3(engine->cameraController->camera->pos));
            light.GenerateOrthoFromFrustum(engine->cameraController->GetViewMatrix(), engine->cameraController->GetProjectionMatrix());

            light.GenerateOrthosFromFrustum(engine->cameraController->GetViewMatrix(), engine->cameraController->GetProjectionMatrix(), engine->cameraController->camera->farZ);


            //renderTexture->SetRenderTarget(context.Get(), shadowStencilView.Get());
            engine->context->OMSetRenderTargets(1, &m_renderTargetView, shadowStencilView.Get());
            m_renderTargetTexture = nullptr;



            for (auto& go_entity : engine->scene->registry.view<MeshComponent>())
            {
                TransformComponent& transform = engine->scene->registry.get<TransformComponent>(go_entity);
                MeshComponent& meshComponent = engine->scene->registry.get<MeshComponent>(go_entity);
                CB_ShadowBuffer dataShadow;
                //dataShadow.baseData.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
                dataShadow.baseData.world = transform.ConstructTransformMatrix();

                //!dataShadow.baseData.worldViewProj =
                //    engineActor->transform->world * engineActor->transform->GetViewMatrix() *
                //    engine->cameraController->GetViewMatrix() * engine->cameraController->GetProjectionMatrix();

                dataShadow.baseData.worldViewProj =
                    transform.ConstructTransformMatrix() *
                    engine->cameraController->GetViewMatrix() * engine->cameraController->GetProjectionMatrix();

                dataShadow.baseData.worldView = transform.ConstructTransformMatrix() *
                    engine->cameraController->GetViewMatrix();

                dataShadow.baseData.worldViewInverseTranspose =
                    DirectX::XMMatrixTranspose(
                        DirectX::XMMatrixInverse(nullptr,
                            transform.ConstructTransformMatrix()*engine->cameraController->GetViewMatrix()));

                for (int i = 0; i < 4; i++)
                    dataShadow.viewProjs[i] = light.viewMatrices[i] * light.orthoMatrices[i];

                D3D11_MAPPED_SUBRESOURCE mappedResource;
                HRESULT res = engine->context->Map(shadowConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
                CopyMemory(mappedResource.pData, &dataShadow, sizeof(CB_ShadowBuffer));
                engine->context->Unmap(shadowConstBuffer->buffer.Get(), 0);
                engine->context->VSSetConstantBuffers(0, 1, shadowConstBuffer->buffer.GetAddressOf());
                engine->context->PSSetConstantBuffers(0, 1, shadowConstBuffer->buffer.GetAddressOf());
                engine->context->GSSetConstantBuffers(0, 1, shadowConstBuffer->buffer.GetAddressOf());

                for (auto mesh : meshComponent.meshes)
                {
                    UINT offset[1] = { 0 };
                    UINT stride[1] = { 48 };

                    D3D11_VIEWPORT viewport = {};
                    viewport.Width = static_cast<float>(SHADOWMAP_WIDTH);
                    viewport.Height = static_cast<float>(SHADOWMAP_HEIGHT);
                    viewport.TopLeftX = 0;
                    viewport.TopLeftY = 0;
                    viewport.MinDepth = 0;
                    viewport.MaxDepth = 1.0f;

                    engine->context->RSSetViewports(1, &viewport);

                    engine->context->IASetInputLayout(shadowShader->layout.Get());
                    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    engine->context->OMSetRenderTargets(1, &m_renderTargetView, shadowStencilView.Get());
                    engine->context->VSSetShader(shadowShader->vertexShader.Get(), nullptr, 0);
                    engine->context->GSSetShader(shadowShader->geomShader.Get(), nullptr, 0);

                    engine->context->IASetIndexBuffer(mesh->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    //!engine->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                    //    engineActor->model->strides, engineActor->model->offsets);
                    engine->context->IASetVertexBuffers(0, 1, mesh->vertexBuffer->buffer.GetAddressOf(),
                        meshComponent.strides, meshComponent.offsets);
                    engine->context->DrawInstanced(mesh->indexBuffer->size, 4, 0, 0);
                    engine->context->GSSetShader(nullptr, nullptr, 0);





                }


            }
        }
    }



    viewport.Width = static_cast<float>(engine->window->GetWidth());
    viewport.Height = static_cast<float>(engine->window->GetHeight());
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;
    engine->context->RSSetViewports(1, &viewport);
}

void RenderPipeline::OpaquePass()
{
    engine->context->RSSetState(cullBackRS.Get());

    for (auto& entity : engine->scene->registry.view<MeshComponent>())
    {
        CB_BaseEditorBuffer dataOpaque;
        TransformComponent& transformComp= engine->scene->registry.get<TransformComponent>(entity);
        MeshComponent& meshComp = engine->scene->registry.get<MeshComponent>(entity);
        //dataOpaque.world = engineActor->transform->world * engineActor->transform->GetViewMatrix();
        dataOpaque.baseData.world = transformComp.ConstructTransformMatrix();

        //dataOpaque.worldViewProj =
        //    engineActor->transform->world * engineActor->transform->GetViewMatrix() *
        //    engine->cameraController->GetViewMatrix() * engine->cameraController->GetProjectionMatrix();

        dataOpaque.baseData.worldViewProj =
            transformComp.ConstructTransformMatrix() *
            engine->cameraController->GetViewMatrix() * engine->cameraController->GetProjectionMatrix();

        //dataOpaque.worldView = engineActor->transform->world * engineActor->transform->GetViewMatrix() *
        //    engine->cameraController->GetViewMatrix();

        dataOpaque.baseData.worldView = transformComp.ConstructTransformMatrix() *
            engine->cameraController->GetViewMatrix();

        //dataOpaque.worldViewInverseTranspose =
        //    DirectX::XMMatrixTranspose(
        //        DirectX::XMMatrixInverse(nullptr,
        //            engineActor->transform->world * engineActor->transform->GetViewMatrix()));

        dataOpaque.baseData.worldViewInverseTranspose =
            DirectX::XMMatrixTranspose(
                DirectX::XMMatrixInverse(nullptr,
                    transformComp.ConstructTransformMatrix()));

        dataOpaque.instanseID =(uint32_t)entity;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = engine->context->Map(opaqueConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &dataOpaque, sizeof(CB_BaseEditorBuffer));
        engine->context->Unmap(opaqueConstBuffer->buffer.Get(), 0);
        engine->context->VSSetConstantBuffers(0, 1, opaqueConstBuffer->buffer.GetAddressOf());
        engine->context->PSSetConstantBuffers(0, 1, opaqueConstBuffer->buffer.GetAddressOf());

        for (int i = 0; i < meshComp.meshes.size(); i++)
        {

            engine->context->OMSetRenderTargets(5, engine->renderPipeline->rtvs, engine->depthStencilView.Get());
            engine->context->IASetInputLayout(opaqueShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            engine->context->IASetIndexBuffer(meshComp.meshes[i]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, meshComp.meshes[i]->vertexBuffer->buffer.GetAddressOf(),
                meshComp.strides, meshComp.offsets);
            engine->context->PSSetSamplers(0, 1, meshComp.samplerState.GetAddressOf());
            engine->context->OMSetDepthStencilState(engine->depthStencilState.Get(), 0);
            engine->context->VSSetShader(opaqueShader->vertexShader.Get(), nullptr, 0);
            engine->context->PSSetShader(opaqueShader->pixelShader.Get(), nullptr, 0);
            engine->context->PSSetShaderResources(0, 1, meshComp.texture.GetAddressOf());
            //engine->context->PSSetShaderResources(1, 1, gBuffer->positionSRV.GetAddressOf());

            engine->context->DrawIndexed(meshComp.meshes[i]->indexBuffer->size, 0, 0);
        }
    }

    //engine->context->CopySubresourceRegion(entityIdTexture.Get(), 0, 0, 0, 3, gBuffer->depthTexture.Get(), 0, NULL);

}

void RenderPipeline::LightPass()
{
    CB_LightBuffer lightBuffer;

    float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    ID3D11ShaderResourceView* resources[] = { gBuffer->diffuseSRV.Get(),gBuffer->normalSRV.Get(),gBuffer->positionSRV.Get(),gBuffer->depthSRV.Get(),gBuffer->specularSRV.Get() };
    //engine->context->ClearRenderTargetView(engine->rtv.Get(), bgColor);
    engine->renderTarget->ClearRenderTarget(engine->depthStencilView.Get());
    UINT strides[1] = { 32 };
    UINT offsets[1] = { 0 };


    for (auto& entity : engine->scene->registry.view<LightComponent>())
    {
        LightComponent& light= engine->scene->registry.get<LightComponent>(entity);
        lightBuffer.lightData.Pos = light.position;
        lightBuffer.lightData.Color = light.color;
        lightBuffer.lightData.Dir = light.direction;
        lightBuffer.lightData.additiveParams = light.params;
        lightBuffer.eyePos = DirectX::SimpleMath::Vector4(engine->cameraController->camera->pos.x,
            engine->cameraController->camera->pos.y,
            engine->cameraController->camera->pos.z,
            1.0f);

        engine->context->ClearDepthStencilView(engine->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1, 0);

        if (light.lightType == LightType::Directional || light.lightType == LightType::Ambient)
        {
            lightBuffer.baseData.world = DirectX::SimpleMath::Matrix::Identity;
            lightBuffer.baseData.worldView = DirectX::SimpleMath::Matrix::Identity * engine->cameraController->GetViewMatrix();
            lightBuffer.baseData.worldViewProj = DirectX::SimpleMath::Matrix::Identity * engine->cameraController->GetViewMatrix() * engine->cameraController->GetProjectionMatrix();
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;


            if (light.lightType == LightType::Directional)
                for (int i = 0; i < 4; i++)
                {
                    lightBuffer.distances[i] = light.distances[i];
                    lightBuffer.viewProjs[i] = light.viewMatrices[i] * light.orthoMatrices[i];
                }


        }
        else if (light.lightType == LightType::PointLight)
        {
            using namespace DirectX::SimpleMath;
            lightBuffer.baseData.world = Matrix::CreateScale(light.params.x, light.params.x, light.params.x) * Matrix::CreateTranslation(light.position.x, light.position.y, light.position.z);

            lightBuffer.baseData.worldView = lightBuffer.baseData.world * engine->cameraController->GetViewMatrix();
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * engine->cameraController->GetProjectionMatrix();
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;
        }
        else if (light.lightType == LightType::SpotLight)
        {
            using namespace DirectX::SimpleMath;
            Vector3 a;
            Vector3 forward = Vector3(0, 0, 1);
            Vector3 dir = Vector3(light.direction);
            dir.Normalize();
            forward.Cross(dir, a);
            a.Normalize();
            float angle = acos(forward.Dot(dir));

            float radius = sqrt(pow((light.params.x / light.params.y), 2.0f) - pow(light.params.x, 2.0f));

            lightBuffer.baseData.world = Matrix::CreateTranslation(Vector3(0,
                0,
                1.0f)) *
                Matrix::CreateScale(radius, radius, light.params.x) *
                Matrix::CreateFromAxisAngle(a, angle) *
                Matrix::CreateTranslation(Vector3(light.position.x,
                    light.position.y,
                    light.position.z));
            lightBuffer.baseData.worldView = lightBuffer.baseData.world * engine->cameraController->GetViewMatrix();
            lightBuffer.baseData.worldViewProj = lightBuffer.baseData.worldView * engine->cameraController->GetProjectionMatrix();
            lightBuffer.baseData.worldViewInverseTranspose = DirectX::SimpleMath::Matrix::Identity;


        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT res = engine->context->Map(lightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &lightBuffer, sizeof(CB_LightBuffer));
        engine->context->Unmap(lightConstBuffer->buffer.Get(), 0);
        engine->context->VSSetConstantBuffers(0, 1, lightConstBuffer->buffer.GetAddressOf());
        engine->context->PSSetConstantBuffers(0, 1, lightConstBuffer->buffer.GetAddressOf());


        engine->context->OMSetBlendState(lightBlendState.Get(), nullptr, 0xffffffff);
        engine->context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
        engine->context->PSSetSamplers(1, 1, samplerDepthState.GetAddressOf());

        //engine->context->OMSetRenderTargets(1, engine->rtv.GetAddressOf(), engine->depthStencilView.Get());
        engine->renderTarget->SetRenderTarget(engine->depthStencilView.Get());
        engine->context->PSSetShaderResources(0, 5, resources);

        if (light.lightType == LightType::Ambient || light.lightType == LightType::Directional)
        {
            engine->context->RSSetState(cullBackRS.Get());
            engine->context->OMSetDepthStencilState(offStencilState.Get(), 0);
            engine->context->VSSetShader(dirLightShader->vertexShader.Get(), nullptr, 0);
            if (light.lightType == LightType::Directional)
            {
                engine->context->PSSetShader(dirLightShader->pixelShader.Get(), nullptr, 0);
                engine->context->PSSetShaderResources(5, 1, &shadowResourceView);
            }
            else
                engine->context->PSSetShader(ambientLightShader->pixelShader.Get(), nullptr, 0);

            engine->context->IASetInputLayout(dirLightShader->layout.Get());
            engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
            engine->context->IASetIndexBuffer(indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            engine->context->IASetVertexBuffers(0, 1, vertexQuadBuffer->buffer.GetAddressOf(),
                strides, offsets);

            engine->context->DrawIndexed(6, 0, 0);
        }
        //else if (light.lightType == LightType::PointLight)
        //{

        //    //Back Face Pass

        //    engine->context->RSSetState(cullFrontRS.Get());
        //    engine->context->OMSetDepthStencilState(backFaceStencilState.Get(), 0);
        //    engine->context->PSSetShader(stencilPassShader->pixelShader.Get(), nullptr, 0);
        //    engine->context->VSSetShader(stencilPassShader->vertexShader.Get(), nullptr, 0);
        //    engine->context->IASetInputLayout(stencilPassShader->layout.Get());
        //    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
        //    engine->context->IASetIndexBuffer(light->aabb->meshes[0]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //    engine->context->IASetVertexBuffers(0, 1, light->aabb->meshes[0]->vertexBuffer->buffer.GetAddressOf(),
        //        stencilPassShader->strides, stencilPassShader->offsets);

        //    engine->context->DrawIndexed(light->aabb->meshes[0]->indexBuffer->size, 0, 0);


        //    //Front Face Pass

        //    engine->context->RSSetState(cullBackRS.Get());
        //    engine->context->OMSetDepthStencilState(frontFaceStencilState.Get(), 1);
        //    engine->context->VSSetShader(stencilPassShader->vertexShader.Get(), nullptr, 0);
        //    engine->context->PSSetShader(stencilPassShader->pixelShader.Get(), nullptr, 0);
        //    engine->context->VSSetConstantBuffers(0, 1, lightConstBuffer->buffer.GetAddressOf());
        //    engine->context->IASetInputLayout(stencilPassShader->layout.Get());
        //    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
        //    engine->context->IASetIndexBuffer(light->aabb->meshes[0]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //    engine->context->IASetVertexBuffers(0, 1, light->aabb->meshes[0]->vertexBuffer->buffer.GetAddressOf(),
        //        stencilPassShader->strides, stencilPassShader->offsets);

        //    engine->context->DrawIndexed(light->aabb->meshes[0]->indexBuffer->size, 0, 0);


        //    //Final Pass

        //    engine->context->RSSetState(cullBackRS.Get());
        //    engine->context->OMSetDepthStencilState(finalPassStencilState.Get(), 0);
        //    engine->context->VSSetShader(dirLightShader->vertexShader.Get(), nullptr, 0);

        //    engine->context->PSSetShader(pointLightShader->pixelShader.Get(), nullptr, 0);
        //    engine->context->IASetInputLayout(dirLightShader->layout.Get());
        //    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
        //    engine->context->IASetIndexBuffer(indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //    engine->context->IASetVertexBuffers(0, 1, vertexQuadBuffer->buffer.GetAddressOf(),
        //        strides, offsets);

        //    engine->context->DrawIndexed(6, 0, 0);


        //}

        //else if (light->lightType == LightType::SpotLight)
        //{
        //    //Back Face Pass

        //    engine->context->RSSetState(cullFrontRS.Get());
        //    engine->context->OMSetDepthStencilState(backFaceStencilState.Get(), 0);
        //    engine->context->PSSetShader(stencilPassShader->pixelShader.Get(), nullptr, 0);
        //    engine->context->VSSetShader(stencilPassShader->vertexShader.Get(), nullptr, 0);
        //    engine->context->IASetInputLayout(stencilPassShader->layout.Get());
        //    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
        //    engine->context->IASetIndexBuffer(light->aabb->meshes[0]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //    engine->context->IASetVertexBuffers(0, 1, light->aabb->meshes[0]->vertexBuffer->buffer.GetAddressOf(),
        //        stencilPassShader->strides, stencilPassShader->offsets);

        //    engine->context->DrawIndexed(light->aabb->meshes[0]->indexBuffer->size, 0, 0);


        //    //Front Face Pass

        //    engine->context->RSSetState(cullBackRS.Get());
        //    engine->context->OMSetDepthStencilState(frontFaceStencilState.Get(), 1); //change???
        //    engine->context->VSSetShader(stencilPassShader->vertexShader.Get(), nullptr, 0);
        //    engine->context->PSSetShader(stencilPassShader->pixelShader.Get(), nullptr, 0);
        //    //engine->context->PSSetShader(spotLightShader->pixelShader.Get(),nullptr,0);
        //    engine->context->VSSetConstantBuffers(0, 1, lightConstBuffer->buffer.GetAddressOf());
        //    engine->context->IASetInputLayout(stencilPassShader->layout.Get());
        //    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
        //    engine->context->IASetIndexBuffer(light->aabb->meshes[0]->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //    engine->context->IASetVertexBuffers(0, 1, light->aabb->meshes[0]->vertexBuffer->buffer.GetAddressOf(),
        //        stencilPassShader->strides, stencilPassShader->offsets);

        //    engine->context->DrawIndexed(light->aabb->meshes[0]->indexBuffer->size, 0, 0);


        //    //Final Pass

        //    engine->context->RSSetState(cullBackRS.Get());
        //    engine->context->OMSetDepthStencilState(finalPassStencilState.Get(), 0);
        //    engine->context->VSSetShader(dirLightShader->vertexShader.Get(), nullptr, 0);
        //    engine->context->PSSetShader(spotLightShader->pixelShader.Get(), nullptr, 0);
        //    engine->context->IASetInputLayout(dirLightShader->layout.Get());
        //    engine->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //?
        //    engine->context->IASetIndexBuffer(indexQuadBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //    engine->context->IASetVertexBuffers(0, 1, vertexQuadBuffer->buffer.GetAddressOf(),
        //        strides, offsets);

        //    engine->context->DrawIndexed(6, 0, 0);



        //}

    }
}

void RenderPipeline::ParticlePass()
{
    //engine->context->OMSetDepthStencilState(offStencilState.Get(), 0);
    ////engine->context->ClearDepthStencilView(engine->depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1.0f, 0);
    //for (auto particleSystem : engine->particleSystems)
    //{
    //    CB_ComputeShader dataGroup;
    //    particleSystem->NullSortList();
    //    dataGroup.GroupCount = Vector4(particleSystem->groupSizeX, particleSystem->groupSizeY, 0, 1);
    //    dataGroup.viewProjBuff.projection = engine->cameraController->GetProjectionMatrix();
    //    dataGroup.viewProjBuff.view = engine->cameraController->GetViewMatrix();
    //    dataGroup.eyePos = Vector4(engine->cameraController->camera->pos.x, engine->cameraController->camera->pos.y, engine->cameraController->camera->pos.z, 1);

    //    D3D11_MAPPED_SUBRESOURCE mappedResource;
    //    HRESULT res = engine->context->Map(particleSystem->groupCountConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    //    CopyMemory(mappedResource.pData, &dataGroup, sizeof(CB_ComputeShader));
    //    engine->context->Unmap(particleSystem->groupCountConstBuffer->buffer.Get(), 0);

    //    engine->context->OMSetBlendState(lightBlendState.Get(), nullptr, 0xffffffff);



    //    engine->context->CSSetShader(particleSystem->particleEmitterShader->computeShader.Get(), nullptr, 0);
    //    const unsigned int end[1] = { particleSystem->maxParticles - 1 };
    //    const unsigned int start[1] = { 0 };
    //    engine->context->CSSetUnorderedAccessViews(1, 1, particleSystem->poolBufferUAV.GetAddressOf(), nullptr);
    //    if (!helper)
    //    {
    //        engine->context->CSSetUnorderedAccessViews(2, 1, particleSystem->deadListUAV.GetAddressOf(), end);
    //        helper = true;
    //    }
    //    else
    //    {
    //        engine->context->CSSetUnorderedAccessViews(2, 1, particleSystem->deadListUAV.GetAddressOf(), nullptr);
    //    }
    //    engine->context->Dispatch(1, 1, 1);

    //    ID3D11UnorderedAccessView* nullViews[] = { nullptr,nullptr,nullptr };
    //    engine->context->CSSetUnorderedAccessViews(1, 3, nullViews, 0);
    //    engine->context->CSSetShader(nullptr, 0, 0);



    //    engine->context->CSSetShader(particleSystem->particleUpdateShader->computeShader.Get(), nullptr, 0);
    //    engine->context->CSSetUnorderedAccessViews(1, 1, particleSystem->poolBufferUAV.GetAddressOf(), nullptr);
    //    engine->context->CSSetUnorderedAccessViews(2, 1, particleSystem->deadListUAV.GetAddressOf(), nullptr);
    //    engine->context->CSSetUnorderedAccessViews(3, 1, particleSystem->sortBufferUAV.GetAddressOf(), start);
    //    engine->context->CSSetConstantBuffers(0, 1, particleSystem->groupCountConstBuffer->buffer.GetAddressOf());

    //    engine->context->Dispatch(particleSystem->groupSizeX, particleSystem->groupSizeY, 1);


    //    engine->context->CSSetUnorderedAccessViews(1, 3, nullViews, 0);
    //    engine->context->CSSetShader(nullptr, 0, 0);

    //    //=============== //
    //    // Sorting        //
    //    //=============== //

    //    particleSystem->SortGPU();
    //    engine->context->CSSetUnorderedAccessViews(0, 1, nullViews, 0);
    //    engine->context->CSSetShader(nullptr, 0, 0);

    //    //=============== //
    //    // Multiply by 6  //
    //    //=============== //
    //    engine->context->CopyStructureCount(particleSystem->counterBuffer->buffer.Get(), 0, particleSystem->sortBufferUAV.Get());

    //    engine->context->CSSetShader(particleSystem->multiply6Shader->computeShader.Get(), nullptr, 0);
    //    engine->context->CSSetUnorderedAccessViews(1, 1, particleSystem->counterUAV.GetAddressOf(), nullptr);
    //    engine->context->Dispatch(1, 1, 1);
    //    engine->context->CSSetUnorderedAccessViews(1, 1, nullViews, 0);
    //    engine->context->CSSetShader(nullptr, 0, 0);

    //    //=============== //
    //    // Visualisation  //
    //    //=============== //


    //    CB_ParticleVisualisation dataParticle;
    //    dataParticle.viewProj.projection = engine->cameraController->GetProjectionMatrix();
    //    dataParticle.viewProj.view = engine->cameraController->GetViewMatrix();
    //    dataParticle.eyePos = DirectX::SimpleMath::Vector4(engine->cameraController->camera->pos.x,
    //        engine->cameraController->camera->pos.y,
    //        engine->cameraController->camera->pos.z,
    //        1.0f);
    //    dataParticle.lightCount = engine->lights.size();
    //    Vector3 vec = engine->cameraController->camera->GetForwardVector();
    //    dataParticle.viewDirection = vec;
    //    dataParticle.cameraRot = engine->cameraController->GetViewMatrix().Invert();
    //    for (int i = 0; i < dataParticle.lightCount; i++)
    //    {
    //        dataParticle.lightData[i].Pos = engine->lights[i]->position;
    //        dataParticle.lightData[i].Color = engine->lights[i]->color;
    //        dataParticle.lightData[i].Dir = engine->lights[i]->direction;
    //        dataParticle.lightData[i].additiveParams = engine->lights[i]->params;

    //        if (engine->lights[i]->lightType == LightType::Directional)
    //        {
    //            for (int j = 0; j < 4; j++)
    //            {
    //                dataParticle.distances[j] = engine->lights[i]->distances[j];
    //                dataParticle.viewProjs[j] = engine->lights[i]->viewMatrices[j] * engine->lights[i]->orthoMatrices[j];
    //            }
    //        }
    //    }




    //    D3D11_MAPPED_SUBRESOURCE mappedResource2;
    //    res = engine->context->Map(particleSystem->constBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
    //    CopyMemory(mappedResource2.pData, &dataParticle, sizeof(CB_ParticleVisualisation));
    //    engine->context->Unmap(particleSystem->constBuffer->buffer.Get(), 0);


    //    engine->context->VSSetConstantBuffers(0, 1, particleSystem->constBuffer->buffer.GetAddressOf());
    //    engine->context->PSSetConstantBuffers(0, 1, particleSystem->constBuffer->buffer.GetAddressOf());

    //    engine->context->VSSetShader(particleSystem->simpleParticleShader->vertexShader.Get(), nullptr, 0);
    //    engine->context->PSSetShader(particleSystem->simpleParticleShader->pixelShader.Get(), nullptr, 0);
    //    engine->context->VSSetShaderResources(0, 1, &shadowResourceView);
    //    engine->context->VSSetShaderResources(1, 1, particleSystem->poolBufferSRV.GetAddressOf());
    //    engine->context->VSSetShaderResources(2, 1, particleSystem->sortBufferSRV.GetAddressOf());
    //    engine->context->VSSetShaderResources(3, 1, particleSystem->textureSRV.GetAddressOf());
    //    engine->context->PSSetShaderResources(0, 1, &shadowResourceView);
    //    engine->context->PSSetShaderResources(1, 1, particleSystem->poolBufferSRV.GetAddressOf());
    //    engine->context->PSSetShaderResources(2, 1, particleSystem->sortBufferSRV.GetAddressOf());
    //    engine->context->PSSetShaderResources(3, 1, particleSystem->textureSRV.GetAddressOf());

    //    engine->context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    //    engine->context->PSSetSamplers(1, 1, samplerDepthState.GetAddressOf());
    //    engine->context->VSSetSamplers(0, 1, samplerState.GetAddressOf());
    //    engine->context->VSSetSamplers(1, 1, samplerDepthState.GetAddressOf());

    //    engine->context->CopySubresourceRegion(particleSystem->indirectDrawBuffer->buffer.Get(), 0, 0, NULL, NULL, particleSystem->counterBuffer->buffer.Get(), 0, NULL);

    //    ID3D11UnorderedAccessView* UAVs[2] = { particleSystem->poolBufferUAV.Get(),particleSystem->sortBufferUAV.Get() };
    //    //engine->context->VSSetShaderResources(1,2,UAVs);
    //    //engine->context->OMSetRenderTargetsAndUnorderedAccessViews(1, engine->rtv.GetAddressOf(), engine->depthStencilView.Get(), 1, 2, UAVs, nullptr);


    //    engine->context->OMSetRenderTargets(1, engine->rtv.GetAddressOf(), engine->depthStencilView.Get());
    //    //engine->context->OMSetBlendState(particlesBlendState.Get(),nullptr,0xffffffff);

    //    engine->context->IASetIndexBuffer(particleSystem->indexBuffer->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    //    engine->context->IASetInputLayout(nullptr);
    //    engine->context->IASetInputLayout(particleSystem->simpleParticleShader->layout.Get());
    //    engine->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //    //engine->context->DrawInstancedIndirect(particleSystem->indirectDrawBuffer->buffer.Get(),0);
    //    engine->context->DrawIndexedInstancedIndirect(particleSystem->indirectDrawBuffer->buffer.Get(), 0);

    //    ID3D11ShaderResourceView* nullSRV[4] = { nullptr,nullptr,nullptr,nullptr };
    //    engine->context->PSSetShaderResources(0, 4, nullSRV);
    //    engine->context->VSSetShaderResources(0, 4, nullSRV);
    //    //=============== //
    //    // Shadow rec.    //
    //    //=============== //



    //}
}

float RenderPipeline::GetPixelValue(int clickX, int clickY)
{
    //engine->context->CopySubresourceRegion(gBuffer->depthTexture.Get(), 0, 0, 0, 0,  gBuffer->depthCpuTexture.Get(), 0, 0);
    engine->context->CopyResource(gBuffer->depthCpuTexture.Get(), gBuffer->depthTexture.Get());

    D3D11_MAPPED_SUBRESOURCE ResourceDesc = {};
    engine->context->Map(gBuffer->depthCpuTexture.Get(), 0, D3D11_MAP_READ, 0, &ResourceDesc);
    int const BytesPerPixel = sizeof(FLOAT);

    if (ResourceDesc.pData)
    {

            auto res = *((float*)ResourceDesc.pData + (clickX+clickY*(gBuffer->t_width))*4);
            return res;
    }
    return -1;
}

DirectX::SimpleMath::Vector2 RenderPipeline::GetRtvResolution()
{
    return DirectX::SimpleMath::Vector2(gBuffer->t_width, gBuffer->t_height);
}
//TODO: ~Переписать GetEntityId, добавить уровень абстракции
//TODO: пофиксить pointlight и spotlight