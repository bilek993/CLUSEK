#include "../Includes/sampling_helper.hlsli"

static const float PI = 3.14159265359f;
static const float EPSILON = 0.00001;
static const uint NUMBER_OF_SAMPLES = 1024;

cbuffer RadianceBuffer : register(b0)
{
    float Roughness;
};

TextureCube InputTexture : register(t0);
RWTexture2DArray<float4> OutputTexture : register(u0);
SamplerState Sampler : register(s0);

void computeBasisVectors(const float3 normal, out float3 S, out float3 T)
{
    T = cross(normal, float3(0.0, 1.0, 0.0));
    T = lerp(cross(normal, float3(1.0, 0.0, 0.0)), T, step(EPSILON, dot(T, T)));

    T = normalize(T);
    S = normalize(cross(normal, T));
}

float3 tangentToWorld(const float3 v, const float3 N, const float3 S, const float3 T)
{
    return S * v.x + T * v.y + N * v.z;
}

float radicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

float2 hammersley(uint i)
{
    return float2(float(i) / float(NUMBER_OF_SAMPLES), radicalInverse_VdC(i));
}

float3 sampleGGX(float2 xi, float roughness)
{
    float alpha = roughness * roughness;

    float phi = 2.0f * PI * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (alpha * alpha - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    return float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSquare = alpha * alpha;

    float denominator = (cosLh * cosLh) * (alphaSquare - 1.0f) + 1.0f;
    return alphaSquare / (PI * denominator * denominator);
}

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    OutputTexture.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 normal = getSamplingVector(threadID, outputWidth, outputHeight, outputDepth);

    float inputWidth, inputHeight, inputLevels;
    InputTexture.GetDimensions(0, inputWidth, inputHeight, inputLevels);

    float wt = 4.0 * PI / (6 * inputWidth * inputHeight);

    float3 S, T;
    computeBasisVectors(normal, S, T);

    float3 color = 0;
    float weight = 0;

    for (uint i = 0; i < NUMBER_OF_SAMPLES; i++)
    {
        float2 u = hammersley(i);
        float3 Lh = tangentToWorld(sampleGGX(u, Roughness), normal, S, T);

        float3 Li = 2.0 * dot(normal, Lh) * Lh - normal;

        float cosLi = dot(normal, Li);

        if (cosLi > 0.0)
        {
            float cosLh = max(dot(normal, Lh), 0.0);
            float pdf = ndfGGX(cosLh, Roughness) * 0.25;
            float ws = 1.0 / (NUMBER_OF_SAMPLES * pdf);

            float mipLevel = max(0.5 * log2(ws / wt) + 1.0, 0.0);

            color += InputTexture.SampleLevel(Sampler, Li, mipLevel).rgb * cosLi;
            weight += cosLi;
        }
    }

    color /= weight;
    
    OutputTexture[threadID] = float4(color, 1.0f);
}