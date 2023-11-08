#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

#define SPHERE_SHAPE 0
#define CONE_SHAPE 1
#define BOX_SHAPE 2

struct SharedParticleData
{
    float4 deltaTime;
    float4 startPosition;
    float4 startSize;
    float4 startColor;
    float4 startLifeTime;
    float4 startVelocity;
    float4 shapeRadiusAngle;
};

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


RWStructuredBuffer<Particle> pool : register(u1);
ConsumeStructuredBuffer<int> deadList : register(u2);
Buffer<uint> particlesCounter : register(t0);
Texture2D<float4> noiseTex : register(t1);

SamplerState textureSampler : SAMPLER : register(s0);



//AppendStructuredBuffer<int> sortList : register(u3);

cbuffer Params : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 proj;
    float4 groupDim;
    float4 eyePos;
    SharedParticleData particleData;
};

//======================
// Compute Shader code  |
//=======================

float rand(float co,float2 seed)
{
    return 0.5 + (frac(sin(dot(float2(sin(co), cos(particleData.deltaTime.x)), float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

float3 randVec3(float co)
{
    float3 vec=noiseTex.SampleLevel(textureSampler, float2(sin(co) * 0.5f + 0.5f, sin(particleData.deltaTime.x) * 0.5f + 0.5f),0);
    vec -= float3(0.5f, 0.5f, 0.5f);
    vec = vec * 2;
    return normalize(vec);
}

float2 randVec2(float co)
{
    return normalize(randVec3(co).xz);
}

void EmitParticle(int index)
{
    pool[index].lifetime.xy = float2(0, particleData.startLifeTime.x);
    pool[index].state.x = true;
    pool[index].color = particleData.startColor;
    pool[index].position = particleData.startPosition;
    pool[index].size = particleData.startSize;
    
    if (particleData.shapeRadiusAngle.x == SPHERE_SHAPE) // Sphere
    {
        pool[index].velocity.xyz = randVec3(index);
        pool[index].position.xyz += randVec3(sqrt(index)) * particleData.shapeRadiusAngle.y;
    }
    else if (particleData.shapeRadiusAngle.x == CONE_SHAPE)
    {
        pool[index].position.xz = randVec2(index);
        pool[index].velocity

    }
    
    
        
}

[numthreads(1, 1, 1)]
void CSMain(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint maxParticles, strides;
    deadList.GetDimensions(maxParticles, strides);
    
    if (particlesCounter[0] < maxParticles)
    {
        int index = deadList.Consume();
        EmitParticle(index);
    }
}





