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
    float4 pos : POSITION0;
    float4 color : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_Position;
    float4 worldPos : Position;
    float4 color : COLOR;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN)0;
    output.pos = mul(input.pos, worldViewProj);
    output.worldPos = mul(input.pos, worldView);
    output.color = input.color;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = input.color;
    return float4(col.xyz,1);

}