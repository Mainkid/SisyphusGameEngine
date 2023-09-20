
cbuffer mycBuffer : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 worldView;
    row_major float4x4 worldViewProj;
    row_major float4x4 worldViewInverseT;
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
    float4 Normals : SV_Target0;
    float4 DiffuseSpec : SV_Target1;
    float4 WorldPos : SV_Target2;
    float4 InstanceIDs : SV_Target3;
    float4 Specular : SV_Target3;
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
    float3 pixelColor = objTexture.Sample(objSamplerState, input.col.xy);
    
    output.Normals = float4(input.normals.xyz/2 + float3(0.5f, 0.5f, 0.5f),1.0f);
    output.WorldPos = float4(input.posW.xyz, 1.0f);
    output.DiffuseSpec = float4(pixelColor, 1.0f);
    output.InstanceIDs = float4(instanceID, instanceID, instanceID, 1.0f);
    //output.Depth = float4(input.posH.z / input.posH.w, input.posH.z / input.posH.w, input.posH.z / input.posH.w, 1.0f);
    output.Specular = float4(0.5f, 0.5f, 0.5f, 50.0f);
      
    
    return output;
}