
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
    row_major float4x4 worldView;
    row_major float4x4 worldViewProj;
    row_major float4x4 worldViewInverseT;
    LightData lightData;
    float4 eyePos;
    row_major float4x4 viewProj[4];
    float4 distances[4];
};


Texture2D<float4> DiffuseTex : register(t0);
Texture2D<float4> NormalTex : register(t1);
Texture2D<float4> WorldPosTex : register(t2);
Texture2D<float4> DepthTex : register(t3);
Texture2D<float4> SpecularTex : register(t4);
Texture2DArray depthMapTexture : register(t5);



SamplerState textureSampler : SAMPLER : register(s0);
SamplerState depthMapSampler : register(s1);

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




float4 PS_Directional(PS_IN input) : SV_Target
{
    
    float3 pixelColor =DiffuseTex.Sample(textureSampler, input.col.xy);
    float3 worldPos = WorldPosTex.Sample(textureSampler, input.col.xy);
    float3 normal = NormalTex.Sample(textureSampler, input.col.xy);
    //pixelColor = pow(pixelColor, 2.2f);
    
    normal = (normal.xyz - float3(0.5f, 0.5f, 0.5f))*2.0f;
    normal = normalize(normal);
    float4 specular = SpecularTex.Sample(textureSampler, input.col.xy);
  

    float3 lightVec = -normalize(lightData.dir.xyz).xyz;
    
    float3 toEye = normalize(eyePos.xyz - worldPos.xyz);
    float lightIntesity = lightData.color.w;
    float specularAlpha = 50.0f;
    
    //www
    float bias = 0.001f;
    float3 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    int layer = 3;
    float depthVal = abs(mul(float4(worldPos, 1), worldView).z);
    for (int i = 0; i < 4; i++)
        if (depthVal < distances[i].x)
        {
            layer = i;
            break;
        }
    
  
    //if (layer == 0)
    //    return float4(1, 0, 0, 1);
    //else if (layer == 1)
    //    return float4(0, 1, 0, 1);
    //else if (layer == 2)
    //    return float4(0, 0, 1, 1);
    
    
    float4 posLight = float4(worldPos, 1);
    posLight = mul(posLight, viewProj[layer]);
    
    
    projectTexCoord.x = posLight.x / posLight.w / 2.0f + 0.5f;
    projectTexCoord.y = -posLight.y / posLight.w / 2.0f + 0.5f;
    projectTexCoord.z = layer;
  
    depthValue = depthMapTexture.Sample(textureSampler, projectTexCoord).r;
    lightDepthValue = posLight.z / posLight.w - bias;
    
   
    
    if (lightDepthValue > depthValue)
        lightIntesity = 0.0f;
        
    
    
    ///
  
    
    float3 diffuseFactor = lightIntesity * lightData.color.xyz * dot(lightVec, normal.xyz);
    
    diffuseFactor.x = max(diffuseFactor.x, 0);
    diffuseFactor.y = max(diffuseFactor.y, 0);
    diffuseFactor.z = max(diffuseFactor.z, 0);
    
    float3 specFactor = float3(0, 0, 0);
    
    if (length(diffuseFactor.xyz) > 0.0f)
    {
        float3 v = reflect(-lightVec, normal.xyz);
        specFactor = specular.xyz * lightIntesity * lightData.color.xyz * pow(max(dot(v, toEye), 0.0f), specularAlpha);
        
    }
    

    float3 resColor = diffuseFactor * pixelColor + specFactor;
    
    //resColor = pow(resColor, 0.45f);
    
    
    return float4(resColor, 1.0f);

}

float4 PS_Ambient(PS_IN input) : SV_Target
{
    
    float3 pixelColor = DiffuseTex.Sample(textureSampler, input.col.xy);
    //pixelColor = pow(pixelColor, 2.2f);
    float3 resColor =pixelColor*lightData.color.xyz;
    
    //resColor = pow(resColor,0.45f);
    return float4(resColor, 1.0f);

}