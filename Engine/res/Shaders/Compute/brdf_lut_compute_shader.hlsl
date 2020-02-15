#include "../Includes/pbr_compute_shared.hlsli"

RWTexture2D<float2> OutputTexture : register(u0);

float GaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0f - k) + k);
}

float GaSchlickGGX_IBL(float cosLight, float cosView, float roughness)
{
    float r = roughness;
    float k = (r * r) / 2.0f;
    return GaSchlickG1(cosLight, k) * GaSchlickG1(cosView, k);
}

[numthreads(32, 32, 1)]
void main(uint2 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight;
    OutputTexture.GetDimensions(outputWidth, outputHeight);

    float cosLo = threadID.x / outputWidth;
    float roughness = threadID.y / outputHeight;

    cosLo = max(cosLo, EPSILON);

    float3 Lo = float3(sqrt(1.0 - cosLo * cosLo), 0.0, cosLo);

    float DFG1 = 0;
    float DFG2 = 0;

    for (uint i = 0; i < NUMBER_OF_SAMPLES; i++)
    {
        float2 u = Hammersley(i);

        float3 Lh = sampleGGX(u, roughness);
        float3 Li = 2.0 * dot(Lo, Lh) * Lh - Lo;

        float cosLi = Li.z;
        float cosLh = Lh.z;
        float cosLoLh = max(dot(Lo, Lh), 0.0);

        if (cosLi > 0.0)
        {
            float G = GaSchlickGGX_IBL(cosLi, cosLo, roughness);
            float Gv = G * cosLoLh / (cosLh * cosLo);
            float Fc = pow(1.0 - cosLoLh, 5);

            DFG1 += (1 - Fc) * Gv;
            DFG2 += Fc * Gv;
        }
    }

    OutputTexture[threadID] = float2(DFG1, DFG2) * (1.0 / float(NUMBER_OF_SAMPLES));
}