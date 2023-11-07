#define PI 3.14159265f

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
// Schlick-Beckmann Geometry Shadowing Function
float G1(float alpha, float3 N, float3 X)
{
    float numerator = max(dot(N, X), 0.0);
    float k = pow(alpha + 1, 2.0f) / 8.0;
    float denominator = max(dot(N, X), 0.0f) * (1.0f - k) + k;
    denominator = max(denominator, 0.000001f);
    
    return numerator / denominator;
}

////Smith Model
float G(float alpha, float3 N, float3 V, float3 L)
{
    return G1(alpha, N, V) * G1(alpha, N, L);
}

//Fresnel-Schlick Function
float3 F(float3 F0, float3 V, float3 H)
{
    
    return F0 + (float3(1.0, 1.0f, 1.0f) - F0) * pow(2, (-5.55473 * max(dot(V, H), 0.0f) - 6.98316) * (max(dot(V, H), 0.0f)));

}

float3 FwithRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 PBR(float3 N, float3 V, float3 L, float3 H, float3 F0, float3 lightColor, float3 albedo, float roughness, float3 metallic, float4 emmisive, float lightIntensity)
{
    float alpha = roughness;
    float3 metallicMesh = float3(metallic.x, metallic.x, metallic.x);
    float4 emissivityMesh = emmisive;
    float3 albedoMesh = albedo;
    
   
    
    float3 Ks = F(F0, V, H);
    float3 Kd = (float3(1.0f, 1.0f, 1.0f) - Ks) * (1.0f - metallicMesh);
    
    float3 lambert = albedoMesh / PI;
    
    float3 cookTorranceNumerator = D(alpha, N, H) * G(alpha, N, V, L) * F(F0, V, H);
    float cookTorranceDenominator = 4.0f * max(dot(V, N), 0.0f) * max(dot(L, N), 0.0f);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001f);
    
    float3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;
    
    float3 BRDF = Kd * lambert + cookTorrance;
    
    float3 outgoingLight = (emissivityMesh.a > 0.1f) * emissivityMesh.xyz * lightIntensity +
    BRDF * lightColor * max(dot(L, N), 0.0f) * lightIntensity;

    return outgoingLight;
}