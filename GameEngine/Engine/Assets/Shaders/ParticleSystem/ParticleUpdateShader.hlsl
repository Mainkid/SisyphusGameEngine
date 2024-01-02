#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

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
    float4 RandomBetweenConstsF; // x - f1, y - f2
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
    float4 state;       //Also contains StartSize (y), StartVelocity(z), StartRot(w)
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
    row_major float4x4 world;
    float4 groupDim;
    float4 eyePos;
    SharedParticleData particleData;
};


RWStructuredBuffer<Particle> pool : register(u1);
AppendStructuredBuffer<uint> deadList : register(u2);
RWStructuredBuffer<ParticleListItem> sortList : register(u3);
//AppendStructuredBuffer<uint> sortListI : register(u4);
float cbrt(in float x)
{
    return sign(x) * pow(abs(x), 1.0 / 3.0);
}

float3 SolveCubic(in float a, in float b, in float c, in float d, out float3 roots)
{
    float u = b / (3.0 * a);

    // Depress to x^3 + px + q by substituting x-b/3a
    // This can be found by substituting x+u and solving for u so that the x^2
    // term gets eliminated (then of course dividing by the leading coefficient)
    float p = (c - b * u) / a;
    float q = (d - (c - 2.0 * b * b / (9.0 * a)) * u) / a;

    // Everything blows up when p=0 so give this case special treatment
    if (abs(p) < 1e-9)
    {
        roots.x = cbrt(-q) - u;
        return 1;
    }

    // In the case of one root, this construction does not work
    float h = 0.25 * q * q + p * p * p / 27.0;
    if (h > 0.0)
    { // Check depressed cubic discriminant
        h = sqrt(h);
        float o = -0.5 * q;
        roots.x = cbrt(o - h) + cbrt(o + h) - u; // Cardano's formula (see https://en.wikipedia.org/wiki/Cubic_equation)
        return 1;
    }

    // Solve by mapping an inverse smoothstep between the critical points
    // I found a whole lot simplified right out so now it probably looks rather obfuscated
    float m = sqrt(-p / 3.0);
    roots.x = -2.0 * m * sin(asin(1.5 * q / (p * m)) / 3.0);

    // Factor out the root to solve for the rest as a quadratic
    h = sqrt(-3.0 * roots.x * roots.x - 4.0 * p);
    roots.yz = 0.5 * float2(h - roots.x, -h - roots.x);
    roots -= u; // Undo the change in variable

    return 3;
}

float GetValueFromSpline(ParticleInputDataCurve inputData,float xT)
{
    float t = 0;
    float2 P1 = inputData.P1;
    float2 P2 = inputData.P2;
    float3 roots;
    SolveCubic(1 - 3 * P2.x + 3 * P1.x, 3 * P2.x - 6 * P1.x, 3 * P1.x, xT, roots);
        
    if (roots.x >= 0 && roots.x <= 1)
         t = roots.x;
    else if (roots.y >= 0 && roots.y <= 1)
         t = roots.y;
    else if (roots.z >= 0 && roots.z <= 1)
         t = roots.z;

    //pool[0].state.yzw = roots;
    
    
    return 3 * (1 - t) * (1 - t) * t * P1.y + 3 * (1 - t) * t * t * P2.y + t * t * t;

}

void UpdateParticle(int index)
{
    pool[index].position.xyz +=
        pool[index].velocity * pool[index].velocity.w * particleData.deltaTime.x;
    pool[index].lifetime.x += particleData.deltaTime.x;
    pool[index].sizeAndRot.y += pool[index].state.w * particleData.deltaTime.x;   
    
    if (particleData.SizeOverLifetime.IsUsing.x == true)
    {
        float xT = -pool[index].lifetime.x / pool[index].lifetime.y;
        float y = GetValueFromSpline(particleData.SizeOverLifetime, xT);
        if (particleData.SizeOverLifetime.IsUsing.w)
            y = 1 - y;

            pool[index].sizeAndRot.x = pool[index].state.y * y;
    }
    
    if (particleData.SpeedOverLifetime.IsUsing.x == true)
    {
        float xT = -pool[index].lifetime.x / pool[index].lifetime.y;
        float y = GetValueFromSpline(particleData.SpeedOverLifetime, xT);   
        if (particleData.SpeedOverLifetime.IsUsing.w)
            y = 1 - y;
        pool[index].velocity.w = pool[index].state.z * y;
    }

    
}



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
        UpdateParticle(index);
        
        if (pool[index].lifetime.x > pool[index].lifetime.y)
        {
           pool[index].state.x = false;
           deadList.Append(index);
        }
        else
        {

            uint ctr=sortList.IncrementCounter();
            float3 tmp = pool[index].position;
            sortList[ctr].index = index;
            sortList[ctr].distance = length(tmp-eyePos.xyz);
        }
    }

}





