static const float SHADOW_BIAS = 0.001f;

float2 calculateOffset(int u, int v, int shadowResolution)
{
    return float2(u * 1.0f / shadowResolution, v * 1.0f / shadowResolution);
}

float CalculateShadows(Texture2D shadowMap, SamplerComparisonState shadowSampler, float4 lightSpacePosition, float3 normal, float3 directionalLightDirection, float shadowMultiplier)
{
    lightSpacePosition.xyz /= lightSpacePosition.w;
    lightSpacePosition.x = lightSpacePosition.x / 2 + 0.5f;
    lightSpacePosition.y = lightSpacePosition.y / -2 + 0.5f;
    
    float bias = clamp(SHADOW_BIAS * (1.0f - dot(normal, directionalLightDirection)), 0.0f, 0.01f);
    lightSpacePosition.z -= bias;
    
    float sum = 0;
    
    for (float y = -1.5; y <= 1.5; y += 1.0)
    {
        for (float x = -1.5; x <= 1.5; x += 1.0)
        {
            sum += shadowMap.SampleCmpLevelZero(shadowSampler, lightSpacePosition.xy + calculateOffset(x, y, 1024), lightSpacePosition.z);
        }

    }
    
    return sum / 16.0;
}