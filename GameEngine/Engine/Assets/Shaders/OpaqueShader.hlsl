
cbuffer mycBuffer : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 view;
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

Texture2D albedoTex : TEXTURE : register(t0);
Texture2D specularTex : TEXTURE: register(t1);
Texture2D roughnessTex : TEXTURE : register(t2);
Texture2D metallicTex : TEXTURE : register(t3);
Texture2D emissiveTex : TEXTURE : register(t4);
Texture2D normalMapTex : TEXTURE : register(t5);
Texture2D opacityTex : TEXTURE : register(t6);
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
    float4 Albedo : SV_Target0;
    float4 Metallic : SV_Target1;
    float4 Specular : SV_Target2;
    float4 Roughness : SV_Target3;
    float4 Emissive : SV_Target4;
    float4 Normals : SV_Target5;
    float4 Position : SV_Target6;
    float4 InstanceID : SV_Target7;
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
    float3 pixelColor = albedoTex.Sample(objSamplerState, input.col.xy)*(albedoVec.w<0) + 
    albedoVec.xyzw*(albedoVec.w>=0);
    float3 specularColor = specularTex.Sample(objSamplerState, input.col.xy) * (specularVec.w < 0) +
    specularVec*(specularVec.w>=0);
    float3 metallicColor = metallicTex.Sample(objSamplerState, input.col.xy) * (metallicVec.w < 0) +
    metallicVec * (metallicVec.w >= 0);
    float roughness = roughnessTex.Sample(objSamplerState, input.col.xy).r * (roughnessVec.r < 0) +
    roughnessVec.r * (roughnessVec.r>=0);
    float4 emissive = emissiveTex.Sample(objSamplerState, input.col.xy);
    
    float3 normal = normalMapTex.Sample(objSamplerState, input.col.xy);
    normal.x = normal.x* 2.0f - 1.0f;
    normal.y = normal.y * 2.0f - 1.0f;
    normal.z = normal.z;
    normal = normalize(normal);
    //normal.y = -normal.y;
    normal = mul(normal, (float3x3)worldView);
    
    
    //normal = input.normals;
    
    output.Normals = float4(normal ,1.0f);
    output.Position = float4(input.posW.xyz, 1.0f);
    output.Albedo = float4(pixelColor, 1.0f);
    output.Specular = float4(specularColor, 1);
    output.Metallic = float4(metallicColor, 1);
    output.Roughness = float4(roughness, roughness, roughness, 1);
    output.Emissive = emissive;
    output.InstanceID = float4(instanceID, instanceID, instanceID, 1.0f);
    //output.Depth = float4(input.posH.z / input.posH.w, input.posH.z / input.posH.w, input.posH.z / input.posH.w, 1.0f);
    //output.Specular = float4(0.5f, 0.5f, 0.5f, 50.0f);
      
    
    return output;
}