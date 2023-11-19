//--------------------------------------------------------------------------------------
// File: ComputeShaderSort11.hlsl
//
// This file contains the compute shaders to perform GPU sorting using DirectX 11.
// 
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

#define BITONIC_BLOCK_SIZE 512

#define TRANSPOSE_BLOCK_SIZE 1


struct ParticleListItem
{
    int index;
    float distance;
};
//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer CB : register(b0)
{
    unsigned int g_iLevel;
    unsigned int g_iLevelMask;
    unsigned int g_iWidth;
    unsigned int g_iHeight;
};

//--------------------------------------------------------------------------------------
// Structured Buffers
//--------------------------------------------------------------------------------------
StructuredBuffer<ParticleListItem> Input : register(t0);
RWStructuredBuffer<ParticleListItem> Data : register(u0);

//--------------------------------------------------------------------------------------
// Bitonic Sort Compute Shader
//--------------------------------------------------------------------------------------
groupshared ParticleListItem shared_data[BITONIC_BLOCK_SIZE];

[numthreads(BITONIC_BLOCK_SIZE, 1, 1)]
void BitonicSort(uint3 Gid : SV_GroupID,
                  uint3 DTid : SV_DispatchThreadID,
                  uint3 GTid : SV_GroupThreadID,
                  uint GI : SV_GroupIndex)
{
    
    // Load shared data
    shared_data[GI]= Data[DTid.x];

    GroupMemoryBarrierWithGroupSync();
    
    // Sort the shared data
    for (unsigned int j = g_iLevel >> 1; j > 0; j >>= 1)
    {
        uint sharedI = (((shared_data[GI & ~j].distance) >= (shared_data[GI | j].distance)) == (bool) (g_iLevelMask & DTid.x)) ? GI ^ j : GI;
        ParticleListItem result;
        result=shared_data[sharedI];
        GroupMemoryBarrierWithGroupSync();
        shared_data[GI] = result;
        GroupMemoryBarrierWithGroupSync();
    }
    
    // Store shared data
    Data[DTid.x] = shared_data[GI];
  
}

//--------------------------------------------------------------------------------------
// Matrix Transpose Compute Shader
//--------------------------------------------------------------------------------------
groupshared ParticleListItem transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void MatrixTranspose(uint3 Gid : SV_GroupID,
                      uint3 DTid : SV_DispatchThreadID,
                      uint3 GTid : SV_GroupThreadID,
                      uint GI : SV_GroupIndex)
{
    
    transpose_shared_data[GI] = Input[DTid.y * g_iWidth + DTid.x];
    GroupMemoryBarrierWithGroupSync();
    uint2 XY = DTid.yx - GTid.yx + GTid.xy;
    Data[XY.y * g_iHeight + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
   
}