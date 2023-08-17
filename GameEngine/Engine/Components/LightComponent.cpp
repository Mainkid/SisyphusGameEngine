#include "LightComponent.h"
#include "../Core/EngineCore.h"


LightComponent::LightComponent( LightType _type)
{
    this->lightType = _type;
    Initialize();
}

void LightComponent::Initialize()
{
    //TODO: ƒоделать дл€ поинтлайтов и спотлайтов
    if (lightType == LightType::PointLight)
    {
        std::string path = "Engine/Assets/Cube.fbx";
        aabb=std::make_unique<MeshComponent>();
        //aabb->Initialize();
    }
    else if (lightType == LightType::SpotLight)
    {
        std::string filePath = "Engine/Assets/Cube.fbx";
        aabb=std::make_unique<MeshComponent>();
        //aabb->Initialize();
    }

}

std::vector<Matrix> LightComponent::GenerateOrthosFromFrustum(const Matrix& view, const Matrix proj, float farZ)
{
    using namespace DirectX::SimpleMath;
    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    orthoMatrices.clear();
    viewMatrices.clear();
    distances.clear();

    int n = 4;
    int exp = 2;



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

        Matrix viewMatrix2 = DirectX::XMMatrixLookAtLH(center, center - Vector3(direction), Vector3::Up);

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

        constexpr float zMult = 10.0f;
        minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
        maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;


        orthoMatrices.push_back(Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ));
        viewMatrices.push_back(viewMatrix2);
        distances.push_back(Vector4(farZ * pow((i * 1.0f / n), exp),
            farZ * pow((i * 1.0f / n), exp), farZ * pow((i * 1.0f / n), exp), 1.0f));
    }

    return std::vector<Matrix>();
}

std::vector<Vector4> LightComponent::GetFrustumCorners(const Matrix& view, const Matrix proj)
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

void LightComponent::GenerateOrthoMatrix(float width, float depthPlane, float nearPlane)
{
    orthoMatrix = DirectX::XMMatrixOrthographicLH(width, width, nearPlane, depthPlane);
}

void LightComponent::GenerateViewMatrix(Vector3 pos)
{
    //Vector4 lookAt=Vector4(0,0,0,1.0f);
    Vector3 tmp = Vector3(EngineCore::instance()->cameraController->camera->GetForwardVector());
    Vector4 lookAt = pos + Vector4(tmp.x, tmp.y, tmp.z, 1.0f);
    Vector4 up = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    viewMatrix = DirectX::XMMatrixLookAtLH(pos, lookAt, up);

}

void LightComponent::GenerateOrthoFromFrustum(const Matrix& view, const Matrix proj)
{
    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    Vector3 center = Vector3::Zero;

    for (const auto& v : frustumCorners)
    {
        center += Vector3(v);
    }

    center /= frustumCorners.size();

    viewMatrix = DirectX::XMMatrixLookAtLH(center, center - Vector3(direction), Vector3::Up);

    float minX = 10000000.0f;
    float maxX = -10000000.0f;
    float minY = 10000000.0f;
    float maxY = -10000000.0f;
    float minZ = 10000000.0f;
    float maxZ = -10000000.0f;

    for (const auto& v : frustumCorners)
    {
        const auto trf = Vector4::Transform(v, viewMatrix);

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


    orthoMatrix = Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);

}