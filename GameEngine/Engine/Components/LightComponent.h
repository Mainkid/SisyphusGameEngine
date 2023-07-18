#pragma once
#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include "../Core/Rendering/Lights/LightType.h"
#include <vector>

class EngineCore;



using namespace DirectX::SimpleMath;
class LightComponent
{
public:
    LightComponent(EngineCore* _engine, LightType _type);
    void Initialize();

    std::vector<Vector4> GetFrustumCorners(const Matrix& view, const Matrix proj);
    std::vector<Matrix> GenerateOrthosFromFrustum(const Matrix& view, const Matrix proj, float _far);
    void GenerateOrthoMatrix(float width, float depthPlane, float nearPlane);
    void GenerateViewMatrix(Vector3 pos);
    void GenerateOrthoFromFrustum(const Matrix& view, const Matrix proj);
    std::vector<Matrix> viewMatrices;
    std::vector<Matrix> orthoMatrices;
    std::vector<Vector4> distances;

    LightType lightType = LightType::Ambient;
    Vector4 position = { 0,0,0,1 };
    Vector4 direction = { 1,0,0,0 };
    Vector4 color = { 1,1,1,1 };
    Vector4 params = { 1,1,1,1 };

    Matrix viewMatrix;
    Matrix orthoMatrix;

    //std::shared_ptr<Model3D> aabb;
    EngineCore* engine;


};

