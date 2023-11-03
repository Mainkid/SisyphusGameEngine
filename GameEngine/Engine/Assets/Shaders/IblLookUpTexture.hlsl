#pragma once
#define THREADS 8
#define PI 3.14159265f
#define IMAGE_SIZE 512


RWTexture2D<float4> lookUpTexture;

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



float2 Hammersley(uint i, uint N)
{
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float rdi = float(bits) * 2.3283064365386963e-10;
    return float2(float(i) / float(N), rdi);
}

// From the filament docs. Geometric Shadowing function
// https://google.github.io/filament/Filament.html#toc4.4.2
float V_SmithGGXCorrelated(float NoV, float NoL, float roughness)
{
    float a2 = pow(roughness, 4.0);
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
    return 0.5 / (GGXV + GGXL);
}

float3 ImportanceSampleGGX(float2 Xi, float roughness, float3 N)
{
    float a = roughness * roughness;
    // Sample in spherical coordinates
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    // Construct tangent space vector
    float3 H;
    H.x = sinTheta * cos(phi);
    H.y = sinTheta * sin(phi);
    H.z = cosTheta;

    // Tangent to world space
    float3 upVector = abs(N.z) < 0.9999f ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangentX = normalize(cross(upVector, N));
    float3 tangentY = cross(N, tangentX);
    return tangentX * H.x + tangentY * H.y + N * H.z;
}

// Karis 2014
float2 integrateBRDF(float roughness, float NoV)
{
    float3 V;
    V.x = sqrt(1.0 - NoV * NoV); // sin
    V.y = 0.0;
    V.z = NoV; // cos

    // N points straight upwards for this integration
    const float3 N = float3(0.0, 0.0, 1.0);

    float A = 0.0;
    float B = 0.0;
    const uint numSamples = 1024;

    for (uint i = 0u; i < numSamples; i++)
    {
        float2 Xi = Hammersley(i, numSamples);
        // Sample microfacet direction
        float3 H = ImportanceSampleGGX(Xi, roughness, N);

        // Get the light direction
        float3 L = 2.0 * dot(V, H) * H - V;

        float NoL = saturate(dot(N, L));
        float NoH = saturate(dot(N, H));
        float VoH = saturate(dot(V, H));

        if (NoL > 0.0)
        {
            // Terms besides V are from the GGX PDF we're dividing by
            float V_pdf = V_SmithGGXCorrelated(NoV, NoL, roughness) * VoH * NoL / NoH;
            float Fc = pow(1.0 - VoH, 5.0);
            A += (1.0 - Fc) * V_pdf;
            B += Fc * V_pdf;
        }
    }

    return 4.0 * float2(A, B) / float(numSamples);
}


[numthreads(THREADS, THREADS, 1)]
void CSMain(int3 dispatchThreadID : SV_DispatchThreadID)
{
    // Normalized pixel coordinates (from 0 to 1)
    float2 uv = float2(dispatchThreadID.xy + 1) / float2(IMAGE_SIZE,IMAGE_SIZE);
    float mu = uv.x;
    float a = uv.y;


    // Output to screen
    float2 res = integrateBRDF(a, mu);

    lookUpTexture[float2(dispatchThreadID.x,dispatchThreadID.y)] = float4(res, 0, 1);
    // Scale and Bias for F0 (as per Karis 2014)
    //imageStore(s_target, ifloat2(dispatchThreadID.xy), vec4(res, 0.0, 0.0));
}