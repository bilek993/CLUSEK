#include "../Includes/sampling_helper.hlsli"

static const float PI = 3.14159265359f;

cbuffer IrradianceBuffer : register(b0)
{
    float SampleDelta;
};

TextureCube InputTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);
SamplerState Sampler : register(s0);

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    OutputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 normal = GetSamplingVector(threadID, outputWidth, outputHeight, outputDepth);

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, normal);
    up = cross(normal, right);

    float3 irradiance = float3(0.0f, 0.0f, 0.0f);
    float nrSamples = 0.0f;

    for (float phi = 0.0f; phi < 2.0f * PI; phi += SampleDelta)
    {
        for (float theta = 0.0f; theta < 0.5f * PI; theta += SampleDelta)
        {
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += InputTexture.SampleLevel(Sampler, sampleVec, 0).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0f / float(nrSamples));
    OutputTexture[threadID] = float4(irradiance, 1.0f);
}