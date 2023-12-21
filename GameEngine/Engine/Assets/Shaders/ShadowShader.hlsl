#define MAX_BONES 100
#define MAX_BONES_INFLUENCE 4

struct CascadeData
{
    row_major float4x4 viewProj[4];
};

cbuffer mycBuffer : register(b0)
{
    
    row_major float4x4 world;
    row_major float4x4 view;
    row_major float4x4 worldView;
    row_major float4x4 worldViewProj;
    row_major float4x4 worldViewInverseT;
    CascadeData cascData;
    
};

cbuffer SkeletalBuffer : register(b1)
{
    row_major float4x4 finalBonesMatrices[MAX_BONES];
};

struct GS_IN
{
    float4 pos : POSITION;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    uint arrInd : SV_RenderTargetArrayIndex;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
    float4 tangents : TANGENT;
    float4 bitangents : BITANGENT;
    float4 boneIDs : BONE;
    float4 boneWeights : BONEWEIGHTS;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};




[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT gs = (GS_OUT) 0;
        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), cascData.viewProj[id]);
        gs.arrInd = id;
        stream.Append(gs);
    }
    
    stream.RestartStrip();
}


GS_IN
    DepthVertexShader(VertexInputType input)
{
    GS_IN output;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    float4 totalPosition = float4(0, 0, 0, 0);
    
    for (int i = 0; i < MAX_BONES_INFLUENCE; i++)
    {
        if (input.boneIDs[i] < 0)
            continue;
        if (input.boneIDs[i] >= MAX_BONES)
        {
            totalPosition = input.position;
            break;
        }
        
  
        float4 localPosition = mul(input.position, finalBonesMatrices[int(input.boneIDs[i])]);
        totalPosition += localPosition * float(input.boneWeights[i]);
    }
    
    if (input.boneIDs[0] < 0)
    {
        totalPosition = input.position;
    }
    
    output.pos = mul(totalPosition, world);
    output.pos.w = 1.0f;
	
    return output;
}

float4 PS_Main(GS_OUT output) :SV_Target
{
    float z = output.pos.z / output.pos.w;
    return float4(z, z * z, 0, 1);

}