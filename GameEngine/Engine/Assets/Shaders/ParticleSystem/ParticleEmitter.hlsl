#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

struct SharedParticleData
{
    float4 deltaTime;
    float4 startPosition;
    float4 startSize;
    float4 startColor;
    float4 startLifeTime;
    float4 startVelocity;
};

struct Particle
{
    float4 position;
    float4 size;
    float4 velocity;
    float4 lifetime;
    float4 color;
    float4 state;
};


RWStructuredBuffer<Particle> pool : register(u1);
ConsumeStructuredBuffer<int> deadList : register(u2);
Buffer<uint> particlesCounter : register(t0);
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


[numthreads(1, 1, 1)]
void CSMain(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint maxParticles, strides;
    deadList.GetDimensions(maxParticles, strides);
    
    if (particlesCounter[0] < maxParticles)
    {
        int index = deadList.Consume();
        pool[index].lifetime.xy = float2(0, particleData.startLifeTime.x);
        pool[index].state.x = true;
        pool[index].color = particleData.startColor;
        pool[index].position = particleData.startPosition;
        pool[index].size = particleData.startSize;
    }
}





