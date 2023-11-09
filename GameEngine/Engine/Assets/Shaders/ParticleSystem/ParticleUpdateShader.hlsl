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
    float4 groupDim;
    float4 eyePos;
    SharedParticleData particleData;
};


RWStructuredBuffer<Particle> pool : register(u1);
AppendStructuredBuffer<uint> deadList : register(u2);
RWStructuredBuffer<ParticleListItem> sortList : register(u3);
//AppendStructuredBuffer<uint> sortListI : register(u4);

float3 SolveCubic(float4 Coefficient)
{
    // Normalize the polynomial
    Coefficient.xyz /= Coefficient.w;
    // Divide middle coefficients by three
    Coefficient.yz /= 3.0f;
    // Compute the Hessian and the discrimant
    float3 Delta = float3(
        mad(-Coefficient.z, Coefficient.z, Coefficient.y),
        mad(-Coefficient.y, Coefficient.z, Coefficient.x),
        dot(float2(Coefficient.z, -Coefficient.y), Coefficient.xy)
    );
    float Discriminant = dot(float2(4.0f * Delta.x, -Delta.y), Delta.zy);
    // Compute coefficients of the depressed cubic 
    // (third is zero, fourth is one)
    float2 Depressed = float2(
        mad(-2.0f * Coefficient.z, Delta.x, Delta.y),
        Delta.x
    );
    // Take the cubic root of a normalized complex number
    float Theta = atan2(sqrt(Discriminant), -Depressed.x) / 3.0f;
    float2 CubicRoot;
    sincos(Theta, CubicRoot.y, CubicRoot.x);
    // Compute the three roots, scale appropriately and 
    // revert the depression transform
    float3 Root = float3(
        CubicRoot.x,
        dot(float2(-0.5f, -0.5f * sqrt(3.0f)), CubicRoot),
        dot(float2(-0.5f, 0.5f * sqrt(3.0f)), CubicRoot)
    );
    Root = mad(2.0f * sqrt(-Depressed.y), Root, -Coefficient.z);
    return Root;
}

float GetValueFromSpline(ParticleInputDataCurve inputData,float xT)
{
    float t = 0;
    float2 P1 = inputData.P1;
    float2 P2 = inputData.P2;
    float3 roots = SolveCubic(float4(xT, 3 * P1.x, 3 * P2.x - 6 * P1.x, 1 - 3 * P2.x + 3 * P1.x));
        
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

        pool[index].sizeAndRot.x = pool[index].state.y * y; 
    }
    
    if (particleData.SpeedOverLifetime.IsUsing.x == true)
    {
        float xT = -pool[index].lifetime.x / pool[index].lifetime.y;
        float y = GetValueFromSpline(particleData.SpeedOverLifetime, xT);                      
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





