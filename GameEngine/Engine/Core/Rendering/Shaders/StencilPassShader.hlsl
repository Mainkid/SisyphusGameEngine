
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

};


Texture2D<float4> DiffuseTex : register(t0);
Texture2D<float4> NormalTex : register(t1);
Texture2D<float4> WorldPosTex : register(t2);
Texture2D<float4> DepthTex : register(t3);
Texture2D<float4> SpecularTex : register(t4);


SamplerState textureSampler : SAMPLER : register(s0);

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float4 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos :SV_POSITION;
    float4 col :COLOR;
    float4 normal : NORMAL;
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
    output.pos = mul(input.pos, worldViewProj);
    output.col = input.col;
    //output.pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //output.col = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}




float4 PSMain(PS_IN input) : SV_Target
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);

}