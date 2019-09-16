#include "../Includes/sampling_helper.hlsli"

static const float PI = 3.14159265359f;

Texture2D InputTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);
SamplerState Sampler : register(s0);

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    OutputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 normal = getSamplingVector(threadID, outputWidth, outputHeight, outputDepth);

    float phi = atan2(normal.z, normal.x);
    float theta = acos(normal.y);

    OutputTexture[threadID] = InputTexture.SampleLevel(Sampler, float2(phi / (2 * PI), theta / PI), 0);
}