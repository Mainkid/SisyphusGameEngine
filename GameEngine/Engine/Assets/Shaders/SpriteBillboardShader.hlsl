cbuffer mycBuffer : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 worldView;
    row_major float4x4 worldViewProj;
    row_major float4x4 worldViewInverseT;
    float4 albedoVec;
    float4 metallicVec;
    float4 roughnessVec;
    float4 emissiveVec;
    float4 specularVec;
    uint instanceID;

};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D objTexture2 : TEXTURE: register(t1);
SamplerState objSamplerState : SAMPLER : register(s0);

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float4 normals : NORMAL;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION;
    float4 posWV : POSITION2;
    float4 col : COLOR;
    float4 normals : NORMAL;
};

struct GBuffer
{
    float4 Diffuse : SV_Target0;
    float4 InstanceIDs : SV_Target1;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    float4 res = input.col;

    output.normals.xyz = normalize(mul(input.normals.xyz, (float3x3) worldViewInverseT));
    output.posH = mul(input.pos, worldViewProj);
    output.posW = mul(input.pos, world);
    output.posWV = mul(input.pos, worldView);
    output.col = input.col;
	
    return output;
}




GBuffer PSMain(PS_IN input) : SV_Target
{
    GBuffer output = (GBuffer) 0;
    
    float4 pixelColor = objTexture.Sample(objSamplerState, input.col.xy);
    

    clip(pixelColor.a - 1.0f);
    output.Diffuse = pixelColor;
    output.InstanceIDs = float4(instanceID, 4, 5, 1.0f);
    
    return output;
}