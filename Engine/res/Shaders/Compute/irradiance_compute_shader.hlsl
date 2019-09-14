#include "../Includes/sampling_helper.hlsli"

RWTexture2DArray<float4> outputTexture : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    outputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 normal = getSamplingVector(threadID, outputWidth, outputHeight, outputDepth);

    outputTexture[threadID] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}