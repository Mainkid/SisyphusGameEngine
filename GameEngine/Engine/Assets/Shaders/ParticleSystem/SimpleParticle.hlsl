#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768



struct Particle
{
    float4 position;
    float4 size;
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
 //   uint index = groupID.x * THREAD_GROUP_TOTAL + groupID.y * 37 * THREAD_GROUP_TOTAL + groupIndex;
	
	//[flatten]
 //   if (index >= 999)
 //       return;

 //   Particle particle = Particles[index];

 //   float3 position = particle.position;
 //   float3 velocity = particle.velocity;

 //       // payload
    
    

 //   particle.position.xyz = position + float3(0.0f, 0.01f, 0.0f);
 //   particle.velocity.xyz = velocity;

 //   Particles[index] = particle;
}



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
        
        o.ambient = lightCol;
    }
    
        return o;
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
    const float size = particle.size.x;
    
    worldPosition = float4(particle.position.xyz, 1);
    worldPosition = mul(worldPosition, View);
    
    if (input.VertexID % 4 == 0)
    {
        worldPosition.xy += float2(-1, -1) * size;
        output.uv.xy = float2(0, 0);

    }
    if (input.VertexID % 4 == 1)
    {
        worldPosition.xy += float2(1, -1) * size;
        output.uv.xy = float2(1, 0);
    }
    if (input.VertexID % 4 == 2)
    {
        output.uv.xy = float2(1, 1);
        worldPosition.xy += float2(1, 1) * size;
    }
    if (input.VertexID % 4 == 3)
    {
        output.uv.xy = float2(0, 1);
        worldPosition.xy += float2(-1, 1) * size;
    }
    
    
    
    //for (int i = 0; i < lightCount; i++)
    //{
    //    float3 dir = light[i].Dir.xyz;
    //    float distance = -1;
    //    float att = light[i].Color.w;
  
    //    if (light[i].additiveParams.r>0)
    //    {
    //        dir = particle.position.xyz - light[i].Pos.xyz;
    //        distance = length(dir);
    //        dir = normalize(dir);
    //        float att = 1.0f / dot(light[i].additiveParams.yzw, float3(1.0f, distance * light[i].additiveParams.r, distance * distance * light[i].additiveParams.r));
    //    }
    //    if (distance<light[i].additiveParams.r)
    //        output.lightO = AccumulateLighting(output.lightO, dir, light[i].Color.xyz, att, ((light[i].Dir.xyz == float3(0, 0, 0)).x && (light[i].Pos.xyz == float3(0, 0, 0)).x).x);

    //}
    

    //float bias = 0.001f;
    //float3 projectTexCoord;
    //float depthValue;
    //float lightDepthValue;
    //int layer = 3;
    //float depthVal = abs(mul(float4(particle.position.xyz, 1), View).z);
    //for (i = 0; i < 4; i++)
    //    if (depthVal < distances[i].x)
    //    {
    //        layer = i;
    //        break;
    //    }
    
    //float4 posLight = float4(particle.position.xyz, 1);
    //posLight = mul(posLight, viewProj[layer]);
    
    
    //projectTexCoord.x = posLight.x / posLight.w / 2.0f + 0.5f;
    //projectTexCoord.y = -posLight.y / posLight.w / 2.0f + 0.5f;
    //projectTexCoord.z = layer;
    //depthValue = depthMapTexture.SampleLevel(textureSampler, projectTexCoord, 1);
    //lightDepthValue = posLight.z / posLight.w - bias;
    
    
    
    
    //if (lightDepthValue > depthValue)
    //{
    //    output.lightO.basis_col0 = output.lightO.ambient;
    //    output.lightO.basis_col1 = output.lightO.ambient;
    //    output.lightO.basis_col2 = output.lightO.ambient;
    //}
    
    
    output.normal = normalize(-viewDirection);
    output.worldPos = particle.position.xyz;

   
    worldPosition = mul(worldPosition, Projection);
    output.Position = worldPosition;
    output.color = particle.color;
    //output.UV = 0;
    return output;

}

    
[maxvertexcount(4)] // результат работы GS Ц 4 вертекса, которые образуют TriangleStrip
void TriangleGS(triangle PixelInput input[3], inout TriangleStream<PixelInput> stream)
{
    PixelInput pointOut = input[0];
	
   
    stream.Append(pointOut);
}


PixelOutput DefaultPS(PixelInput input) : SV_Target
{
    PixelOutput output = (PixelOutput) 0;
    output.Color = float4(input.color);
    return output;
    
    
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
    clip(pixelColor.a -1.0f);
    output.Color = float4(pixelColor.xyz*diffuse, 1);
    
}


