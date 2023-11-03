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
Texture2D<float4> RoughnessTex : register(t2);
Texture2D<float4> EmissiveTex : register(t3);
Texture2D<float4> NormalTex : register(t4);
Texture2D<float4> PositionTex : register(t5);
Texture2D<float4> InstanceIDTex : register(t6);
Texture2D AoTex : register(t7);
Texture2D shadowTexture : register(t8);
Texture2D skyboxTexture : register(t9);
Texture2DArray depthMapTexture : register(t10);


//IBL Stuff
TextureCube irradianceTexture : register(t11);
Texture2D iblLookUpTexture : register(t12);
TextureCube prefilteredEnvironmentTexture : register(t13);


SamplerState textureSampler : SAMPLER : register(s0);
SamplerState shadowSampler : SAMPLER : register(s1);

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

float LinStep(float low, float high, float v)
{
    return clamp((v - low) / (high - low), 0.0f, 1.0f);
}


float SampleVarianceShadowMap(float3 worldPos,int layer)
{
    float4 posLight = float4(worldPos, 1);
    float3 projectTexCoord;
    float shadowSum = 0;
    float depthValue = 0;
    float bias = 0.001f;
    
    posLight = mul(posLight, viewProj[layer]);
    projectTexCoord.x = posLight.x / posLight.w / 2.0f + 0.5f;
    projectTexCoord.y = -posLight.y / posLight.w / 2.0f + 0.5f;
    projectTexCoord.z = layer;
    
    float compare = posLight.z / posLight.w - bias;
    
    float2 moments = depthMapTexture.Sample(shadowSampler, projectTexCoord).rg;
    float p = step(compare, moments.x);
    float variance = max(moments.y - moments.x * moments.x, 0.00002);
    
    float d = compare - moments.x;
    float pMax = LinStep(0.4, 1.0, variance / (variance + d * d));
   
 
    
    return 1-min(max(p, pMax), 1.0f);

}


float4 PS_Directional(PS_IN input) : SV_Target
{
    
    float3 albedoColor = DiffuseTex.Sample(textureSampler, input.col.xy);
    float3 worldPos = PositionTex.Sample(textureSampler, input.col.xy);
    float3 normal = NormalTex.Sample(textureSampler, input.col.xy);
    float3 metallicColor = MetallicTex.Sample(textureSampler, input.col.xy);
    float roughnessColor = MetallicTex.Sample(textureSampler, input.col.xy).a;
    float4 emissiveColor = EmissiveTex.Sample(textureSampler, input.col.xy);
    normal = normalize(normal);
  

    float3 lightVec = -normalize(lightData.dir.xyz).xyz;
    float3 toEye = normalize(eyePos.xyz - worldPos.xyz);
    float lightIntesity = lightData.color.w;
    float specularAlpha = 50.0f;
    float3 H = normalize(toEye + lightVec);

    float blendArea = 0.2f;
    
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
    float bias = 0.01f * (1 + layer);
    layer2 = clamp(layer + 1, 0, 3);
    
   
    float shadowSum1 = SampleVarianceShadowMap(worldPos,layer);

    
    lightIntesity = lightIntesity;
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedoColor, metallicColor);
    float3 instanceID = InstanceIDTex.Sample(textureSampler, input.col.xy);
    float3 resColor = PBR(normal, toEye, lightVec, H, F0,
    lightData.color.xyz, albedoColor, roughnessColor, metallicColor, emissiveColor, lightIntesity) * (instanceID.x > 0);
    
    
    return float4(resColor * (1 - shadowSum1), 1.0f);

}

float4 PS_Ambient(PS_IN input) : SV_Target
{
    float3 worldPos = PositionTex.Sample(textureSampler, input.col.xy);
    float3 toEye = normalize(eyePos.xyz - worldPos.xyz);
    float roughness = MetallicTex.Sample(textureSampler, input.col.xy).w;
    float3 metallic = MetallicTex.Sample(textureSampler, input.col.xy).rgb;
    float3 pixelColor = DiffuseTex.Sample(textureSampler, input.col.xy);
    float3 instanceID =InstanceIDTex.Sample(textureSampler, input.col.xy);
    float3 skyBoxColor = skyboxTexture.Sample(textureSampler, input.col.xy);
    float ssaoParam = AoTex.Sample(textureSampler, input.col.xy).r;
    float3 resColor;
    float lightIntensity = lightData.color.w;


    //Diffuse
    float3 normal = normalize(NormalTex.Sample(textureSampler, input.col).rgb);
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, pixelColor,metallic);

    float3 kS = FwithRoughness(max(dot(normal, toEye), 0.0f), F0, roughness);
    float3 lightDir = reflect(-toEye, normal);
    
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;   
    float3 irradiance = irradianceTexture.Sample(textureSampler, normal).rgb;
    float3 diffuse = kD * irradiance * pixelColor;

    //Specular
    const float maxReflectionLoad = 9;
    float3 prefilteredColor = prefilteredEnvironmentTexture.SampleLevel(textureSampler, lightDir, roughness * maxReflectionLoad);
    //prefilteredColor = pow(prefilteredColor, 1.0 / 2.2f);
    float2 envBRDF = iblLookUpTexture.Sample(textureSampler, float2(max(dot(normal, toEye), 0.0f), roughness));
    float3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);

    resColor = (instanceID.x > 0) * ssaoParam * lightIntensity*
    (diffuse + specular) + skyBoxColor * (instanceID.x < 0);
    return float4(resColor, 1.0f);
}