
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
};

struct PS_IN
{
    float4 pos :SV_POSITION;
    float4 col :COLOR;
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
    output.pos = input.pos;
    output.col = input.col;
    //output.pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //output.col = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}




float4 PS_SpotLight(PS_IN input) : SV_Target
{
    //return float4(1.0f, 1.0f, 0.0f, 1.0f);
    
    float3 pixelColor =DiffuseTex.Sample(textureSampler, input.col.xy);
    float3 worldPos = WorldPosTex.Sample(textureSampler, input.col.xy);
    float3 normal = NormalTex.Sample(textureSampler, input.col.xy);
    normal = (normal.xyz - float3(0.5f, 0.5f, 0.5f)) * 2.0f;
    normal = normalize(normal);
   
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    
   
    float3 lightVec = worldPos.xyz - lightData.pos.xyz;
    float distance = length(lightVec);
    
    // *** NEW IMPLEMENTATION *** //
    //distance.w - length of spotlight
    //param.r - length of spotlight;
    //param.g - theta;
    //param.b - innerCuttoff;
    //param.a - outerCuttof;
    
    float theta = dot(normalize(lightVec), normalize(lightData.dir.xyz));
    float innerCuttoff = lightData.params.b;
    float outerCuttoff = lightData.params.a;
    float epsilon = innerCuttoff - outerCuttoff;
    float intensity = clamp((theta - outerCuttoff) / epsilon, 0.0f, 1.0f);
    
    //Convert to param!!!
    if (theta < lightData.params.g || distance > lightData.params.r)      
        return float4(0, 0, 0, 1);
    else
        return float4(intensity * lightData.color.w * lightData.color.xyz * pixelColor, 1);
    
    
    //lightVec /= distance;
   
    
    float3 toEye = normalize(eyePos.xyz - worldPos.xyz);
    float lightIntesity = lightData.color.w;
    float specularAlpha = 50.0f;
  

    
    float3 diffuseFactor = lightIntesity * lightData.color.xyz * dot(lightVec, normal.xyz);
    
    diffuseFactor.x = max(diffuseFactor.x, 0);
    diffuseFactor.y = max(diffuseFactor.y, 0);
    diffuseFactor.z = max(diffuseFactor.z, 0);
    
    float3 specFactor = float3(0, 0, 0);
    
    //if (length(diffuseFactor.xyz) > 0.0f)
    //{
    //    float3 v = reflect(-lightVec, normal.xyz);
    //    specFactor = specular.xyz * lightIntesity * lightData.color.xyz * pow(max(dot(v, toEye), 0.0f), specularAlpha);
        
    //}
    
    //float spot =pow(max(dot(-lightVec,lightData.dir.xyz),0.0f),2.0f);
    //float att = 1.0f / dot(lightData.params.yzw, float3(1.0f, distance, distance * distance));

    
    float3 resColor = diffuseFactor*intensity* pixelColor;
    
    
    return float4(resColor, 1.0f);
    
    

}

