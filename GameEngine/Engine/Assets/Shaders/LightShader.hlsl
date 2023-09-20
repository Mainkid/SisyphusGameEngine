
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
Texture2D shadowTexture : register(t5);
Texture2D skyboxTexture : register(t6);
Texture2D depthTexture : register(t7);


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
    
    //PCF --->
    
    float texelWidth = 1.0 / 1280.0f;
    float texelHeight = 1.0 / 720.0f;
    float2 texelSize = float2(texelWidth, texelHeight);
    float shadowSum = 0.0f;
    
    for (int y = -3; y <= 3; y++)
    {
        for (int x = -3; x <= 3; x++)
        {
            float2 offset = float2(x, y) * texelSize;
            shadowSum=shadowSum+ shadowTexture.Sample(textureSampler, input.col.xy + offset);
        }
    }
    shadowSum = shadowSum / 49.0f;
    
    //<--- PCF
    
    lightIntesity = lightIntesity * shadowSum;
        
    
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
    
    
    return float4(resColor, 1.0f);

}

float4 PS_Ambient(PS_IN input) : SV_Target
{
    
    float3 pixelColor = DiffuseTex.Sample(textureSampler, input.col.xy);
    float3 instanceID = DepthTex.Sample(textureSampler, input.col.xy);
    float3 skyBoxColor = skyboxTexture.Sample(textureSampler, input.col.xy);
    float3 resColor;
    resColor = pixelColor * lightData.color.xyz * (instanceID.x > 0) + skyBoxColor*(instanceID.x<0);
    

    return float4(resColor, 1.0f);


}