static const int CASCADES_COUNT = 4;
static const float SHADOW_BIAS = 0.0025f;

float2 CalculateOffset(int u, int v, float shadowMapWidth, float shadowMapHeight)
{
    return float2(u * 1.0f / shadowMapWidth, v * 1.0f / shadowMapHeight);
}

float PerformPCF(Texture2D shadowMap, float shadowMapWidth, float shadowMapHeight, SamplerComparisonState shadowSampler, float4 lightSpacePosition)
{
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

float CalculateShadows(Texture2D shadowMap, SamplerComparisonState shadowSampler, float4 lightSpacePosition)
{
    float shadowMapWidth, shadowMapHeight;
    shadowMap.GetDimensions(shadowMapWidth, shadowMapHeight);
    
    if (shadowMapWidth < 1.0f || shadowMapHeight < 1.0f)
        return 1.0f;
    
    lightSpacePosition.xyz /= lightSpacePosition.w;
    lightSpacePosition.x = lightSpacePosition.x / 2 + 0.5f;
    lightSpacePosition.y = lightSpacePosition.y / -2 + 0.5f;
    
    if (lightSpacePosition.z > 1.0f || lightSpacePosition.z < 0.0f)
        return 1.0f;
    
    lightSpacePosition.z -= SHADOW_BIAS;
    
    return PerformPCF(shadowMap, shadowMapWidth, shadowMapHeight, shadowSampler, lightSpacePosition);
}