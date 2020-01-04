static const float SHADOW_BIAS = 0.002f;
static const float SHADOW_MIN_BIAS = 0.0f;
static const float SHADOW_MAX_BIAS = 0.01f;

float2 CalculateOffset(int u, int v, float shadowMapWidth, float shadowMapHeight)
{
    return float2(u * 1.0f / shadowMapWidth, v * 1.0f / shadowMapHeight);
}

float PerformPCF(Texture2D shadowMap, SamplerComparisonState shadowSampler, float4 lightSpacePosition)
{
    float shadowMapWidth, shadowMapHeight;
    shadowMap.GetDimensions(shadowMapWidth, shadowMapHeight);
    
    float sum = 0.0f;
    
    for (float y = -1.5; y <= 1.5; y += 1.0)
    {
        for (float x = -1.5; x <= 1.5; x += 1.0)
        {
            sum += shadowMap.SampleCmpLevelZero(shadowSampler, lightSpacePosition.xy + CalculateOffset(x, y, shadowMapWidth, shadowMapHeight), lightSpacePosition.z);
        }

    }
    
    return sum / 16.0f;
}

float CalculateShadows(Texture2D shadowMap, SamplerComparisonState shadowSampler, float4 lightSpacePosition, float3 normal, float3 directionalLightDirection)
{
    lightSpacePosition.xyz /= lightSpacePosition.w;
    lightSpacePosition.x = lightSpacePosition.x / 2 + 0.5f;
    lightSpacePosition.y = lightSpacePosition.y / -2 + 0.5f;
    
    if (lightSpacePosition.z > 1.0f || lightSpacePosition.z < 0.0f)
        return 1.0f;
    
    float bias = clamp(SHADOW_BIAS * (1.0f - dot(normal, directionalLightDirection)), SHADOW_MIN_BIAS, SHADOW_MAX_BIAS);
    lightSpacePosition.z -= bias;
    
    return PerformPCF(shadowMap, shadowSampler, lightSpacePosition);
}