#pragma once
#define TWO_PI 6.2831853071795864769252867665590
#define HALF_PI 1.5707963267948966192313216916398
#define THREADS 6

cbuffer mycBuffer : register(b0)
{
    float4 params;
}


TextureCube environmentMap : register(t0);
RWTexture2DArray<float4> irradianceCubeMap : register(u0);
SamplerState sampler_ : register(s0);

float3 toWorldCoords(uint3 globalId, float size)
{
    float2 uvc = (float2(globalId.xy) + 0.5) / size;
    uvc = 2.0 * uvc - 1.0;
    uvc.y *= -1.0;
    switch (globalId.z)
    {
        case 0:
            return float3(1.0, uvc.y, -uvc.x);
        case 1:
            return float3(-1.0, uvc.y, uvc.x);
        case 2:
            return float3(uvc.x, 1.0, -uvc.y);
        case 3:
            return float3(uvc.x, -1.0, uvc.y);
        case 4:
            return float3(uvc.x, uvc.y, 1.0);
        default:
            return float3(-uvc.x, uvc.y, -1.0);
    }
}


[numthreads (THREADS,THREADS,6)]
void CSMain(int3 dispatchThreadID : SV_DispatchThreadID,
uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    float imgSize = params.x;
    uint3 globalId = dispatchThreadID;
    float3 N = normalize(toWorldCoords(globalId, imgSize));

    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 right = normalize(cross(up, N));
    up = cross(N, right);

    float3 colorIrradiance = float3(0,0,0);
    uint sampleCount = 0;
    float deltaPhi = TWO_PI / 360.0;
    float deltaTheta = HALF_PI / 90.0;


    for (float phi = 0.0; phi < TWO_PI; phi += deltaPhi)
    {
        for (float theta = 0.0; theta < HALF_PI; theta += deltaTheta)
        {
            // Spherical to World Space in two steps...
            float3 tempVec = cos(phi) * right + sin(phi) * up;
            float3 sampleVector = cos(theta) * N + sin(theta) * tempVec;
            colorIrradiance += pow(environmentMap.SampleLevel(sampler_, sampleVector, 0).rgb * cos(theta) * sin(theta), 1.0 / 2.2f);
            sampleCount++;
        }
    }
    float3 res = HALF_PI * 2.0 * colorIrradiance * (1.0 / float(sampleCount));
    irradianceCubeMap[dispatchThreadID] = float4(res, 1.0f);
}