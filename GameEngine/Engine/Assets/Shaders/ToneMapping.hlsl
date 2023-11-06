#pragma once

Texture2D<float4> HDRTex : register(t0);
SamplerState textureSampler : SAMPLER : register(s0);

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


PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = input.pos;
    output.col = input.col;
    
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float exposure = 1.5f;
    float3 hdr = HDRTex.Load(int3(input.pos.xy,0), 0);
    float3 ldr =float3(1.0,1.0f,1.0f) - exp(-hdr * exposure);
    //float3 ldr = hdr / (hdr + 1.0f);
    hdr = pow(ldr, 1.0f / 2.2f);
    
    
    return float4(hdr, 1.0f);
}