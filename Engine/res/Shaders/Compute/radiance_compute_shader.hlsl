#include "../Includes/sampling_helper.hlsli"

static const float PI = 3.14159265359f;

cbuffer RadianceBuffer : register(b0)
{
    float Roughness;
};

TextureCube InputTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);
SamplerState Sampler : register(s0);

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    OutputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 normal = getSamplingVector(threadID, outputWidth, outputHeight, outputDepth);

    OutputTexture[threadID] = float4(InputTexture.SampleLevel(Sampler, normal, 0).rgb, 1.0f);
}