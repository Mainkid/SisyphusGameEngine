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



RWStructuredBuffer<Particle> pool : register(u1);
ConsumeStructuredBuffer<int> deadList : register(u2);
//AppendStructuredBuffer<int> sortList : register(u3);



//===============
// Compute Shader code  |
//===============


[numthreads(15, 15, 1)]
void DefaultCS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    
    int index = deadList.Consume();
    pool[index].state.x = true;
}





