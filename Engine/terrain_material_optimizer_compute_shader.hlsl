#pragma once

RWTexture2D<float4> OutputOcclusionTexture : register(u0);
RWTexture2D<float4> OutputMetalicTexture : register(u1);
RWTexture2D<float4> OutputSmoothnessTexture : register(u2);

[numthreads(32, 32, 1)]
void main(uint2 threadID : SV_DispatchThreadID)
{
    OutputOcclusionTexture[threadID] = float4(1.0f, 0.0f, 0.0f, 1.0f);
    OutputMetalicTexture[threadID] = float4(1.0f, 0.0f, 0.0f, 1.0f);
    OutputSmoothnessTexture[threadID] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}