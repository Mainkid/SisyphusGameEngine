

struct VS_OUTPUT
{
    float4 Position : SV_POSITION; // vertex position
    float2 TexCoord : COLOR0; // vertex interpolation value
};

struct PS_OUTPUT
{
    float4 Color : COLOR0;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float4 TexCoord : COLOR0;
    uint arrInd : SV_RenderTargetArrayIndex;
};


Texture2DArray depthMapTex : register(t0);
SamplerState textureSampler : register(s0);



VS_OUTPUT VS_Blur(in float3 Position : POSITION,
                        in float4 TexCoord : COLOR0)
{
    VS_OUTPUT Output;
    
    Output.Position = float4(Position, 1.0);
    Output.TexCoord = TexCoord;
    
    return Output;
}


[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle VS_OUTPUT p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT gs = (GS_OUT) 0;
        gs.pos = p[i].Position;
        gs.TexCoord = float4(p[i].TexCoord.xy, 0, 1);
        gs.arrInd = id;
        stream.Append(gs);
    }
    
    stream.RestartStrip();
}


float4 PS_BlurX(GS_OUT Input) : SV_Target
{

    float TextelScale = 1.0 / 1024.0;

    float2 Value = float2(0.0, 0.0);

    float Coefficients[21] =
    {
        0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
     0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
     0.14107424,
     0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
     0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
    };

    for (int Index = 0; Index < 21; Index++)
    {
        float3 location = float3(0,0,Input.arrInd);
        location.xy = float2(Input.TexCoord.x + (Index - 10) * TextelScale, Input.TexCoord.y);
        Value += depthMapTex.Sample(textureSampler, location).xy * Coefficients[Index];
    }

    return float4(Value.x, Value.y, 0.0,1);
}

float4 PS_BlurY(GS_OUT Input) : SV_Target
{

    float TextelScale = 1.0 / 1024.0;
    float2 Value = float2(0.0, 0.0);
    
    float Coefficients[21] =
    {
        0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
     0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
     0.14107424,
     0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
     0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
    };
    for (int Index = 0; Index < 21; Index++)
    {
        float3 location = float3(0, 0, Input.arrInd);
        location.xy = float2(Input.TexCoord.x, Input.TexCoord.y +  (Index - 10) * TextelScale);
        Value += depthMapTex.Sample(textureSampler, location).xy * Coefficients[Index];
    }


    return float4(Value.x, Value.y, 0.0,1);
}