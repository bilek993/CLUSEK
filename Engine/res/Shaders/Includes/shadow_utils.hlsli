float CalculateShadows(Texture2D shadowMap, SamplerState shadowSampler, float4 lightSpacePosition, float3 normal, float3 directionalLightDirection, float shadowMultiplier)
{
    lightSpacePosition.xyz /= lightSpacePosition.w;
    lightSpacePosition.x = lightSpacePosition.x / 2 + 0.5f;
    lightSpacePosition.y = lightSpacePosition.y / -2 + 0.5f;
    
    if (lightSpacePosition.x < -1.0f || lightSpacePosition.x > 1.0f ||
        lightSpacePosition.y < -1.0f || lightSpacePosition.y > 1.0f ||
        lightSpacePosition.z < 0.0f  || lightSpacePosition.z > 1.0f)
        return 1.0f;
    
    float bias = clamp(0.001f * (1.0f - dot(normal, directionalLightDirection)), 0.0f, 0.01f);
    lightSpacePosition.z -= bias;
    
    float shadowMapDepth = shadowMap.Sample(shadowSampler, lightSpacePosition.xy);
    
    if (shadowMapDepth < lightSpacePosition.z)
        return shadowMultiplier;
    else
        return 1.0f;
}