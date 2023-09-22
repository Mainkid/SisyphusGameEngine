#include "LightSystem.h"
#include "HardwareContext.h"
#include "EngineContext.h"
#include "Rendering\RenderContext.h"
#include "../Core/ServiceLocator.h"

void LightSystem::Init()
{
    ec = ServiceLocator::instance()->Get<EngineContext>();
    rc = ServiceLocator::instance()->Get<RenderContext>();
    hc = ServiceLocator::instance()->Get<HardwareContext>();
}

void LightSystem::Run()
{
    auto view = ec->scene->registry.view<TransformComponent,LightComponent>();
	for (auto& entity : view)
	{
		LightComponent& lc = view.get<LightComponent>(entity);
        TransformComponent& tc = view.get<TransformComponent>(entity);
        if (lc.lightType != LightType::Ambient)
        {
            GenerateViewMatrix(lc,
                Vector3(ec->scene->cameraTransform->localPosition));
              //GenerateOrthoFromFrustum(lc, Vector3::Transform(Vector3::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation)),
              //    ec->scene->camera->view,
              //    ec->scene->camera->projection);
            GenerateOrthosFromFrustum(lc, Vector3::Transform(Vector3::UnitX, Matrix::CreateFromYawPitchRoll(tc.localRotation)),
                ec->scene->camera->view,
                ec->scene ->camera->projection,
                ec->scene->camera->farPlane);
        }

        if (!lc.aabb)
        {
            lc.aabb = MeshLoader::LoadSimpleMesh("Engine/Assets/Cube.fbx");
        }
        
        if (lc.lightType == LightType::PointLight && lc.shadowMapTexture==nullptr)
            InitPointLightResources(lc);
	}
}

void LightSystem::Destroy()
{
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

std::vector<Matrix> LightSystem::GenerateOrthosFromFrustum(LightComponent& lc,Vector3 direction,const Matrix& view, const Matrix proj, float farZ)
{
    using namespace DirectX::SimpleMath;
    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    lc.orthoMatrices.clear();
    lc.viewMatrices.clear();
    lc.distances.clear();

    int n = 4;
    int exp = 3;



    for (int i = 1; i <= n; i++)
    {
        std::vector<Vector4> newCorners = std::vector<Vector4>();
        for (int j = 0; j < 8; j += 2)
        {
            newCorners.push_back(frustumCorners[j] + frustumCorners[j + 1] * pow((i - 1 + 0.0001f) / n, exp));
            newCorners[j].w = 1.0f;

            newCorners.push_back(frustumCorners[j] + frustumCorners[j + 1] * pow((i + 0.0001f) / n, exp));
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
            maxX = max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = max(maxZ, trf.z);
        }

        constexpr float zMult = 5.0f;
        minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
        maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;


        lc.orthoMatrices.push_back(Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ));
        lc.viewMatrices.push_back(viewMatrix2);
        lc.distances.push_back(Vector4(farZ * pow((i * 1.0f / n), exp),
            farZ * pow((i * 1.0f / n), exp), farZ * pow((i * 1.0f / n), exp), 1.0f));
    }

    return std::vector<Matrix>();
}

void LightSystem::GenerateOrthoMatrix(LightComponent& lc,float width, float depthPlane, float nearPlane)
{
    lc.orthoMatrix = DirectX::XMMatrixOrthographicLH(width, width, nearPlane, depthPlane);
}

void LightSystem::GenerateViewMatrix(LightComponent& lc, Vector3 pos)
{
    Vector3 tmp = Vector3(ec->scene->camera->forward);
    Vector4 lookAt = pos + Vector4(tmp.x, tmp.y, tmp.z, 1.0f);
    Vector4 up = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    lc.viewMatrix = DirectX::XMMatrixLookAtLH(pos, lookAt, up);
}

void LightSystem::GenerateOrthoFromFrustum(LightComponent& lc,Vector3 direction,const Matrix& view, const Matrix proj)
{
    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    Vector3 center = Vector3::Zero;

    for (const auto& v : frustumCorners)
    {
        center += Vector3(v);
    }

    center /= frustumCorners.size();

    lc.viewMatrix = DirectX::XMMatrixLookAtLH(center, center - direction, Vector3::Up);

    float minX = 10000000.0f;
    float maxX = -10000000.0f;
    float minY = 10000000.0f;
    float maxY = -10000000.0f;
    float minZ = 10000000.0f;
    float maxZ = -10000000.0f;

    for (const auto& v : frustumCorners)
    {
        const Vector4 trf = Vector4::Transform(v, lc.viewMatrix);

        minX = std::min(minX, trf.x);
        maxX = max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;
    minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
    maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;


    lc.orthoMatrix = Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
}

void LightSystem::InitPointLightResources(LightComponent& lc)
{
    D3D11_TEXTURE2D_DESC textureDesc_ = {};
    textureDesc_.Width = rc->SHADOWMAP_WIDTH;
    textureDesc_.Height = rc->SHADOWMAP_HEIGHT;
    textureDesc_.MipLevels = 1;
    textureDesc_.ArraySize = 6;
    textureDesc_.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc_.SampleDesc.Count = 1;
    textureDesc_.SampleDesc.Quality = 0;
    textureDesc_.Usage = D3D11_USAGE_DEFAULT;
    textureDesc_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc_.CPUAccessFlags = 0;
    textureDesc_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    HRESULT result = hc->device->CreateTexture2D(&textureDesc_, nullptr, lc.shadowMapTexture.GetAddressOf());

    
    result = hc->device->CreateRenderTargetView(lc.shadowMapTexture.Get(),nullptr, lc.shadowMapRTV.GetAddressOf());

    D3D11_TEXTURE2D_DESC textureDescDSV = {};
    textureDescDSV.Width = rc->SHADOWMAP_WIDTH;
    textureDescDSV.Height = rc->SHADOWMAP_HEIGHT;
    textureDescDSV.MipLevels = 1;
    textureDescDSV.ArraySize = 6;
    textureDescDSV.Format = DXGI_FORMAT_R32_TYPELESS;
    textureDescDSV.SampleDesc.Count = 1;
    textureDescDSV.SampleDesc.Quality = 0;
    textureDescDSV.Usage = D3D11_USAGE_DEFAULT;
    textureDescDSV.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    textureDescDSV.CPUAccessFlags = 0;
    textureDescDSV.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    result = hc->device->CreateTexture2D(&textureDescDSV, nullptr, lc.depthStencilViewTexture.GetAddressOf());


    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewPointDesc;
    ZeroMemory(&depthStencilViewPointDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewPointDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewPointDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewPointDesc.Texture2DArray.MipSlice = 0; 
    depthStencilViewPointDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewPointDesc.Texture2DArray.ArraySize = 6;

    result = hc->device->CreateDepthStencilView(lc.depthStencilViewTexture.Get(), &depthStencilViewPointDesc, lc.shadowMapDSV.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    result = hc->device->CreateShaderResourceView(lc.shadowMapTexture.Get(), &srvDesc, lc.shadowMapSRV.GetAddressOf());
}
