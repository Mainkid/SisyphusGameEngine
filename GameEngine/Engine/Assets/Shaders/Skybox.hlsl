TextureCube g_TexCube : register(t0);
SamplerState g_Sam : register(s0);

cbuffer CBChangesEveryFrame : register(b0)
{
    row_major matrix viewProj;
}

struct PosIn
{
    float3 PosL : POSITION;
};

struct PosOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

PosOut VS(PosIn vIn)
{
    PosOut vOut;
         // Set z = w make z / w = 1 (the sky box is kept far plane)
    float4 posH = mul(float4(vIn.PosL, 0.0f), viewProj);
    vOut.PosH = float4(posH.xy, posH.w * 0.9999f, posH.w);
    vOut.PosL = vIn.PosL;
    return vOut;
}


float4 PS(PosOut pIn) : SV_Target
{
    return float4(g_TexCube.Sample(g_Sam, pIn.PosL).xyz, 1.0f);
}