#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768



struct Particle
{
    float4 position;
    float4 initialPos;
    float4 velocity;
    float4 lifetime;
    bool4 state;
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
    
};


RWStructuredBuffer<Particle> pool : register(u1);
AppendStructuredBuffer<uint> deadList : register(u2);
RWStructuredBuffer<ParticleListItem> sortList : register(u3);
//AppendStructuredBuffer<uint> sortListI : register(u4);

float3 GetVectorFieldVelocity(float x, float y, float z)
{
    return normalize(float3(-z * y * y * y / (x * x + z * z + 0.0001f), y + 0.0001f,
        x * y * y * y / (x * x + z * z+0.0001f)));
    
    //return normalize(float3(x / (x * x + y * y + z * z), y / (x * x + y * y + z * z), z / (x * x + y * y + z * z)));

}

float3 GetVectorFieldFromCamera(float3 particlePos, float3 eyePos)
{
    float x=particlePos.x-eyePos.x;
    float y = particlePos.y - eyePos.y;
    float z = particlePos.z - eyePos.z;
    return normalize(float3(x / (x * x + y * y + z * z + 0.0001f), y / (x * x + y * y + z * z + 0.0001f), z / (x * x + y * y + z * z + 0.0001f)));

}


[numthreads(THREAD_GROUP_X, THREAD_GROUP_Y, 1)]
void DefaultCS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint size, stride;
    float4 worldPos;
    pool.GetDimensions(size, stride);
    uint index = groupID.x * THREAD_GROUP_TOTAL + groupID.y * groupDim.x * THREAD_GROUP_TOTAL + groupIndex;
    
    float distance = length(eyePos.xyz - pool[index].position.xyz);
    
    if (index < size - 1 && pool[index].state.x)
    {
        pool[index].position.xyz += 
        normalize(GetVectorFieldVelocity(pool[index].position.x, pool[index].position.y, pool[index].position.z) * (clamp(distance / 5.0f, 0, 1)) +
        GetVectorFieldFromCamera(pool[index].position.xyz, eyePos.xyz) * (1 - clamp(distance / 5.0f, 0, 1))) * 0.05f;
        pool[index].lifetime.x += 0.01f;
        
        if (pool[index].lifetime.x > pool[index].lifetime.y)
        {
            pool[index].lifetime.x = 0;
            pool[index].position.xyz = pool[index].initialPos;
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





