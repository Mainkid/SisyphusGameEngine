TextureCube cubeMap : register(t0);
Texture2D<float4> WorldPosTex : register(t1);
SamplerState texSampler : register(s0);

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

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = input.pos;
    output.col = input.col;
    return output;
}


float4 PS(PS_IN input) : SV_Target
{
    float3 worldPos = WorldPosTex.Sample(texSampler, input.col.xy);
    float3 fragToLight = -(lightData.pos.xyz - worldPos).xyz;
    float currentDepth = length(fragToLight.xyz);
    bool res = currentDepth < lightData.params.r;
    fragToLight.xyz=normalize(fragToLight.xyz);
    
    float3 depthCoord = cubeMap.Sample(texSampler, fragToLight.xyz);
    float3 depthToLight = -(lightData.pos.xyz - depthCoord).xyz;
    //depthToLight.y = depthToLight.y * (-1);
    float bias =0.03f;
    float4 posLight = float4(worldPos, 1);
    float pixelColor = currentDepth-bias  < length(depthToLight);
    return float4(pixelColor*res, pixelColor*res, pixelColor*res, 1.0f*res);
}