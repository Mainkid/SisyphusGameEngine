
#define MAX_BONES 100
#define MAX_BONES_INFLUENCE 4

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
    float4 instanceID;
};

cbuffer SkeletalBuffer : register(b1)
{
   row_major float4x4 finalBonesMatrices[MAX_BONES];
};

Texture2D albedoTex : TEXTURE : register(t0);
Texture2D roughnessTex : TEXTURE : register(t1);
Texture2D metallicTex : TEXTURE : register(t2);
Texture2D emissiveTex : TEXTURE : register(t3);
Texture2D normalMapTex : TEXTURE : register(t4);
Texture2D opacityTex : TEXTURE : register(t5);
SamplerState objSamplerState : SAMPLER : register(s0);

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float4 normals : NORMAL;
    float4 tangents : TANGENT;
    float4 bitangents : BITANGENT;
    float4x1 boneIDs : BONE;
    float4x1 boneWeights : BONEWEIGHTS;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION;
    float4 posWV : POSITION2;
    float4 col : COLOR;
    float4 normals : NORMAL;
    float3x3 TBN : POSITION3;
    float4 debugColor : COLOR2;
};

struct GBuffer
{
    float4 Albedo : SV_Target0;
    float4 Metallic : SV_Target1;
    float Depth : SV_Target2;
    float4 Emissive : SV_Target3;
    float4 Normals : SV_Target4;
    float4 Position : SV_Target5;
    float4 InstanceID : SV_Target6;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.debugColor = float4(0, 0, 0, 1);
    float4 totalPosition = float4(0, 0, 0, 0);
    for (int i = 0; i < MAX_BONES_INFLUENCE; i++)
    {
        if (input.boneIDs[i] < 0)
            continue;
        if (input.boneIDs[i] >= MAX_BONES)
        {
            totalPosition = input.pos;
            break;
        }
        
        if (input.boneIDs[i] == 14)
            output.debugColor = float4(input.boneWeights[i], 0, 0, 1);
        
        float4 localPosition = mul(input.pos, finalBonesMatrices[int(input.boneIDs[i])]);
        totalPosition += localPosition * float(input.boneWeights[i]);
        input.normals = mul(input.normals, finalBonesMatrices[int(input.boneIDs[i])]);
    }
    
    
    
    if (input.boneIDs[0] < 0)
        totalPosition = input.pos;

    totalPosition.w = 1;
    
    output.normals.xyz = normalize(mul(input.normals.xyz, (float3x3) worldViewInverseT));
    output.posH = mul(totalPosition, worldViewProj);
    output.posW = mul(totalPosition, world);
    output.posWV = mul(totalPosition, worldView);
    output.col = input.col;
    
    
    float3 T = normalize(mul(float3(input.tangents.xyz), (float3x3) world));
    float3 B = normalize(mul(float3(input.bitangents.xyz), (float3x3) world));
    float3 N = normalize(mul(float3(input.normals.xyz), (float3x3) world));
    output.TBN = float3x3(T.xyz, B.xyz,N.xyz);
	
    return output;
}




GBuffer PSMain(PS_IN input) : SV_Target
{
    GBuffer output = (GBuffer) 0;
    
    
    float3 pixelColor = albedoTex.Sample(objSamplerState, input.col.xy).xyz * (albedoVec.w < 0) + albedoVec.xyz * (albedoVec.w >= 0);
    float3 metallicColor = metallicTex.Sample(objSamplerState, input.col.xy).xyz * (metallicVec.w < 0) + metallicVec.xyz * (metallicVec.w >= 0);
    float roughness = roughnessTex.Sample(objSamplerState, input.col.xy).r * (roughnessVec.w < 0) + roughnessVec.xyz * (roughnessVec.w >= 0);
    float4 emissive = emissiveTex.Sample(objSamplerState, input.col.xy) * (emissiveVec.w < 0) + emissiveVec.xyzw * (emissiveVec.w >= 0);
    
    pixelColor = input.debugColor;
    
    float3 normal = normalMapTex.Sample(objSamplerState, input.col.xy);
    normal = normal* 2.0f - 1.0f;
    
    normal = normalize(normal);
    //normal.y = -normal.y;
    //normal.y = -normal.y;
    normal = normalize(mul(normal, input.TBN));
    
    //normal = input.normals;
    //normal = input.normals;
    
    output.Normals = float4(normal * (roughnessVec.g < 0) + input.normals.xyz * (roughnessVec.g >= 0), input.posH.z/input.posH.w);
    output.Position = float4(input.posW.xyz, 1.0f);
    output.Albedo = float4(pixelColor, 1.0f);
    output.Metallic = float4(metallicColor, roughness);
    //output.Roughness = float4(roughness, roughness, roughness, 1);
    output.Emissive = emissive;
    output.InstanceID = instanceID;
    output.Depth = input.posH.z / input.posH.w;
    //output.Depth = float4(input.posH.z / input.posH.w, input.posH.z / input.posH.w, input.posH.z / input.posH.w, 1.0f);
    //output.Specular = float4(0.5f, 0.5f, 0.5f, 50.0f);
      
    
    return output;
}