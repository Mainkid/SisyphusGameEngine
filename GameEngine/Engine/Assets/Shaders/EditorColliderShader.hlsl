cbuffer mycBuffer : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 view;
    row_major float4x4 worldView;
    row_major float4x4 worldViewProj;
    row_major float4x4 worldViewInverseT;
    row_major float4x4 viewProjs[4];
    float4 eyePos;
}

struct VS_IN
{
    float3 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_Position;
    float4 worldPos : Position;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = mul(float4(input.pos, 1), worldViewProj);
    output.worldPos = mul(float4(input.pos, 1), worldView);
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    return float4(1, 0, 0, 1);
}