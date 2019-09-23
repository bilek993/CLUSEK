static const float PI = 3.14159265359f;
static const float EPSILON = 0.00001;
static const uint NUMBER_OF_SAMPLES = 1024;

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