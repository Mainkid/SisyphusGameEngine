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



RWBuffer<uint> counter : register(u1);
//ConsumeStructuredBuffer<int> deadList : register(u2);
//AppendStructuredBuffer<int> sortList : register(u3);



//===============
// Compute Shader code  |
//===============


[numthreads(1, 1, 1)]
void DefaultCS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint amount = counter[0];
    amount = amount * 6;
    counter[0] = amount;
}





