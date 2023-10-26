cbuffer mycBuffer : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 view;
    row_major float4x4 worldView;
    row_major float4x4 worldViewProj;
    row_major float4x4 worldViewInverseT;
    row_major float4x4 viewProjs[4];
    float4 eyePos;
}

struct VS_IN
{
    float4 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_Position;
    float4 worldPos : Position;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = mul(input.pos, worldViewProj);
    output.worldPos = mul(input.pos, worldView);
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float3 toVertex = (input.worldPos.xyz - eyePos.xyz);
    float inverseMaxHeight = 1-clamp(abs(eyePos.y) / eyePos.w, 0, 1);
    float3 inPlaneVec = toVertex;
    inPlaneVec.y = 0;
    
    toVertex = normalize(toVertex);
    inPlaneVec = normalize(inPlaneVec);
    
    float val = pow(1 - dot(toVertex, inPlaneVec), 1);
    val = val * inverseMaxHeight;
    return float4(val,val,val,val);

}