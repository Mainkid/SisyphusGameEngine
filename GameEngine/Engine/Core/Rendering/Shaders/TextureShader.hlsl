cbuffer PerFrameBuffer
{
    row_major float4x4 worldMatrix;
    row_major float4x4 viewMatrix;
    row_major float4x4 projectionMatrix;
    float4 indexx;
};



struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

Texture2DArray shaderTexture;
SamplerState SampleType;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    return output;
}

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    return shaderTexture.Sample(SampleType, float3(input.tex,indexx.x));
}