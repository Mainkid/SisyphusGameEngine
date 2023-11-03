#define THREADS 8
#define NUM_SAMPLES 64u
#define PI 3.14159265f


TextureCube environmentMap : register(t0);
RWTexture2DArray<float4> prefilteredCubeMap : register(u0);
SamplerState sampler_ : register(s0);

cbuffer mycBuffer : register(b0)
{
    float4 u_params;
};

// GGX/Trowbridge-
//Reitz Normal
//Distribution Function
float D(float alpha, float3 N, float3 H)
{
    float numerator = pow(alpha, 2.0f);
    
    float NdotH = max(dot(N, H), 0.0);
    float denominator = PI * pow(pow(NdotH, 2.0f) * (pow(alpha, 2.0f) - 1.0f) + 1.0f, 2.0f);
    denominator = max(denominator, 0.000001f);
    return numerator / denominator;
}

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

// Based on Karis 2014
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
    float3 upVector = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangentX = normalize(cross(upVector, N));
    float3 tangentY = cross(N, tangentX);
    return tangentX * H.x + tangentY * H.y + N * H.z;
}



float3 PrefilterEnvMap(float roughness, float3 R, float imgSize)
{
    //// Isotropic approximation: we lose stretchy reflections :(
    float3 N = R;
    float3 V = R;

    float3 prefilteredColor = float3(0, 0, 0);
    float totalWeight = 0.0;
    for (uint i = 0u; i < NUM_SAMPLES; i++)
    {
        float2 Xi = Hammersley(i, NUM_SAMPLES);
        float3 H = ImportanceSampleGGX(Xi, roughness, N);
        float VoH = dot(V, H);
        float NoH = VoH; // Since N = V in our approximation
        // Use microfacet normal H to find L
        float3 L = 2.0 * VoH * H - V;
        float NoL = saturate(dot(N, L));
        // Clamp 0 <= NoH <= 1
        NoH = saturate(NoH);

        if (NoL > 0.0)
        {
            // Based off https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
            // Typically you'd have the following:
            // float pdf = D_GGX(NoH, roughness) * NoH / (4.0 * VoH);
            // but since V = N => VoH == NoH
            float pdf = D(roughness, N, H) / 4.0 + 0.001;
            // Solid angle of current sample -- bigger for less likely samples
            float omegaS = 1.0 / (float(NUM_SAMPLES) * pdf);
            // Solid angle of texel
            float omegaP = 4.0 * PI / (6.0 * imgSize * imgSize);
            // Mip level is determined by the ratio of our sample's solid angle to a texel's solid angle
            float mipLevel = max(0.5 * log2(omegaS / omegaP), 0.0);
            prefilteredColor += environmentMap.SampleLevel(sampler_, L, mipLevel).rgb * NoL;
            totalWeight += NoL;
        }
    }
    return prefilteredColor / totalWeight;
}


[numthreads(8, 8, 6)]
void CSMain(int3 dispatchThreadID : SV_DispatchThreadID,
uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    float mipLevel = u_params.y;
    float imgSize = u_params.z;
    float mipImageSize = imgSize / pow(2.0, mipLevel);
    uint3 globalId = uint3(dispatchThreadID);

    if (globalId.x >= mipImageSize || globalId.y >= mipImageSize)
    {
        return;
    }

    float3 R = normalize(toWorldCoords(globalId, mipImageSize));

    // Don't need to integrate for roughness == 0, since it's a perfect reflector
    if (u_params.x <= 0.01)
    {
        float4 color = environmentMap.SampleLevel(sampler_, R, 0);
        prefilteredCubeMap[globalId] = color;
        return;
    }

    float3 color = PrefilterEnvMap(u_params.x, R, imgSize);
    // We access our target cubemap as a 2D texture array, where z is the face index
    prefilteredCubeMap[globalId] = float4(color, 1);
}