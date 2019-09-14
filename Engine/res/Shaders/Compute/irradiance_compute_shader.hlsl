#include "../Includes/sampling_helper.hlsli"

static const float PI = 3.14159265359f;

TextureCube InputTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);
SamplerState Sampler : register(s0);

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    OutputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 normal = getSamplingVector(threadID, outputWidth, outputHeight, outputDepth);

    float3 irradiance = float3(1.0f, 0.0f, 0.0f);
    irradiance = InputTexture.SampleLevel(Sampler, normal, 0).rgb;

    OutputTexture[threadID] = float4(irradiance, 1.0f);
}