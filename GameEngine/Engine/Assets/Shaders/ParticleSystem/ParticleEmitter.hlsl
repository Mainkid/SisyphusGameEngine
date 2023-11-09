#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

//Shapes
#define SPHERE_SHAPE 0
#define CONE_SHAPE 1
#define BOX_SHAPE 2

//InputTypes
#define SimpleFloat 0
#define  RandomBetweenFloats 1
#define SimpleVector 2
#define RandomBetweenVectors 3
#define SimpleCurve 4


struct ParticleInputDataCurve
{
    float2 P0;
    float2 P1;
    float2 P2;
    float2 P3;
    float4 IsUsing;
};


struct ParticleInputDataF
{
    float4 Fvalue;
    float4 RandomBetweenConstsF;   // x - f1, y - f2
    float4 InputType;
};

struct SharedParticleData
{
    float4 deltaTime;
    float4 startPosition;
    ParticleInputDataF startSize;
    float4 startColor;
    ParticleInputDataF startLifeTime;
    ParticleInputDataF startVelocity;
    ParticleInputDataF startRotation;
    ParticleInputDataCurve SizeOverLifetime;
    ParticleInputDataCurve SpeedOverLifetime;
    ParticleInputDataF RotationOverLifetime;
    float4 shapeRadiusAngle;
};

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

float ProcessFloatInput(ParticleInputDataF inputData, int seed)
{
    if (inputData.InputType.x == SimpleFloat)
    {
        return inputData.Fvalue.x;
    }
    else if (inputData.InputType.x == RandomBetweenFloats)
    {
        return lerp(inputData.RandomBetweenConstsF.x, inputData.RandomBetweenConstsF.y, saturate(randVec3(seed).x * 0.5f + 0.5f));
    }
    return 0;
}

void EmitParticle(int index)
{
    pool[index].lifetime.xy = float2(0, ProcessFloatInput(particleData.startLifeTime, index));
    pool[index].state.x = true;
    pool[index].color = particleData.startColor;
    pool[index].position = particleData.startPosition;
    pool[index].sizeAndRot.x = ProcessFloatInput(particleData.startSize,index);
    pool[index].sizeAndRot.y = ProcessFloatInput(particleData.startRotation, index); //оепедекюрэ
    pool[index].velocity.w = ProcessFloatInput(particleData.startVelocity, index);
    
    pool[index].state.y = pool[index].sizeAndRot.x;
    pool[index].state.w = pool[index].sizeAndRot.y;
    pool[index].state.z = pool[index].velocity.w;
    
    if (particleData.shapeRadiusAngle.x == SPHERE_SHAPE) // Sphere
    {
        pool[index].velocity.xyz = randVec3(index);
        pool[index].position.xyz += randVec3(sqrt(index)) * particleData.shapeRadiusAngle.y;
    }
    else if (particleData.shapeRadiusAngle.x == CONE_SHAPE)
    {
        float3 randVec = randVec3(index);
        float2 offsetSpawnPosition = randVec.xy * particleData.shapeRadiusAngle.y * abs(randVec.z)/1.0f;
        
        pool[index].position.xz += offsetSpawnPosition;
        
        float lerpRadius = length(offsetSpawnPosition)/particleData.shapeRadiusAngle.y;
        
        float2 targetTopOffset = offsetSpawnPosition + normalize(offsetSpawnPosition) * tan(radians(particleData.shapeRadiusAngle.z))*lerpRadius;
        
        float3 velocityVec = normalize(float3(targetTopOffset.x, 1.0f, targetTopOffset.y)
        - float3(offsetSpawnPosition.x, 0, offsetSpawnPosition.y));
        pool[index].velocity.xyz = velocityVec;
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





