#include "LightSystem.h"


void LightSystem::Init()
{

}

void LightSystem::Run()
{
	for (auto& entity : EngineCore::instance()->scene->registry.view<LightComponent>())
	{
		LightComponent& lc = EngineCore::instance()->scene->registry.get<LightComponent>(entity);
        if (lc.lightType != LightType::Ambient)
        {
            GenerateViewMatrix(lc,
                Vector3(EngineCore::instance()->cameraController->camera->pos));
            /*  GenerateOrthoFromFrustum(lc,
                  EngineCore::instance()->cameraController->GetViewMatrix(),
                  EngineCore::instance()->cameraController->GetProjectionMatrix());*/
            GenerateOrthosFromFrustum(lc,
                EngineCore::instance()->cameraController->GetViewMatrix(),
                EngineCore::instance()->cameraController->GetProjectionMatrix(),
                EngineCore::instance()->cameraController->camera->farZ);
        }

        if (!lc.aabb)
        {
            lc.aabb = MeshLoader::LoadSimpleMesh("Engine/Assets/sphere.fbx");
        }
        
        /*
		if (hasher(lc) != lc.hash)
		{
			lc.hash = hasher(lc);
		}*/
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

std::vector<Matrix> LightSystem::GenerateOrthosFromFrustum(LightComponent& lc,const Matrix& view, const Matrix proj, float farZ)
{
    using namespace DirectX::SimpleMath;
    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    lc.orthoMatrices.clear();
    lc.viewMatrices.clear();
    lc.distances.clear();

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

        Matrix viewMatrix2 = DirectX::XMMatrixLookAtLH(center, center - Vector3(lc.direction), Vector3::Up);

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
    Vector3 tmp = Vector3(EngineCore::instance()->cameraController->camera->GetForwardVector());
    Vector4 lookAt = pos + Vector4(tmp.x, tmp.y, tmp.z, 1.0f);
    Vector4 up = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    lc.viewMatrix = DirectX::XMMatrixLookAtLH(pos, lookAt, up);
}

void LightSystem::GenerateOrthoFromFrustum(LightComponent& lc,const Matrix& view, const Matrix proj)
{

    std::vector<Vector4> frustumCorners = GetFrustumCorners(view, proj);
    Vector3 center = Vector3::Zero;

    for (const auto& v : frustumCorners)
    {
        center += Vector3(v);
    }

    center /= frustumCorners.size();

    lc.viewMatrix = DirectX::XMMatrixLookAtLH(center, center - Vector3(lc.direction), Vector3::Up);

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
