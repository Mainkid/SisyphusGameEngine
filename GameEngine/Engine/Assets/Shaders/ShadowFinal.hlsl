
Texture2D<float4> ShadowTex : register(t0);
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


PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = input.pos;
    output.col = input.col;
    return output;
}






float4 PS(PS_IN input) : SV_Target
{
  
    float3 pixelColor = clamp(ShadowTex.Sample(textureSampler, input.col.xy), 0, 1);
    return float4(pixelColor - 1.0, pixelColor.x - 1.0);
}