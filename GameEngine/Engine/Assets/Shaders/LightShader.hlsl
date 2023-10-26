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
    row_major float4x4 viewProj[4];
    float4 distances[4];
};


Texture2D<float4> DiffuseTex : register(t0);
Texture2D<float4> MetallicTex : register(t1);
Texture2D<float4> SpecularTex : register(t2);
Texture2D<float4> RoughnessTex : register(t3);
Texture2D<float4> EmissiveTex : register(t4);
Texture2D<float4> NormalTex : register(t5);
Texture2D<float4> PositionTex : register(t6);
Texture2D<float4> InstanceIDTex : register(t7);
Texture2D shadowTexture : register(t8);
Texture2D skyboxTexture : register(t9);
Texture2DArray depthMapTexture : register(t10);
//Texture2D depthTexture : register(t10);


SamplerState textureSampler : SAMPLER : register(s0);
SamplerState shadowSampler : register(s1);

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




PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = input.pos;
    output.col = input.col;
    //output.pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //output.col = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}

float calcShadowValue(float3 worldPos,float layer,float bias)
{
    float4 posLight = float4(worldPos, 1);
    float3 projectTexCoord;
    float shadowSum = 0;
    float depthValue = 0;
    
    float texelWidth = 1.0 / 1280.0f;
    float texelHeight = 1.0 / 720.0f;
    float texelOffset = 2;
    float2 texelSize = float2(texelWidth, texelHeight) * texelOffset;
    
    posLight = mul(posLight, viewProj[layer]);
    
    
    projectTexCoord.x = posLight.x / posLight.w / 2.0f + 0.5f;
    projectTexCoord.y = -posLight.y / posLight.w / 2.0f + 0.5f;
    projectTexCoord.z = layer;
    float lightDepthValue = posLight.z / posLight.w - bias;
    
    for (int y = -2; y <= 2; y++)
    {
        for (int x = -2; x <= 2; x++)
        {
            float3 offset = float3(float2(x, y) * texelSize, 0);
            depthValue = depthMapTexture.Sample(textureSampler, projectTexCoord + offset).r;
            shadowSum += lightDepthValue > depthValue;
        }
    }
    
    shadowSum = shadowSum / 25.0f;
    return shadowSum;
}


float4 PS_Directional(PS_IN input) : SV_Target
{
    
    float3 albedoColor = DiffuseTex.Sample(textureSampler, input.col.xy);
    //pixelColor = pow(pixelColor, 1.0f / 2.2f);
    float3 worldPos = PositionTex.Sample(textureSampler, input.col.xy);
    float3 normal = NormalTex.Sample(textureSampler, input.col.xy);
    float3 metallicColor = MetallicTex.Sample(textureSampler, input.col.xy);
    float3 specularColor = SpecularTex.Sample(textureSampler, input.col.xy);
    float roughnessColor = RoughnessTex.Sample(textureSampler, input.col.xy).r;
    float4 emissiveColor = EmissiveTex.Sample(textureSampler, input.col.xy);
    normal = normalize(normal);
    float4 specular = SpecularTex.Sample(textureSampler, input.col.xy);
  

    float3 lightVec = -normalize(lightData.dir.xyz).xyz;
    float3 toEye = normalize(eyePos.xyz - worldPos.xyz);
    float lightIntesity = lightData.color.w;
    float specularAlpha = 50.0f;
    float3 H = normalize(toEye + lightVec);
    
    //PCF --->
    float blendArea = 0.2f;
    float bias = 0.005f;
    float3 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    int layer = 3;
    int layer2;
    float depthVal = abs(mul(float4(worldPos, 1), worldView).z);
    for (int i = 0; i < 4; i++)
        if (depthVal < distances[i].x)
        {
            layer = i;
            break;
        }
    layer2 = clamp(layer + 1, 0, 3);
    
   
    float shadowSum1 = calcShadowValue(worldPos, layer, bias);
    float shadowSum2 = calcShadowValue(worldPos, layer2, bias);
    
    float lerpValue = 1-clamp((-depthVal+distances[layer].x) / blendArea, 0, 1);
    //return float4(lerpValue, 0, 0, 1);
    shadowSum1 = lerp(shadowSum1, shadowSum2, lerpValue);
    //<--- PCF
    
    //depthValue = depthMapTexture.Sample(textureSampler, projectTexCoord).r;
    //shadowSum = lightDepthValue > depthValue;
    
    lightIntesity = lightIntesity;
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedoColor, (metallicColor.r + metallicColor.g + metallicColor.b) / 3.0f);
    float3 instanceID = InstanceIDTex.Sample(textureSampler, input.col.xy);
    float3 resColor = PBR(normal, toEye, lightVec, H, F0,
    lightData.color.xyz, albedoColor, roughnessColor, metallicColor, emissiveColor, lightIntesity) * (instanceID.x > 0);
    
    
    return float4(resColor * (1 - shadowSum1), 1.0f);

}

float4 PS_Ambient(PS_IN input) : SV_Target
{
    
    float3 pixelColor = DiffuseTex.Sample(textureSampler, input.col.xy);
    //pixelColor = pow(pixelColor, 2.2f);
    float3 instanceID =InstanceIDTex.Sample(textureSampler, input.col.xy);
    float3 skyBoxColor = skyboxTexture.Sample(textureSampler, input.col.xy);
    float3 resColor;
    resColor = pixelColor * lightData.color.xyz * (instanceID.x > 0) + skyBoxColor*(instanceID.x<0);
    return float4(resColor, 1.0f);
}