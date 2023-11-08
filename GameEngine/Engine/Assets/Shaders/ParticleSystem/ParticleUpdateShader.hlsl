#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

struct ParticleInputDataF
{
    float4 Fvalue;
    float4 RandomBetweenConstsF;
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

struct ParticleListItem
{
    uint index;
    float distance;
};

cbuffer Params : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 proj;
    float4 groupDim;
    float4 eyePos;
    SharedParticleData particleData;
};


RWStructuredBuffer<Particle> pool : register(u1);
AppendStructuredBuffer<uint> deadList : register(u2);
RWStructuredBuffer<ParticleListItem> sortList : register(u3);
//AppendStructuredBuffer<uint> sortListI : register(u4);




[numthreads(THREAD_GROUP_X, THREAD_GROUP_Y, 1)]
void CSMain(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint size, stride;
    float4 worldPos;
    pool.GetDimensions(size, stride);
    uint index = groupID.x * THREAD_GROUP_TOTAL + groupID.y * groupDim.x * THREAD_GROUP_TOTAL + groupIndex;
    
    float distance = length(eyePos.xyz - pool[index].position.xyz);
    
    if (index < size - 1 && pool[index].state.x)
    {
        pool[index].position.xyz += 
        pool[index].velocity * pool[index].velocity.w * particleData.deltaTime.x;
        pool[index].lifetime.x += particleData.deltaTime.x;
        
        if (pool[index].lifetime.x > pool[index].lifetime.y)
        {
           pool[index].state.x = false;
           deadList.Append(index);
        }
        else
        {

            uint ctr=sortList.IncrementCounter();
            float3 tmp = pool[index].position;
            //worldPos.xyz = mul(tmp, (float3x3) view);
            sortList[ctr].index = index;
            sortList[ctr].distance = length(tmp-eyePos.xyz);
        }
    }

}





