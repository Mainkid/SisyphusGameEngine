#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

#define AMBIENT 0
#define DIRECTIONAL 1
#define POINTLIGHT 2
#define SPOTLIGHT 3


struct Particle
{
    float4 position;
    float4 sizeAndRot;
    float4 velocity;
    float4 lifetime;
    float4 color;
    float4 state;
    float4 shapeRadiusAngle;
};

struct ParticleListItem
{
    uint index;
    float distance;
};

struct LightData
{
    float4 Pos;
    float4 Dir;
    float4 Color;
    float4 additiveParams;
};

struct LightingO
{
    float3 basis_col0;
    float3 basis_col1;
    float3 basis_col2;
    float3 ambient;
};



Texture2DArray depthMapTexture : register(t0);

StructuredBuffer<Particle> Particles : register(t1);
StructuredBuffer<ParticleListItem> Indices : register(t2);

Texture2D textureDiffuse : register(t3);

SamplerState textureSampler : SAMPLER : register(s0);
SamplerState depthMapSampler : register(s1);
//AppendStructuredBuffer<uint> gOutput;

cbuffer Params : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
    row_major float4x4 cameraRot;
    LightData light[10];
    float3 viewDirection;
    int lightCount;
    float4 eyePos;
    row_major float4x4 viewProj[4];
    float4 distances[4];
    float4 IsLit_Ambient_params;
};

struct VertexInput
{
    uint VertexID : SV_VertexID;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    LightingO lightO : COLOR;
    float3 worldPos : POS;
    float3 normal : POSITION;
    float2 uv:COL;
    float4 color : NOR;
    //float4 UV : POSITION;
};

static const float4 basisX = float4(0.8164965f, 0, 0.57735027f, 0);
static const float4 basisY = float4(-0.4082482f, 0.70710678f, 0.57735027f, 0);
static const float4 basisZ = float4(-0.4082482f, -0.70710678f, 0.57735027f, 0);

struct PixelOutput
{
    float4 Color : SV_Target;
};

//===============
// Compute Shader code  |
//===============


[numthreads(THREAD_GROUP_X, THREAD_GROUP_Y, 1)]
void DefaultCS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{

}

//float LinStep(float low, float high, float v)
//{
//    return clamp((v - low) / (high - low), 0.0f, 1.0f);
//}

//float SampleVarianceShadowMap(float3 worldPos, int layer)
//{
//    float4 posLight = float4(worldPos, 1);
//    float3 projectTexCoord;
//    float shadowSum = 0;
//    float depthValue = 0;
//    float bias = 0.001f;
    
//    posLight = mul(posLight, viewProj[layer]);
//    projectTexCoord.x = posLight.x / posLight.w / 2.0f + 0.5f;
//    projectTexCoord.y = -posLight.y / posLight.w / 2.0f + 0.5f;
//    projectTexCoord.z = layer;
    
//    float compare = posLight.z / posLight.w - bias;
    
//    float2 moments = depthMapTexture.Sample(textureSampler, projectTexCoord).rg;
//    float p = step(compare, moments.x);
//    float variance = max(moments.y - moments.x * moments.x, 0.00002);
    
//    float d = compare - moments.x;
//    float pMax = LinStep(0.4, 1.0, variance / (variance + d * d));
   
 
    
//    return 1 - min(max(p, pMax), 1.0f);

//}


LightingO AccumulateLighting(in LightingO o, float3 lightDir, float3 lightCol, float atten,bool is_ambient)
{
    
    float3 hl2_basis0 = mul(basisX, cameraRot).xyz;
    float3 hl2_basis1 = mul(basisY, cameraRot).xyz;
    float3 hl2_basis2 = mul(basisZ, cameraRot).xyz;
    
    hl2_basis0.xyz = normalize(hl2_basis0.xyz);
    hl2_basis1.xyz = normalize(hl2_basis1.xyz);
    hl2_basis2.xyz = normalize(hl2_basis2.xyz);
    
    lightCol *= atten;
    float3 weights;
    weights.x = saturate(dot(lightDir, hl2_basis0));
    weights.y = saturate(dot(lightDir, hl2_basis1));
    weights.z = saturate(dot(lightDir, hl2_basis2));
    

   if(!is_ambient)
    {
        o.basis_col0 += lightCol * weights.x;
        o.basis_col1 += lightCol * weights.y;
        o.basis_col2 += lightCol * weights.z;
    }
    else
    {
        //o.basis_col0 += lightCol;
        //o.basis_col1 += lightCol;
        //o.basis_col2 += lightCol;
        
       
    }
    
        return o;
    }


float2 Rotate2D(float2 p, float angle)
{
    return float2(p.x * cos(angle) - p.y * sin(angle), p.x * sin(angle) + p.y * cos(angle));
}

//===============
// Shader code  |
//===============

PixelInput _offsetNprojected(PixelInput data, float2 offset, float2 uv)
{
    data.Position.xy += offset;
    data.Position = mul(data.Position, Projection);
    //data.UV = uv;

    return data;
}

PixelInput DefaultVS(VertexInput input)
{
    PixelInput output = (PixelInput) 0;

    uint index = Indices[input.VertexID/4].index;
    Particle particle = Particles[index];
    
    float4 worldPosition;
    const float size = particle.sizeAndRot.x;
    
    worldPosition = float4(particle.position.xyz, 1);
    worldPosition = mul(worldPosition, View);
    float4 startWorldPosition = worldPosition;
    
    if (input.VertexID % 4 == 0)
    {
        worldPosition.xy += Rotate2D(float2(-1, -1), radians(particle.sizeAndRot.y)) * size;
        output.uv.xy = float2(0, 0);

    }
    if (input.VertexID % 4 == 1)
    {
        worldPosition.xy += Rotate2D(float2(1, -1), radians(particle.sizeAndRot.y)) * size;
        output.uv.xy = float2(1, 0);
    }
    if (input.VertexID % 4 == 2)
    {
        output.uv.xy = float2(1, 1);
        worldPosition.xy += Rotate2D(float2(1, 1), radians(particle.sizeAndRot.y)) * size;
    }
    if (input.VertexID % 4 == 3)
    {
        output.uv.xy = float2(0, 1);
        worldPosition.xy += Rotate2D(float2(-1, 1), radians(particle.sizeAndRot.y)) * size;
    }
    
    
    
    for (int i = 0; i < lightCount; i++)
    {
        float3 dir;
        float att;
        float distance;
        
        //float bias = 0.001f;
        //float3 projectTexCoord;
        //float depthValue;
        //float lightDepthValue;
        //int layer = 3;
        //float depthVal = abs(mul(float4(particle.position.xyz, 1), View).z);
        //for (int j = 0; j < 4; j++)
        //    if (depthVal < distances[j].x)
        //    {
        //        layer = j;
        //        break;
        //    }
    
        //float4 posLight = float4(particle.position.xyz, 1);
        //posLight = mul(posLight, viewProj[layer]);
    
    
        //projectTexCoord.x = posLight.x / posLight.w / 2.0f + 0.5f;
        //projectTexCoord.y = -posLight.y / posLight.w / 2.0f + 0.5f;
        //projectTexCoord.z = layer;
        
        //float depth = depthMapTexture.SampleLevel(textureSampler, projectTexCoord, 0);
        //float shadow = 1.0f;
        //if (depth > depthVal)
        //{
        //    shadow = 0.0f;
        //}
        output.lightO.ambient = IsLit_Ambient_params.y;
        if (IsLit_Ambient_params.x)
        {
        
  
            if (abs(light[i].additiveParams.w - AMBIENT) < 0.01f)
                output.lightO = AccumulateLighting(output.lightO, dir, light[i].Color.xyz, light[i].Color.w, true);
            else if (abs(light[i].additiveParams.w - DIRECTIONAL) < 0.01f)
            {
                output.lightO = AccumulateLighting(output.lightO, light[i].Dir, light[i].Color.xyz, light[i].Color.w, false);
            }
            else if (abs(light[i].additiveParams.w - POINTLIGHT) < 0.01f)
            {
                dir = particle.position.xyz - light[i].Pos.xyz;
                distance = length(dir);
                dir = normalize(dir);
                att = pow((1 - distance / light[i].additiveParams.r), 2.0f);
                if (distance < light[i].additiveParams.r)
                    output.lightO = AccumulateLighting(output.lightO, dir, light[i].Color.xyz, att, false);
            }
        }
    }
    

    
    
    
    output.normal = normalize(lerp(normalize(-viewDirection), normalize(worldPosition.xyz - startWorldPosition.xyz), 0.5f));
    output.worldPos = particle.position.xyz;

   
    worldPosition = mul(worldPosition, Projection);
    output.Position = worldPosition;
    output.color = particle.color;
    //output.UV = 0;
    return output;

}

    
[maxvertexcount(4)] // ��������� ������ GS � 4 ��������, ������� �������� TriangleStrip
void TriangleGS(triangle PixelInput input[3], inout TriangleStream<PixelInput> stream)
{
    PixelInput pointOut = input[0];
	
   
    stream.Append(pointOut);
}


PixelOutput DefaultPS(PixelInput input) : SV_Target
{
    PixelOutput output = (PixelOutput) 0;
    
    
    float3 n = normalize(-input.normal);
    float3 w;
    float3 hl2_basis0 = mul(basisX, cameraRot).xyz;
    float3 hl2_basis1 = mul(basisY, cameraRot).xyz;
    float3 hl2_basis2 = mul(basisZ, cameraRot).xyz;
    
    hl2_basis0.xyz = normalize(hl2_basis0.xyz);
    hl2_basis1.xyz = normalize(hl2_basis1.xyz);
    hl2_basis2.xyz = normalize(hl2_basis2.xyz);
    
    w.x = saturate(dot(n, hl2_basis0));
    w.y = saturate(dot(n, hl2_basis1));
    w.z = saturate(dot(n, hl2_basis2));
    float3 diffuse = input.lightO.basis_col0 * w.x + input.lightO.basis_col1 * w.y + input.lightO.basis_col2 * w.z + input.lightO.ambient;
    
    float4 pixelColor = textureDiffuse.Sample(textureSampler, input.uv.xy);
    //clip(pixelColor.a -1.0f);
    if (IsLit_Ambient_params.x)
        output.Color = float4(pixelColor.xyz * diffuse, pixelColor.r);
    else
        output.Color = float4(pixelColor.xyz, pixelColor.r);
    return output;
}


