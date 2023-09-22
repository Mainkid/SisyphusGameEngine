struct CubemapData
{
    row_major float4x4 view[6];
    row_major float4x4 proj;
};

cbuffer mycBuffer : register(b0)
{
    
    row_major float4x4 world;
    CubemapData cascData;
    
};

struct GS_IN
{
    float4 pos : POSITION;
    
};

struct GS_OUT
{
    float4 pos : SV_Position;
    uint arrInd : SV_RenderTargetArrayIndex;
    float4 worldPos : POSITION1;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
};


[instance(6)]
[maxvertexcount(3)]
void GSMain(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT gs = (GS_OUT) 0;
        
        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), mul(cascData.view[id], cascData.proj));
        gs.worldPos = float4(p[i].pos.xyz, 1);
        gs.arrInd = id;
        stream.Append(gs);
    }
    stream.RestartStrip();
}

float4 PSMain(GS_OUT input) : SV_Target
{ 
    return float4(input.worldPos.xyz, 1);
}



GS_IN DepthVertexShader(VertexInputType input)
{
    GS_IN output;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.position, world);
    output.pos.w = 1.0f;
	
    return output;
}