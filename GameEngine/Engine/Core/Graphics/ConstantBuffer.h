#pragma once
#include "SimpleMath.h"
#include "../../Core/Rendering/Lights/LightData.h"
#include "../../Components/Particle.h"

class CB_BaseBuffer
{
public:
    DirectX::SimpleMath::Matrix world;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix worldView;
    DirectX::SimpleMath::Matrix worldViewProj;
    DirectX::SimpleMath::Matrix worldViewInverseTranspose;
};

class CB_MaterialData
{
public:
    DirectX::SimpleMath::Vector4 albedo;
    DirectX::SimpleMath::Vector4 metallic;
    DirectX::SimpleMath::Vector4 roughness;
    DirectX::SimpleMath::Vector4 emissive;
    DirectX::SimpleMath::Vector4 specular;
};

class CB_BaseEditorBuffer
{
public:
    CB_BaseBuffer baseData;
    CB_MaterialData materialData;
    DirectX::SimpleMath::Vector4 instanseID;
    
};

class CB_GridEditorBuffer
{
public:
    CB_BaseBuffer baseData;
    DirectX::SimpleMath::Matrix viewProjs[4];
    DirectX::SimpleMath::Vector4 eyePos;
};

class CB_LightBuffer
{
public:
    CB_BaseBuffer baseData;
    LightData lightData;
    DirectX::SimpleMath::Vector4 eyePos;
    DirectX::SimpleMath::Matrix viewProjs[4];
    DirectX::SimpleMath::Vector4 distances[4];
};

class CB_ShadowBuffer
{
public:
    CB_BaseBuffer baseData;
    DirectX::SimpleMath::Matrix viewProjs[4];
    DirectX::SimpleMath::Vector4 distances[4];
};

class CB_PointlightShadowBuffer
{
public:
    DirectX::SimpleMath::Matrix world;
    DirectX::SimpleMath::Matrix view[6];
    DirectX::SimpleMath::Matrix proj;
};



class CB_viewProjBuffer
{
public:
    Matrix view;
    Matrix projection;
};

class CB_ParticleVisualisation
{
public:
    CB_viewProjBuffer viewProj;
    Matrix cameraRot;
    LightData lightData[10];
    Vector3 viewDirection;
    int lightCount;
    Vector4 eyePos;
    DirectX::SimpleMath::Matrix viewProjs[4];
    DirectX::SimpleMath::Vector4 distances[4];
    
};

struct CB_SharedParticlesData
{
    Vector4 deltaTime;
    Vector4 startPosition;
    Vector4 startSize;
    Vector4 startColor;
    Vector4 startLifeTime;
    Vector4 startVelocity;
};

class CB_ComputeShader
{
public:
    CB_viewProjBuffer viewProjBuff;
    Vector4 GroupCount;
    Vector4 eyePos;
    CB_SharedParticlesData SharedParticlesData;
    
};

struct CB_SimpleVector4
{
    Vector4 params;
};