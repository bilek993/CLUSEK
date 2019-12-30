float CalculateShadows(Texture2D shadowMap, SamplerState shadowSampler, float4 lightSpacePosition, float shadowMultiplier)
{
    lightSpacePosition.xyz /= lightSpacePosition.w;
    lightSpacePosition.x = lightSpacePosition.x / 2 + 0.5f;
    lightSpacePosition.y = lightSpacePosition.y / -2 + 0.5f;
    
    lightSpacePosition.z -= 0.001;
    
    float shadowMapDepth = shadowMap.Sample(shadowSampler, lightSpacePosition.xy);
    
    if (shadowMapDepth < lightSpacePosition.z)
        return shadowMultiplier;
    else
        return 1.0f;
}