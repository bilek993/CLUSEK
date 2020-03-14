#pragma once

Texture2D BaseMetalicSmoothnessTexture : register(t0);
Texture2D RedMetalicSmoothnessTexture : register(t1);
Texture2D GreenMetalicSmoothnessTexture : register(t2);
Texture2D BlueMetalicSmoothnessTexture : register(t3);

Texture2D BaseOcclusionTexture : register(t4);
Texture2D RedOcclusionTexture : register(t5);
Texture2D GreenOcclusionTexture : register(t6);
Texture2D BlueOcclusionTexture : register(t7);

RWTexture2D<float4> OutputOcclusionTexture : register(u0);
RWTexture2D<float4> OutputMetalicTexture : register(u1);
RWTexture2D<float4> OutputSmoothnessTexture : register(u2);

[numthreads(32, 32, 1)]
void main(uint2 threadID : SV_DispatchThreadID)
{
    OutputOcclusionTexture[threadID] = float4(  RedOcclusionTexture.Load(int3(threadID.xy, 0)).r,
                                                GreenOcclusionTexture.Load(int3(threadID.xy, 0)).r,
                                                BlueOcclusionTexture.Load(int3(threadID.xy, 0)).r,
                                                BaseOcclusionTexture.Load(int3(threadID.xy, 0)).r);
    
    OutputMetalicTexture[threadID] = float4(    RedMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).r,
                                                GreenMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).r,
                                                BlueMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).r,
                                                BaseMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).r);
    
    OutputSmoothnessTexture[threadID] = float4( RedMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).a,
                                                GreenMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).a,
                                                BlueMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).a,
                                                BaseMetalicSmoothnessTexture.Load(int3(threadID.xy, 0)).a);
}