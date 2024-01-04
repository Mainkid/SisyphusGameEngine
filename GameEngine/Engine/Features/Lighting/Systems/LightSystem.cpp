#include "LightSystem.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/RenderContext.h"
#include "../../../Components/CameraComponent.h"
#include "../../../Core/ServiceLocator.h"
#include "../../../Components/TransformComponent.h"
#include "../../../Core/Tools/MathHelper.h"
#include "../../../Scene/CameraHelper.h"

SyResult LightSystem::Init()
{
    _ec = ServiceLocator::instance()->Get<EngineContext>();
    _rc = ServiceLocator::instance()->Get<RenderContext>();
    _hc = ServiceLocator::instance()->Get<HardwareContext>();
    return SyResult();
}

SyResult LightSystem::Run()
{
    OPTICK_EVENT();
    auto [camera, cameraTf] = CameraHelper::Find(_ecs, _ec->playModeState);
    auto viewLights = _ecs->view<TransformComponent, LightComponent>();
    for (auto& ent : viewLights)
    {
        LightComponent& lc = viewLights.get<LightComponent>(ent);
        TransformComponent& tc = viewLights.get<TransformComponent>(ent);
        if (lc.LightType != ELightType::Ambient)
        {
            GenerateViewMatrix(Vector3(camera.forward), lc, Vector3(cameraTf.localPosition));

            if (lc.LightType == ELightType::Directional)
            {
                GenerateOrthosFromFrustum(lc,
                    Vector3::Transform(Vector3::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation)),
                    camera.view,
                    camera.projection,
                    camera.farPlane);
            }
        }

        if (!lc.Aabb)
        {
            lc.Aabb = MeshLoader::LoadSimpleMesh("Engine/Assets/Resources/Cube.fbx");
        }

        if (lc.LightType == ELightType::PointLight && lc.ShadowCubeMapTexture == nullptr)
            InitPointLightResources(lc);
        else if (lc.LightType == ELightType::Directional && lc.DirShadowTexture == nullptr)
            InitDirLightResources(lc);
    }
    return SyResult();
}

SyResult LightSystem::Destroy()
{
    return SyResult();
}

std::vector<Vector4> LightSystem::GetFrustumCorners(const Matrix& view, const Matrix proj)
{
    using namespace DirectX::SimpleMath;
    const auto viewProj = view * proj;
    const auto inv = viewProj.Invert();

    std::vector<Vector4> frustumCorners;
    frustumCorners.reserve(8);
    for (unsigned int x = 0; x < 2; ++x)
        for (unsigned int y = 0; y < 2; ++y)
            for (unsigned int z = 0; z < 2; ++z)
            {
                const Vector4 pt = Vector4::Transform(Vector4
                (
                    2.0f * x - 1.0f,
                    2.0f * y - 1.0f,
                    z,
                    1.0f), inv);
                frustumCorners.push_back(pt / pt.w);
            }


    return frustumCorners;
}

std::vector<Matrix> LightSystem::GenerateOrthosFromFrustum(LightComponent& lc, Vector3 direction, const Matrix& view, const Matrix proj, float farZ)
{
    if ((direction - Vector3(0, 1, 0)).Length() < 0.001f || (direction - Vector3(0, -1, 0)).Length() < 0.001f)
    {
        direction += Vector3(0, 0, 0.01f);
        direction.Normalize();
    }
    using namespace DirectX::SimpleMath;
    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    std::vector<float> planesProportions = {0, 0.05,0.1,0.35,1 };
    lc.OrthoMatrices.clear();
    lc.ViewMatrices.clear();
    lc.Distances.clear();

    int n = 4;


    for (int i = 1; i <= n; i++)
    {
        std::vector<Vector4> newCorners = std::vector<Vector4>();
        for (int j = 0; j < 8; j += 2)
        {
            newCorners.push_back(frustumCorners[j] + frustumCorners[j + 1] * planesProportions[i-1]);
            newCorners[j].w = 1.0f;

            newCorners.push_back(frustumCorners[j] + frustumCorners[j + 1] * planesProportions[i]);
            newCorners[j + 1].w = 1.0f;
        }

        Vector3 center = Vector3::Zero;

        for (const auto& v : newCorners)
        {
            center += Vector3(v);
        }


        center /= newCorners.size();
       
        Matrix viewMatrix2 = DirectX::XMMatrixLookAtLH(center, center - direction, Vector3::Up);

        float minX = 10000000.0f;
        float maxX = -10000000.0f;
        float minY = 10000000.0f;
        float maxY = -10000000.0f;
        float minZ = 10000000.0f;
        float maxZ = -10000000.0f;

        for (const auto& v : newCorners)
        {
            const auto trf = Vector4::Transform(v, viewMatrix2);

            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        constexpr float zMult = 3.0f;
        minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
        maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

        
        lc.OrthoMatrices.push_back(Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ));
        lc.ViewMatrices.push_back(viewMatrix2);
        lc.Distances.push_back(Vector4(farZ * planesProportions[i],
            farZ * planesProportions[i], farZ * planesProportions[i], 1.0f));
    }

    return std::vector<Matrix>();
}

void LightSystem::GenerateOrthoMatrix(LightComponent& lc, float width, float depthPlane, float nearPlane)
{
    lc.OrthoMatrix = DirectX::XMMatrixOrthographicLH(width, width, nearPlane, depthPlane);
}

void LightSystem::GenerateViewMatrix(Vector3 cameraForward, LightComponent& lc, Vector3 pos)
{
    Vector4 lookAt = pos + Vector4(cameraForward.x, cameraForward.y, cameraForward.z, 1.0f);
    Vector4 up = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    lc.ViewMatrix = DirectX::XMMatrixLookAtLH(pos, lookAt, up);
}

void LightSystem::GenerateOrthoFromFrustum(LightComponent& lc, Vector3 direction, const Matrix& view, const Matrix proj)
{
    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    Vector3 center = Vector3::Zero;

    for (const auto& v : frustumCorners)
    {
        center += Vector3(v);
    }

    center /= frustumCorners.size();

    lc.ViewMatrix = DirectX::XMMatrixLookAtLH(center, center - direction, Vector3::Up);

    float minX = 10000000.0f;
    float maxX = -10000000.0f;
    float minY = 10000000.0f;
    float maxY = -10000000.0f;
    float minZ = 10000000.0f;
    float maxZ = -10000000.0f;

    for (const auto& v : frustumCorners)
    {
        const Vector4 trf = Vector4::Transform(v, lc.ViewMatrix);

        minX = SyMathHelper::Min(minX, trf.x);
        maxX = SyMathHelper::Max(maxX, trf.x);
        minY = SyMathHelper::Min(minY, trf.y);
        maxY = SyMathHelper::Max(maxY, trf.y);
        minZ = SyMathHelper::Min(minZ, trf.z);
        maxZ = SyMathHelper::Max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;
    minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
    maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;


    lc.OrthoMatrix = Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
}

void LightSystem::InitPointLightResources(LightComponent& lc)
{
    D3D11_TEXTURE2D_DESC textureDesc_ = {};
    textureDesc_.Width = _rc->ShadowmapWidth;
    textureDesc_.Height = _rc->ShadowmapHeight;
    textureDesc_.MipLevels = 1;
    textureDesc_.ArraySize = 6;
    textureDesc_.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc_.SampleDesc.Count = 1;
    textureDesc_.SampleDesc.Quality = 0;
    textureDesc_.Usage = D3D11_USAGE_DEFAULT;
    textureDesc_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc_.CPUAccessFlags = 0;
    textureDesc_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    HRESULT result = _hc->device->CreateTexture2D(&textureDesc_, nullptr, lc.ShadowCubeMapTexture.GetAddressOf());


    result = _hc->device->CreateRenderTargetView(lc.ShadowCubeMapTexture.Get(), nullptr, lc.ShadowCubeMapRtv.GetAddressOf());

    D3D11_TEXTURE2D_DESC textureDescDSV = {};
    textureDescDSV.Width = _rc->ShadowmapWidth;
    textureDescDSV.Height = _rc->ShadowmapHeight;
    textureDescDSV.MipLevels = 1;
    textureDescDSV.ArraySize = 6;
    textureDescDSV.Format = DXGI_FORMAT_R32_TYPELESS;
    textureDescDSV.SampleDesc.Count = 1;
    textureDescDSV.SampleDesc.Quality = 0;
    textureDescDSV.Usage = D3D11_USAGE_DEFAULT;
    textureDescDSV.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    textureDescDSV.CPUAccessFlags = 0;
    textureDescDSV.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    result = _hc->device->CreateTexture2D(&textureDescDSV, nullptr, lc.DepthStencilViewCubeTexture.GetAddressOf());


    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewPointDesc;
    ZeroMemory(&depthStencilViewPointDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewPointDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewPointDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewPointDesc.Texture2DArray.MipSlice = 0;
    depthStencilViewPointDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewPointDesc.Texture2DArray.ArraySize = 6;

    result = _hc->device->CreateDepthStencilView(lc.DepthStencilViewCubeTexture.Get(), &depthStencilViewPointDesc, lc.ShadowCubeMapDsv.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    result = _hc->device->CreateShaderResourceView(lc.ShadowCubeMapTexture.Get(), &srvDesc, lc.ShadowCubeMapSrv.GetAddressOf());
}

void LightSystem::InitDirLightResources(LightComponent& lc)
{
    D3D11_TEXTURE2D_DESC shadowMapDesc;
    ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
    shadowMapDesc.Width = lc.ShadowMapSize;
    shadowMapDesc.Height = lc.ShadowMapSize;
    shadowMapDesc.MipLevels = 1;
    shadowMapDesc.ArraySize = 4;
    shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    shadowMapDesc.SampleDesc.Count = 1;
    shadowMapDesc.SampleDesc.Quality = 0;
    shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
    shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    shadowMapDesc.CPUAccessFlags = 0;
    shadowMapDesc.MiscFlags = 0;

    HRESULT hr = _hc->device->CreateTexture2D(&shadowMapDesc, nullptr, lc.DirShadowTexture.GetAddressOf());

    D3D11_TEXTURE2D_DESC textureDesc;
    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = _rc->ShadowmapWidth;
    textureDesc.Height = _rc->ShadowmapHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 4;
    textureDesc.Format = DXGI_FORMAT_R32G32_TYPELESS;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    auto result = _hc->device->CreateTexture2D(&textureDesc, NULL, lc.DirShadowRtTexture.GetAddressOf());
    result = _hc->device->CreateTexture2D(&textureDesc, NULL, lc.DirBluredShadowTexture.GetAddressOf());


    D3D11_SAMPLER_DESC sampDesc2;
    ZeroMemory(&sampDesc2, sizeof(sampDesc2));
    sampDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc2.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc2.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc2.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc2.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc2.MinLOD = 0;
    sampDesc2.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc2.MaxAnisotropy = 0;
    sampDesc2.MipLODBias = 0;
    hr = _hc->device->CreateSamplerState(&sampDesc2, lc.ShadowMapSampler.GetAddressOf()); //Create sampler state



    D3D11_DEPTH_STENCIL_DESC depthstencildesc2;
    ZeroMemory(&depthstencildesc2, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthstencildesc2.DepthEnable = true;
    depthstencildesc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthstencildesc2.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hr = _hc->device->CreateDepthStencilState(&depthstencildesc2, lc.DirShadowStencilState.GetAddressOf());

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewDesc.Texture2DArray.ArraySize = 4;

    hr = _hc->device->CreateDepthStencilView(lc.DirShadowTexture.Get(), &depthStencilViewDesc, lc.DirShadowStencilView.GetAddressOf());




    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
    shaderResourceViewDesc.Texture2DArray.ArraySize = 4;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc2;


    // Setup the description of the render target view.



    //result=engine->device->CreateRenderTargetView(MRenderTargetTexture, nullptr, &MRenderTargetView);
    // Setup the description of the shader resource view.
    shaderResourceViewDesc2.Format = DXGI_FORMAT_R32G32_FLOAT;
    shaderResourceViewDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc2.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc2.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc2.Texture2DArray.ArraySize = 4;
    shaderResourceViewDesc2.Texture2DArray.FirstArraySlice = 0;

    hr = _hc->device->CreateShaderResourceView(lc.DirBluredShadowTexture.Get(), &shaderResourceViewDesc2, lc.DirShadowBluredSrv.GetAddressOf());
    hr = _hc->device->CreateShaderResourceView(lc.DirShadowRtTexture.Get(), &shaderResourceViewDesc2, lc.DirShadowSrv.GetAddressOf());



    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;


    renderTargetViewDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    renderTargetViewDesc.Texture2DArray.MipSlice = 0;
    renderTargetViewDesc.Texture2DArray.ArraySize = 4;
    renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;


    // Create the render target view.
    result = _hc->device->CreateRenderTargetView(lc.DirShadowRtTexture.Get(), &renderTargetViewDesc,
        lc.DirShadowRtv.GetAddressOf());
    result = _hc->device->CreateRenderTargetView(lc.DirBluredShadowTexture.Get(), &renderTargetViewDesc,
        lc.DirShadowBluredRtv.GetAddressOf());
}
