#include "PBR_Functions.hlsli"


struct LightData
{
    float4 pos;
    float4 dir;
    float4 color;
    float4 params;
};


cbuffer mycBuffer : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 view;
    row_major float4x4 worldView;
    row_major float4x4 worldViewProj;
    row_major float4x4 worldViewInverseT;
    LightData lightData;
    float4 eyePos;
    row_major float4x4 MatrixHelper[4];
    
    float4 shadowParams;                       // use shadow cast (X)

};


Texture2D<float4> DiffuseTex : register(t0);
Texture2D<float4> MetallicTex : register(t1);
Texture2D<float4> SpecularTex : register(t2);
Texture2D<float4> RoughnessTex : register(t3);
Texture2D<float4> EmissiveTex : register(t4);
Texture2D<float4> NormalTex : register(t5);
Texture2D<float4> PositionTex : register(t6);
Texture2D<float4> InstanceIDTex : register(t7);
TextureCube cubeMap : register(t8);

SamplerState textureSampler : SAMPLER : register(s0);

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos :SV_POSITION;
    float4 col :COLOR;
};

//struct GBuffer
//{
//    float4 Normals : SV_Target0;
//    float4 DiffuseSpec : SV_Target1;
//    float4 WorldPos : SV_Target2;
    
//};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = input.pos;
    output.col = input.col;
    //output.pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //output.col = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}




float4 PS_PointLight(PS_IN input) : SV_Target
{
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    float3 albedoColor =DiffuseTex.Sample(textureSampler, input.col.xy);
    float3 worldPos =PositionTex.Sample(textureSampler, input.col.xy);
    float3 normal = NormalTex.Sample(textureSampler, input.col.xy);
    float3 metallicColor = MetallicTex.Sample(textureSampler, input.col.xy);
    float3 specularColor = SpecularTex.Sample(textureSampler, input.col.xy);
    float roughnessColor = RoughnessTex.Sample(textureSampler, input.col.xy).r;
    float4 emissiveColor = EmissiveTex.Sample(textureSampler, input.col.xy);
    //float shadowVal = clamp(ShadowMap.Sample(textureSampler, input.col.xy), 0, 1);
    normal = normalize(normal);
   
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    
   
    float3 lightVec = -(worldPos.xyz - lightData.pos.xyz);
    float distance = length(lightVec);
    
    float3 fragToLight = -(lightData.pos.xyz - worldPos).xyz;
    float currentDepth = length(fragToLight.xyz);
    bool res = currentDepth < lightData.params.r;
    fragToLight.xyz = normalize(fragToLight.xyz);
    
    float3 depthCoord = cubeMap.Sample(textureSampler, fragToLight.xyz);
    float3 depthToLight = -(lightData.pos.xyz - depthCoord).xyz;
    //depthToLight.y = depthToLight.y * (-1);
    float bias = 0.1f;
    float4 posLight = float4(worldPos, 1);
    float shadowVal = currentDepth - bias < length(depthToLight);

    if (distance > lightData.params.r)
        return float4(0.0f, 0.0f, 0.0f, 1.0f);

    
    lightVec /= distance;
    
   
    
    float3 toEye = normalize(eyePos.xyz - worldPos.xyz);
    float3 H = normalize(toEye + lightVec);
    float lightIntesity = lightData.color.w;
  

    
    float3 diffuseFactor = lightIntesity * lightData.color.xyz * dot(lightVec, normal.xyz);
    
    diffuseFactor.x = max(diffuseFactor.x, 0);
    diffuseFactor.y = max(diffuseFactor.y, 0);
    diffuseFactor.z = max(diffuseFactor.z, 0);
    

    
    float att = pow((1 - distance / lightData.params.r), 2.0f) ;
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedoColor, (metallicColor.r + metallicColor.g + metallicColor.b) / 3.0f);
    float3 resColor = PBR(normal, toEye, lightVec, H, F0, lightData.color.xyz,
    albedoColor, roughnessColor, metallicColor, emissiveColor, lightIntesity*att);
    
    
    shadowVal = shadowVal * (shadowParams.x >0.5f) + (shadowParams.x<=0.5f);
    resColor = resColor*shadowVal;
    
    
    return float4(resColor, 1.0f);
    
    

}

